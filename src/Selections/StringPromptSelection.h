#ifndef STRING_PROMPT_SELECTION_H
#define STRING_PROMPT_SELECTION_H

#include <Views/CardputerView.h>
#include <Inputs/CardputerInput.h>
#include <Contexts/GlobalContext.h>

using namespace views;
using namespace inputs;
using namespace contexts;

namespace selections {

class StringPromptSelection {
public:
    StringPromptSelection(CardputerView& display, CardputerInput& input, GlobalContext& globalContext);
    std::string select(std::string description);
private:
    GlobalContext& globalContext;
    CardputerView& display;
    CardputerInput& input;
};

}

#endif