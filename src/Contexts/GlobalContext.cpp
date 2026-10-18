#include "GlobalContext.h"
#include <cerrno>
#include <cstdio>

namespace contexts {

// Singleton instance accessor
GlobalContext& GlobalContext::getInstance() {
    static GlobalContext instance;
    return instance;
}

// Define the private default constructor
GlobalContext::GlobalContext() = default;

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

int GlobalContext::getScreenTimeoutSeconds() const {
    return screenTimeoutSeconds;
}

void GlobalContext::setScreenTimeoutSeconds(int seconds) {
    // Keep the settings file and UI values in sync; ignore invalid values.
    if (seconds == 0 || seconds == 15 || seconds == 30 || seconds == 60 ||
        seconds == 120 || seconds == 300) screenTimeoutSeconds = seconds;
}

std::string GlobalContext::getLircDevicePath() const {
    return lircDevicePath;
}

void GlobalContext::setLircDevicePath(const std::string& lircDevicePath) {
    this->lircDevicePath = lircDevicePath;
}

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

} // namespace contexts

