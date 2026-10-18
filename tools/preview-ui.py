#!/usr/bin/env python3
"""Render actual framebuffer screens and compare 16/24/32-bit output. Requires Pillow."""
import argparse
from pathlib import Path
import subprocess
import tempfile
from PIL import Image, ImageChops, ImageDraw, ImageFont, ImageStat

root = Path(__file__).resolve().parents[1]
parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument("--output-dir", type=Path, default=root / "dist/ui-preview")
parser.add_argument("--sanitize", action="store_true")
args = parser.parse_args()
args.output_dir.mkdir(parents=True, exist_ok=True)
with tempfile.TemporaryDirectory(prefix="ultimate-remote-preview-") as temporary:
    binary = Path(temporary) / "preview"
    command = ["g++", "-std=c++17", "-Wall", "-Wextra", "-Wpedantic", "-g", "-Isrc",
               "tools/preview-ui.cpp", "src/Platform/Linux/LinuxFramebufferView.cpp",
               "src/Contexts/GlobalContext.cpp", "src/Repositories/ManufacturerRepository.cpp",
               "-o", str(binary)]
    if args.sanitize:
        command += ["-fsanitize=address,undefined", "-fno-omit-frame-pointer"]
    subprocess.run(command, cwd=root, check=True)
    subprocess.run([str(binary), str(args.output_dir.resolve())], check=True)

for screenshot in sorted(args.output_dir.glob("*-32.ppm")):
    full = Image.open(screenshot).convert("RGB")
    rgb24 = Image.open(screenshot.with_name(screenshot.name.replace("-32.ppm", "-24.ppm")))
    rgb16 = Image.open(screenshot.with_name(screenshot.name.replace("-32.ppm", "-16.ppm")))
    assert full.size == (320, 170)
    assert ImageChops.difference(full, rgb24).getbbox() is None, screenshot.name
    assert max(ImageStat.Stat(ImageChops.difference(full, rgb16)).mean) < 6, screenshot.name
    assert len(full.getcolors(320 * 170)) > 20, screenshot.name
    full.save(screenshot.with_name(screenshot.name.replace("-32.ppm", ".png")))

screens = [("home-0", "Remotes"), ("home-1", "Scan"), ("home-2", "Favorites"),
           ("home-3", "Online IR"), ("home-4", "Settings"), ("home-5", "Info"),
           ("info-0", "Embedded database"), ("info-1", "Online database"),
           ("info-2", "Download once"), ("info-3", "Find & save"),
           ("exit-hold", "Hold Esc to exit"), ("exit-cancelled", "Release to cancel"),
           ("manufacturers", "Manufacturers"), ("remotes", "Remote profiles"),
           ("commands", "Commands"), ("command-directions", "Fn directions"),
           ("command-playback", "Fn playback"), ("scan", "Scanning"), ("favorite", "Save a favorite"),
           ("irdb-categories", "Flipper-IRDB categories"), ("irdb-remotes", "Online remotes"), ("irdb-download", "Download")]
sheet = Image.new("RGB", (1024, 10 + ((len(screens) + 2) // 3) * 216), "#080d14")
draw = ImageDraw.Draw(sheet)
font = ImageFont.truetype("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 13)
for index, (name, title) in enumerate(screens):
    x, y = 16 + (index % 3) * 336, 12 + (index // 3) * 216
    draw.text((x, y), title, fill="#95a6bb", font=font)
    sheet.paste(Image.open(args.output_dir / f"{name}.png"), (x, y + 24))
sheet.save(args.output_dir / "overview.png")
info_sheet = Image.new("RGB", (688, 442), "#080d14")
info_draw = ImageDraw.Draw(info_sheet)
for index in range(4):
    x, y = 16 + (index % 2) * 336, 12 + (index // 2) * 216
    info_draw.text((x, y), f"Info {index + 1}/4", fill="#95a6bb", font=font)
    info_sheet.paste(Image.open(args.output_dir / f"info-{index}.png"), (x, y + 24))
info_sheet.save(args.output_dir / "info-overview.png")
print(f"Preview and pixel-format checks passed: {args.output_dir / 'overview.png'}")
