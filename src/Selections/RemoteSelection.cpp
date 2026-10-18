#include "RemoteSelection.h"
#include <Utils/StringUtils.h>
#include <Inputs/InputKeys.h>
#include <Inputs/ScopedTextInput.h>
#include <Inputs/ScopedZBack.h>

namespace selections {

RemoteSelection::RemoteSelection(interfaces::IDisplayView& display, interfaces::IInputProvider& input)
    : display(display), input(input), lastIndex(-1) {}

Remote RemoteSelection::select(const std::vector<Remote>& remotes, const std::string& title, uint16_t& selectionIndex,
                               Remote emptyRemote, const std::vector<std::string>& labels,
                               const std::function<void(size_t)>& onFavorite,
                               const bool* refreshAfterFavorite) {
    inputs::ScopedZBack zBack(input);
    inputs::ScopedTextInput typing(input);
    std::vector<std::string> names = utils::StringUtils::addRemotesPrefixes(
        utils::StringUtils::extractFieldNames(remotes, "fileName"));
    if (labels.size() == remotes.size()) names = labels;
    std::string query;
    std::vector<size_t> matches;
    auto rebuild = [&]() {
        matches.clear();
        const std::string needle = utils::StringUtils::toUpperCase(query);
        for (size_t i = 0; i < names.size(); ++i) {
            if (utils::StringUtils::toUpperCase(names[i]).find(needle) != std::string::npos)
                matches.push_back(i);
        }
    };
    rebuild();
    if (selectionIndex >= remotes.size()) selectionIndex = 0;
    size_t cursor = 0;
    for (size_t i = 0; i < matches.size(); ++i)
        if (matches[i] == selectionIndex) { cursor = i; break; }
    bool redraw = true;
    for (;;) {
        if (redraw) {
            std::vector<std::string> filtered;
            filtered.reserve(matches.size());
            for (size_t i : matches) filtered.push_back(names[i]);
            display.displayFilteredTopBar(title, query);
            display.displaySelection(filtered, static_cast<uint16_t>(cursor));
            redraw = false;
        }
        const char key = input.handler();
        if (key == KEY_NONE) continue;
        if (key == KEY_RETURN) return emptyRemote;
        if (key == KEY_OK) {
            if (matches.empty()) continue;
            selectionIndex = static_cast<uint16_t>(matches[cursor]);
            return remotes[selectionIndex];
        }
        if (key == KEY_ARROW_DOWN && !matches.empty()) cursor = (cursor + 1) % matches.size();
        else if (key == KEY_ARROW_UP && !matches.empty()) cursor = (cursor + matches.size() - 1) % matches.size();
        else if (key == KEY_FAVORITE) {
            if (onFavorite && !matches.empty()) {
                selectionIndex = static_cast<uint16_t>(matches[cursor]);
                onFavorite(selectionIndex);
                // The display was temporarily replaced by a modal prompt.
                // A confirmed ESP32 delete requires fresh remotes/labels.
                if (refreshAfterFavorite && *refreshAfterFavorite) return emptyRemote;
            }
        } else if (key == KEY_FILTER_SPACE) {
            // Space stays bound to Favorite; Fn+Space inserts a search space.
            if (!query.empty() && query.size() < 64) {
                query += ' ';
                rebuild();
                cursor = 0;
            } else continue;
        } else if (key == KEY_DEL) {
            if (!query.empty()) {
                query.pop_back();
                rebuild();
                cursor = 0;
            }
        } else if ((key >= 'a' && key <= 'z') || (key >= 'A' && key <= 'Z') ||
                   (key >= '0' && key <= '9') || key == '-' || key == '_') {
            query += key;
            rebuild();
            cursor = 0;
        } else continue;
        redraw = true;
    }
}

}
