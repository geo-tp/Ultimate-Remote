#!/usr/bin/env python3
"""Inspect a built package and optionally exercise a native build without hardware."""
import argparse
import configparser
import hashlib
import io
import importlib.util
import json
import os
from pathlib import Path
import shlex
import struct
import subprocess
import sys
import tarfile
import tempfile


def run(*args, **kwargs):
    return subprocess.run([str(arg) for arg in args], check=True, capture_output=True, **kwargs)


parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument("deb", type=Path)
parser.add_argument("--native-binary", type=Path)
args = parser.parse_args()
assert run("dpkg-deb", "-f", args.deb, "Architecture").stdout.strip() == b"arm64"
assert run("dpkg-deb", "-f", args.deb, "Package").stdout.strip() == b"ultimate-remote"
with tarfile.open(fileobj=io.BytesIO(run("dpkg-deb", "--fsys-tarfile", args.deb).stdout)) as archive:
    for entry in archive:
        assert (entry.uid, entry.gid) == (0, 0), entry.name
        assert entry.issym() or not (entry.mode & 0o022), entry.name
        assert "libretro" not in entry.name, entry.name

with tempfile.TemporaryDirectory(prefix="ultimate-remote-check-") as temporary:
    base = Path(temporary)
    stage = base / "package"
    run("dpkg-deb", "-R", args.deb, stage)
    for line in (stage / "DEBIAN/md5sums").read_text().splitlines():
        digest, name = line.split("  ", 1)
        assert hashlib.md5((stage / name).read_bytes()).hexdigest() == digest, name
    assert (stage / "DEBIAN/conffiles").read_text().strip() == "/etc/default/ultimate-remote"
    binary = stage / "usr/lib/ultimate-remote/ultimate-remote"
    elf = binary.read_bytes()[:20]
    assert elf[:6] == b"\x7fELF\x02\x01" and int.from_bytes(elf[18:20], "little") == 183
    assert b"INTERP" not in run("readelf", "-lW", binary).stdout
    assert b"NEEDED" not in run("readelf", "-dW", binary).stdout
    assert os.access(binary, os.X_OK)
    assert (stage / "usr/lib/ultimate-remote/irdb-client.py").is_file()
    dependencies = run("dpkg-deb", "-f", args.deb, "Depends").stdout
    assert b"python3" in dependencies and b"ca-certificates" in dependencies

    app = stage / "usr/share/APPLaunch"
    desktop = configparser.ConfigParser(interpolation=None)
    desktop.read(app / "applications/ultimate-remote.desktop")
    entry = desktop["Desktop Entry"]
    launcher = app / entry["Exec"]
    assert os.access(launcher, os.X_OK)
    icon = (app / entry["Icon"]).read_bytes()
    assert icon[:8] == b"\x89PNG\r\n\x1a\n" and struct.unpack(">II", icon[16:24]) == (256, 256)
    assert (stage / "usr/bin/ultimate-remote").resolve() == launcher

    # Substitute only installation paths; exercise the real packaged wrapper.
    probe = base / "probe"
    probe.write_text('#!/usr/bin/env python3\nimport json, os, sys\nprint(json.dumps({"env": dict(os.environ), "args": sys.argv[1:]}))\n')
    probe.chmod(0o755)
    wrapper = base / "launcher"
    text = launcher.read_text().replace("/etc/default/ultimate-remote", str(stage / "etc/default/ultimate-remote"))
    wrapper.write_text(text.replace("/usr/lib/ultimate-remote/ultimate-remote", shlex.quote(str(probe))))
    wrapper.chmod(0o755)
    env = {key: value for key, value in os.environ.items() if not key.startswith("ULTIMATE_REMOTE_")}
    env.update({"XDG_CONFIG_HOME": str(base / "config"), "XDG_STATE_HOME": str(base / "state"),
                "XDG_DATA_HOME": str(base / "data"), "ULTIMATE_REMOTE_INPUT_DEVICE": "/nonexistent-ultimate-remote-input"})
    result = json.loads(run(wrapper, "--input-device", "/path with spaces", env=env).stdout)
    assert result["env"]["ULTIMATE_REMOTE_IR_TX_PIN"] == "12"
    assert result["args"] == ["--input-device", "/path with spaces"]
    state = base / "state/ultimate-remote"
    remotes = base / "data/ultimate-remote/remotes"
    assert state.is_dir() and remotes.is_dir()
    favorites = state / "favoriteRemotes.txt"
    favorites.write_text("keep favorites\n")
    env["ULTIMATE_REMOTE_IR_TX_PIN"] = "23"
    assert json.loads(run(wrapper, env=env).stdout)["env"]["ULTIMATE_REMOTE_IR_TX_PIN"] == "23"
    config = base / "config/ultimate-remote/config"
    config.parent.mkdir(parents=True)
    config.write_text('ULTIMATE_REMOTE_IR_TX_PIN=12\nULTIMATE_REMOTE_IR_STUB=1\nULTIMATE_REMOTE_INPUT_DEBUG=1\n')
    configured = json.loads(run(wrapper, env=env).stdout)["env"]
    assert configured["ULTIMATE_REMOTE_IR_TX_PIN"] == "12"
    assert configured["ULTIMATE_REMOTE_IR_STUB"] == "1"
    assert configured["ULTIMATE_REMOTE_INPUT_DEBUG"] == "1"
    assert favorites.read_text() == "keep favorites\n"

    if args.native_binary:
        wrapper.write_text(text.replace("/usr/lib/ultimate-remote/ultimate-remote", shlex.quote(str(args.native_binary.resolve()))))
        assert b"Usage:" in run(wrapper, "--help", env=env).stdout
        ir = run(wrapper, "--ir-test", env=env)
        assert b"tx pin 12" in ir.stderr and b"sent NEC test frame" in ir.stderr
        # Use live key press/release records: Esc no longer terminates a probe.
        spec = importlib.util.spec_from_file_location('probe', Path(__file__).with_name('native-app-test.py'))
        probe = importlib.util.module_from_spec(spec)
        spec.loader.exec_module(probe)
        app = probe.App(args.native_binary.resolve(), base / 'native')
        try:
            app.tap('enter', 's', 'o', 'n', 'y', 'enter')
            app.until(b'== < Sony [search] ==')
            app.until(b'84,-1.csv - TV')
            assert b'== < Remote ' not in app.history
            assert b'raw sequence' not in app.history
            app.tap('x', 'enter')
            app.until(b'== < Remote 84,-1.csv - TV ==')
            app.tap('x', 'x', 'x', 'enter')
            app.until(b'raw sequence')
            app.until(b'== < Remote 84,-1.csv - TV ==')
            app.tap('esc')
            app.until(b'== < Sony [search] ==')
            app.until(b'> 84,-1.csv - TV')
            # A favorite from another product category retains its association.
            app.tap('f', *(['x'] * 12))
            app.until(b'> 58,9.csv - DVD Player')
            app.tap('space', 'f', 'z', 'x', 'c', 'a', 'enter')
            app.until(b'== < Sony [search] ==')
            saved = (base / 'native/state/favoriteRemotes.txt').read_text()
            assert 'Sony|DVD Player|58,9.csv|fzxca' in saved, saved
            app.tap('esc', 'esc')
            app.until(b'== Ultimate Remote ==')
            app.tap('enter')
            app.until(b'== < Type to filter [search] ==')
            app.press('fn')
            app.tap('f', 'z', 'x', 'c', 'a')
            app.release('fn')
            app.until(b'== < fzxca [search] ==')
            app.tap(*(['backspace'] * 5), 's')
            app.until(b'== < s [search] ==')
            # Base letters and physical arrows navigate without editing text.
            for down, up in (('x', 'f'), ('down', 'up')):
                app.tap(down)
                app.until(b'== < s [search] ==')
                app.tap(up)
                app.until(b'== < s [search] ==')
            app.tap('esc')
            app.until(b'== Ultimate Remote ==')
        finally:
            app.close()

        for name in ('check-info.py', 'check-irdb.py', 'check-escape.py', 'check-shortcuts.py'):
            for extra in ((), ('--cardputer-events',)) if name == 'check-shortcuts.py' else ((),):
                result = run(sys.executable, Path(__file__).with_name(name),
                             '--native-binary', args.native_binary.resolve(), *extra, timeout=90,
                             env={**os.environ, 'PYTHONDONTWRITEBYTECODE': '1'})
                print(result.stdout.decode(), end='')

print("Package structure, ownership, checksums, ARM64 linkage and launcher checks passed.")
if args.native_binary:
    print("Native startup, GPIO12, manufacturer/remote/command navigation and favorite checks passed.")
    print("Fn filtering, letter/arrow navigation, favorite naming and timed Esc holds passed.")
