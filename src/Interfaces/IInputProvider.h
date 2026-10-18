#ifndef I_INPUT_PROVIDER_H
#define I_INPUT_PROVIDER_H

namespace interfaces {

class IInputProvider {
public:
    virtual ~IInputProvider() = default;
    virtual char handler() = 0;
    // A platform with key-release events may offer a hold-to-exit gesture.
    virtual void setHomeMenuActive(bool active) { (void)active; }
    virtual int exitHoldMilliseconds() const { return -1; }
    virtual bool supportsModifiedCommandKeys() const { return false; }
    // Zero uses Z/C for its horizontal home selector; Cardputer ESP32 uses the four arrow keys.
    virtual bool homeMenuUsesLetterNavigation() const { return true; }
    // Only ESP32 currently needs its local favorites list recreated in-place.
    virtual bool refreshFavoritesAfterDelete() const { return false; }
    // Preserve desktop file-browser filtering; ESP32 ignores an initial space.
    virtual bool allowLeadingFilterSpace() const { return true; }
    // Platform-specific UI keys should not be allocated to IR commands.
    virtual bool isNavigationReservedKey(char key) const { (void)key; return false; }

    bool isCommandInputEnabled() const { return commandInputEnabled; }
    void setCommandInputEnabled(bool enabled) { commandInputEnabled = enabled; }
    bool isLiteralTextInputEnabled() const { return literalTextInputEnabled; }
    void setLiteralTextInputEnabled(bool enabled) { literalTextInputEnabled = enabled; }
    bool isTextInputEnabled() const { return textInputEnabled; }
    void setTextInputEnabled(bool enabled) { textInputEnabled = enabled; }
    bool isZBackEnabled() const { return zBackEnabled; }
    void setZBackEnabled(bool enabled) { zBackEnabled = enabled; }

protected:
    bool commandInputEnabled = false;
    bool textInputEnabled = false;
    bool literalTextInputEnabled = false;
    bool zBackEnabled = false;
};

}

#endif
