#!/usr/bin/env python3
"""Build the self-contained arm64 package without root or network access."""

import argparse
from datetime import datetime, timezone
from email.utils import format_datetime
import gzip
import hashlib
import os
from pathlib import Path
import platform
import re
import shutil
import subprocess
import tarfile
import tempfile

ROOT = Path(__file__).resolve().parents[1]
PACKAGE = "ultimate-remote"
APP_VERSION = re.search(
    r'^#define ULTIMATE_REMOTE_VERSION "([0-9]+\.[0-9]+(?:\.[0-9]+)?)"$',
    (ROOT / "src/Data/AppVersion.h").read_text(), re.MULTILINE,
).group(1)


def run(*args, **kwargs):
    return subprocess.run([str(arg) for arg in args], check=True, **kwargs)


def output(*args):
    return run(*args, capture_output=True, text=True).stdout.strip()


def install(source, destination, mode=0o644):
    destination.parent.mkdir(parents=True, exist_ok=True)
    shutil.copyfile(source, destination)
    destination.chmod(mode)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--version", default=APP_VERSION,
                        help="Debian package version (defaults to the shared application version)")
    parser.add_argument("--maintainer", default="Geo <tp53.geo@gmail.com>")
    parser.add_argument("--cxx", default=os.environ.get("CXX"))
    parser.add_argument("--jobs", type=int, default=2)
    parser.add_argument("--output-dir", type=Path, default=ROOT / "dist")
    args = parser.parse_args()
    if args.jobs < 1:
        parser.error("--jobs must be positive")
    run("dpkg", "--validate-version", args.version)
    if any(char in args.version for char in "/\\\n\r"):
        parser.error("invalid version")
    if "\n" in args.maintainer or "\r" in args.maintainer:
        parser.error("--maintainer must be a single line")

    compiler = args.cxx or ("g++" if platform.machine() == "aarch64" else "aarch64-linux-gnu-g++")
    if not output(compiler, "-dumpmachine").startswith("aarch64"):
        parser.error("the compiler must target aarch64 Linux")
    compiler_path = shutil.which(compiler)
    if not compiler_path:
        parser.error(f"compiler not found: {compiler}")
    build = ROOT / "build-deb" / "arm64"
    run("cmake", "-S", ROOT, "-B", build,
        "-DCMAKE_BUILD_TYPE=Release", f"-DCMAKE_CXX_COMPILER={compiler_path}",
        "-DCMAKE_SYSTEM_NAME=Linux", "-DCMAKE_SYSTEM_PROCESSOR=aarch64",
        "-DCMAKE_CXX_FLAGS=-march=armv8-a -mtune=cortex-a53",
        "-DULTIMATE_REMOTE_STATIC=ON", "-DULTIMATE_REMOTE_IR_TX_PIN=12")
    run("cmake", "--build", build, "--parallel", args.jobs)
    binary = build / PACKAGE
    elf = binary.read_bytes()[:20]
    if elf[:6] != b"\x7fELF\x02\x01" or int.from_bytes(elf[18:20], "little") != 183:
        raise RuntimeError("the generated executable is not little-endian ELF64 AArch64")
    readelf = output(compiler, "-print-prog-name=readelf")
    if "INTERP" in output(readelf, "-lW", binary) or "NEEDED" in output(readelf, "-dW", binary):
        raise RuntimeError("the package requires a fully static executable")

    args.output_dir.mkdir(parents=True, exist_ok=True)
    deb = args.output_dir.resolve() / f"{PACKAGE}_{args.version}_arm64.deb"
    with tempfile.TemporaryDirectory(prefix="stage-", dir=build.parent) as temporary:
        stage = Path(temporary)
        doc = stage / "usr/share/doc" / PACKAGE
        app = stage / "usr/share/APPLaunch"
        installed_binary = stage / "usr/lib" / PACKAGE / PACKAGE
        install(binary, installed_binary, 0o755)
        install(ROOT / "src/Platform/Linux/irdb-client.py", stage / "usr/lib/ultimate-remote/irdb-client.py")
        run(output(compiler, "-print-prog-name=strip"), "--strip-unneeded", installed_binary)
        install(ROOT / "packaging/applaunch/ultimate-remote", app / "bin/ultimate-remote", 0o755)
        install(ROOT / "packaging/applaunch/ultimate-remote.desktop", app / "applications/ultimate-remote.desktop")
        install(ROOT / "packaging/applaunch/ultimate-remote.png", app / "share/images/ultimate-remote.png")
        install(ROOT / "packaging/ultimate-remote.default", stage / "etc/default/ultimate-remote")
        install(ROOT / "packaging/70-ultimate-remote.rules", stage / "usr/lib/udev/rules.d/70-ultimate-remote.rules")
        install(ROOT / "README.md", doc / "README.md")
        install(ROOT / "src/Platform/Linux/Assets/UiFont.LICENSE", doc / "font-copyright")
        install(ROOT / "packaging/gpio-ir-tx.conf", doc / "gpio-ir-tx.conf")
        install(ROOT / "LICENSE", doc / "copyright")

        # Preserve library notices for the statically linked GNU runtimes.
        compiler_major = output(compiler, "-dumpversion").split(".")[0]
        for package, name in [("libc6", "glibc-copyright"), (f"gcc-{compiler_major}-base", "gcc-copyright")]:
            notice = Path("/usr/share/doc") / package / "copyright"
            if not notice.is_file():
                raise RuntimeError(f"missing runtime copyright notice: {notice}")
            install(notice, doc / name)

        # Ship the exact application sources and objects to allow rebuilding or
        # relinking the static executable with a replacement C library.
        with (doc / "source-and-objects.tar.gz").open("wb") as source_file:
            with gzip.GzipFile(fileobj=source_file, mode="wb", filename="", mtime=0) as compressed:
                with tarfile.open(fileobj=compressed, mode="w|") as archive:
                    for name in ["src", "packaging", "CMakeLists.txt", "Makefile", "README.md", "LICENSE"]:
                        archive.add(ROOT / name, arcname=f"ultimate-remote/{name}")
                    for path in sorted((build / "CMakeFiles/ultimate-remote.dir").rglob("*.o")):
                        archive.add(path, arcname="ultimate-remote/objects/" + path.relative_to(build / "CMakeFiles/ultimate-remote.dir").as_posix())
        (doc / "BUILD-INFO").write_text(
            f"Application version: {APP_VERSION}\n"
            f"Package version: {args.version}\n"
            f"Compiler: {output(compiler, '--version').splitlines()[0]}\n"
            "Target: aarch64 Linux, ARMv8-A / Cortex-A53\n"
            "Link: -static; IR TX default: GPIO12\n"
            "Rebuild: python3 packaging/build-deb.py\n"
            "Relink extracted objects: find objects -name '*.o' -print0 | "
            "xargs -0 aarch64-linux-gnu-g++ -static -o ultimate-remote\n"
            "GNU runtime sources: https://gcc.gnu.org/ and https://sourceware.org/glibc/\n"
        )

        man = stage / "usr/share/man/man1/ultimate-remote.1.gz"
        man.parent.mkdir(parents=True)
        manual = (ROOT / "packaging/ultimate-remote.1").read_text().replace("@APP_VERSION@", APP_VERSION)
        man.write_bytes(gzip.compress(manual.encode(), mtime=0))
        release_date = format_datetime(datetime.now(timezone.utc))
        changelog = (
            f"{PACKAGE} ({args.version}) unstable; urgency=medium\n\n"
            "  * Share the application version between Linux and ESP32 releases.\n"
            "  * Record the application version in firmware and package metadata.\n"
            "  * Include the reorganized application and cleaned packaging resources.\n\n"
            f" -- {args.maintainer}  {release_date}\n"
        )
        (doc / "changelog.Debian.gz").write_bytes(gzip.compress(changelog.encode(), mtime=0))
        overrides = stage / "usr/share/lintian/overrides/ultimate-remote"
        overrides.parent.mkdir(parents=True)
        overrides.write_text(
            "# The appliance package deliberately bundles the GNU runtimes to run\n"
            "# on Cardputer Zero images older than the cross-build host. Exact\n"
            "# application sources, relinkable objects and notices are included.\n"
            "ultimate-remote: statically-linked-binary\n"
        )
        (stage / "usr/bin").mkdir(parents=True)
        (stage / "usr/bin/ultimate-remote").symlink_to("../share/APPLaunch/bin/ultimate-remote")

        control = stage / "DEBIAN"
        control.mkdir()
        size_kib = (sum(path.stat().st_size for path in stage.rglob("*") if path.is_file() and not path.is_symlink()) + 1023) // 1024
        (control / "control").write_text(
            f"Package: {PACKAGE}\nVersion: {args.version}\nArchitecture: arm64\n"
            "Section: utils\nPriority: optional\n"
            f"Maintainer: {args.maintainer}\n"
            f"Installed-Size: {size_kib}\n"
            "Homepage: https://github.com/geo-tp/Ultimate-Remote\n"
            "Depends: python3 (>= 3.9), ca-certificates\n"
            "Recommends: udev\nSuggests: v4l-utils\n"
            "Description: Universal infrared remote for Cardputer Zero\n"
            " Framebuffer application with an embedded remote database, favorites,\n"
            " Flipper-IRDB browsing and Linux LIRC transmission on GPIO12 by default.\n"
            " Includes an APPLaunch entry and a self-contained ARM64 executable.\n"
        )
        (control / "conffiles").write_text("/etc/default/ultimate-remote\n")
        checksums = []
        for path in sorted(stage.rglob("*")):
            relative = path.relative_to(stage)
            if path.is_file() and not path.is_symlink() and relative.parts[0] != "DEBIAN":
                checksums.append(f"{hashlib.md5(path.read_bytes()).hexdigest()}  {relative.as_posix()}\n")
        (control / "md5sums").write_text("".join(checksums))
        # Explicit permissions also make builds work under a restrictive umask.
        for directory in [stage, *(path for path in stage.rglob("*") if path.is_dir())]:
            directory.chmod(0o755)
        for path in stage.rglob("*"):
            if path.is_file() and not path.is_symlink():
                path.chmod(0o755 if path.stat().st_mode & 0o111 else 0o644)
        run("dpkg-deb", "--root-owner-group", "-Zxz", "--build", stage, deb)

    digest = hashlib.sha256(deb.read_bytes()).hexdigest()
    deb.with_suffix(deb.suffix + ".sha256").write_text(f"{digest}  {deb.name}\n")
    print(f"\nPackage ready: {deb}")


if __name__ == "__main__":
    main()
