#ifndef STRING_PROMPT_SELECTION_H
#define STRING_PROMPT_SELECTION_H

#include <Interfaces/IDisplayView.h>
#include <Interfaces/IInputProvider.h>
#include <Contexts/GlobalContext.h>

using namespace contexts;

namespace selections {

class StringPromptSelection {
public:
    StringPromptSelection(interfaces::IDisplayView& display, interfaces::IInputProvider& input);
    std::string select(std::string description);
private:
    GlobalContext& globalContext = GlobalContext::getInstance();
    interfaces::IDisplayView& display;
    interfaces::IInputProvider& input;
};

}

#endif