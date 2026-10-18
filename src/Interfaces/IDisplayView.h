#ifndef I_DISPLAY_VIEW_H
#define I_DISPLAY_VIEW_H

#include <cstdint>
#include <string>
#include <vector>

namespace interfaces {

class IDisplayView {
public:
    virtual ~IDisplayView() = default;

    virtual void initialise() = 0;
    virtual void displayTopBar(const std::string& title, bool submenu = false, bool searchBar = false) = 0;
    // List filter heading. Platforms may use a compact query-only search bar,
    // while the default keeps the existing Linux "Title | query" presentation.
    virtual void displayFilteredTopBar(const std::string& title, const std::string& query) {
        displayTopBar(query.empty() ? title : title + " | " + query, true, true);
    }
    virtual void displaySelection(const std::vector<std::string>& selectionStrings,
                                  uint16_t selectionIndex,
                                  const std::vector<std::string>& selectionDescription = {}) = 0;
    // IR Builder shows key badges on devices with room for them. The Cardputer
    // implementation can omit them without introducing platform conditionals
    // in the controller.
    virtual void displayIrBuilderSelection(const std::vector<std::string>& labels,
                                           uint16_t selected,
                                           const std::vector<std::string>& shortcutBadges) {
        displaySelection(labels, selected, shortcutBadges);
    }
    // Only the root mode selector is a horizontal carousel. Other selections
    // remain ordinary lists. The default delegates to the existing renderer,
    // preserving the Linux framebuffer implementation unchanged.
    virtual void displayHomeCarousel(const std::vector<std::string>& modes,
                                     uint16_t selected,
                                     const std::vector<std::string>& descriptions = {}) {
        displaySelection(modes, selected, descriptions);
    }
    virtual void displayTransmissionFeedback(const std::string& command, bool success) {
        displayMessage(success ? "IR command sent" : "IR send failed", {command});
    }
    virtual void displayScanInfo() = 0;
    virtual void displayScanRemote(std::string remoteName) = 0;
    virtual void displayScanSelection(std::string manufacturerName) = 0;
    virtual void displayScanCommand(std::string commandName) = 0;
    virtual void displayScanOver() = 0;
    virtual void displayStringPrompt(std::string stringDescription, std::string stringInput) = 0;
    virtual void displayConfirmationPrompt(std::string stringDescription) = 0;
    virtual void displayLoading() = 0;
    virtual void displayIrFileInfo() = 0;
    virtual void displayExitHold(int elapsedMilliseconds) { (void)elapsedMilliseconds; }
    virtual void displayInfoPage(const std::string& title, const std::vector<std::string>& lines,
                                 uint16_t page, uint16_t pageCount) {
        displayMessage("Info " + std::to_string(page + 1) + "/" + std::to_string(pageCount) + ": " + title, lines);
    }
    virtual void displayMessage(const std::string& title, const std::vector<std::string>& lines, bool busy = false) {
        (void)busy;
        displayTopBar(title, true);
        displaySelection(lines, 0);
    }
};

}

#endif
