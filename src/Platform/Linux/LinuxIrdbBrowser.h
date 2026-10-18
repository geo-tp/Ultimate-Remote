#pragma once
#ifndef ARDUINO
#include <Interfaces/IDisplayView.h>
#include <Interfaces/IInputProvider.h>
#include <Services/FileService.h>
#include <string>
#include <vector>

namespace platform::linux {

class LinuxIrdbBrowser {
public:
    LinuxIrdbBrowser(interfaces::IDisplayView& display, interfaces::IInputProvider& input,
                     services::FileService& files) : display(display), input(input), files(files) {}
    models::FileRemote select();
    struct GlobalFavorite { std::string path, label; };
    std::vector<GlobalFavorite> globalFavorites();
    models::FileRemote openGlobalFavorite(const std::string& path);
    bool removeGlobalFavorite(const std::string& path);

private:
    struct Remote { std::string path; bool saved; };
    struct Entry { std::string path, label, detail; bool folder = false; bool refresh = false; };
    struct Page {
        std::string path, query;
        uint16_t selected = 0;
        enum class Source { Home, Downloaded, Online } source = Source::Home;
    };
    struct Result { bool ok = false, cancelled = false; std::string output; };
    interfaces::IDisplayView& display;
    interfaces::IInputProvider& input;
    services::FileService& files;
    std::vector<Remote> remotes;
    std::vector<std::string> favoritePaths;
    std::vector<Page> pages;
    bool catalogLoaded = false;
    void loadDownloaded();
    void loadFavorites();
    bool saveFavorites() const;
    bool toggleFavorite(const Entry& remote);
    bool isFavorite(const std::string& path) const;
    static bool readDownloaded(const std::string& path, std::string& content);
    static bool saveDownloaded(const std::string& path, const std::string& content);
    Result request(const std::string& action, const std::string& path = "", bool refresh = false);
    bool load(bool refresh = false);
    std::vector<Entry> entries(const Page& page) const;
    void notice(const std::string& title, const std::string& message);
};
}
#endif
