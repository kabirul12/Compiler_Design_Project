"""MyOwnCompiler Studio: lightweight Tkinter IDE for the MyOwnCompiler language."""
from __future__ import annotations
import os, sys, subprocess, threading
from pathlib import Path
import tkinter as tk
from tkinter import ttk, filedialog, messagebox

ROOT = Path(__file__).resolve().parent
SRC = ROOT / "src"
BUILD = ROOT / "build"
BUILD.mkdir(exist_ok=True)
EXE = BUILD / ("myowncompiler.exe" if os.name == "nt" else "myowncompiler")
SOURCES = [SRC / "main.cpp", SRC / "Lexer.cpp", SRC / "Parser.cpp", SRC / "SymbolTable.cpp"]

class MyOwnCompilerIDE:
    BG = "#17191f"; PANEL = "#20232b"; PANEL2 = "#262a34"; FG = "#e6e8ef"
    MUTED = "#969eae"; ACCENT = "#65a8ff"; GREEN = "#7bd88f"; RED = "#ff7272"

    def __init__(self, root: tk.Tk):
        self.root = root
        self.root.title("MyOwnCompiler Studio")
        self.root.geometry("1180x780")
        self.root.minsize(850, 560)
        self.root.configure(bg=self.BG)
        self.current_file: Path | None = None
        self._build_ui()
        self.set_code('''// Welcome to MyOwnCompiler Studio\n// Write your MyOwnCompiler code and press Run (F5).\n\npurno a = 10;\npurno b = 20;\npurno result = Jog(a, b);\nDekhao(result);\n\ndoshomik average = Gor(a, b);\nDekhao(average);\n\nlekha message = "Welcome to MyOwnCompiler";\nDekhao(message);\n''')
        self.root.bind("<F5>", lambda _e: self.run_code())
        self.root.bind("<Control-s>", lambda _e: self.save_file())
        self.root.bind("<Control-o>", lambda _e: self.open_file())
        self.root.bind("<Control-n>", lambda _e: self.new_file())
        self.root.after(250, self.update_line_numbers)
        self.root.after(350, self.highlight)
        self.write_output("MyOwnCompiler Studio is ready. Click Build & Run or press F5.\n", "info")

    def _build_ui(self):
        style = ttk.Style()
        try: style.theme_use("clam")
        except tk.TclError: pass
        style.configure("TCombobox", fieldbackground=self.PANEL2, background=self.PANEL2, foreground=self.FG)
        # Menu
        menubar = tk.Menu(self.root, bg=self.PANEL, fg=self.FG, activebackground=self.ACCENT)
        filemenu = tk.Menu(menubar, tearoff=0, bg=self.PANEL, fg=self.FG)
        filemenu.add_command(label="New      Ctrl+N", command=self.new_file)
        filemenu.add_command(label="Open     Ctrl+O", command=self.open_file)
        filemenu.add_command(label="Save     Ctrl+S", command=self.save_file)
        filemenu.add_separator(); filemenu.add_command(label="Exit", command=self.root.destroy)
        menubar.add_cascade(label="File", menu=filemenu)
        runmenu = tk.Menu(menubar, tearoff=0, bg=self.PANEL, fg=self.FG)
        runmenu.add_command(label="Build", command=self.build_compiler)
        runmenu.add_command(label="Build & Run    F5", command=self.run_code)
        menubar.add_cascade(label="Run", menu=runmenu)
        helpmenu = tk.Menu(menubar, tearoff=0, bg=self.PANEL, fg=self.FG)
        helpmenu.add_command(label="About", command=lambda: messagebox.showinfo("About", "MyOwnCompiler Studio\nA lightweight IDE for the MyOwnCompiler educational language."))
        menubar.add_cascade(label="Help", menu=helpmenu)
        self.root.config(menu=menubar)

        header = tk.Frame(self.root, bg=self.PANEL, height=58); header.pack(fill="x")
        tk.Label(header, text="◈  MyOwnCompiler", bg=self.PANEL, fg=self.FG, font=("Segoe UI", 16, "bold")).pack(side="left", padx=(18, 5), pady=12)
        tk.Label(header, text="STUDIO", bg=self.PANEL, fg=self.ACCENT, font=("Segoe UI", 9, "bold")).pack(side="left", pady=15)
        self.status = tk.Label(header, text="● Ready", bg=self.PANEL, fg=self.GREEN, font=("Segoe UI", 9))
        self.status.pack(side="right", padx=18)

        toolbar = tk.Frame(self.root, bg=self.BG, height=52); toolbar.pack(fill="x", padx=14, pady=(10, 7))
        self._button(toolbar, "＋ New", self.new_file, self.PANEL2).pack(side="left", padx=(0, 6))
        self._button(toolbar, "Open", self.open_file, self.PANEL2).pack(side="left", padx=3)
        self._button(toolbar, "Save", self.save_file, self.PANEL2).pack(side="left", padx=3)
        self._button(toolbar, "⚙ Build", self.build_compiler, self.PANEL2).pack(side="left", padx=(15, 3))
        self._button(toolbar, "▶  Build & Run", self.run_code, "#176b43").pack(side="left", padx=3)
        self._button(toolbar, "Clear Output", self.clear_output, self.PANEL2).pack(side="right", padx=3)

        self.file_label = tk.Label(self.root, text="  ●  untitled.moc", bg=self.PANEL2, fg=self.FG, anchor="w", font=("Segoe UI", 9))
        self.file_label.pack(fill="x", padx=14)
        bottom = tk.Frame(self.root, bg=self.PANEL, height=235); bottom.pack(side="bottom", fill="x", padx=14, pady=(0, 12))
        bottom.pack_propagate(False)
        top = tk.Frame(bottom, bg=self.PANEL); top.pack(fill="x", padx=12, pady=(8, 4))
        tk.Label(top, text="OUTPUT  /  CONSOLE", bg=self.PANEL, fg=self.MUTED, font=("Segoe UI", 9, "bold")).pack(side="left")
        tk.Label(top, text="Program input (for Nao):", bg=self.PANEL, fg=self.MUTED, font=("Segoe UI", 9)).pack(side="right", padx=(8, 4))
        self.stdin_entry = tk.Entry(top, bg=self.PANEL2, fg=self.FG, insertbackground="white", relief="flat", font=("Consolas", 10), width=20)
        self.stdin_entry.pack(side="right", ipady=5)
        out_frame = tk.Frame(bottom, bg=self.PANEL); out_frame.pack(fill="both", expand=True, padx=10, pady=(0, 10))
        self.output = tk.Text(out_frame, height=8, bg="#111319", fg="#d6dae3", insertbackground="white", relief="flat",
                              font=("Consolas", 10), padx=10, pady=8, wrap="word", state="disabled")
        self.output.pack(side="left", fill="both", expand=True)
        outscroll = ttk.Scrollbar(out_frame, command=self.output.yview); outscroll.pack(side="right", fill="y")
        self.output.configure(yscrollcommand=outscroll.set)
        self.output.tag_configure("error", foreground=self.RED)
        self.output.tag_configure("success", foreground=self.GREEN)
        self.output.tag_configure("info", foreground=self.ACCENT)


        editor_frame = tk.Frame(self.root, bg=self.PANEL, highlightbackground="#343946", highlightthickness=1)
        editor_frame.pack(fill="both", expand=True, padx=14, pady=(0, 10))
        self.gutter = tk.Text(editor_frame, width=4, padx=8, takefocus=0, border=0, background=self.PANEL,
                              foreground=self.MUTED, state="disabled", font=("Consolas", 11), wrap="none")
        self.gutter.pack(side="left", fill="y")
        self.editor = tk.Text(editor_frame, undo=True, wrap="none", border=0, padx=10, pady=10,
                              background="#1d2028", foreground=self.FG, insertbackground="white",
                              selectbackground="#344d70", font=("Consolas", 11), tabs=(36,),
                              spacing1=2, spacing3=2)
        self.editor.pack(side="left", fill="both", expand=True)
        scroll = ttk.Scrollbar(editor_frame, orient="vertical", command=self._scroll_both)
        scroll.pack(side="right", fill="y")
        self.editor.configure(yscrollcommand=lambda a,b: self._yscroll(a,b,scroll))
        self.editor.bind("<KeyRelease>", self._on_edit)
        self.editor.bind("<MouseWheel>", lambda _e: self.root.after_idle(self.update_line_numbers))
        self.editor.tag_configure("keyword", foreground="#c792ea")
        self.editor.tag_configure("function", foreground="#82aaff")
        self.editor.tag_configure("string", foreground="#c3e88d")
        self.editor.tag_configure("comment", foreground="#737d8c")
        self.editor.tag_configure("number", foreground="#f78c6c")


        footer = tk.Frame(self.root, bg=self.PANEL, height=24); footer.pack(fill="x", side="bottom")
        tk.Label(footer, text="MyOwnCompiler Language  •  C++17 Backend  •  F5 to Run", bg=self.PANEL, fg=self.MUTED, font=("Segoe UI", 8)).pack(side="left", padx=12, pady=4)
        self.position = tk.Label(footer, text="Ln 1, Col 1", bg=self.PANEL, fg=self.MUTED, font=("Segoe UI", 8)); self.position.pack(side="right", padx=12)

    def _button(self, parent, label, command, color):
        return tk.Button(parent, text=label, command=command, bg=color, fg=self.FG, activebackground="#3a4353",
                         activeforeground="white", relief="flat", bd=0, padx=13, pady=8, cursor="hand2", font=("Segoe UI", 9, "bold"))
    def _yscroll(self, a, b, scrollbar):
        scrollbar.set(a,b); self.gutter.yview_moveto(a)
    def _scroll_both(self, *args):
        self.editor.yview(*args); self.gutter.yview(*args)
    def _on_edit(self, _event=None):
        self.update_line_numbers(); self.highlight(); self.update_position()
    def update_position(self):
        line, col = self.editor.index("insert").split(".")
        self.position.config(text=f"Ln {line}, Col {int(col)+1}")
    def update_line_numbers(self):
        count = int(self.editor.index("end-1c").split(".")[0])
        self.gutter.config(state="normal"); self.gutter.delete("1.0", "end")
        self.gutter.insert("1.0", "\n".join(str(i) for i in range(1, count+1)))
        self.gutter.config(state="disabled")
        self.gutter.yview_moveto(self.editor.yview()[0])
    def highlight(self):
        text = self.editor.get("1.0", "end-1c")
        for tag in ("keyword", "function", "string", "comment", "number"):
            self.editor.tag_remove(tag, "1.0", "end")
        import re
        patterns = {
            "comment": r"//[^\n]*|#[^\n]*",
            "string": r'"(?:\\.|[^"\\])*"|\'(?:\\.|[^\'\\])*\'',
            "keyword": r"\b(?:purno|doshomik|okkhor|lekha|jodi|nahole|jotokkhon|jonno|ferot|kaj|shotto|mittha)\b",
            "function": r"\b(?:Jog|Biyog|Gun|Vag|Vagsesh|Ghat|Borgomul|Porom|Boro|Choto|Gunfol|Gor|Dekhao|Nao|BoroJodi|ChotoJodi|Shoman|Oshoman|Ebong|Othoba|Noy|Baraw|Komaw|Bodol|Elomelo)\b",
            "number": r"\b\d+(?:\.\d+)?\b",
        }
        for tag, pattern in patterns.items():
            for match in re.finditer(pattern, text):
                start = f"1.0+{match.start()}c"; end = f"1.0+{match.end()}c"
                self.editor.tag_add(tag, start, end)
        self.editor.tag_raise("string"); self.editor.tag_raise("comment")
    def set_code(self, code):
        self.editor.delete("1.0", "end"); self.editor.insert("1.0", code)
        self.update_line_numbers(); self.highlight()
    def get_code(self): return self.editor.get("1.0", "end-1c")
    def write_output(self, text, tag=None):
        self.output.config(state="normal"); self.output.insert("end", text, tag or ())
        self.output.see("end"); self.output.config(state="disabled")
    def clear_output(self):
        self.output.config(state="normal"); self.output.delete("1.0", "end"); self.output.config(state="disabled")
    def set_status(self, text, color=None): self.status.config(text=f"● {text}", fg=color or self.GREEN)
    def new_file(self):
        if self.get_code().strip() and not messagebox.askyesno("New File", "Create a new file? Unsaved changes may be lost."): return
        self.current_file = None; self.file_label.config(text="  ●  untitled.moc"); self.set_code(""); self.set_status("New file")
    def open_file(self):
        path = filedialog.askopenfilename(filetypes=[("MyOwnCompiler files", "*.moc"), ("Text files", "*.txt"), ("All files", "*.*")])
        if not path: return
        try:
            self.current_file = Path(path); self.set_code(self.current_file.read_text(encoding="utf-8"))
            self.file_label.config(text=f"  ●  {self.current_file.name}"); self.set_status("File opened")
        except Exception as exc: messagebox.showerror("Open Error", str(exc))
    def save_file(self):
        if self.current_file is None:
            path = filedialog.asksaveasfilename(defaultextension=".moc", filetypes=[("MyOwnCompiler files", "*.moc")])
            if not path: return
            self.current_file = Path(path)
        try:
            self.current_file.write_text(self.get_code(), encoding="utf-8")
            self.file_label.config(text=f"  ●  {self.current_file.name}"); self.set_status("Saved")
            self.write_output(f"Saved: {self.current_file}\n", "info")
        except Exception as exc: messagebox.showerror("Save Error", str(exc))
    def compile_backend(self):
        missing = [str(p) for p in SOURCES if not p.exists()]
        if missing: raise RuntimeError("Missing compiler source files:\n" + "\n".join(missing))
        command = ["g++", "-std=c++17", "-Wall", "-Wextra", "-Wpedantic", *map(str, SOURCES), "-o", str(EXE)]
        result = subprocess.run(command, cwd=ROOT, capture_output=True, text=True, timeout=90)
        if result.returncode != 0: raise RuntimeError((result.stderr or result.stdout or "Compilation failed").strip())
        return result.stdout + result.stderr
    def build_compiler(self):
        self.set_status("Building...", "#f6c85f"); self.write_output("\n[Build] Compiling MyOwnCompiler backend...\n", "info")
        def work():
            try:
                log = self.compile_backend()
                self.root.after(0, lambda: (self.write_output((log or "Build successful. No compiler warnings.\n"), "success"), self.set_status("Build successful")))
            except Exception as exc:
                self.root.after(0, lambda e=str(exc): (self.write_output(e + "\n", "error"), self.set_status("Build failed", self.RED)))
        threading.Thread(target=work, daemon=True).start()
    def run_code(self):
        code = self.get_code()
        if not code.strip(): messagebox.showwarning("Empty editor", "Write some MyOwnCompiler code first."); return
        self.set_status("Building & running...", "#f6c85f")
        self.write_output("\n> Build & Run\n", "info")
        self._run_button_worker(code)
    def _run_button_worker(self, code):
        def work():
            try:
                log = self.compile_backend()
                if log.strip(): self.root.after(0, lambda s=log: self.write_output(s + "\n", "info"))
                temp = BUILD / "current_program.moc"
                temp.write_text(code, encoding="utf-8")
                user_input = self.stdin_entry.get()
                result = subprocess.run([str(EXE), str(temp)], cwd=ROOT, input=(user_input + "\n") if user_input else "",
                                         capture_output=True, text=True, timeout=15)
                if result.stdout: self.root.after(0, lambda s=result.stdout: self.write_output(s, "success"))
                if result.stderr: self.root.after(0, lambda s=result.stderr: self.write_output(s, "error"))
                if result.returncode == 0:
                    self.root.after(0, lambda: self.set_status("Execution successful"))
                    self.root.after(0, lambda: self.write_output("\n[Process finished with exit code 0]\n", "success"))
                else:
                    self.root.after(0, lambda: self.set_status("Execution failed", self.RED))
                    self.root.after(0, lambda c=result.returncode: self.write_output(f"\n[Process finished with exit code {c}]\n", "error"))
            except FileNotFoundError:
                self.root.after(0, lambda: (self.write_output("g++ was not found. Install MinGW-w64 and add its bin folder to PATH.\n", "error"), self.set_status("g++ not found", self.RED)))
            except Exception as exc:
                self.root.after(0, lambda e=str(exc): (self.write_output(e + "\n", "error"), self.set_status("Run failed", self.RED)))
        threading.Thread(target=work, daemon=True).start()

if __name__ == "__main__":
    app_root = tk.Tk()
    MyOwnCompilerIDE(app_root)
    app_root.mainloop()
