#!/usr/bin/env python3
"""Offline regression checks for the GitHub client and native IRDB navigation."""
import argparse
import importlib.util
import json
import os
from pathlib import Path
import select
import signal
import struct
import subprocess
import sys
import tempfile
import time

ROOT = Path(__file__).resolve().parents[1]
spec = importlib.util.spec_from_file_location('irdb', ROOT / 'src/Platform/Linux/irdb-client.py')
irdb = importlib.util.module_from_spec(spec)
spec.loader.exec_module(irdb)
RAW = b'Filetype: IR signals file\nVersion: 1\n#\nname: Power\ntype: raw\nfrequency: 38000\nduty_cycle: .5\ndata: 9000 4500 560 70000 560\n'
PARSED = b'Filetype: IR signals file\nVersion: 1\nname: Volume\ntype: parsed\nprotocol: NEC\naddress: 04 00 00 00\ncommand: 08 00 00 00\n'


def fail(*args, **kwargs):
    raise irdb.CatalogError('Cannot connect. Check Wi-Fi and retry')


def seed(root):
    files = {'TVs/Samsung/fzxc_raw.ir': irdb.blob_sha(RAW),
             'TVs/Samsung/Second_remote.ir': irdb.blob_sha(PARSED),
             'ACs/Brand/Model.ir': irdb.blob_sha(RAW)}
    irdb.atomic_write(root / 'index.json', json.dumps({'version': 1, 'revision': 'a' * 40, 'files': files}).encode())
    for data in (RAW, PARSED):
        irdb.atomic_write(root / 'remotes' / (irdb.blob_sha(data) + '.ir'), data)
    return files


def client_checks(root):
    seen = []
    def fetch(url, limit, raw=False):
        seen.append(url)
        if raw:
            return RAW
        return json.dumps({'sha': 'a' * 40, 'truncated': False, 'tree': [
            {'path': 'TVs/Brand/Remote.ir', 'sha': irdb.blob_sha(RAW), 'type': 'blob', 'mode': '100644'},
            {'path': 'README.md', 'sha': 'a' * 40, 'type': 'blob', 'mode': '100644'},
            {'path': '../escape.ir', 'sha': 'a' * 40, 'type': 'blob', 'mode': '100644'},
            {'path': 'TVs/link.ir', 'sha': 'a' * 40, 'type': 'blob', 'mode': '120000'},
        ]}).encode()
    catalog = irdb.Catalog(root, fetch)
    assert list(catalog.index()['files']) == ['TVs/Brand/Remote.ir']
    assert len(seen) == 1
    assert catalog.remote('TVs/Brand/Remote.ir') == RAW and len(seen) == 2
    catalog.fetch = fail
    assert catalog.index()['files'] and catalog.remote('TVs/Brand/Remote.ir') == RAW
    assert catalog.index(refresh=True)['files'] and 'Using saved catalog' in catalog.notice
    old_index = (root / 'index.json').read_bytes()
    catalog.fetch = lambda *a, **k: b'{"sha":"' + b'a' * 40 + b'", "truncated":true, "tree":[]}'
    assert catalog.index(refresh=True)['files']
    assert (root / 'index.json').read_bytes() == old_index
    for path in ('../escape.ir', '/tmp/escape.ir', 'TVs//foo.ir', 'bad\nname.ir', 'TVs/./Remote.ir', 'TVs\\Remote.ir'):
        try:
            catalog.remote(path)
            raise AssertionError(path)
        except irdb.CatalogError:
            pass
    (root / 'remotes' / (irdb.blob_sha(RAW) + '.ir')).write_bytes(b'corrupted')
    catalog.fetch = fail
    try:
        catalog.remote('TVs/Brand/Remote.ir')
        raise AssertionError('Corrupt cache accepted')
    except irdb.CatalogError:
        pass
    catalog.fetch = lambda *a, **k: b'wrong download'
    try:
        catalog.remote('TVs/Brand/Remote.ir')
        raise AssertionError('Wrong hash accepted')
    except irdb.CatalogError:
        pass
    for content in (b'', b'Filetype: Other\nVersion: 1\nname: Bad', b'Filetype: IR signals file\nVersion: 2\nname: Bad'):
        try:
            irdb.Catalog.validate_remote(content)
            raise AssertionError('Invalid header accepted')
        except irdb.CatalogError:
            pass
    print('IRDB fetch/cache, offline fallback, truncated indexes, path safety, hashes and invalid file checks passed.')


class App:
    def __init__(self, binary, env):
        self.process = subprocess.Popen([str(binary)], stdin=subprocess.PIPE, stdout=subprocess.PIPE,
                                        stderr=subprocess.STDOUT, env=env)
        self.buffer = b''
        self.history = b''

    def send(self, keys):
        self.process.stdin.write(keys)
        self.process.stdin.flush()

    def until(self, expected, timeout=8):
        deadline = time.monotonic() + timeout
        while expected not in self.buffer:
            if time.monotonic() > deadline:
                raise AssertionError(f'Timeout waiting for {expected!r}: {self.history[-3000:]!r}')
            ready, _, _ = select.select([self.process.stdout], [], [], .1)
            if ready:
                chunk = os.read(self.process.stdout.fileno(), 65536)
                if not chunk:
                    raise AssertionError(f'App exited: {self.history[-3000:]!r}')
                self.buffer += chunk
                self.history += chunk
        end = self.buffer.index(expected) + len(expected)
        result, self.buffer = self.buffer[:end], self.buffer[end:]
        return result

    def close(self):
        if self.process.poll() is None:
            # Esc now goes back; this test only terminates its own probe.
            # The timed physical hold is covered by check-escape.py.
            self.process.terminate()
            output, _ = self.process.communicate(timeout=3)
            self.history += output
            assert self.process.returncode == -signal.SIGTERM
        else:
            assert self.process.returncode == 0


