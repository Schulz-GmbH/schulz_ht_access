#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import argparse
import os
import sys
import tkinter as tk
from tkinter import filedialog, messagebox, ttk


def collect_and_write(paths, out_file, filter_comments):
    """
    Durchsucht die angegebenen Dateien und Verzeichnisse,
    schreibt ihren Inhalt in out_file und filtert optional Kommentare heraus.
    Unterstützt einzeilige (#, //) und mehrzeilige (/* ... */) Kommentare.
    """
    out_basename = os.path.basename(out_file)
    with open(out_file, 'w', encoding='utf-8') as fout:
        for path in paths:
            if os.path.isfile(path):
                _append_file(path, fout, filter_comments)
            elif os.path.isdir(path):
                for root, dirs, files in os.walk(path):
                    for name in files:
                        if name == out_basename:
                            continue
                        file_path = os.path.join(root, name)
                        _append_file(file_path, fout, filter_comments)
            else:
                print(f"Warnung: Pfad nicht gefunden oder kein regulärer Dateityp: {path}", file=sys.stderr)
    print(f'Fertig: Inhalt von {paths} wurde in "{out_file}" zusammengeführt.')


def _append_file(file_path, fout, filter_comments):
    """
    Hängt den Inhalt einer einzelnen Datei an den Writer an,
    inklusive eines Trenner-Headers. Filtert optional Kommentare.
    """
    fout.write(f'\n===== Datei: {file_path} =====\n\n')
    try:
        with open(file_path, 'r', encoding='utf-8') as fin:
            in_block = False
            for line in fin:
                if filter_comments:
                    stripped = line.lstrip()
                    # Ende eines Block-Kommentars
                    if in_block:
                        if '*/' in line:
                            in_block = False
                            idx = line.find('*/') + 2
                            rest = line[idx:]
                            if rest.strip():
                                fout.write(rest)
                        continue
                    # Einzeilige Kommentare
                    if stripped.startswith('#') or stripped.startswith('//'):
                        continue
                    # Block-Kommentar auf einer Zeile
                    if '/*' in line:
                        start = line.find('/*')
                        end = line.find('*/', start + 2)
                        if end != -1:
                            new_line = line[:start] + line[end+2:]
                            if new_line.strip():
                                fout.write(new_line)
                            continue
                        else:
                            if start > 0:
                                fout.write(line[:start])
                            in_block = True
                            continue
                fout.write(line)
    except UnicodeDecodeError:
        fout.write(f'[Datei {file_path} konnte nicht gelesen werden]\n')


def parse_args():
    parser = argparse.ArgumentParser(
        description='Inhalt mehrerer Dateien/Ordner in eine Datei zusammenführen.'
    )
    parser.add_argument(
        '-o', '--output',
        help='Name der Ausgabedatei (Default: merged_files.txt)',
        default='merged_files.txt'
    )
    parser.add_argument(
        '-f', '--filter-comments',
        help='Kommentare (Zeilen mit #, // oder /*...*/) herausfiltern',
        action='store_true'
    )
    parser.add_argument(
        'paths',
        nargs='*',
        help='Dateien oder Verzeichnisse, die zusammengeführt werden sollen'
    )
    return parser.parse_args()


class App(tk.Tk):
    def __init__(self):
        super().__init__()
        self.title("Datei-Zusammenführer")
        self.geometry("600x400")
        self.paths = []
        self.out_file = tk.StringVar(value="merged_files.txt")
        self.filter_comments = tk.BooleanVar(value=False)
        self._build_ui()

    def _build_ui(self):
        frm = ttk.Frame(self, padding=10)
        frm.pack(fill=tk.BOTH, expand=True)

        ttk.Label(frm, text="Ausgewählte Dateien/Ordner:").pack(anchor=tk.W)
        self.listbox = tk.Listbox(frm, height=8)
        self.listbox.pack(fill=tk.BOTH, expand=True)

        btn_frame = ttk.Frame(frm)
        btn_frame.pack(fill=tk.X, pady=5)
        ttk.Button(btn_frame, text="Dateien hinzufügen", command=self.add_files).pack(side=tk.LEFT)
        ttk.Button(btn_frame, text="Ordner hinzufügen", command=self.add_directory).pack(side=tk.LEFT)
        ttk.Button(btn_frame, text="Entfernen", command=self.remove_selected).pack(side=tk.LEFT)

        out_frame = ttk.Frame(frm)
        out_frame.pack(fill=tk.X, pady=5)
        ttk.Label(out_frame, text="Ausgabedatei:").pack(side=tk.LEFT)
        ttk.Entry(out_frame, textvariable=self.out_file).pack(fill=tk.X, side=tk.LEFT, expand=True)

        ttk.Checkbutton(
            frm,
            text="Kommentare herausfiltern (Zeilen mit #, // oder /*...*/)",
            variable=self.filter_comments
        ).pack(anchor=tk.W, pady=5)

        action_frame = ttk.Frame(frm)
        action_frame.pack(fill=tk.X, pady=10)
        ttk.Button(action_frame, text="Starten", command=self.on_start).pack(side=tk.RIGHT)
        ttk.Button(action_frame, text="Abbrechen", command=self.destroy).pack(side=tk.RIGHT)

    def add_files(self):
        files = filedialog.askopenfilenames(title="Dateien auswählen")
        for p in files:
            if p not in self.paths:
                self.paths.append(p)
                self.listbox.insert(tk.END, p)

    def add_directory(self):
        directory = filedialog.askdirectory(title="Ordner auswählen", mustexist=True)
        if directory and directory not in self.paths:
            self.paths.append(directory)
            self.listbox.insert(tk.END, directory)

    def remove_selected(self):
        for idx in reversed(self.listbox.curselection()):
            self.paths.pop(idx)
            self.listbox.delete(idx)

    def on_start(self):
        if not self.paths:
            messagebox.showwarning("Keine Pfade", "Bitte mindestens eine Datei oder einen Ordner auswählen.")
            return
        out = self.out_file.get().strip()
        if not out:
            messagebox.showwarning("Ungültige Ausgabedatei", "Bitte einen gültigen Ausgabedateinamen angeben.")
            return
        try:
            collect_and_write(self.paths, out, self.filter_comments.get())
            messagebox.showinfo("Fertig", f'Inhalte wurden in "{out}" geschrieben.')
            self.destroy()
        except Exception as e:
            messagebox.showerror("Fehler", str(e))


def main():
    args = parse_args()
    if len(sys.argv) == 1 or (not args.paths and sys.argv[0] == os.path.basename(__file__)):
        app = App()
        app.mainloop()
    else:
        collect_and_write(args.paths, args.output, args.filter_comments)

if __name__ == "__main__":
    main()
