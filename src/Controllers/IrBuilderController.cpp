#include "IrBuilderController.h"
#include <Inputs/InputKeys.h>
#include <Inputs/ScopedTextInput.h>
#include <Inputs/ScopedLiteralTextInput.h>
#include <Inputs/ScopedZBack.h>
#include <Repositories/ProtocolRepository.h>
#include <Models/RemoteCommand.h>
#include <Enums/ProtocolEnum.h>
#include <algorithm>
#include <cctype>
#include <cstdint>
#include <string>
#include <vector>

namespace controllers {
using namespace repositories;
using namespace models;
using namespace enums;
void IrBuilderController::handle() {
    inputs::ScopedZBack zBack(input);
    // Only protocols backed by the actual MakeHex definitions are offered.
    struct Choice { ProtocolEnum protocol; std::string name; };
    std::vector<Choice> available;
    for (int id = 0; id < static_cast<int>(ProtocolEnum::RAW); ++id) {
        const auto protocol = static_cast<ProtocolEnum>(id);
        const std::string name = ProtocolRepository::getProtocolString(protocol);
        if (name.empty() || name == "unsupported" || name == "raw") continue;
        if (ProtocolRepository::getProtocolByName(name).name[0] == '\0' &&
            name.compare(0, 4, "rc6-") != 0 && name != "zenith" &&
            name != "apple" && name != "SIRC" && name != "SIRC15" &&
            name != "SIRC20" && name != "Samsung32" &&
            name != "NEC42" && name != "NEC42ext") continue;
        if (std::find_if(available.begin(), available.end(), [&](const Choice& item) {
            return item.name == name;
        }) == available.end()) available.push_back({protocol, name});
    }
    if (available.empty()) {
        display.displayMessage("IR Builder", {"No supported protocols"});
        while (input.handler() != KEY_OK) {}
        selectionContext.setIsModeSelected(false);
        return;
    }

    size_t protocolIndex = 0;
    for (size_t i = 0; i < available.size(); ++i)
        if (available[i].name == "nec1") { protocolIndex = i; break; }
    int device = 0, subdevice = -1, command = 0;
    uint16_t selected = 0;
    bool done = false;

    auto selectProtocol = [&]() {
        inputs::ScopedTextInput typing(input);
        std::string query;
        size_t index = 0;
        bool redraw = true;
        while (true) {
            std::vector<size_t> matches;
            std::vector<std::string> choices;
            for (size_t i = 0; i < available.size(); ++i) {
                std::string name = available[i].name;
                std::string lowered = name;
                std::transform(lowered.begin(), lowered.end(), lowered.begin(), [](unsigned char c) { return std::tolower(c); });
                if (lowered.find(query) != std::string::npos) {
                    matches.push_back(i);
                    choices.push_back(name);
                }
            }
            if (index >= matches.size()) index = 0;
            if (redraw) {
                display.displayTopBar(query.empty() ? "IR protocol" : "Find: " + query, true, true);
                display.displaySelection(choices, static_cast<uint16_t>(index));
                redraw = false;
            }
            const char key = input.handler();
            if (key == KEY_RETURN) return;
            if (key == KEY_OK && !matches.empty()) {
                const std::string previous = available[protocolIndex].name;
                protocolIndex = matches[index];
                if (available[protocolIndex].name == "apple" && previous != "apple") {
                    device = 238; subdevice = 0; command = 0;
                } else if ((available[protocolIndex].name == "SIRC20" ||
                            available[protocolIndex].name == "NEC42ext") &&
                           previous != available[protocolIndex].name) {
                    device = 0; subdevice = 0; command = 0;
                }
                return;
            }
            if (key == KEY_ARROW_DOWN && !matches.empty()) {
                index = (index + 1) % matches.size(); redraw = true;
            } else if (key == KEY_ARROW_UP && !matches.empty()) {
                index = index ? index - 1 : matches.size() - 1; redraw = true;
            } else if (key == KEY_DEL && !query.empty()) {
                query.pop_back(); index = 0; redraw = true;
            } else if ((key >= 'a' && key <= 'z') || (key >= '0' && key <= '9') ||
                       ((key == ' ' || key == KEY_FILTER_SPACE) && !query.empty()) || key == '-') {
                if (query.size() < 40) { query += key == KEY_FILTER_SPACE ? ' ' : key; index = 0; redraw = true; }
            }
        }
    };

    auto editNumber = [&](const std::string& title, int& value, bool signedAllowed,
                          int maximum = 32767) {
        inputs::ScopedLiteralTextInput typing(input);
        std::string text;  // Empty input replaces the current value; OK keeps it.
        bool redraw = true;
        while (true) {
            if (redraw) { display.displayStringPrompt(title, text); redraw = false; }
            const char key = input.handler();
            if (key == KEY_RETURN) return;
            if (key == KEY_DEL) {
                if (!text.empty()) { text.pop_back(); redraw = true; }
                continue;
            }
            if (key == KEY_OK) {
                if (text.empty() || text == "-") return;
                try {
                    size_t end = 0;
                    const bool hex = text.size() > 2 && text.compare(0, 2, "0x") == 0;
                    const long next = std::stol(text, &end, hex ? 16 : 10);  // decimal or 0xHEX
                    if (end == text.size() && next >= (signedAllowed ? -1 : 0) && next <= maximum) {
                        value = static_cast<int>(next);
                        return;
                    }
                } catch (...) {}
                display.displayMessage("Invalid value", {
                    "Enter " + std::to_string(signedAllowed ? -1 : 0) +
                    " to " + std::to_string(maximum), "OK to retry"});
                while (input.handler() != KEY_OK) {}
                redraw = true;
                continue;
            }
            if ((key >= '0' && key <= '9') || (key >= 'a' && key <= 'f') || key == 'x') {
                if (text.size() < 8) { text.push_back(key); redraw = true; }
            } else if (key == '-' && signedAllowed && text.empty()) {
                text.push_back(key);
                redraw = true;
            }
        }
    };

    bool redraw = true;
    while (!done) {
        if (redraw) {
        const std::vector<std::string> names = {
            "Protocol: " + available[protocolIndex].name,
            "Device: " + std::to_string(device),
            (available[protocolIndex].name == "apple" ? "Pair ID: " :
                 available[protocolIndex].name == "NEC42ext" ? "Addr high: " : "Subdevice: ") +
                (subdevice < 0 ? std::string("None (-1)") : std::to_string(subdevice)),
            "Command: " + std::to_string(command),
            "SEND IR COMMAND"
        };
        const std::vector<std::string> details = {
            "IRP", "D", "S", "F", "OK"
        };
        display.displayTopBar("IR Builder", true);
        display.displayIrBuilderSelection(names, selected, details);
        redraw = false;
        }
        const char key = input.handler();
        if (key == KEY_RETURN) { done = true; continue; }
        if (key == KEY_ARROW_DOWN) { selected = (selected + 1) % 5; redraw = true; }
        if (key == KEY_ARROW_UP) { selected = selected ? selected - 1 : 4; redraw = true; }
        if (key != KEY_OK) continue;
        redraw = true;
        switch (selected) {
            case 0: selectProtocol(); break;
            case 1: {
                const std::string& protocolName = available[protocolIndex].name;
                const int maximum = protocolName == "apple" || protocolName == "Samsung32" ||
                    protocolName == "SIRC15" ? 255 :
                    protocolName == "SIRC" || protocolName == "SIRC20" ? 31 :
                    protocolName == "NEC42" || protocolName == "NEC42ext" ? 8191 : 32767;
                editNumber("Device (0-" + std::to_string(maximum) + ")", device, false, maximum);
                break;
            }
            case 2: editNumber(available[protocolIndex].name == "apple" ?
                "Apple Pair ID (0-255)" :
                (available[protocolIndex].name == "NEC42ext" ? "Address high (0-8191)" :
                "Subdevice (-1 = none)"), subdevice,
                available[protocolIndex].name != "apple" &&
                available[protocolIndex].name != "NEC42ext",
                available[protocolIndex].name == "apple" ||
                available[protocolIndex].name == "SIRC20" ? 255 :
                available[protocolIndex].name == "NEC42ext" ? 8191 : 32767); break;
            case 3: {
                const std::string& protocolName = available[protocolIndex].name;
                const int maximum = protocolName == "apple" ||
                    protocolName == "SIRC" || protocolName == "SIRC15" ||
                    protocolName == "SIRC20" ? 127 :
                    protocolName == "Samsung32" || protocolName == "NEC42" ? 255 : 32767;
                editNumber("Command (0-" + std::to_string(maximum) + ")", command, false, maximum);
                break;
            }
            case 4: {
                const RemoteCommand custom {"Custom", available[protocolIndex].protocol,
                    static_cast<int16_t>(device), static_cast<int16_t>(subdevice),
                    static_cast<int16_t>(command)};
                const bool sent = infraredService.sendRemoteCommand(custom, "");
                if (sent) ledService.blink();
                // Reuse the transmission feedback view: its success icon is a check
                // rather than the generic information icon used by displayMessage().
                display.displayTransmissionFeedback(
                    available[protocolIndex].name + "  D:" + std::to_string(device) +
                    " S:" + std::to_string(subdevice) + " F:" + std::to_string(command), sent);
                while (true) {
                    const char response = input.handler();
                    if (response == KEY_OK || response == KEY_RETURN) break;
                }
                break;
            }
        }
    }
    selectionContext.setIsModeSelected(false);
}

}
