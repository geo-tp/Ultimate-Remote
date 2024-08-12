#ifndef MODE_SELECTION_H
#define MODE_SELECTION_H

#include <Views/CardputerView.h>
#include <Inputs/CardputerInput.h>
#include <Contexts/GlobalContext.h>

using namespace views;
using namespace inputs;
using namespace contexts;

namespace selections {

// Modes de sélection
enum class SelectionMode {
    SCAN,
    FAVORITES,
    FILES,
    ALL_REMOTES,
    COUNT,
};


class ModeSelection {
public:
    ModeSelection(CardputerView& display, CardputerInput& input);
    SelectionMode select();

    // Fonctions utilitaires pour obtenir les chaînes de caractères et descriptions
    static const std::string getSelectionModeToString(SelectionMode mode);
    static const std::string getSelectionModeDescription(SelectionMode mode);
    static const std::vector<std::string> getSelectionModeStrings();
    static const std::vector<std::string> getSelectionModeDescriptionStrings();

private:
    GlobalContext& context = GlobalContext::getInstance();
    CardputerView& display;
    CardputerInput& input;
    uint8_t selectionIndex;
    int8_t lastIndex;
};

}

#endif // MODE_SELECTION_H
