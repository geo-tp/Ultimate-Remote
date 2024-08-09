#include <M5Cardputer.h>
#include <Views/CardputerView.h>
#include <Inputs/CardputerInput.h>
#include <Controllers/AppController.h>

using namespace controllers;

CardputerView display;
CardputerInput input;
AppController controller(display, input);

void setup() {
    auto cfg = M5.config();
    M5Cardputer.begin(cfg, true);

    CardputerView display = CardputerView();
    CardputerInput input = CardputerInput();

    AppController controller = AppController(display, input);
    controller.setup();
}

void loop() {
    controller.run();
}
