#!/usr/bin/env python3
# esp32_flasher.py
import tkinter as tk
from tkinter import filedialog, messagebox, ttk
import subprocess
import configparser
import os
import sys
import tempfile
import shutil
import zipfile
import threading
import glob

class ESP32FlasherApp(tk.Tk):
    def __init__(self):
        super().__init__()
        self.title("ESP32 Flasher")
        self.geometry("800x400")
        self.resizable(False, False)

        self.pio_env = None
        self.proj_dir = None
        self.venv_dir = None
        self.venv_python = None
        self.fs_subtype = None

        # Layout
        left = tk.Frame(self)
        left.pack(side=tk.LEFT, fill=tk.Y, padx=5, pady=5)
        right = tk.Frame(self)
        right.pack(side=tk.RIGHT, fill=tk.BOTH, expand=True, padx=5, pady=5)

        # Controls
        labels = ["Port:", "Baudrate:", "Bootloader (0x1000):", "Partition-Table (0x8000):",
                  "Firmware (0x10000):", "Partition CSV (.csv):", "FS-Daten (Ordner/.zip/.bin):", "FS-Offset (hex):"]
        self.entries = {}
        commands = [None, None, self.browse_file, self.browse_file,
                    self.browse_file, self.browse_csv, self.browse_fs, None]
        row = 0
        tk.Button(left, text="PlatformIO Config laden", command=self.load_pio_config) \
            .grid(row=row, column=0, columnspan=3, pady=3, sticky="ew")
        row += 1
        for i, lbl in enumerate(labels):
            tk.Label(left, text=lbl).grid(row=row, column=0, sticky="e")
            entry = tk.Entry(left, width=30)
            entry.grid(row=row, column=1)
            if commands[i]:
                tk.Button(left, text="...", command=lambda e=entry, cmd=commands[i]: cmd(e)).grid(row=row, column=2)
            self.entries[lbl] = entry
            row += 1
        tk.Button(left, text="Flash starten", command=self.start_flash) \
            .grid(row=row, column=0, columnspan=3, pady=5, sticky="ew")
        row += 1
        self.progress = ttk.Progressbar(left, mode='indeterminate')
        self.progress.grid(row=row, column=0, columnspan=3, sticky="ew")

        # Console
        self.console = tk.Text(right, state=tk.DISABLED)
        self.console.pack(fill=tk.BOTH, expand=True)

    def browse_file(self, entry_widget):
        path = filedialog.askopenfilename(title="Datei auswählen", filetypes=[("Binär", "*.bin"), ("Alle", "*")])
        if path:
            entry_widget.delete(0, tk.END)
            entry_widget.insert(0, path)

    def browse_csv(self, entry_widget):
        path = filedialog.askopenfilename(title="Partitionstabelle wählen", filetypes=[("CSV", "*.csv")])
        if path:
            entry_widget.delete(0, tk.END)
            entry_widget.insert(0, path)
            # Offset automatisch einlesen
            self.load_offset()

    def browse_fs(self, entry_widget):
        d = filedialog.askdirectory(title="FS-Ordner wählen")
        if d:
            entry_widget.delete(0, tk.END)
            entry_widget.insert(0, d)
            return
        f = filedialog.askopenfilename(title="FS-ZIP/bin wählen", filetypes=[("ZIP", "*.zip"), ("BIN", "*.bin")])
        if f:
            entry_widget.delete(0, tk.END)
            entry_widget.insert(0, f)

    def load_csv_offset(self):
        path = self.entries["Partition CSV (.csv):"].get().strip()
        try:
            with open(path, encoding='utf-8') as f:
                lines = [l.strip() for l in f if l.strip()]
            hdr = lines[0].lstrip('#').split(','); cols = [h.strip().lower() for h in hdr]
            for l in lines[1:]:
                v = [x.strip() for x in l.split(',')]
                r = dict(zip(cols, v))
                if r.get('type','').lower()=='data' and r.get('subtype','').lower() in ('littlefs','spiffs'):
                    return hex(int(r.get('offset','0'),0)), r.get('subtype').lower()
        except:
            pass
        return None, None

    def load_offset(self):
        off, subtype = self.load_csv_offset()
        if off:
            self.entries["FS-Offset (hex):"].delete(0, tk.END)
            self.entries["FS-Offset (hex):"].insert(0, off)
            self.fs_subtype = subtype
        else:
            messagebox.showwarning('Offset', 'Kein spiffs/littlefs Eintrag in CSV gefunden')

    def load_pio_config(self):
        ini = filedialog.askopenfilename(title="platformio.ini wählen", filetypes=[("INI", "*.ini")])
        if not ini:
            return
        cfg = configparser.ConfigParser()
        cfg.read(ini)
        env = next((s for s in cfg.sections() if s.startswith('env:')), None)
        if not env:
            messagebox.showwarning('env', 'Kein [env:*] in INI')
            return
        # set env & paths
        self.pio_env = env.split(':',1)[1]
        self.proj_dir = os.path.dirname(ini)
        build = os.path.join(self.proj_dir, '.pio', 'build', self.pio_env)

        # Setup venv
        self.progress.config(mode='determinate', maximum=100, value=0)
        self.log('Erstelle venv...')
        self.venv_dir = os.path.join(self.proj_dir, '.pio_cli_env')
        if not os.path.isdir(self.venv_dir):
            subprocess.run([sys.executable, '-m', 'venv', self.venv_dir], check=True)
        self.progress.step(30)
        self.log('Installiere PlatformIO & esptool...')
        pip_exe = os.path.join(self.venv_dir, 'bin', 'pip')
        subprocess.run([pip_exe, 'install', '--upgrade', 'pip'], check=True)
        subprocess.run([pip_exe, 'install', 'platformio', 'esptool'], check=True)
        self.progress.step(70)
        self.venv_python = os.path.join(self.venv_dir, 'bin', 'python')
        self.progress.config(mode='indeterminate')

        # Fill fields
        self.entries["Port:"].delete(0, tk.END)
        self.entries["Port:"].insert(0, cfg.get(env, 'upload_port', fallback=''))
        self.entries["Baudrate:"].delete(0, tk.END)
        self.entries["Baudrate:"].insert(0, cfg.get(env, 'upload_speed', fallback='921600'))
        self.entries["Bootloader (0x1000):"].delete(0, tk.END)
        self.entries["Bootloader (0x1000):"].insert(0, os.path.join(build, 'bootloader.bin'))
        self.entries["Partition-Table (0x8000):"].delete(0, tk.END)
        self.entries["Partition-Table (0x8000):"].insert(0, os.path.join(build, 'partition-table.bin'))
        self.entries["Firmware (0x10000):"].delete(0, tk.END)
        self.entries["Firmware (0x10000):"].insert(0, os.path.join(build, 'firmware.bin'))
        # Auto-load offset if CSV present
        if self.entries["Partition CSV (.csv):"].get().strip():
            self.load_offset()
        self.log(f'Config & venv bereit: {self.pio_env}')

    def start_flash(self):
        threading.Thread(target=self.flash, daemon=True).start()
        self.progress.start()

    def log(self, msg):
        self.console.config(state=tk.NORMAL)
        self.console.insert(tk.END, msg + '\n')
        self.console.see(tk.END)
        self.console.config(state=tk.DISABLED)

    def flash(self):
        tmp = None
        try:
            # gather
            p = self.entries["Port:"].get().strip(); b = self.entries["Baudrate:"].get().strip()
            bl = self.entries["Bootloader (0x1000):"].get().strip(); pt = self.entries["Partition-Table (0x8000):"].get().strip()
            fw = self.entries["Firmware (0x10000):"].get().strip(); fsd = self.entries["FS-Daten (Ordner/.zip/.bin):"].get().strip()
            off = self.entries["FS-Offset (hex):"].get().strip(); subtype = self.fs_subtype
            if not all([p, b, bl, pt, fw]):
                raise Exception('Bitte Hauptfelder ausfüllen')

            # Flash firmware
            py = self.venv_python or sys.executable
            cmd_fw = [py, '-m', 'esptool', '--port', p, '--baud', b,
                      'write_flash', '-z', '0x1000', bl, '0x8000', pt, '0x10000', fw]
            self.log('Starte Firmware-Flash')
            r1 = subprocess.run(cmd_fw, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
            if r1.returncode:
                raise Exception(r1.stdout.decode())

            # Flash filesystem
            if fsd:
                py = self.venv_python or sys.executable
                # Directory: use PlatformIO uploadfs
                if os.path.isdir(fsd):
                    self.log('Upload FS-Ordner via PlatformIO uploadfs')
                    cli = os.path.join(self.venv_dir, 'bin', 'pio')
                    # set fs path in environment
                    env = os.environ.copy()
                    env['PLATFORMIO_DIR'] = self.proj_dir
                    # Run uploadfs target
                    subprocess.run([cli, 'run', '-e', self.pio_env, '-t', 'uploadfs'], cwd=self.proj_dir, check=True, env=env)
                else:
                    # ZIP or BIN: handle as before
                    img = None
                    tmp = None
                    if fsd.lower().endswith('.zip'):
                        tmp = tempfile.mkdtemp()
                        with zipfile.ZipFile(fsd, 'r') as zf:
                            zf.extractall(tmp)
                        bins = [f for f in os.listdir(tmp) if f.lower().endswith('.bin')]
                        if not bins:
                            raise Exception('Keine .bin im ZIP gefunden')
                        img = os.path.join(tmp, bins[0])
                    elif fsd.lower().endswith('.bin'):
                        img = fsd
                    else:
                        raise Exception('Ungültiger FS-Pfad, bitte Ordner, .zip oder .bin angeben')
                    if not off.startswith('0x'):
                        raise Exception('Offset fehlt oder fehlerhaft')
                    self.log(f'Flash FS at {off}: {img}')
                    r2 = subprocess.run([py, '-m', 'esptool', '--port', p, '--baud', b, 'write_flash', off, img],
                                         stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
                    if r2.returncode:
                        raise Exception(r2.stdout.decode())
            self.log('Flash abgeschlossen')
            messagebox.showinfo('Erfolg', 'Flash abgeschlossen')
        except Exception as e:
            self.log('Fehler: ' + str(e))
            messagebox.showerror('Fehler', str(e))
        finally:
            if tmp:
                shutil.rmtree(tmp, ignore_errors=True)
            self.progress.stop()

if __name__ == '__main__':
    ESP32FlasherApp().mainloop()
