/*

############################################################################################################################################
#                                                                                                                                          #
#                                                                                                                                          #
#                              Controls the selection of different screens and returns the selected struct                                 #
#                                      In the case of a user return, the returned structure is empty                                       #           
#                                                                                                                                          #
#                                                                                                                                          #
############################################################################################################################################

*/


#ifndef SELECTION_H
#define SELECTION_H

#include <M5Cardputer.h>
#include <iostream>
#include <array>
#include <string.h>
#include "remote_data.h"
#include "display_manager.h"
#include "keyboard.h"

#define MAX_STRING_INPUT 14


// Modes de sélection
enum class SelectionMode {
    SCAN,
    FAVORITES,
    POPULARS,
    ALL_REMOTES,
    COUNT,
};

const std::string getSelectionModeToString(SelectionMode mode);
const std::vector<std::string> getSelectionModeStrings();
const std::string getSelectionModeDescription(SelectionMode mode);
const std::vector<std::string> getSelectionModeDescriptionStrings();

class Selection {
public:
    Selection(DisplayManager& displayManager, CustomKeyboard& keyboard,
              const Manufacturer& emptyManufacturer,
              const Product& emptyProduct,
              const Remote& emptyRemote,
              const RemoteCommand& emptyRemoteCommand);

    SelectionMode modeSelection();
    std::string manufacturerSelection(const std::vector<std::string>& manufacturerNames, uint16_t& selectionIndex, bool scan=false);
    Product productSelection(const std::vector<Product>& products, const std::string& manufacturerName, uint16_t& selectionIndex);
    Remote remoteSelection(const std::vector<Remote>& remotes, const std::string& productName, uint16_t& selectionIndex);
    RemoteCommand remoteCommandSelection(const std::vector<RemoteCommand>& commands, 
                                         const std::string& remoteFileName, 
                                         const std::string& productName, 
                                         uint16_t& selectionIndex, 
                                         std::vector<FavoriteRemote> favoriteRemotes, 
                                         bool isFavoriteMode,
                                         std::function<std::vector<FavoriteRemote>(const std::string&, const std::string&)> addFavorite, 
                                         std::function<std::vector<FavoriteRemote>(const std::string&)> deleteFavorite);
    Remote scanSelection(const std::vector<Remote>& remotes, std::string manufacturerName, std::function<void(const RemoteCommand&)> sendCommand, std::string& favoriteName, bool last);
    std::string stringPromptSelection(std::string description);
    bool confirmationSelection(std::string description);
private:
    uint16_t selectionIndex;
    uint16_t lastIndex;
    char input;
    DisplayManager& displayManager;
    CustomKeyboard& keyboard;
    Manufacturer emptyManufacturer;
    Product emptyProduct;
    Remote emptyRemote;
    RemoteCommand emptyRemoteCommand;
};

#endif // SELECTION_H
