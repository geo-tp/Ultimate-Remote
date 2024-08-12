#include "CardputerInput.h"

namespace inputs {


char CardputerInput::handler() {

    // Update keyboard state
    M5Cardputer.update();

    // Bouton GO
    if (M5Cardputer.BtnA.isPressed()) {
        delay(150); // debounce
        return KEY_RETURN;
    }
    
    if (M5Cardputer.Keyboard.isChange()) {

        if (M5Cardputer.Keyboard.isPressed()) {
            Keyboard_Class::KeysState status = M5Cardputer.Keyboard.keysState();

            if (status.enter) { // go to next menu
                return KEY_OK;
            }
            if (status.del) { 
                return KEY_DEL;
            }
            
            if(M5Cardputer.Keyboard.isKeyPressed(KEY_ARROW_LEFT)) { // go back to previous menu
                return KEY_RETURN;
            }

            if(M5Cardputer.Keyboard.isKeyPressed(KEY_ARROW_RIGHT)) { // go to next menu
                return KEY_OK;
            }

            for (auto c : status.word) {
                return c; // retourner le premier char saisi
            }
        }
    }
    delay(10); // debounce
    return KEY_NONE;
}


}