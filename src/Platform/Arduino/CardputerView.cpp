#ifdef ARDUINO
#include "CardputerView.h"

namespace platform::arduino {

namespace {
// Only cache the four visible rows, not entire remote/command directories.
struct RowSnapshot {
    std::string label;
    std::string detail;
    bool selected = false;
    bool present = false;
};
std::array<RowSnapshot, 4> previousRows{};
bool rowsValid = false;
uint16_t previousStartRow = 0;

bool topBarValid = false;
std::string previousTitle;
bool previousSubmenu = false;
bool previousSearch = false;

std::string fitRowText(M5GFX& display, std::string text, int maxWidth) {
    if (maxWidth <= 0) return "";
    if (display.textWidth(text.c_str()) <= maxWidth) return text;
    const char* dots = "...";
    if (display.textWidth(dots) > maxWidth) return "";
    while (!text.empty() && display.textWidth((text + dots).c_str()) > maxWidth) {
        // Pop a complete UTF-8 code point.
        unsigned char c = 0;
        do {
            c = static_cast<unsigned char>(text.back());
            text.pop_back();
        } while (!text.empty() && (c & 0xc0) == 0x80);
    }
    return text + dots;
}

// ESP32 display-only cleanup. Keep the original identifiers intact so that
// selection, filtering, IR lookup and saved favorites continue to work.
std::string readableRemoteLabel(std::string name) {
    const auto removeAt = [&](size_t pos) {
        for (const char* prefix : {"Unknown_", "Unknow_"}) {
            const size_t length = std::strlen(prefix);
            if (name.compare(pos, length, prefix) == 0) {
                name.erase(pos, length);
                break;
            }
        }
    };

    removeAt(0);
    // Some selection names are presented as "Remote Unknown_Model".
    if (name.compare(0, 7, "Remote ") == 0) removeAt(7);
    // In mixed lists the remote filename is followed by " - Unknown_Model".
    const size_t productStart = name.find(" - ");
    if (productStart != std::string::npos) removeAt(productStart + 3);
    return name;
}
}

M5GFX* CardputerView::Display = nullptr;

void CardputerView::invalidateCache() {
    topBarValid = false;
    rowsValid = false;
}


void CardputerView::initialise() {
    invalidateCache();
    Display = &M5Cardputer.Display;
    Display->setRotation(1);
    Display->setTextColor(TEXT_COLOR);
    Display->fillScreen(BACKGROUND_COLOR);
    M5Cardputer.Display.setTextDatum(middle_center);
    M5Cardputer.Display.setFont(&fonts::Orbitron_Light_24);
}

void CardputerView::displayTopBar(const std::string& title, bool submenu, bool searchBar) {
    if (topBarValid && previousTitle == title && previousSubmenu == submenu &&
        previousSearch == searchBar) return;
    previousTitle = title;
    previousSubmenu = submenu;
    previousSearch = searchBar;
    topBarValid = true;
    rowsValid = false; // New screen/title: refresh all four rows once.
    uint8_t marginX = 4;
    uint8_t marginY = 14;
    float offsetX; // for text align
    size_t limiter; // char limitation
    float sizeText; // pixels offset depending on text size

    clearTopBar();

    if (submenu) {
        drawSubMenuReturn(marginX+3, marginY); // for return <
        limiter = 15; // limit string size
        sizeText = 5.1; // pixels offset for each char
    } else {
        Display->setTextSize(TEXT_LARGE);
        limiter = 16;
        sizeText = 6.95;
    }
    
    if (searchBar) {
        Display->setTextColor(TEXT_COLOR);

        // Empty search query
        const std::string searchQuery = title.empty() ? "Type to search" : title.substr(0, limiter);
        
        // To center text
        offsetX = utils::StringUtils::getTextCenterOffset(searchQuery, Display->width(), sizeText);

        // Search icon
        drawSearchIcon(Display->width() - 20, marginY-2, 10, PRIMARY_COLOR);

        Display->setCursor(offsetX, marginY);
        Display->printf(searchQuery.c_str());
    } else {
        Display->setTextColor(TEXT_COLOR);
        
        // Format to remove 'Unknown_' and '.csv'from title, limit text
        std::string formattedTitle = readableRemoteLabel(title);
        formattedTitle = utils::StringUtils::removeCsvSuffix(formattedTitle).substr(0, limiter);

        // To center text
        offsetX = utils::StringUtils::getTextCenterOffset(formattedTitle, Display->width(), sizeText);

        Display->setCursor(offsetX, marginY);
        Display->printf(formattedTitle.c_str());
    }
}


void CardputerView::displaySelection(
                    const std::vector<std::string>& selectionStrings,
                    uint16_t selectionIndex,
                    const std::vector<std::string>& selectionDescription) {
    constexpr int rowsPerScreen = 4;
    const int startY = 30;
    const int stepY = 26;
    const int rowHeight = 22;
    const int margin = DEFAULT_MARGIN;
    const int rowWidth = Display->width() - 10;
    const uint16_t currentStartRow = (selectionIndex / rowsPerScreen) * rowsPerScreen;
    const bool wasEmpty = rowsValid && !previousRows[0].present;
    const bool fullRefresh = !rowsValid || currentStartRow != previousStartRow ||
                             (rowsValid && selectionStrings.empty() != wasEmpty);

    if (fullRefresh) {
        // Never erase the top bar when only the list needs a refresh.
        Display->fillRect(0, TOP_BAR_HEIGHT, Display->width(),
                          Display->height() - TOP_BAR_HEIGHT, BACKGROUND_COLOR);
    }

    for (int row = 0; row < rowsPerScreen; ++row) {
        const size_t index = currentStartRow + row;
        RowSnapshot next;
        if (index < selectionStrings.size()) {
            next.present = true;
            next.selected = index == selectionIndex;
            next.label = selectionStrings[index];
            if (index < selectionDescription.size()) next.detail = selectionDescription[index];
        }
        const auto& before = previousRows[row];
        if (!fullRefresh && before.present == next.present &&
            before.selected == next.selected && before.label == next.label &&
            before.detail == next.detail) continue;

        // On a simple selection move, only the former and new rows are
        // repainted. Other rows and the top bar stay in video RAM.
        if (!fullRefresh) {
            Display->fillRect(margin, startY + stepY * row, rowWidth, rowHeight,
                              BACKGROUND_COLOR);
        }
        previousRows[row] = next;
        if (!next.present) continue;

        Display->setTextSize(TEXT_WIDE);
        drawRect(next.selected, margin, startY, rowWidth, rowHeight, stepY * row);

        std::string label = readableRemoteLabel(next.label);
        label = utils::StringUtils::removeCsvSuffix(label);
        if (label.empty()) label = "UNKNOWN";
        const bool hasKeyBadge = next.detail.size() == 1;
        label = fitRowText(*Display, label, rowWidth - (hasKeyBadge ? 52 : 13));
        Display->setCursor(margin + DEFAULT_MARGIN * 2, 41 + stepY * row);
        Display->printf("%s", label.c_str());

        if (hasKeyBadge) {
            Display->setTextSize(TEXT_WIDE);
            Display->fillRoundRect(Display->width() - 44, startY + stepY * row - 1,
                                   39, rowHeight, DEFAULT_ROUND_RECT, RECT_COLOR_DARK);
            Display->drawRoundRect(Display->width() - 45, startY + stepY * row,
                                   40, rowHeight, DEFAULT_ROUND_RECT, PRIMARY_COLOR);
            Display->setCursor(Display->width() - 32, 41 + stepY * row);
            Display->printf("%s", next.detail.c_str());
        } else if (!next.detail.empty()) {
            // Keep descriptions only when they actually fit after the label.
            const int labelWidth = static_cast<int>(Display->textWidth(label.c_str()));
            const int after = margin + DEFAULT_MARGIN * 2 + labelWidth + 7;
            Display->setTextSize(TEXT_TINY);
            if (after + Display->textWidth(next.detail.c_str()) < Display->width() - margin) {
                Display->setCursor(after, 41 + stepY * row);
                Display->printf("%s", next.detail.c_str());
            }
        }
    }

    if (selectionStrings.empty() && fullRefresh) {
        Display->setTextSize(TEXT_WIDE);
        Display->setTextColor(TEXT_COLOR);
        Display->drawCenterString("No results", Display->width() / 2, Display->height() / 2);
    }

    previousStartRow = currentStartRow;
    rowsValid = true;
}

void CardputerView::displayScanInfo() {
    Display->fillScreen(BACKGROUND_COLOR);

    // Box frame
    Display->drawRect(1, 1, Display->width() - 1, Display->height() - 1, PRIMARY_COLOR);

    // Main title
    Display->setTextSize(TEXT_BIG);
    Display->setCursor(40, 18);
    Display->setTextColor(PRIMARY_COLOR);
    Display->printf("About Scan");

    // Sub title
    Display->setTextSize(TEXT_MEDIUM);
    Display->setTextColor(TEXT_COLOR);
    Display->setCursor(16, 42);
    Display->printf("Scan all remotes for a brand");
    Display->setTextSize(TEXT_TINY);
    Display->setCursor(15, 59);

    // Text
    Display->printf("When a remote reacts to your device");
    Display->setTextColor(PRIMARY_COLOR);
    Display->setCursor(15, 72);
    Display->printf(" press space ");
    Display->setCursor(94, 72);
    Display->setTextColor(TEXT_COLOR);
    Display->printf("to add to your favorites");
    Display->setTextSize(TEXT_SMALL);
    Display->setTextColor(TEXT_COLOR);
    Display->setCursor(18, 90);
    Display->printf("You will need to select a brand");

    // Button OK
    Display->fillRoundRect(70, 105, 100, 20, DEFAULT_ROUND_RECT, PRIMARY_COLOR);
    Display->setTextColor(TEXT_COLOR);
    Display->setTextSize(TEXT_MEDIUM);
    Display->setCursor(80, 115);
    Display->printf("OK to start");
}

void CardputerView::displayScanSelection(std::string manufacturerName) {
    Display->fillRect(0, 30, Display->width(), Display->height(), BACKGROUND_COLOR);

    drawRemoteIcon(Display->width() - 55, 36, 40, 75, RECT_COLOR_DARK, RECT_COLOR_LIGHT);

    // Manufacturer
    Display->setTextSize(TEXT_WIDE);
    Display->setTextColor(TEXT_COLOR);
    Display->setCursor(15, 55);
    Display->printf(manufacturerName.c_str());
}

void CardputerView::displayIrFileInfo() {
    Display->fillScreen(BACKGROUND_COLOR);

    // Box frame
    Display->drawRect(1, 1, Display->width() - 1, Display->height() - 1, PRIMARY_COLOR);

    // Main title
    Display->setTextSize(TEXT_BIG);
    Display->setCursor(50, 22);
    Display->setTextColor(PRIMARY_COLOR);
    Display->printf("About File");

    // Sub title
    Display->setTextSize(TEXT_SMALL);
    Display->setTextColor(TEXT_COLOR);
    Display->setCursor(23, 46);
    Display->printf("You can read .ir files from SD");

    // Text
    Display->setCursor(23, 65);
    Display->printf("Visit GitHub to get these files");
    Display->setTextColor(PRIMARY_COLOR);
    Display->setCursor(45, 88);
    Display->setTextSize(TEXT_WIDE);
    Display->printf("Ultimate-Remote");

    // Button OK
    Display->fillRoundRect(70, 105, 100, 20, DEFAULT_ROUND_RECT, PRIMARY_COLOR);
    Display->setTextColor(TEXT_COLOR);
    Display->setTextSize(TEXT_MEDIUM);
    Display->setCursor(80, 115);
    Display->printf("OK to start");
}

void CardputerView::displayScanRemote(std::string remoteName) {
    Display->fillRect(1, 70, 184, 19, BACKGROUND_COLOR); //clear
    Display->setTextColor(TEXT_COLOR);
    Display->setTextSize(0.65);
    Display->setCursor(15, 75);
    const std::string label = readableRemoteLabel(utils::StringUtils::removeCsvSuffix(remoteName));
    // Keep the label clear of the remote icon at the right of the scan screen.
    // Measure against the active font (rather than truncating N characters).
    constexpr int textX = 15;
    constexpr int textRight = 184;
    const std::string fitted = fitRowText(*Display, label, textRight - textX);
    Display->printf("%s", fitted.c_str());
    Display->setTextSize(0.53);
    Display->setTextColor(PRIMARY_COLOR);
    Display->setCursor(7, 126);
    Display->printf("Press space ");
    Display->setTextColor(TEXT_COLOR);
    Display->printf("to add to favorites");
}

void CardputerView::displayScanCommand(std::string commandName) { 
    Display->fillRect(1, 89, 184, 20, BACKGROUND_COLOR); //clear
    Display->setTextSize(TEXT_MEDIUM);
    Display->setCursor(15, 95);
    Display->setTextColor(PRIMARY_COLOR);
    Display->printf("%.16s", utils::StringUtils::removeCsvSuffix(commandName).c_str());
}

void CardputerView::displayScanOver() { 
    Display->fillScreen(BACKGROUND_COLOR);

    // Box frame
    Display->drawRect(1, 1, Display->width() - 1, Display->height() - 1, PRIMARY_COLOR);

    // Main title
    Display->setTextSize(TEXT_BIG);
    Display->setTextColor(PRIMARY_COLOR);
    Display->setCursor(16, 18);
    Display->printf("Scan complete");

    // Sub title
    Display->setTextSize(TEXT_MEDIUM);
    Display->setTextColor(TEXT_COLOR);
    Display->setCursor(20, 42);
    Display->printf("All remotes have been tried");
    Display->setCursor(40, 59);
    Display->printf("for the selected brand");

    // Text
    Display->setCursor(14, 76);
    Display->printf("If you haven't found a remote");
    Display->setCursor(11, 92);
    Display->setTextColor(PRIMARY_COLOR);
    Display->printf("You can try those in favorites");

    // Button OK
    Display->fillRoundRect(75, 105, 90, 20, DEFAULT_ROUND_RECT, PRIMARY_COLOR);
    Display->setTextColor(TEXT_COLOR);
    Display->setCursor(85, 115);
    Display->printf("OK to quit");
}

void CardputerView::displayStringPrompt(std::string stringDescription, std::string stringInput) {

    // Clear
    displayClearMainView(5);

    // Box frame
    Display->drawRoundRect(10, 35, Display->width() - 20, 90, DEFAULT_ROUND_RECT, PRIMARY_COLOR);

    // Description
    Display->setTextSize(TEXT_MEDIUM);
    Display->setTextColor(TEXT_COLOR);
    Display->setCursor(40, 47);
    Display->printf(stringDescription.c_str());

    // input
    Display->setTextSize(TEXT_MEDIUM_LARGE);
    Display->drawRoundRect(42, 62, 155, 25, DEFAULT_ROUND_RECT, RECT_COLOR_DARK);
    Display->setCursor(51, 73);
    Display->printf(stringInput.c_str());

    // < button
    Display->drawRoundRect(53, 95, 40, 20, DEFAULT_ROUND_RECT, PRIMARY_COLOR);
    Display->setCursor(70, 103);
    Display->printf("<");

    // Button save
    if (stringInput.empty()) {
        Display->drawRoundRect(105, 95, 80, 20, DEFAULT_ROUND_RECT, PRIMARY_COLOR);
    } else {
        Display->fillRoundRect(105, 95, 80, 20, DEFAULT_ROUND_RECT, PRIMARY_COLOR);
    }
    Display->setTextSize(TEXT_MEDIUM);
    Display->setCursor(125, 105);
    Display->printf("Save");
}

void CardputerView::displayConfirmationPrompt(std::string stringDescription) {
    displayClearMainView(5);

    const int width = Display->width();
    const int boxX = 10;
    const int boxWidth = width - 20;
    const int middleX = width / 2;
    Display->drawRoundRect(boxX, 35, boxWidth, 90, DEFAULT_ROUND_RECT, PRIMARY_COLOR);
    Display->setTextColor(TEXT_COLOR);
    Display->setTextDatum(middle_center);

    constexpr const char* deletePrefix = "Delete favorite: ";
    const bool deletingFavorite = stringDescription.compare(0, std::strlen(deletePrefix), deletePrefix) == 0;
    if (deletingFavorite) {
        // Keep the action and the selected name on separate centered lines.
        // The name is abbreviated by actual rendered pixel width, not bytes.
        Display->setTextSize(TEXT_MEDIUM);
        Display->drawString("Delete favorite?", middleX, 51);
        Display->setTextSize(TEXT_WIDE);
        const std::string name = stringDescription.substr(std::strlen(deletePrefix));
        const std::string shortName = fitRowText(*Display, name, boxWidth - 24);
        Display->drawString(shortName.c_str(), middleX, 69);
    } else {
        Display->setTextSize(TEXT_MEDIUM);
        const std::string description = fitRowText(*Display, stringDescription, boxWidth - 24);
        Display->drawString(description.c_str(), middleX, 62);
    }

    const int buttonWidth = 40;
    const int buttonGap = 22;
    const int leftX = middleX - buttonWidth - buttonGap / 2;
    const int rightX = middleX + buttonGap / 2;
    Display->drawRoundRect(leftX, 85, buttonWidth, 20, DEFAULT_ROUND_RECT, PRIMARY_COLOR);
    Display->fillRoundRect(rightX, 85, buttonWidth, 20, DEFAULT_ROUND_RECT, PRIMARY_COLOR);
    Display->setTextSize(TEXT_MEDIUM);
    Display->drawString("<", leftX + buttonWidth / 2, 95);
    Display->drawString("OK", rightX + buttonWidth / 2, 95);

    // The Cardputer view is initialized with middle_center. Keep that
    // alignment after either OK or Back: switching to top_left here shifted
    // the explorer's row labels on the next redraw.
    Display->setTextDatum(middle_center);
}

void CardputerView::displayLoading() {
    // Clear
    displayClearMainView(5);

    // Box frame
    Display->drawRoundRect(10, 35, Display->width() - 20, 90, DEFAULT_ROUND_RECT, PRIMARY_COLOR);

    // Description
    Display->setTextSize(TEXT_WIDE);
    Display->setTextColor(TEXT_COLOR);
    Display->setCursor(77, 80);
    Display->printf("Loading...");
    Display->setTextSize(TEXT_MEDIUM);
}


void CardputerView::drawSearchIcon(int x, int y, int size, uint16_t color) {
    int radius = size / 2;
    int handleLength = size / 2;

    // Dessiner cercle
    Display->drawCircle(x, y, radius, color);
    // Dessiner poignée
    Display->drawLine(x + radius, y + radius, x + radius + handleLength, y + radius + handleLength, color);
}

void CardputerView::drawRemoteIcon(int startX, int startY, int width, int height, uint16_t iconColor, uint16_t buttonColor) {
    // Dessiner la remote
    Display->fillRoundRect(startX, startY, width, height, DEFAULT_ROUND_RECT*2, iconColor);
    Display->drawRoundRect(startX, startY, width, height, DEFAULT_ROUND_RECT*2, PRIMARY_COLOR);

    // Calculer les dimensions 
    int buttonWidth = width / 6;
    int buttonHeight = height / 10;
    int buttonSpacing = (width - 2 * buttonWidth) / 5;
    int verticalSpacing = (height - 3 * buttonHeight) / 6;
    
    // Dessiner les boutons
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 3; ++j) {
            int buttonX = startX + buttonSpacing + j * (buttonWidth + buttonSpacing);
            int buttonY = startY + verticalSpacing + i * (buttonHeight + verticalSpacing);
            
            // No buttons for those indexes
            if (i == 0 && j == 0) continue;
            if (i == 0 && j == 1) continue;

            // color primary for centered button 
            if (i == 2 && j == 1) {
                Display->fillRoundRect(buttonX, buttonY, buttonWidth, buttonHeight, DEFAULT_ROUND_RECT, PRIMARY_COLOR);
            } else {
                Display->fillRoundRect(buttonX, buttonY, buttonWidth, buttonHeight, DEFAULT_ROUND_RECT, buttonColor);
            }
        }
    }
}

