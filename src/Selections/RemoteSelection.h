#ifndef REMOTE_SELECTION_H
#define REMOTE_SELECTION_H

#include <Interfaces/IDisplayView.h>
#include <functional>
#include <Interfaces/IInputProvider.h>
#include <Models/Remote.h>
#include <Contexts/GlobalContext.h>

using namespace contexts;
using namespace models;

namespace selections {

class RemoteSelection {
public:
    RemoteSelection(interfaces::IDisplayView& display, interfaces::IInputProvider& input);
    Remote select(const std::vector<Remote>& remotes, const std::string& title, uint16_t& selectionIndex,
                  Remote emptyRemote, const std::vector<std::string>& labels = {},
                  const std::function<void(size_t)>& onFavorite = {},
                  const bool* refreshAfterFavorite = nullptr);
private:
    interfaces::IDisplayView& display;
    interfaces::IInputProvider& input;
    int16_t lastIndex;
};

}

#endif
