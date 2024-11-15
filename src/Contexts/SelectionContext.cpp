#include "SelectionContext.h"

namespace contexts {

SelectionContext& SelectionContext::getInstance() {
    static SelectionContext instance;
    return instance;
}

SelectionContext::SelectionContext() = default;

// Getters and setters for each member variable
bool SelectionContext::getIsModeSelected() const { return isModeSelected; }
void SelectionContext::setIsModeSelected(bool isModeSelected) { this->isModeSelected = isModeSelected; }

bool SelectionContext::getIsManufacturerSelected() const { return isManufacturerSelected; }
void SelectionContext::setIsManufacturerSelected(bool isManufacturerSelected) { this->isManufacturerSelected = isManufacturerSelected; }

bool SelectionContext::getIsProductSelected() const { return isProductSelected; }
void SelectionContext::setIsProductSelected(bool isProductSelected) { this->isProductSelected = isProductSelected; }

bool SelectionContext::getIsRemoteSelected() const { return isRemoteSelected; }
void SelectionContext::setIsRemoteSelected(bool isRemoteSelected) { this->isRemoteSelected = isRemoteSelected; }

bool SelectionContext::getIsFileRemoteSelected() const { return isFileRemoteSelected; }
void SelectionContext::setIsFileRemoteSelected(bool isFileRemoteSelected) { this->isFileRemoteSelected = isFileRemoteSelected; }

uint16_t SelectionContext::getCurrentManufacturerIndex() const { return currentManufacturerIndex; }
void SelectionContext::setCurrentManufacturerIndex(uint16_t index) { this->currentManufacturerIndex = index; }

uint16_t SelectionContext::getCurrentProductIndex() const { return currentProductIndex; }
void SelectionContext::setCurrentProductIndex(uint16_t index) { this->currentProductIndex = index; }

uint16_t SelectionContext::getCurrentRemoteIndex() const { return currentRemoteIndex; }
void SelectionContext::setCurrentRemoteIndex(uint16_t index) { this->currentRemoteIndex = index; }

uint16_t SelectionContext::getCurrentRemoteCommandIndex() const { return currentRemoteCommandIndex; }
void SelectionContext::setCurrentRemoteCommandIndex(uint16_t index) { this->currentRemoteCommandIndex = index; }

uint16_t SelectionContext::getCurrentFileRemoteIndex() const { return currentFileRemoteIndex; }
void SelectionContext::setCurrentFileRemoteIndex(uint16_t index) { this->currentFileRemoteIndex = index; }

uint16_t SelectionContext::getCurrentFileRemoteCommandIndex() const { return currentFileRemoteCommandIndex; }
void SelectionContext::setCurrentFileRemoteCommandIndex(uint16_t index) { this->currentFileRemoteCommandIndex = index; }

bool SelectionContext::getFileSelectionFirstRun() const { return fileSelectionFirstRun; }
void SelectionContext::setFileSelectionFirstRun(bool firstRun) { this->fileSelectionFirstRun = firstRun; }

SelectionModeEnum SelectionContext::getCurrentSelectedMode() const { return currentSelectedMode; }
void SelectionContext::setCurrentSelectedMode(SelectionModeEnum mode) { this->currentSelectedMode = mode; }

Manufacturer SelectionContext::getCurrentSelectedManufacturer() const { return currentSelectedManufacturer; }
void SelectionContext::setCurrentSelectedManufacturer(const Manufacturer& manufacturer) { this->currentSelectedManufacturer = manufacturer; }

Product SelectionContext::getCurrentSelectedProduct() const { return currentSelectedProduct; }
void SelectionContext::setCurrentSelectedProduct(const Product& product) { this->currentSelectedProduct = product; }

Remote SelectionContext::getCurrentSelectedRemote() const { return currentSelectedRemote; }
void SelectionContext::setCurrentSelectedRemote(const Remote& remote) { this->currentSelectedRemote = remote; }

RemoteCommand SelectionContext::getCurrentSelectedRemoteCommand() const { return currentSelectedRemoteCommand; }
void SelectionContext::setCurrentSelectedRemoteCommand(const RemoteCommand& command) { this->currentSelectedRemoteCommand = command; }

FileRemote SelectionContext::getCurrentSelectedFileRemote() const { return currentSelectedFileRemote; }
void SelectionContext::setCurrentSelectedFileRemote(const FileRemote& fileRemote) { this->currentSelectedFileRemote = fileRemote; }

FileRemoteCommand SelectionContext::getCurrentSelectedFileRemoteCommand() const { return currentSelectedFileRemoteCommand; }
void SelectionContext::setCurrentSelectedFileRemoteCommand(const FileRemoteCommand& fileRemoteCommand) { this->currentSelectedFileRemoteCommand = fileRemoteCommand; }

std::string SelectionContext::getCurrentSelectedFilePath() const { return currentSelectedFilePath; }
void SelectionContext::setCurrentSelectedFilePath(const std::string& filePath) { this->currentSelectedFilePath = filePath; }

} // namespace contexts
