#ifndef SELECTION_CONTEXT_H
#define SELECTION_CONTEXT_H

#include <string>
#include <Models/Manufacturer.h>
#include <Models/Product.h>
#include <Models/Remote.h>
#include <Models/RemoteCommand.h>
#include <Models/FileRemote.h>
#include <Models/FileRemoteCommand.h>
#include <Enums/SelectionModeEnum.h>

namespace contexts {

class SelectionContext {
public:
    // Singleton instance accessor
    static SelectionContext& getInstance();

    bool getIsModeSelected() const;
    void setIsModeSelected(bool isModeSelected);

    bool getIsManufacturerSelected() const;
    void setIsManufacturerSelected(bool isManufacturerSelected);

    bool getIsProductSelected() const;
    void setIsProductSelected(bool isProductSelected);

    bool getIsRemoteSelected() const;
    void setIsRemoteSelected(bool isRemoteSelected);

    bool getIsFileRemoteSelected() const;
    void setIsFileRemoteSelected(bool isFileRemoteSelected);

    uint16_t getCurrentManufacturerIndex() const;
    void setCurrentManufacturerIndex(uint16_t index);

    uint16_t getCurrentProductIndex() const;
    void setCurrentProductIndex(uint16_t index);

    uint16_t getCurrentRemoteIndex() const;
    void setCurrentRemoteIndex(uint16_t index);

    uint16_t getCurrentRemoteCommandIndex() const;
    void setCurrentRemoteCommandIndex(uint16_t index);

    uint16_t getCurrentFileRemoteIndex() const;
    void setCurrentFileRemoteIndex(uint16_t index);

    uint16_t getCurrentFileRemoteCommandIndex() const;
    void setCurrentFileRemoteCommandIndex(uint16_t index);

    bool getFileSelectionFirstRun() const;
    void setFileSelectionFirstRun(bool firstRun);

    SelectionModeEnum getCurrentSelectedMode() const;
    void setCurrentSelectedMode(SelectionModeEnum mode);

    models::Manufacturer getCurrentSelectedManufacturer() const;
    void setCurrentSelectedManufacturer(const models::Manufacturer& manufacturer);

    models::Product getCurrentSelectedProduct() const;
    void setCurrentSelectedProduct(const models::Product& product);

    models::Remote getCurrentSelectedRemote() const;
    void setCurrentSelectedRemote(const models::Remote& remote);

    models::RemoteCommand getCurrentSelectedRemoteCommand() const;
    void setCurrentSelectedRemoteCommand(const models::RemoteCommand& command);

    models::FileRemote getCurrentSelectedFileRemote() const;
    void setCurrentSelectedFileRemote(const models::FileRemote& fileRemote);

    models::FileRemoteCommand getCurrentSelectedFileRemoteCommand() const;
    void setCurrentSelectedFileRemoteCommand(const models::FileRemoteCommand& fileRemoteCommand);

    std::string getCurrentSelectedFilePath() const;
    void setCurrentSelectedFilePath(const std::string& filePath);

private:
    // Private constructor to prevent instantiation
    SelectionContext();

    bool isModeSelected = false;
    bool isManufacturerSelected = false;
    bool isProductSelected = false;
    bool isRemoteSelected = false;
    bool isFileRemoteSelected = false;

    uint16_t currentManufacturerIndex = 0;
    uint16_t currentProductIndex = 0;
    uint16_t currentRemoteIndex = 0;
    uint16_t currentRemoteCommandIndex = 0;
    uint16_t currentFileRemoteIndex = 0;
    uint16_t currentFileRemoteCommandIndex = 0;
    SelectionModeEnum currentSelectedMode;

    bool fileSelectionFirstRun = true;

    models::Manufacturer currentSelectedManufacturer;
    models::Product currentSelectedProduct;
    models::Remote currentSelectedRemote;
    models::RemoteCommand currentSelectedRemoteCommand;
    models::FileRemote currentSelectedFileRemote;
    models::FileRemoteCommand currentSelectedFileRemoteCommand;
    std::string currentSelectedFilePath = "/";
};

} // namespace contexts

#endif // SELECTION_CONTEXT_H
