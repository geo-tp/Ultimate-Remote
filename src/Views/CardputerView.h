#ifndef CARDPUTER_VIEW_H
#define CARDPUTER_VIEW_H

#include <vector>
#include <string>
#include <cstring>
#include <M5Cardputer.h>
#include <Utils/StringUtils.h>

// SIZING
#define DEFAULT_MARGIN 5
#define DEFAULT_ROUND_RECT 5
#define TOP_BAR_HEIGHT 30

// PALETTE
#define BACKGROUND_COLOR TFT_BLACK
#define PRIMARY_COLOR 0xfa03
#define RECT_COLOR_DARK 0x0841
#define RECT_COLOR_LIGHT 0xd69a
#define TEXT_COLOR 0xef7d

// TEXT SIZE
#define TEXT_BIG 1
#define TEXT_LARGE 0.95
#define TEXT_WIDE 0.7
#define TEXT_MEDIUM_LARGE 0.6
#define TEXT_MEDIUM 0.55
#define TEXT_SMALL 0.5
#define TEXT_TINY 0.45

namespace views {

class CardputerView {
public:
    static void initialise();
    static void displayTopBar(const std::string& title, bool submenu=false, bool searchBar=false);
    static void displaySelection(const std::vector<std::string>& selectionStrings,  uint16_t selectionIndex, const std::vector<std::string>& selectionDescription={});
    static void displayScanInfo();
    static void displayScanRemote(std::string remoteName);
    static void displayScanSelection(std::string manufacturerName);
    static void displayScanCommand(std::string commandName);
    static void displayScanOver();
    static void displayStringPrompt(std::string stringDescription, std::string stringInput);
    static void displayConfirmationPrompt(std::string stringDescription);
    static void displayLoading();
    static void displayIrFileInfo();
private:
    static M5GFX* Display;
    static void drawSearchIcon(int x, int y, int size, uint16_t color);
    static void drawRemoteIcon(int startX, int startY, int width, int height, uint16_t iconColor, uint16_t buttonColor);
    static void drawRect(bool selected, uint8_t margin, uint16_t startY, uint16_t sizeX, uint16_t sizeY, uint16_t stepY);
    static void drawSubMenuReturn(uint8_t x, uint8_t y);
    static void displayClearMainView(uint8_t offsetY=0);
    static void clearTopBar();
};

}

#endif
