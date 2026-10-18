#ifndef MODE_SELECTION_H
#define MODE_SELECTION_H

#include <Interfaces/IDisplayView.h>
#include <Interfaces/IInputProvider.h>
#include <Interfaces/IAppPlatform.h>
#include <Contexts/GlobalContext.h>
#include <Enums/SelectionModeEnum.h>

using namespace contexts;
using namespace enums;

namespace selections {


class ModeSelection {
public:
    ModeSelection(interfaces::IDisplayView& display, interfaces::IInputProvider& input, interfaces::IAppPlatform& platform);
    SelectionModeEnum select();

    // Fonctions utilitaires pour obtenir les chaînes de caractères et descriptions
    const std::string getSelectionModeToString(SelectionModeEnum mode);
    const std::string getSelectionModeDescription(SelectionModeEnum mode);
    const std::vector<std::string> getSelectionModeStrings();
    const std::vector<std::string> getSelectionModeDescriptionStrings();

private:
    interfaces::IDisplayView& display;
    interfaces::IInputProvider& input;
    interfaces::IAppPlatform& platform;
    GlobalContext& globalContext = GlobalContext::getInstance();
    uint8_t selectionIndex;
    int8_t lastIndex;
};

}

#endif // MODE_SELECTION_H
