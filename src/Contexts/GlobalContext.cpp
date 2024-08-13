#include "GlobalContext.h"

namespace contexts {

// Initialize the singleton instance
GlobalContext& GlobalContext::getInstance() {
    static GlobalContext instance;
    return instance;
}

GlobalContext::GlobalContext()
    : appName("Ultimate Remote"),
      maxFavorites(80),
      protocolCount(62),
      manufacturerCount(634),
      maxInputCharCount(14),
      ledPin(21),
      irTxPin(44),
      sdCardCSPin(12),
      sdCardMISOPin(39),
      sdCardMOSIPin(14),
      sdCardCLKPin(40),
      fileCacheLimit(20),
      fileCountLimit(256) {
}

std::string GlobalContext::getAppName() const {
    return appName;
}

void GlobalContext::setAppName(const std::string& appName) {
    this->appName = appName;
}

int GlobalContext::getMaxFavorites() const {
    return maxFavorites;
}

void GlobalContext::setMaxFavorites(int maxFavorites) {
    this->maxFavorites = maxFavorites;
}

int GlobalContext::getProtocolCount() const {
    return protocolCount;
}

void GlobalContext::setProtocolCount(int protocolCount) {
    this->protocolCount = protocolCount;
}

int GlobalContext::getManufacturerCount() const {
    return manufacturerCount;
}

void GlobalContext::setManufacturerCount(int manufacturerCount) {
    this->manufacturerCount = manufacturerCount;
}

int GlobalContext::getMaxInputCharCount() const {
    return maxInputCharCount;
}

void GlobalContext::setMaxInputCharCount(int maxInputCharCount) {
    this->maxInputCharCount = maxInputCharCount;
}

int GlobalContext::getLedPin() const {
    return ledPin;
}

void GlobalContext::setLedPin(int ledPin) {
    this->ledPin = ledPin;
}

int GlobalContext::getIrTxPin() const {
    return irTxPin;
}

void GlobalContext::setIrTxPin(int irTxPin) {
    this->irTxPin = irTxPin;
}

// Getters and setters for SD card pins
int GlobalContext::getSdCardCSPin() const {
    return sdCardCSPin;
}

void GlobalContext::setSdCardCSPin(int sdCardCSPin) {
    this->sdCardCSPin = sdCardCSPin;
}

int GlobalContext::getSdCardMISOPin() const {
    return sdCardMISOPin;
}

void GlobalContext::setSdCardMISOPin(int sdCardMISOPin) {
    this->sdCardMISOPin = sdCardMISOPin;
}

int GlobalContext::getSdCardMOSIPin() const {
    return sdCardMOSIPin;
}

void GlobalContext::setSdCardMOSIPin(int sdCardMOSIPin) {
    this->sdCardMOSIPin = sdCardMOSIPin;
}

int GlobalContext::getSdCardCLKPin() const {
    return sdCardCLKPin;
}

void GlobalContext::setSdCardCLKPin(int sdCardCLKPin) {
    this->sdCardCLKPin = sdCardCLKPin;
}

// Getters and setters for file cache and count limits
int GlobalContext::getFileCacheLimit() const {
    return fileCacheLimit;
}

void GlobalContext::setFileCacheLimit(int fileCacheLimit) {
    this->fileCacheLimit = fileCacheLimit;
}

int GlobalContext::getFileCountLimit() const {
    return fileCountLimit;
}

void GlobalContext::setFileCountLimit(int fileCountLimit) {
    this->fileCountLimit = fileCountLimit;
}

}
