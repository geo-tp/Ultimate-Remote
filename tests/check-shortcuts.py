#!/usr/bin/env python3
"""Check Fn command routing with real evdev records and a stub IR sender."""
import argparse
import importlib.util
from pathlib import Path
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[1]
spec = importlib.util.spec_from_file_location('probe', Path(__file__).with_name('native-app-test.py'))
probe = importlib.util.module_from_spec(spec)
spec.loader.exec_module(probe)
parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument('--native-binary', type=Path)
parser.add_argument('--cardputer-events', action='store_true',
                    help='Replay the official tca8418c KEY_DVD/media/arrow events')
args = parser.parse_args()
app_type = probe.CardputerApp if args.cardputer_events else probe.App


def remote(path, names):
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text('Filetype: IR signals file\nVersion: 1\n' + ''.join(
        f'#\nname: {name}\ntype: raw\nfrequency: 38000\nduty_cycle: .33\ndata: 9000 4500 560 560\n'
        for name in names))


def send_fn(app, letter, command, title):
    app.press('fn')
    app.tap(letter)
    app.release('fn')
    app.until(f'IR sent: {command}\n'.encode())
    app.until(title)


with tempfile.TemporaryDirectory(prefix='ultimate-remote-shortcuts-') as temporary:
    root = Path(temporary)
    test = root / 'mapping-check'
    subprocess.run(['g++', '-std=c++17', '-Isrc', 'tests/check-shortcuts.cpp',
                    'src/Utils/StringUtils.cpp', '-o', str(test)], cwd=ROOT, check=True)
    subprocess.run([str(test)], check=True)
    if args.native_binary:
        binary = args.native_binary.resolve()
        commands = dict(s='KEY_VOLUMEDOWN', a='Mute', d='VOL +', q='Play', w='Rewind',
                        e='Fast Forward', f='KEY_UP', x='Down', c='Right', z='Left')
        remote(root / 'app/downloaded/Full.ir', list(commands.values()))
        remote(root / 'app/downloaded/Transport.ir', ['Previous', 'Next', 'PLAY/PAUSE'])
        app = app_type(binary, root / 'app')
        title = b'== < Full =='
        try:
            app.tap('c', 'c', 'c', 'enter', 'enter', 'enter')
            app.until(title)
            app.until(b'Left - Fn+Z')
            assert b'raw sequence' not in app.history
            # Keep Fn held across all ten letters, including F/X/Z navigation keys.
            app.press('fn')
            for letter, command in commands.items():
                app.tap(letter)
                app.until(f'IR sent: {command}\n'.encode())
                app.until(title)
            app.release('fn')
            count = app.history.count(b'raw sequence')
            assert count == 10, app.history
            # Repeats must not change into a plain shortcut if Fn is released first.
            app.press('fn')
            app.press('s')
            app.until(b'IR sent: KEY_VOLUMEDOWN\n')
            app.until(title)
            app.event(probe.KEYS['s'], 2)
            app.release('fn')
            app.event(probe.KEYS['s'], 2)
            app.alive_for(.3)
            app.release('s')
            assert app.history.count(b'raw sequence') == count + 1
            # Base F/X navigate; an unassigned Fn shortcut sends nothing.
            app.tap('x')
            app.until(b'> Mute - Fn+A')
            app.tap('f')
            app.until(b'> KEY_VOLUMEDOWN - Fn+S')
            app.press('fn')
            app.tap('k')
            app.release('fn')
            app.alive_for(.1)
            assert app.history.count(b'raw sequence') == count + 1
            app.tap('z')
            app.until(b'== < Downloaded [search] ==')
            # Fn returns to text input after leaving the command screen.
            app.press('fn')
            app.tap('f')
            app.release('fn')
            app.until(b'== < Find: f [search] ==')
            app.tap('backspace', 'x', 'enter')
            title = b'== < Transport =='
            app.until(title)
            for letter, command in [('w', 'Previous'), ('e', 'Next'), ('q', 'PLAY/PAUSE')]:
                send_fn(app, letter, command, title)
            # Terminals offer the same shortcuts with Alt, preserving ANSI arrows.
            app.send(b'\x1bw')
            app.until(b'IR sent: Previous\n')
            app.until(title)
            app.send(b'\x1b[B\x1b[A')
            app.until(b'> Next - Fn+E')
            app.until(b'> Previous - Fn+W')
        finally:
            app.close()

        # Exercise the embedded selection/controller too, with a real Sony profile.
        app = app_type(binary, root / 'builtin')
        try:
            app.tap('enter', 's', 'o', 'n', 'y', 'enter', 'x', 'x', 'enter')
            title = b'== < Remote 1,-1.csv - TV =='
            app.until(title)
            for letter, name in [('s', 'KEY_VOLUMEDOWN'), ('d', 'KEY_VOLUMEUP'), ('a', 'KEY_MUTE')]:
                send_fn(app, letter, name, title)
            app.tap('esc')
            app.until(b'== < Sony [search] ==')
        finally:
            app.close()
        if args.cardputer_events:
            app = app_type(binary, root / 'keyboard')
            try:
                app.tap('enter')
                app.until(b'== < Type to filter [search] ==')
                app.press('fn')
                # Regression: Fn+X is KEY_DOWN on the actual device, not KEY_X.
                app.tap('x')
                app.until(b'== < x [search] ==')
                app.tap('backspace')
                app.until(b'== < Type to filter [search] ==')
                query = ''
                for letter in 'qwertyuiopasdfghjklzxcvbnm':
                    app.tap(letter)
                    query += letter
                    app.until(f'== < {query} [search] =='.encode())
                app.release('fn')
                app.tap(*(['backspace'] * len(query)), 's')
                app.until(b'== < s [search] ==')
                app.tap('x')
                app.until(b'== < s [search] ==')
                app.tap('f')
                app.until(b'== < s [search] ==')
                assert b'raw sequence' not in app.history
            finally:
                app.close()
        profile = 'Cardputer Zero hardware events' if args.cardputer_events else 'Generic KEY_FN events'
        print(f'{profile}: all ten Fn commands, one-shot handling, navigation, filters, transport fallback, terminal Alt and embedded sends passed.')
