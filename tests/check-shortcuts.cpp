#include <Interfaces/IInputProvider.h>
#include <Inputs/InputKeys.h>
#include <Inputs/ScopedCommandInput.h>
#include <Inputs/ScopedZBack.h>
#include <Inputs/CommandShortcuts.h>
#include <Models/FileRemoteCommand.h>
#include <Models/RemoteCommand.h>
#include <cassert>
#include <iostream>
#include <set>

struct Input : interfaces::IInputProvider {
    char handler() override { return KEY_NONE; }
    bool supportsModifiedCommandKeys() const override { return true; }
};

int main() {
    Input input;
    inputs::ScopedZBack back(input);
    const std::vector<const char*> names = {
        "KEY_VOLUMEDOWN", "VOL +", "Mute", "KEY_PLAY", "Rewind <<", "Fast Forward >>",
        "KEY_UP", "Down", "Right", "Left"
    };
    std::vector<models::RemoteCommand> builtin;
    std::vector<models::FileRemoteCommand> online;
    for (const char* name : names) {
        models::RemoteCommand a {};
        a.functionName = name;
        builtin.push_back(a);
        models::FileRemoteCommand b;
        b.functionName = name;
        online.push_back(b);
    }
    const std::vector<std::string> expected = {
        "Fn+S", "Fn+D", "Fn+A", "Fn+Q", "Fn+W", "Fn+E", "Fn+F", "Fn+X", "Fn+C", "Fn+Z"
    };
    assert(inputs::getCommandLabels(inputs::getCommandMappings(builtin, input)) == expected);
    assert(inputs::getCommandLabels(inputs::getCommandMappings(online, input)) == expected);

    // No transport command may steal the rewind/forward key just by coming first.
    online.clear();
    for (const char* name : {"KEY_PREVIOUS", "Next Track", "PLAY/PAUSE", "Rewind", "Forward", "Play", "Mute", "KEY_MUTE"}) {
        models::FileRemoteCommand command;
        command.functionName = name;
        online.push_back(command);
    }
    const auto keys = inputs::getCommandMappings(online, input);
    assert(keys[3] == 'W' && keys[4] == 'E' && keys[5] == 'Q' && keys[6] == 'A');
    assert(keys[0] != 'W' && keys[1] != 'E' && keys[2] != 'Q' && keys[7] != 'A');
    assert(std::set<char>(keys.begin(), keys.end()).size() == keys.size());
    online.resize(3);
    assert(inputs::getCommandLabels(inputs::getCommandMappings(online, input)) ==
           std::vector<std::string>({"Fn+W", "Fn+E", "Fn+Q"}));
    for (const char* name : {"Replay", "Channel Previous", "PowerOff", "Left Input"})
        assert(inputs::modifiedCommandShortcut(name).first == KEY_NONE);

    assert(!input.isCommandInputEnabled());
    {
        inputs::ScopedCommandInput commands(input);
        assert(input.isCommandInputEnabled());
        { inputs::ScopedCommandInput nested(input); }
        assert(input.isCommandInputEnabled());
    }
    assert(!input.isCommandInputEnabled());
    std::cout << "Built-in/file mappings, labels, aliases, transport fallback and collision checks passed.\n";
}
