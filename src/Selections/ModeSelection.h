#ifndef MODE_SELECTION_H
#define MODE_SELECTION_H

#include <Views/CardputerView.h>
#include <Inputs/CardputerInput.h>
#include <Contexts/GlobalContext.h>
#include <Enums/SelectionModeEnum.h>

namespace selections {


class ModeSelection {
public:
    ModeSelection(views::CardputerView& display, inputs::CardputerInput& input);
    enums::SelectionModeEnum select();

    // Fonctions utilitaires pour obtenir les chaînes de caractères et descriptions
    static const std::string getSelectionModeToString(enums::SelectionModeEnum mode);
    static const std::string getSelectionModeDescription(enums::SelectionModeEnum mode);
    static const std::vector<std::string> getSelectionModeStrings();
    static const std::vector<std::string> getSelectionModeDescriptionStrings();

private:
    views::CardputerView& display;
    inputs::CardputerInput& input;
    contexts::GlobalContext& globalContext = contexts::GlobalContext::getInstance();
    uint8_t selectionIndex;
    int8_t lastIndex;
};

}

#endif // MODE_SELECTION_H
