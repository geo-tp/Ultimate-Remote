#ifdef ARDUINO
#include "CardputerHomeCarousel.h"

#include <M5Cardputer.h>
#include <Platform/Arduino/CardputerView.h>
#include <Contexts/GlobalContext.h>
#include <Enums/SelectionModeEnum.h>
#include <algorithm>
#include <cctype>

namespace platform::arduino {
namespace {

// Reuse the original Cardputer palette from normal selections/dialogs.
// The ESP32 home should not abruptly switch to the Linux mint/blue theme.
constexpr uint16_t Background = BACKGROUND_COLOR;
constexpr uint16_t Panel      = RECT_COLOR_DARK;
// A slightly brighter fill distinguishes the active card without an orange frame.
constexpr uint16_t Selected   = 0x18c3;
constexpr uint16_t Border     = PRIMARY_COLOR;
constexpr uint16_t Primary    = PRIMARY_COLOR;
constexpr uint16_t Text       = TEXT_COLOR;
constexpr uint16_t Muted      = RECT_COLOR_LIGHT;

bool carouselValid = false;
int lastWidth = 0;
int lastHeight = 0;

enum class Symbol { Remote, Scanner, Star, Folder, Builder, Settings, Info };

struct ModeAppearance { Symbol icon; uint16_t accent; };

// Individual icon accents add visual distinction. Orange remains the
// primary color elsewhere; carousel page indicators are neutral white/gray.
constexpr uint16_t rgb565(uint8_t r, uint8_t g, uint8_t b) {
    return static_cast<uint16_t>(((r & 0xf8) << 8) | ((g & 0xfc) << 3) | (b >> 3));
}

ModeAppearance appearanceFor(size_t index) {
    using enums::SelectionModeEnum;
    switch (static_cast<SelectionModeEnum>(index)) {
        case SelectionModeEnum::SCAN:       return {Symbol::Scanner, rgb565(55, 190, 210)};
        case SelectionModeEnum::FAVORITES:  return {Symbol::Star, rgb565(245, 205, 85)};
        case SelectionModeEnum::FILES:      return {Symbol::Folder, rgb565(105, 160, 250)};
        case SelectionModeEnum::IR_BUILDER: return {Symbol::Builder, rgb565(118, 212, 120)};
        case SelectionModeEnum::SETTINGS:   return {Symbol::Settings, rgb565(185, 145, 236)};
        case SelectionModeEnum::INFO:       return {Symbol::Info, rgb565(125, 200, 220)};
        default:                            return {Symbol::Remote, Primary};
    }
}

// Preserve acronyms and non-ASCII bytes instead of guessing a character width.
std::string displayTitle(std::string title) {
    bool startOfWord = true;
    for (char& ch : title) {
        const unsigned char c = static_cast<unsigned char>(ch);
        if (ch == ' ' || ch == '-' || ch == '/') {
            startOfWord = true;
            continue;
        }
        if (c >= 'A' && c <= 'Z' && !startOfWord) ch = static_cast<char>(c - 'A' + 'a');
        startOfWord = false;
    }
    // IR is a well-known acronym, never "Ir".
    for (size_t pos = 0; (pos = title.find("Ir", pos)) != std::string::npos; pos += 2) {
        const bool before = pos == 0 || !std::isalnum(static_cast<unsigned char>(title[pos - 1]));
        const bool after = pos + 2 == title.size() || !std::isalnum(static_cast<unsigned char>(title[pos + 2]));
        if (before && after) title.replace(pos, 2, "IR");
    }
    return title;
}

// Width is measured with the actual M5GFX font, not with a fixed number
// of characters. Text that cannot fit ends with ASCII "...".
std::string fitText(lgfx::LGFXBase& gfx, std::string value, int availablePixels) {
    if (availablePixels <= 0) return "";
    if (gfx.textWidth(value.c_str()) <= availablePixels) return value;
    const char* dots = "...";
    if (gfx.textWidth(dots) > availablePixels) return "";
    while (!value.empty() && gfx.textWidth((value + dots).c_str()) > availablePixels) {
        // Avoid leaving a partial UTF-8 codepoint at the end of the string.
        // Remove the complete last UTF-8 character, not just its final byte.
        unsigned char removed = 0;
        do {
            removed = static_cast<unsigned char>(value.back());
            value.pop_back();
        } while (!value.empty() && (removed & 0xc0) == 0x80);
    }
    return value + dots;
}

void centeredText(lgfx::LGFXBase& gfx, const std::string& value, int x, int y,
                  int maxWidth, float scale, uint16_t color) {
    gfx.setTextSize(scale);
    gfx.setTextColor(color);
    gfx.setTextDatum(middle_center);
    const std::string fitted = fitText(gfx, value, maxWidth);
    if (!fitted.empty()) gfx.drawString(fitted.c_str(), x, y);
}

void icon(lgfx::LGFXBase& gfx, Symbol symbol, int x, int y, int size, uint16_t color) {
    // Draw vector glyphs directly into the shared frame, without per-icon buffers.
    const int s = size;
    const auto px = [&](int v) { return x + v * s / 32; };
    const auto py = [&](int v) { return y + v * s / 32; };
    const auto unit = [&](int v) { return std::max(1, v * s / 32); };

    switch (symbol) {
        case Symbol::Remote:
        case Symbol::Builder:
            gfx.drawRoundRect(px(10), py(2), unit(13), unit(28), unit(3), color);
            gfx.drawRoundRect(px(12), py(5), unit(9), unit(7), unit(1), color);
            gfx.fillCircle(px(16), py(16), unit(2), color);
            gfx.fillCircle(px(13), py(22), unit(1), color);
            gfx.fillCircle(px(19), py(22), unit(1), color);
            gfx.fillCircle(px(16), py(26), unit(1), color);
            if (symbol == Symbol::Builder) {
                gfx.fillCircle(px(25), py(7), unit(7), Panel);
                gfx.drawCircle(px(25), py(7), unit(6), color);
                gfx.drawLine(px(21), py(7), px(29), py(7), color);
                gfx.drawLine(px(25), py(3), px(25), py(11), color);
            }
            break;
        case Symbol::Scanner:
            gfx.drawCircle(px(14), py(14), unit(10), color);
            gfx.drawCircle(px(14), py(14), unit(6), color);
            gfx.drawLine(px(21), py(22), px(29), py(30), color);
            gfx.drawLine(px(13), py(7), px(13), py(12), color);
            gfx.drawLine(px(10), py(10), px(16), py(10), color);
            break;
        case Symbol::Star: {
            const int vx[] = {16, 20, 30, 23, 26, 16, 6, 9, 2, 12};
            const int vy[] = {1, 11, 12, 19, 30, 24, 30, 19, 12, 11};
            for (int i = 0; i < 10; ++i) {
                const int j = (i + 1) % 10;
                gfx.drawLine(px(vx[i]), py(vy[i]), px(vx[j]), py(vy[j]), color);
            }
            gfx.fillCircle(px(16), py(16), unit(2), color);
            break;
        }
        case Symbol::Folder:
            gfx.drawRoundRect(px(2), py(9), unit(28), unit(19), unit(3), color);
            gfx.drawLine(px(4), py(9), px(4), py(6), color);
            gfx.drawLine(px(4), py(6), px(14), py(6), color);
            gfx.drawLine(px(14), py(6), px(18), py(9), color);
            gfx.drawLine(px(7), py(19), px(11), py(19), color);
            gfx.drawLine(px(11), py(19), px(14), py(14), color);
            gfx.drawLine(px(14), py(14), px(18), py(24), color);
            gfx.drawLine(px(18), py(24), px(21), py(19), color);
            gfx.drawLine(px(21), py(19), px(26), py(19), color);
            break;
        case Symbol::Settings:
            gfx.drawCircle(px(16), py(16), unit(9), color);
            gfx.drawCircle(px(16), py(16), unit(4), color);
            for (int a = 0; a < 8; ++a) {
                constexpr int dx[] = {0, 7, 10, 7, 0, -7, -10, -7};
                constexpr int dy[] = {-10, -7, 0, 7, 10, 7, 0, -7};
                gfx.drawLine(px(16 + dx[a]), py(16 + dy[a]),
                             px(16 + dx[a] * 13 / 10), py(16 + dy[a] * 13 / 10), color);
            }
            break;
        case Symbol::Info:
            gfx.drawCircle(px(16), py(16), unit(14), color);
            gfx.fillCircle(px(16), py(9), unit(2), color);
            gfx.drawLine(px(16), py(14), px(16), py(24), color);
            gfx.drawLine(px(13), py(24), px(19), py(24), color);
            break;
    }
}

void card(lgfx::LGFXBase& gfx, const std::vector<std::string>& names, size_t index,
          int x, int y, int w, int h, bool active) {
    const ModeAppearance mode = appearanceFor(index);
    gfx.fillRoundRect(x, y, w, h, active ? 9 : 7, active ? Selected : Panel);

    // Match the Orbitron font used by the file browser and other ESP32
    // selections. Keep smaller scales for the narrow carousel cards.
    gfx.setFont(&fonts::Orbitron_Light_24);
    if (active) {
        constexpr int iconSize = 38;
        icon(gfx, mode.icon, x + (w - iconSize) / 2, y + 9, iconSize, mode.accent);
        // No secondary descriptions on the 240x135 Cardputer display: they
        // are illegible and crowd out the title.
        centeredText(gfx, displayTitle(names[index]), x + w / 2,
                     y + h - 18, w - 12, 0.62f, Text);
    } else {
        constexpr int iconSize = 26;
        icon(gfx, mode.icon, x + (w - iconSize) / 2, y + 9, iconSize, mode.accent);
        centeredText(gfx, displayTitle(names[index]), x + w / 2,
                     y + h - 11, w - 6, 0.40f, Muted);
    }
    // The pin badge and navigation hints keep the compact Font0.
    gfx.setFont(&fonts::Font0);
}

} // namespace

void CardputerHomeCarousel::invalidate() {
    carouselValid = false;
}

void CardputerHomeCarousel::render(const std::vector<std::string>& labels,
                                   uint16_t selectionIndex,
                                   const std::vector<std::string>& descriptions) {
    (void)descriptions; // Used by Linux; intentionally hidden on ESP32.
    auto& screen = M5Cardputer.Display;
    const int width = screen.width();
    const int height = screen.height();

    // Keep one RGB565 frame (64,800 pixel bytes at 240x135) across menu visits
    // and IR use. Redrawing or invalidating the view must not reallocate it.
    static M5Canvas frame(&screen);
    if (!frame.getBuffer() || frame.width() != width || frame.height() != height) {
        frame.deleteSprite();
        frame.setPsram(false);
        frame.setColorDepth(16);
        if (!frame.createSprite(width, height)) {
            // Keep buffered drawing available when only a smaller block is free.
            frame.setColorDepth(8);
            frame.createSprite(width, height);
        }
    }
    const bool buffered = frame.getBuffer() != nullptr;
    lgfx::LGFXBase& gfx = buffered
        ? static_cast<lgfx::LGFXBase&>(frame)
        : static_cast<lgfx::LGFXBase&>(screen);

    // Compose a complete off-screen frame before touching the LCD. If neither
    // allocation succeeds, retain the previous direct-drawing fallback.
    const bool firstFrame = buffered || !carouselValid || lastWidth != width || lastHeight != height;
    if (firstFrame) {
        gfx.fillScreen(Background);
        carouselValid = true;
        lastWidth = width;
        lastHeight = height;
    } else {
        // Only repaint the card/dot region. It ends above the footer text;
        // include the last row of dots so a move never leaves stale pixels.
        gfx.fillRect(0, 23, width, std::max(0, height - 40), Background);
    }
    gfx.setFont(&fonts::Font0);
    gfx.setTextSize(1.0f);
    gfx.setTextDatum(middle_center);

    if (firstFrame) {
        const std::string pin = "G" + std::to_string(contexts::GlobalContext::getInstance().getIrTxPin());
        const int badgeWidth = std::max(30, static_cast<int>(gfx.textWidth(pin.c_str())) + 12);
        const int bx = width - badgeWidth - 6;
        // No decorative logo: align the application title with the left edge,
        // and reserve space for the pin badge on narrower ESP32 displays.
        gfx.setTextDatum(middle_left);
        gfx.setTextColor(Text);
        // Match the file-selector and carousel-card titles. Size the Orbitron
        // header to leave room for the pin badge on the narrow display.
        gfx.setFont(&fonts::Orbitron_Light_24);
        gfx.setTextSize(0.58f);
        const std::string appTitle = fitText(gfx, "ULTIMATE REMOTE", std::max(0, bx - 12));
        if (!appTitle.empty()) gfx.drawString(appTitle.c_str(), 6, 11);
        gfx.setFont(&fonts::Font0);
        gfx.setTextSize(1.0f);
        gfx.setTextDatum(middle_center);
        gfx.fillRoundRect(bx, 3, badgeWidth, 17, 4, Panel);
        gfx.drawRoundRect(bx, 3, badgeWidth, 17, 4, Border);
        centeredText(gfx, pin, bx + badgeWidth / 2, 11, badgeWidth - 7, 1.0f, Primary);
        centeredText(gfx, "<>  MOVE", width / 4, height - 8,
                     width / 2 - 9, 1.0f, Muted);
        centeredText(gfx, "OK  OPEN", width * 3 / 4, height - 8,
                     width / 2 - 9, 1.0f, Text);
    }

    if (labels.empty()) {
        centeredText(gfx, "Nothing to display", width / 2, height / 2,
                     width - 20, 1.0f, Muted);
    } else {
        const size_t count = labels.size();
        const size_t selected = std::min<size_t>(selectionIndex, count - 1);
        // Responsive geometry: both original and ADV share a smaller screen;
        // the min/max prevent clipping on a different display width/height.
        const int centerWidth = std::min(118, std::max(78, width * 49 / 100));
        const int centerX = (width - centerWidth) / 2;
        const int sideWidth = std::max(0, std::min(50, centerX - 12));
        const int usableHeight = std::max(58, height - 58);
        const int centerHeight = std::min(77, usableHeight);
        // Raise the cards (and the indicators below them) slightly.
        const int centerY = 28;
        const int sideHeight = std::max(38, centerHeight - 19);
        const int sideY = centerY + (centerHeight - sideHeight) / 2;
        if (count > 1 && sideWidth >= 32) {
            card(gfx, labels, (selected + count - 1) % count,
                 5, sideY, sideWidth, sideHeight, false);
            card(gfx, labels, (selected + 1) % count,
                 width - sideWidth - 5, sideY, sideWidth, sideHeight, false);
        }
        card(gfx, labels, selected, centerX, centerY,
             centerWidth, centerHeight, true);

        // Page indicators only count modes available on this platform.
        const int gap = 11;
        const int dotsWidth = static_cast<int>((count - 1) * gap + 12);
        int x = (width - dotsWidth) / 2;
        const int dy = std::min(height - 23, centerY + centerHeight + 7);
        for (size_t i = 0; i < count; ++i) {
            gfx.fillRoundRect(x, dy, i == selected ? 10 : 4, 4, 2,
                              i == selected ? 0xFFFF : rgb565(112, 112, 112));
            x += gap;
        }
    }
    if (buffered) {
        frame.pushSprite(0, 0);
        // Finish the transfer before this shared buffer can be drawn into again.
        screen.waitDMA();
    }

    // Restore the legacy text settings, including after direct-drawing fallback.
    screen.setFont(&fonts::Orbitron_Light_24);
    screen.setTextSize(1.0f);
    screen.setTextDatum(middle_center);
    screen.setTextColor(TEXT_COLOR);
}

} // namespace platform::arduino
#endif // ARDUINO