def app_checks(binary, root):
    cache = root / 'cache'
    seed(cache)
    env = {k: v for k, v in os.environ.items() if not k.startswith('ULTIMATE_REMOTE_')}
    env.update({'ULTIMATE_REMOTE_IRDB_CACHE': str(cache),
                'ULTIMATE_REMOTE_IRDB_HELPER': str(ROOT / 'src/Platform/Linux/irdb-client.py'),
                'ULTIMATE_REMOTE_INPUT_DEVICE': '/nonexistent-irdb-test',
                'ULTIMATE_REMOTE_IRDB_DOWNLOAD_DIR': str(root / 'downloaded'),
                'XDG_CONFIG_HOME': str(root / 'config'),
                'ULTIMATE_REMOTE_IR_STUB': '1', 'ULTIMATE_REMOTE_STATE_DIR': str(root / 'state')})
    app = App(binary, env)
    try:
        app.until(b'== Ultimate Remote ==')
        app.send(b'ccc\n')
        app.until(b'== < Online IR ==')
        app.send(b'x\n')
        app.until(b'Refresh online catalog - Sync')
        assert b'[IR stub] raw sequence' not in app.history
        app.send(b'x\n')
        app.until(b'== < TVs [search] ==')
        app.send(b'\n')
        app.until(b'== < TVs/Samsung [search] ==')
        # Fn-equivalent terminal override types the four navigation letters.
        app.send(b'\x1bf\x1bz\x1bx\x1bc')
        app.until(b'== < Find: fzxc [search] ==')
        app.send(b'\n')
        app.until(b'== < fzxc raw ==')
        assert b'[IR stub] raw sequence' not in app.history
        app.send(b'\n')
        app.until(b'raw sequence, pulses=5, frequency=38000Hz')
        app.send(b'z')
        app.until(b'== < Find: fzxc [search] ==')
        # Reopening the same cached remote works without network access.
        app.send(b'\n')
        app.until(b'== < fzxc raw ==')
        app.send(b'zzzzz')
        app.until(b'== Ultimate Remote ==')
        # Leaving the browser restores Z as left in the main carousel.
        app.send(b'z')
        app.until(b'> FAVORITES')
    finally:
        app.close()

    # A failed request returns a useful error and can return to the main menu.
    helper = root / 'offline.py'
    helper.write_text('import sys\nprint("ERROR\\tCannot connect. Check Wi-Fi and retry")\nsys.exit(1)\n')
    env['ULTIMATE_REMOTE_IRDB_HELPER'] = str(helper)
    app = App(binary, env)
    try:
        app.until(b'== Ultimate Remote ==')
        app.send(b'ccc\nx\n')
        app.until(b'Cannot connect. Check Wi-Fi and retry')
        app.send(b'\n')
        app.until(b'== < Online IR ==')
        app.send(b'z')
        app.until(b'== Ultimate Remote ==')
    finally:
        app.close()

    helper.write_text('import time\ntime.sleep(30)\n')
    app = App(binary, env)
    try:
        app.until(b'== Ultimate Remote ==')
        app.send(b'ccc\nx\n')
        app.until(b'Loading the catalog...')
        app.send(b'z')
        app.until(b'== < Online IR ==', timeout=2)
        app.send(b'z')
        app.until(b'== Ultimate Remote ==', timeout=2)
    finally:
        app.close()
    print('Native category/brand/remote/command navigation, Fn filtering, raw send, cached reopen, network errors and cancellation passed.')

    # Real evdev press/release/repeat records, using Downloaded to avoid timing
    # dependencies on a helper process. Fn+Z types; releasing Fn restores Back.
    downloaded = root / 'downloaded/TVs/Samsung/fzxc_raw.ir'
    downloaded.parent.mkdir(parents=True, exist_ok=True)
    downloaded.write_bytes(RAW)
    key = dict(esc=1, backspace=14, enter=28, tab=15, f=33, z=44, x=45, c=46, fn=464)
    def event(name, value):
        return struct.pack('@llHHi', 0, 0, 1, key[name], value)
    def taps(*names):
        return b''.join(event(name, 1) + event(name, 0) for name in names)
    device = root / 'input-events'
    enter_files = taps('c', 'c', 'c', 'enter', 'enter', 'enter', 'enter')
    typed = (event('fn', 1) + taps('z', 'z') +
             event('fn', 0) + taps('backspace'))
    # F/X navigate without changing the filter; Z returns from commands and
    # each folder, Esc remains available, and Z then moves the carousel left.
    device.write_bytes(enter_files + typed + taps('x', 'f', 'enter', 'z', 'z', 'esc', 'z', 'z', 'z'))
    app = App(binary, {**env, 'ULTIMATE_REMOTE_INPUT_DEVICE': str(device)})
    try:
        app.until(b'== < fzxc raw ==')
        app.until(b'== Ultimate Remote ==')
        app.until(b'> FAVORITES')
    finally:
        app.close()
    assert b'== < Find: zz [search] ==' in app.history, app.history
    assert app.history.count(b'== < Find: z [search] ==') == 5, app.history
    assert b'== < fzxc raw ==' in app.history
    assert app.history.count(b'== Ultimate Remote ==') == 2
    assert b'raw sequence' not in app.history
    print('Evdev Fn+Z typing, Fn release, F/X movement, Z/Esc Back and restored carousel navigation passed.')


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--native-binary', type=Path)
    args = parser.parse_args()
    with tempfile.TemporaryDirectory(prefix='ultimate-remote-irdb-test-') as folder:
        root = Path(folder)
        client_checks(root / 'client')
        if args.native_binary:
            app_checks(args.native_binary.resolve(), root / 'app')


if __name__ == '__main__':
    main()
