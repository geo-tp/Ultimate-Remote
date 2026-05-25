#ifndef STRING_PROMPT_SELECTION_H
#define STRING_PROMPT_SELECTION_H

#include <Views/CardputerView.h>
#include <Inputs/CardputerInput.h>
#include <Contexts/GlobalContext.h>

namespace selections {

class StringPromptSelection {
public:
    StringPromptSelection(views::CardputerView& display, inputs::CardputerInput& input);
    std::string select(std::string description);
private:
    contexts::GlobalContext& globalContext = contexts::GlobalContext::getInstance();
    views::CardputerView& display;
    inputs::CardputerInput& input;
};

}

#endif