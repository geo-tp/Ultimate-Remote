"""Native app probe with a live evdev FIFO (including key releases)."""
import os
from pathlib import Path
import select
import signal
import struct
import subprocess
import time

KEYS = dict(esc=1, tab=15, backspace=14, enter=28, space=57, fn=464,
            up=103, down=108, left=105, right=106)
KEYS.update(zip('qwertyuiop', range(16, 26)))
KEYS.update(zip('asdfghjkl', range(30, 39)))
KEYS.update(zip('zxcvbnm', range(44, 51)))


class App:
    def __init__(self, binary, root, extra_env=None):
        root = Path(root)
        root.mkdir(parents=True, exist_ok=True)
        self.device = root / 'input.fifo'
        os.mkfifo(self.device)
        self.writer = os.open(self.device, os.O_RDWR | os.O_NONBLOCK)
        env = {k: v for k, v in os.environ.items() if not k.startswith('ULTIMATE_REMOTE_')}
        env.update({f'XDG_{kind}_HOME': str(root / kind.lower()) for kind in ('CONFIG', 'DATA', 'STATE', 'CACHE')})
        env.update(ULTIMATE_REMOTE_IR_STUB='1', ULTIMATE_REMOTE_INPUT_DEVICE=str(self.device),
                   ULTIMATE_REMOTE_STATE_DIR=str(root / 'state'),
                   ULTIMATE_REMOTE_IRDB_DOWNLOAD_DIR=str(root / 'downloaded'))
        env.update(extra_env or {})
        self.process = subprocess.Popen([str(binary)], stdin=subprocess.PIPE, stdout=subprocess.PIPE,
                                        stderr=subprocess.STDOUT, env=env)
        self.buffer = b''
        self.history = b''
        self.until(b'== Ultimate Remote ==')

    def event(self, code, value, event_type=1):
        os.write(self.writer, struct.pack('@llHHi', 0, 0, event_type, code, value))

    def press(self, name):
        self.event(KEYS[name], 1)

    def release(self, name):
        self.event(KEYS[name], 0)

    def tap(self, *names):
        for name in names:
            self.press(name)
            self.release(name)

    def send(self, text):
        self.process.stdin.write(text)
        self.process.stdin.flush()

    def read(self, timeout=.05):
        ready, _, _ = select.select([self.process.stdout], [], [], timeout)
        if ready:
            chunk = os.read(self.process.stdout.fileno(), 65536)
            self.buffer += chunk
            self.history += chunk
            return bool(chunk)
        return False

    def until(self, expected, timeout=6):
        deadline = time.monotonic() + timeout
        while expected not in self.buffer:
            assert self.process.poll() is None, self.history[-3000:]
            assert time.monotonic() < deadline, (expected, self.history[-3000:])
            self.read()
        end = self.buffer.index(expected) + len(expected)
        result, self.buffer = self.buffer[:end], self.buffer[end:]
        return result

    def alive_for(self, seconds):
        deadline = time.monotonic() + seconds
        while time.monotonic() < deadline:
            assert self.process.poll() is None, self.history[-3000:]
            self.read(min(.05, max(0, deadline - time.monotonic())))
        assert self.process.poll() is None, self.history[-3000:]

    def close(self):
        alive = self.process.poll() is None
        if alive:
            self.process.terminate()
        output, _ = self.process.communicate(timeout=3)
        self.history += output
        if self.writer >= 0:
            os.close(self.writer)
            self.writer = -1
        # Test cleanup uses SIGTERM; successful hold-to-exit returns zero.
        assert self.process.returncode == (-signal.SIGTERM if alive else 0), self.history[-3000:]


class CardputerApp(App):
    """Emit the official tca8418c scan codes and translated Fn key layer.

    Derived from m5stack-linux-dtoverlays commit
    75518549a2640486f5d13ea72ca4935b198d7c20, cardputerzero-v5-overlay.dts.
    The driver reports KEY_DVD (389), not KEY_FN (464), and remembers the
    translated code until release even when Fn is released first.
    """
    FN_CODES = dict(q=164, w=165, e=163, u=224, i=225, o=87, p=88,
                    a=113, s=114, d=115, f=103, h=138, j=99, k=102, l=104,
                    z=105, x=108, c=106, b=110, n=107, m=109, backspace=111)
    SCANS = dict(fn=0x42, esc=0x40, tab=0x49, enter=0x29, space=0x39, backspace=0x09)
    SCANS.update(zip('qwertyuiop', range(0x10, 0x1a)))
    SCANS.update(zip('asdfghjkl', range(0x20, 0x29)))
    SCANS.update(zip('zxcvbnm', range(0x32, 0x39)))

    def __init__(self, *args, **kwargs):
        self.fn_layer = False
        self.pressed_codes = {}
        super().__init__(*args, **kwargs)

    def event(self, code, value, event_type=1):
        if event_type != 1:
            return super().event(code, value, event_type)
        name = next((name for name, number in KEYS.items() if number == code), None)
        if name == 'fn':
            self.fn_layer = value != 0
            mapped = 389
        elif value == 1:
            mapped = self.FN_CODES.get(name, code) if self.fn_layer else code
            self.pressed_codes[code] = mapped
        else:
            mapped = self.pressed_codes.get(code, code)
            if value == 0:
                self.pressed_codes.pop(code, None)
        if name in self.SCANS:
            super().event(4, self.SCANS[name], event_type=4)  # EV_MSC / MSC_SCAN
        super().event(mapped, value)
        super().event(0, 0, event_type=0)  # SYN_REPORT
