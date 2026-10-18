#!/usr/bin/env python3
"""Exercise Info paging and return paths without framebuffer or IR hardware."""
import argparse
import importlib.util
from pathlib import Path
import tempfile

spec = importlib.util.spec_from_file_location('probe', Path(__file__).with_name('native-app-test.py'))
probe = importlib.util.module_from_spec(spec)
spec.loader.exec_module(probe)

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument('--native-binary', type=Path, required=True)
args = parser.parse_args()
with tempfile.TemporaryDirectory(prefix='ultimate-remote-info-') as temporary:
    root = Path(temporary)
    app = probe.App(args.native_binary.resolve(), root)
    try:
        app.tap('z', 'enter')
        app.until(b'== < Info: 3366 embedded remotes ==')
        for title in (b'Thousands more online', b'Download once', b'Find & save', b'3366 embedded remotes'):
            app.tap('x')
            app.until(b'== < Info: ' + title + b' ==')
        app.tap('f')
        app.until(b'== < Info: Find & save ==')
        app.tap('enter')
        app.until(b'== < Info: 3366 embedded remotes ==')
        app.tap('esc')
        app.until(b'== Ultimate Remote ==')
        pages = app.history

        # Z resumes its carousel role after Esc returns from Info.
        app.tap('z', 'enter')
        app.until(b'== < Settings ==')
        app.tap('esc')
        app.until(b'== Ultimate Remote ==')
        app.tap('c', 'enter')
        app.until(b'== < Info: 3366 embedded remotes ==')
        app.tap('z')
        app.until(b'== Ultimate Remote ==')
        app.tap('c', 'enter')
        app.until(b'== < Type to filter [search] ==')
        app.tap('j', 'b', 'l', 'enter')
        app.until(b'== < JBL [search] ==')
        app.until(b'2,189.csv - Soundbar')
    finally:
        app.close()
    assert b'raw sequence' not in app.history
    assert b'> INFO - About and quick guide' in pages
    assert b'FLIPPER IR' not in pages and b'ONLINE IR' in pages
    assert b'ONLINE IR - Flipper Database' in pages
    for title in (b'3366 embedded remotes', b'Thousands more online', b'Download once', b'Find & save'):
        assert b'== < Info: ' + title + b' ==' in pages, title
    for title in (b'Ultimate Remote', b'Keys & filtering', b'IR transmitter', b'Compatibility'):
        assert b'== < Info: ' + title + b' ==' not in pages, title
    assert b'636 manufacturers in Remotes.' in pages
    assert b'Source: Lucaslhm/Flipper-IRDB.' in pages
    assert b'Page 4 / 4' in pages
    assert pages.count(b'== < Info: 3366 embedded remotes ==') == 3
    assert pages.count(b'== < Info: Find & save ==') == 2
    assert pages.count(b'== Ultimate Remote ==') == 2

print('Info entry, all pages, counts, wraparound, Z/Esc return, carousel state and no-transmission checks passed.')
