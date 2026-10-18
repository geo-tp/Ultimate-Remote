#!/usr/bin/env python3
"""Check real timed Esc press/release behavior through the Linux input reader."""
import argparse
import importlib.util
import os
from pathlib import Path
import tempfile
import time

spec = importlib.util.spec_from_file_location('probe', Path(__file__).with_name('native-app-test.py'))
probe = importlib.util.module_from_spec(spec)
spec.loader.exec_module(probe)
parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument('--native-binary', type=Path, required=True)
args = parser.parse_args()
binary = args.native_binary.resolve()
HINT = b'Hold 3sec to return to home'

with tempfile.TemporaryDirectory(prefix='ultimate-remote-escape-') as temporary:
    root = Path(temporary)
    app = probe.App(binary, root / 'hold')
    try:
        # Even a near-complete hold must cancel, and the next press starts anew.
        app.press('esc')
        app.until(HINT)
        app.alive_for(2.75)
        app.release('esc')
        app.until(b'> REMOTES')
        app.alive_for(.4)
        app.press('esc')
        started = time.monotonic()
        app.until(HINT)
        app.alive_for(.2)
        app.event(probe.KEYS['esc'], 2)  # kernel repeat cannot restart the timer
        app.alive_for(2.5)
        app.process.wait(timeout=1)
        elapsed = time.monotonic() - started
        assert 3.0 <= elapsed < 3.7, elapsed
        assert app.process.returncode == 0
    finally:
        app.close()
    assert b'ESC held for 3 seconds on home, exiting' in app.history
    assert b'raw sequence' not in app.history

    app = probe.App(binary, root / 'submenus')
    try:
        app.tap('z', 'z', 'enter')  # Settings, immediately left of Info
        app.until(b'== < Settings ==')
        app.press('esc')
        app.until(b'== Ultimate Remote ==')
        for _ in range(4):
            app.event(probe.KEYS['esc'], 2)
            app.alive_for(.8)
        assert HINT not in app.history  # old press cannot become a root hold
        app.release('esc')
        app.tap('enter')
        app.until(b'== < Settings ==')
        app.tap('tab')
        app.alive_for(.15)
        app.tap('enter')
        app.until(b'== < Select IR TX GPIO ==')  # Tab did not go back
        app.tap('esc')
        app.until(b'== < Settings ==')
        app.tap('esc')
        app.until(b'== Ultimate Remote ==')
        app.tap('c', 'enter')
        app.until(b'== < Info: 3366 embedded remotes ==')
        app.tap('esc')
        app.until(b'== Ultimate Remote ==')
        app.tap('c', 'enter')  # wrap to Remotes
        app.until(b'== < Type to filter [search] ==')
        app.tap('s', 'o', 'n', 'y', 'enter')
        app.until(b'== < Sony [search] ==')
        app.tap('enter')
        app.until(b'== < Remote 26,42.csv - TV ==')
        app.press('esc')
        app.until(b'== < Sony [search] ==')
        app.send(b'\x1b')  # duplicate console input from the physical Esc
        app.event(probe.KEYS['esc'], 2)
        app.alive_for(.2)
        assert b'== < Type to filter' not in app.buffer
        app.release('esc')
        app.tap('esc')
        app.until(b'== < Type to filter [search] ==')
        # ANSI arrows and Alt+letters still work; a lone terminal Esc is Back.
        app.send(b'\x1b[B\x1b[A\x1bf')
        app.until(b'== < f [search] ==')
        app.alive_for(.16)  # this next Esc is a separate terminal input
        app.send(b'\x1b')
        app.until(b'== Ultimate Remote ==')
        app.send(b'\x1b')
        app.alive_for(.15)
        assert app.process.poll() is None
        app.tap('c', 'enter')
        app.until(b'How scanning works')
        app.tap('esc')
        app.until(b'== Ultimate Remote ==')
        app.tap('c', 'enter')
        app.until(b'== < Favorite remotes ==')
        app.tap('esc')
        app.until(b'== Ultimate Remote ==')
        app.tap('c', 'c', 'enter')
        app.until(b'== < IR Builder')
        app.tap('esc')
        app.until(b'== Ultimate Remote ==')
    finally:
        app.close()
    assert b'raw sequence' not in app.history

    # Esc must also cancel the asynchronous catalog request and return one page.
    helper = root / 'slow.py'
    helper.write_text('import time\ntime.sleep(30)\n')
    app = probe.App(binary, root / 'download', {'ULTIMATE_REMOTE_IRDB_HELPER': str(helper)})
    try:
        app.tap('c', 'c', 'c', 'enter')
        app.until(b'== < Online IR ==')
        app.tap('x', 'enter')
        app.until(b'Loading the catalog...')
        app.press('esc')
        app.until(b'== < Online IR ==', timeout=2)
        app.event(probe.KEYS['esc'], 2)
        app.alive_for(.2)
        assert b'== Ultimate Remote ==' not in app.buffer
        app.release('esc')
        app.tap('esc')
        app.until(b'== Ultimate Remote ==')
    finally:
        app.close()

    # Lost input synchronization and unplugged devices must cancel the hold.
    for case in ('dropped', 'disconnected'):
        app = probe.App(binary, root / case)
        try:
            app.press('esc')
            app.until(HINT)
            if case == 'dropped':
                app.event(3, 0, event_type=0)  # SYN_DROPPED
                app.event(0, 0, event_type=0)  # SYN_REPORT
            else:
                os.close(app.writer)
                app.writer = -1
            app.until(b'> REMOTES')
            app.alive_for(3.1)
        finally:
            app.close()

print('Esc Back, one action per press, release cancellation, 3s root exit, lost-input cancellation and terminal escape parsing passed.')
