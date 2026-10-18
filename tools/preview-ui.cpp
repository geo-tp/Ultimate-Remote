#include <Platform/Linux/LinuxFramebufferView.h>
#include <Data/AppInfo.h>
#include <Repositories/ManufacturerRepository.h>
#include <filesystem>
#include <iostream>
#include <stdexcept>

int main(int argc, char** argv) {
    if (argc != 2) return 2;
    std::filesystem::path output = argv[1];
    std::filesystem::create_directories(output);
    const auto info = data::getAppInfoPages(repositories::ManufacturerRepository::getEmbeddedRemoteCount(),
                                           repositories::ManufacturerRepository::getEmbeddedManufacturerCount());
    for (int depth : {16, 24, 32}) {
        platform::linux::LinuxFramebufferView view;
        view.initialisePreview(depth);
        auto save = [&](const std::string& name) {
            if (!view.savePreview((output / (name + "-" + std::to_string(depth) + ".ppm")).string()))
                throw std::runtime_error("Cannot write screenshot");
        };
        const std::vector<std::string> modes = {"REMOTES", "SCANNER", "FAVORITES", "ONLINE IR", "SETTINGS", "INFO"};
        for (size_t i = 0; i < modes.size(); ++i) {
            view.displayTopBar("Ultimate Remote");
            view.displaySelection(modes, i);
            save("home-" + std::to_string(i));
        }
        view.displaySelection(modes, 0);
        view.displayExitHold(1500);
        save("exit-hold");
        view.displaySelection(modes, 0);
        save("exit-cancelled");
        for (size_t i = 0; i < info.size(); ++i) {
            view.displayInfoPage(info[i].title, info[i].lines, i, info.size());
            save("info-" + std::to_string(i));
        }
        view.displayTopBar("", true, true);
        view.displaySelection({"Akai", "Apple", "Denon", "LG", "Panasonic", "Philips", "Samsung", "Sony", "Yamaha"}, 7);
        save("manufacturers");
        view.displayTopBar("Sony", true);
        view.displaySelection({"26,42.csv - TV", "84,-1.csv - TV", "1,-1.csv - TV", "58,9.csv - DVD Player", "164,-1.csv - DVD Player"}, 3);
        save("remotes");
        view.displayTopBar("Remote 84,-1.csv", true);
        view.displaySelection({"KEY_POWER", "KEY_VOLUMEUP", "KEY_VOLUMEDOWN", "KEY_MUTE", "A_VERY_LONG_COMMAND_NAME_TO_CHECK_CLIPPING"},
                              2, {"p", "Fn+D", "Fn+S", "Fn+A", "b"});
        save("commands");
        view.displaySelection({"KEY_UP", "KEY_DOWN", "KEY_RIGHT", "KEY_LEFT"}, 0,
                              {"Fn+F", "Fn+X", "Fn+C", "Fn+Z"});
        save("command-directions");
        view.displaySelection({"PLAY", "REWIND", "FORWARD"}, 0, {"Fn+Q", "Fn+W", "Fn+E"});
        save("command-playback");
        view.displaySelection({"A_VERY_LONG_COMMAND_NAME_TO_CHECK_CLIPPING"}, 65535, {"Fn+D"});
        save("long-command");
        view.displayTopBar("zzzz", true, true);
        view.displaySelection({}, 0);
        save("empty");
        view.displayScanSelection("Sony");
        view.displayScanRemote("DVD Player / 58,9.csv");
        view.displayScanCommand("KEY_POWER");
        save("scan");
        view.displayScanOver();
        save("scan-complete");
        view.displayStringPrompt("Type a favorite name", "Living room");
        save("favorite");
        view.displayConfirmationPrompt("Delete this favorite?");
        save("confirmation");
        view.displayTopBar("Flipper-IRDB", true, true);
        view.displaySelection({"ACs", "Audio and Video Receivers", "TVs", "Refresh catalog"}, 2,
                              {"580 IR", "230 IR", "1800 IR", "Sync"});
        save("irdb-categories");
        view.displayTopBar("TVs / Samsung", true, true);
        view.displaySelection({"Samsung BN59-01175N", "Samsung BN59-01247A", "Samsung Smart TV"}, 0,
                              {"Saved", "Fetch", "Fetch"});
        save("irdb-remotes");
        view.displayMessage("Loading remote", {"Samsung BN59-01175N", "Please wait"}, true);
        save("irdb-download");
        view.displayMessage("IR database unavailable", {"Check Wi-Fi and retry", "Enter to continue"});
        save("irdb-offline");
        view.displayLoading();
        save("loading");
    }
    std::cout << "Rendered all screens in RGB565, RGB888 and XRGB8888.\n";
}
