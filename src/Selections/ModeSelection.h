#ifndef MODE_SELECTION_H
#define MODE_SELECTION_H

#include <Views/CardputerView.h>
#include <Inputs/CardputerInput.h>
#include <Contexts/GlobalContext.h>
#include <Enums/SelectionModeEnum.h>

using namespace views;
using namespace inputs;
using namespace contexts;
using namespace enums;

namespace selections {


class ModeSelection {
public:
    ModeSelection(CardputerView& display, CardputerInput& input, GlobalContext& globalContext);
    SelectionModeEnum select();

    // Fonctions utilitaires pour obtenir les chaînes de caractères et descriptions
    static const std::string getSelectionModeToString(SelectionModeEnum mode);
    static const std::string getSelectionModeDescription(SelectionModeEnum mode);
    static const std::vector<std::string> getSelectionModeStrings();
    static const std::vector<std::string> getSelectionModeDescriptionStrings();

private:
    CardputerView& display;
    CardputerInput& input;
    GlobalContext& globalContext;
    uint8_t selectionIndex;
    int8_t lastIndex;
};

}

#endif // MODE_SELECTION_H