void CardputerView::drawRect(bool selected, uint8_t margin, uint16_t startY, uint16_t sizeX, uint16_t sizeY, uint16_t stepY) {
        // Draw rect
        if (selected) {
            Display->fillRoundRect(margin, startY + stepY, sizeX, sizeY, DEFAULT_ROUND_RECT, PRIMARY_COLOR);
            Display->setTextColor(TEXT_COLOR);
        } else {
            Display->fillRoundRect(margin, startY + stepY , sizeX, sizeY, DEFAULT_ROUND_RECT, RECT_COLOR_DARK);
            Display->drawRoundRect(margin, startY + stepY, sizeX, sizeY, DEFAULT_ROUND_RECT, PRIMARY_COLOR);
            Display->setTextColor(TEXT_COLOR);
        }
}

void CardputerView::drawSubMenuReturn(uint8_t x, uint8_t y) {
    Display->setTextSize(TEXT_WIDE);
    Display->setTextColor(PRIMARY_COLOR);
    Display->setCursor(x, y);
    Display->printf("<");
}

void CardputerView::displayClearMainView(uint8_t offsetY) {
    Display->fillRect(0, TOP_BAR_HEIGHT-offsetY, Display->width(), Display->height(), BACKGROUND_COLOR);
}

void CardputerView::clearTopBar() {
    Display->fillRect(0, 0, Display->width(), TOP_BAR_HEIGHT, BACKGROUND_COLOR);
}

}

#endif // ARDUINO
