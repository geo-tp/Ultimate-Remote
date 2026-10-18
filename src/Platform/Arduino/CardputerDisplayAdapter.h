#ifndef CARDPUTER_DISPLAY_ADAPTER_H
#define CARDPUTER_DISPLAY_ADAPTER_H

#include <Interfaces/IDisplayView.h>
#include <Platform/Arduino/CardputerView.h>
#include <Platform/Arduino/CardputerHomeCarousel.h>

namespace platform::arduino {

class CardputerDisplayAdapter : public interfaces::IDisplayView {
public:
    void initialise() override {
        CardputerHomeCarousel::invalidate();
        platform::arduino::CardputerView::initialise();
    }
    void displayTopBar(const std::string& title, bool submenu = false, bool searchBar = false) override {
        CardputerHomeCarousel::invalidate();
        platform::arduino::CardputerView::displayTopBar(title, submenu, searchBar);
    }
    void displayFilteredTopBar(const std::string& title, const std::string& query) override {
        // The ESP32 search area is too narrow to show a list heading as well
        // as the filter. Restore the original title when the query is cleared.
        displayTopBar(query.empty() ? title : query, true, true);
    }
    void displaySelection(const std::vector<std::string>& selectionStrings,
                          uint16_t selectionIndex,
                          const std::vector<std::string>& selectionDescription = {}) override {
        CardputerHomeCarousel::invalidate();
        platform::arduino::CardputerView::displaySelection(selectionStrings, selectionIndex, selectionDescription);
    }
    void displayIrBuilderSelection(const std::vector<std::string>& labels,
                                   uint16_t selected,
                                   const std::vector<std::string>& shortcutBadges) override {
        (void)shortcutBadges;
        displaySelection(labels, selected); // No O/S/F/IRP/OK badges on ESP32.
    }
    void displayHomeCarousel(const std::vector<std::string>& modes,
                             uint16_t selected,
                             const std::vector<std::string>& descriptions = {}) override {
        platform::arduino::CardputerView::invalidateCache();
        CardputerHomeCarousel::render(modes, selected, descriptions);
    }
    void displayScanInfo() override { beforeOtherScreen(); platform::arduino::CardputerView::displayScanInfo(); }
    void displayScanRemote(std::string remoteName) override { beforeOtherScreen(); platform::arduino::CardputerView::displayScanRemote(remoteName); }
    void displayScanSelection(std::string manufacturerName) override { beforeOtherScreen(); platform::arduino::CardputerView::displayScanSelection(manufacturerName); }
    void displayScanCommand(std::string commandName) override { beforeOtherScreen(); platform::arduino::CardputerView::displayScanCommand(commandName); }
    void displayScanOver() override { beforeOtherScreen(); platform::arduino::CardputerView::displayScanOver(); }
    void displayStringPrompt(std::string stringDescription, std::string stringInput) override {
        beforeOtherScreen();
        platform::arduino::CardputerView::displayStringPrompt(stringDescription, stringInput);
    }
    void displayConfirmationPrompt(std::string stringDescription) override {
        beforeOtherScreen();
        platform::arduino::CardputerView::displayConfirmationPrompt(stringDescription);
    }
    void displayLoading() override { beforeOtherScreen(); platform::arduino::CardputerView::displayLoading(); }
    void displayIrFileInfo() override { beforeOtherScreen(); platform::arduino::CardputerView::displayIrFileInfo(); }
private:
    static void beforeOtherScreen() {
        CardputerHomeCarousel::invalidate();
        platform::arduino::CardputerView::invalidateCache();
    }
};

}

#endif
