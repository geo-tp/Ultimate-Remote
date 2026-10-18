#ifdef ARDUINO
#include <Platform/Arduino/ArduinoApplication.h>
void setup() { platform::arduino::applicationSetup(); }
void loop() { platform::arduino::applicationLoop(); }
#else
#include <Platform/Linux/LinuxApplication.h>
int main(int argc, char** argv) { return platform::linux::applicationMain(argc, argv); }
#endif
