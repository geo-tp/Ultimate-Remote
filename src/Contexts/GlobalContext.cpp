#include "GlobalContext.h"

namespace contexts {

// Initialize the singleton instance
GlobalContext& GlobalContext::getInstance() {
    static GlobalContext instance;
    return instance;
}

// Private constructor with default values
GlobalContext::GlobalContext()
    : appName("Ultimate Remote"),
      maxFavorites(80),
      protocolCount(62),
      manufacturerCount(634),
      maxInputCharCount(14),
      ledPin(21),
      irTxPin(44) {
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

// Getters and Setters for irTxPin
int GlobalContext::getIrTxPin() const {
    return irTxPin;
}

void GlobalContext::setIrTxPin(int irTxPin) {
    this->irTxPin = irTxPin;
}


}
