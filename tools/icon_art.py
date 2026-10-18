"""Code-defined icon geometry for the framebuffer UI."""
import math
from PIL import Image, ImageDraw, ImageChops

ICON_NAMES = ("Remote", "Scan", "Star", "Folder", "Search", "Back", "Check", "Info", "Settings")
ICON_SIZES = (15, 18, 20, 24, 25, 28, 33, 34)
SAMPLES = 8


class Canvas:
    def __init__(self, size):
        self.size = size
        self.scale = size * SAMPLES / 64
        self.image = Image.new("L", (size * SAMPLES, size * SAMPLES))
        self.draw = ImageDraw.Draw(self.image)
        self.stroke = max(3.2, 1.15 * 64 / size)

    def box(self, coords):
        return tuple(round(value * self.scale) for value in coords)

    def disk(self, x, y, radius, color=255):
        self.draw.ellipse(self.box((x - radius, y - radius, x + radius, y + radius)), fill=color)

    def path(self, points, stroke=None):
        stroke = self.stroke if stroke is None else stroke
        self.draw.line([self.box(point) for point in points], fill=255,
                       width=round(stroke * self.scale), joint="curve")
        for x, y in points:
            self.disk(x, y, stroke / 2)

    def curve(self, start, control, end):
        points = []
        for step in range(49):
            t = step / 48
            points.append(tuple((1 - t) ** 2 * start[i] + 2 * (1 - t) * t * control[i] +
                                t * t * end[i] for i in (0, 1)))
        self.path(points)

    def ring(self, x, y, radius):
        self.disk(x, y, radius + self.stroke / 2)
        self.disk(x, y, radius - self.stroke / 2, 0)

    def rounded(self, coords, radius, filled=False):
        self.draw.rounded_rectangle(self.box(coords), round(radius * self.scale),
                                    fill=255 if filled else None, outline=255,
                                    width=round(self.stroke * self.scale))

    def finish(self):
        # Exact pixel coverage avoids ringing/halos from sharpening filters.
        return self.image.resize((self.size, self.size), Image.Resampling.BOX)


def remote_layers(size, filled=False):
    body, controls, waves = Canvas(size), Canvas(size), Canvas(size)
    body.rounded((18, 21, 46, 61), 7, filled)
    controls.disk(32, 29, 2.1)
    controls.ring(32, 42, 6.2)
    if size > 20:
        controls.disk(32, 42, 1.4)
        controls.rounded((25, 53, 29, 56), 1.5, True)
        controls.rounded((35, 53, 39, 56), 1.5, True)
    waves.curve((20, 10), (32, -1), (44, 10))
    waves.curve((26, 15), (32, 10), (38, 15))
    return body, controls, waves


def icon_mask(name, size):
    canvas = Canvas(size)
    if name == "Remote":
        body, controls, waves = remote_layers(size)
        canvas.image = ImageChops.lighter(ImageChops.lighter(body.image, controls.image), waves.image)
    elif name == "Scan":
        canvas.ring(32, 32, 25)
        canvas.ring(32, 32, 14)
        canvas.path([(32, 32), (49, 13)])
        canvas.disk(21, 36, 3)
    elif name == "Star":
        points = []
        for i in range(10):
            angle = -math.pi / 2 + i * math.pi / 5
            radius = 26 if i % 2 == 0 else 12
            points.append((32 + math.cos(angle) * radius, 33 + math.sin(angle) * radius))
        canvas.path(points + [points[0]])
    elif name == "Folder":
        canvas.path([(8, 25), (8, 16), (25, 16), (31, 22), (55, 22), (55, 49), (8, 49), (8, 25), (55, 25)])
    elif name == "Search":
        canvas.ring(27, 27, 17)
        canvas.path([(40, 40), (54, 54)])
    elif name == "Back":
        canvas.path([(40, 13), (21, 32), (40, 51)])
    elif name == "Check":
        canvas.ring(32, 32, 25)
        canvas.path([(20, 32), (28, 40), (45, 23)])
    elif name == "Info":
        canvas.ring(32, 32, 25)
        canvas.disk(32, 21, 2.4)
        canvas.path([(32, 31), (32, 44)])
    elif name == "Settings":
        # Eight broad teeth with rounded joins remain readable at carousel size.
        points = []
        for i in range(32):
            angle = math.radians(i * 11.25 - 16.875)
            radius = 26 if i % 4 in (1, 2) else 20
            points.append((32 + radius * math.cos(angle), 32 + radius * math.sin(angle)))
        canvas.path(points + [points[0]])
        canvas.ring(32, 32, 9)
    else:
        raise ValueError(name)
    return canvas.finish()
