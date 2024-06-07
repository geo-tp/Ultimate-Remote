#include <IRremote.hpp>
#include <M5Cardputer.h>
#include "ir_manager.h"

IrManager::IrManager() {}

void IrManager::begin() {
    IrSender.begin(IR_TX_PIN);
    FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS); // Builtin
}

void IrManager::sendRemoteCommand(const RemoteCommand& command, const char* protocolString) {
    int frequency = 38; // Default frequency, passed by reference to encodeRemoteCommand
    std::vector<float> sequence = encodeRemoteCommand(command, protocolString, frequency);

    // Convert the sequence to uint16_t format as required by sendRaw
    uint16_t raw[sequence.size()];
    for (size_t i = 0; i < sequence.size(); ++i) {
        raw[i] = static_cast<uint16_t>(sequence[i]);
    }

    // Send the raw generated sequence with the correct frequency
    IrSender.sendRaw(raw, sequence.size(), frequency);

    // Led
    leds[0] = CRGB::OrangeRed;
    FastLED.show();      // led on
    delay(10);
    FastLED.clear(true); // led off
}

std::vector<float> IrManager::encodeRemoteCommand(const RemoteCommand& cmd, const char* protocolString, int& frequency) {

    // Adapted from
    // https://github.com/probonopd/MakeHex/tree/master
    // By John Fine

    char irp[512] = ""; // IRP string, will contains all necessary infos to generate the IR sequence

    // Device
    if (cmd.subdevice >= 0)
        snprintf(irp, sizeof(irp), "Device=%d.%d\nFunction=%d\n", cmd.device, cmd.subdevice, cmd.function);
    else
        snprintf(irp, sizeof(irp), "Device=%d\nFunction=%d\n", cmd.subdevice, cmd.function);

    // Protocol
    int p = -1;
    for (int i = 0; i < KNOWN_PROTOCOL_COUNT; i++) {
        if (strcmp(protocolString, protdefs[i].prot) == 0) {
            p = i;
            break;
        }
    }
    if (p < 0) {
        // Protocol not found, try for special protocols
        int M = 0;
        int L = 0;
        char tempProt[100];
        strncpy(tempProt, protocolString, sizeof(tempProt));
        tempProt[sizeof(tempProt) - 1] = '\0';
        for (int i = 0; i < strlen(tempProt); i++)
            tempProt[i] = toupper(tempProt[i]); // Convert to uppercase

        if (sscanf(tempProt, "RC6-%d-%d", &M, &L) == 2) {
            char temp[512];
            snprintf(temp, sizeof(temp), "Define M=%d\nDefine L=%d\n", M, L);
            strncat(irp, temp, sizeof(irp) - strlen(irp) - 1);
            protocolString = "rc6-M-L";
        } else if (strcmp("NEC", tempProt) == 0) {
            protocolString = "nec2";
        } else if (strcmp("NECX", tempProt) == 0) {
            protocolString = "NECx2";
        }

        // Search again for protocol
        for (int i = 0; i < KNOWN_PROTOCOL_COUNT; i++) {
            if (strcmp(protocolString, protdefs[i].prot) == 0) {
                p = i;
                break;
            }
        }

        // Default NEC2 if no protocol found
        // NEC2 for reliabity which continuously sends the full command frame
        if (p < 0) {
            protocolString = "nec2";
            for (int i = 0; i < KNOWN_PROTOCOL_COUNT; i++) {
                if (strcmp(protocolString, protdefs[i].prot) == 0) {
                    p = i;
                    break;
                }
            }
        }  
    }

    strncat(irp, protdefs[p].def, sizeof(irp) - strlen(irp) - 1);

    // Frequency
    const char* freqStr = strstr(protdefs[p].def, "Frequency=");
    if (freqStr != nullptr) {
        sscanf(freqStr, "Frequency=%d", &frequency);
        frequency = frequency / 1000; // Convert to kHz for sendRaw
    }

    // Validate IRP string
    IRP Irp;
    if (!Irp.readIrpString(irp)) {
        // Bad IRP
        return {};
    }

    // Set values
    Irp.m_value['D' - 'A'] = cmd.device;
    Irp.m_value['S' - 'A'] = cmd.subdevice;
    Irp.m_value['F' - 'A'] = cmd.function;
    Irp.m_value['N' - 'A'] = -1;

    // Generate IR sequence
    int s, r;
    float seq[256];
    Irp.generate(&s, &r, seq);

    // Convert to vector for easier handling
    std::vector<float> result(seq, seq + 2 * (s + r));
    
    return result;
}