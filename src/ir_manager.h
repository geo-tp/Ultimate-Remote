#ifndef IR_MANAGER_H
#define IR_MANAGER_H

#include <FastLED.h>
#include "remote_data.h"
#include "irp.h"

#define IR_TX_PIN  44 // Builtin
#define LED_PIN    21 // Builtin
#define NUM_LEDS   1

class IrManager {
public:
    IrManager();
    void begin();
    void sendRemoteCommand(const RemoteCommand& command, const char* protocolString) ;
private:
    std::vector<float> encodeRemoteCommand(const RemoteCommand& cmd, const char* protocolString, int& frequency);

    CRGB leds[NUM_LEDS]; // Déclaration LED pour la builtin
};
#endif