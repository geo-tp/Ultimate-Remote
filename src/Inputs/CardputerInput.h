#ifndef CARDPUTER_KEYBOARD_INPUT_H
#define CARDPUTER_KEYBOARD_INPUT_H

#include <M5Cardputer.h>
#include <map> 
#include <Models/RemoteCommand.h>
#include <Utils/StringUtils.h>

#define KEY_OK '\n'
#define KEY_DEL '\b'
#define KEY_NONE '\0'
#define KEY_RETURN '\r'
#define KEY_ARROW_UP ';'
#define KEY_ARROW_DOWN '.'
#define KEY_ARROW_LEFT ','
#define KEY_ARROW_RIGHT '/'
#define KEY_FAVORITE ' ' // space

using namespace models;

namespace inputs {


class CardputerInput {
public:
    char handler();
    std::vector<char> getCommandMappings(const std::vector<RemoteCommand>& commands);

private:

    char assignableKeys[42] = {
        'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
        'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
        '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '-', '=', '[',
        ']', '\\', '`'
    };

    // Define some common key mapping
    std::map<std::string, char> keyMappings = {
        {"keypower", 'p'},
        {"power", 'p'},
        {"poweron", 'p'},
        {"keysleep", 'b'},
        {"sleep", 'b'},
        {"hdmi", 'h'},
        {"option", 'i'},
        {"keyplaypause", 'u'},
        {"keyexit", '\\'},
        {"keymode", 'i'},
        {"timer", 't'},
        {"keymenu", '`'},
        {"key1", '1'},
        {"key2", '2'},
        {"key3", '3'},
        {"key4", '4'},
        {"key5", '5'},
        {"key6", '6'},
        {"key7", '7'},
        {"key8", '8'},
        {"key9", '9'},
        {"key0", '0'},
        {"1", '1'},
        {"2", '2'},
        {"3", '3'},
        {"4", '4'},
        {"5", '5'},
        {"6", '6'},
        {"7", '7'},
        {"8", '8'},
        {"9", '9'},
        {"0", '0'},
        {"keyok", 'o'},
        {"keychanneldown", '['},
        {"keychannelup", ']'},
        {"keyvolumeup", '='},
        {"keyvolumedown", '-'},
        {"volumeup", '='},
        {"volumedown", '-'},
        {"keyup", 'e'},
        {"keydown", 's'},
        {"keyleft", 'a'},
        {"keyright", 'd'},
        {"keymute", 'm'},
        {"keyplay", 'k'},
        {"keystop", 'l'},
        {"keypause", 'j'},
        {"keyforward", 'f'},
        {"keyrewind", 'w'}
    };
};

}


#endif // KEYBOARD_H
