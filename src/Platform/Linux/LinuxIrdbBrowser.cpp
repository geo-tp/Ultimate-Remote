#ifndef ARDUINO
#include "LinuxIrdbBrowser.h"
#include <Inputs/InputKeys.h>
#include <Inputs/ScopedTextInput.h>
#include <Inputs/ScopedZBack.h>
#include <Selections/ConfirmationSelection.h>
#include <algorithm>
#include <cerrno>
#include <chrono>
#include <csignal>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <fcntl.h>
#include <map>
#include <sstream>
#include <sys/prctl.h>
#include <sys/wait.h>
#include <unistd.h>

namespace platform::linux {
namespace {
bool back(char key) { return key == KEY_RETURN; }
std::string friendly(std::string text) {
    std::replace(text.begin(), text.end(), '_', ' ');
    if (text.size() > 3 && text.substr(text.size() - 3) == ".ir") text.resize(text.size() - 3);
    return text;
}
std::string lower(std::string text) {
    for (char& c : text) if (c >= 'A' && c <= 'Z') c += 'a' - 'A';
    return text;
}
namespace fs = std::filesystem;

fs::path downloadedDir() {
    if (const char* overridePath = std::getenv("ULTIMATE_REMOTE_IRDB_DOWNLOAD_DIR"); overridePath && *overridePath)
        return fs::path(overridePath);
    if (const char* xdg = std::getenv("XDG_DATA_HOME"); xdg && *xdg)
        return fs::path(xdg) / "ultimate-remote" / "downloaded";
    if (const char* home = std::getenv("HOME"); home && *home)
        return fs::path(home) / ".local" / "share" / "ultimate-remote" / "downloaded";
    return {};
}

// Untrusted catalog paths must never escape the downloaded folder.
bool safeRemotePath(const fs::path& path) {
    if (path.empty() || path.is_absolute() || path.extension() != ".ir") return false;
    for (const auto& part : path) {
        if (part == "." || part == ".." || part.empty()) return false;
        for (const unsigned char c : part.string())
            if (c < 32 || c == 127) return false;
    }
    return true;
}

fs::path favoritesFile() {
    const auto dir = downloadedDir();
    return dir.empty() ? fs::path{} : dir.parent_path() / "favorites.txt";
}

std::string errorMessage(const std::string& output) {
    const auto start = output.find("ERROR\t");
    if (start == std::string::npos) return "Cannot load IR database. Please retry";
    const auto end = output.find('\n', start);
    return output.substr(start + 6, end == std::string::npos ? end : end - start - 6);
}
}

bool LinuxIrdbBrowser::readDownloaded(const std::string& name, std::string& content) {
    const fs::path relative(name), root = downloadedDir();
    if (root.empty() || !safeRemotePath(relative)) return false;
    std::ifstream in(root / relative, std::ios::binary);
    if (!in) return false;
    in.seekg(0, std::ios::end);
    const auto length = in.tellg();
    if (length < 0 || length > 16 * 1024 * 1024) return false;
    in.seekg(0);
    content.assign(std::istreambuf_iterator<char>(in), {});
    return in.good() || in.eof();
}

bool LinuxIrdbBrowser::saveDownloaded(const std::string& name, const std::string& content) {
    const fs::path relative(name), root = downloadedDir();
    if (root.empty() || !safeRemotePath(relative) || content.size() > 16 * 1024 * 1024) return false;
    std::error_code error;
    fs::create_directories((root / relative).parent_path(), error);
    if (error) return false;
    const fs::path target = root / relative;
    std::ofstream out(target.string() + ".tmp", std::ios::binary | std::ios::trunc);
    if (!out) return false;
    out.write(content.data(), static_cast<std::streamsize>(content.size()));
    out.close();
    if (!out) return false;
    fs::rename(target.string() + ".tmp", target, error);
    if (error) { fs::remove(target.string() + ".tmp"); return false; }
    return true;
}

void LinuxIrdbBrowser::loadDownloaded() {
    const fs::path root = downloadedDir();
    if (root.empty()) return;
    std::error_code error;
    if (!fs::is_directory(root, error)) return;
    fs::recursive_directory_iterator it(root, fs::directory_options::skip_permission_denied, error), end;
    for (; !error && it != end; it.increment(error)) {
        if (!it->is_regular_file(error)) continue;
        const fs::path relative = fs::relative(it->path(), root, error);
        if (error || !safeRemotePath(relative)) continue;
        const std::string name = relative.generic_string();
        auto found = std::find_if(remotes.begin(), remotes.end(), [&](const Remote& r) { return r.path == name; });
        if (found == remotes.end()) remotes.push_back({name, true});
        else found->saved = true;
    }
}

bool LinuxIrdbBrowser::isFavorite(const std::string& path) const {
    return std::find(favoritePaths.begin(), favoritePaths.end(), path) != favoritePaths.end();
}

void LinuxIrdbBrowser::loadFavorites() {
    favoritePaths.clear();
    const auto file = favoritesFile();
    if (file.empty()) return;
    std::ifstream in(file);
    std::string line;
    while (favoritePaths.size() < 2048 && std::getline(in, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        if (!safeRemotePath(fs::path(line)) || isFavorite(line)) continue;
        favoritePaths.push_back(line);
    }
}

bool LinuxIrdbBrowser::saveFavorites() const {
    const auto file = favoritesFile();
    if (file.empty()) return false;
    std::error_code error;
    fs::create_directories(file.parent_path(), error);
    if (error) return false;
    const auto temp = fs::path(file.string() + ".tmp");
    std::ofstream out(temp, std::ios::binary | std::ios::trunc);
    if (!out) return false;
    for (const auto& path : favoritePaths) out << path << '\n';
    out.flush();
    if (!out) { out.close(); fs::remove(temp, error); return false; }
    out.close();
    fs::rename(temp, file, error);
    if (error) { fs::remove(temp); return false; }
    return true;
}

bool LinuxIrdbBrowser::toggleFavorite(const Entry& remote) {
    if (remote.folder || remote.refresh || !safeRemotePath(fs::path(remote.path))) return false;
    // Removing a favorite never deletes the locally downloaded .ir file.
    if (isFavorite(remote.path)) {
        selections::ConfirmationSelection confirmation(display, input);
        if (!confirmation.select("Delete favorite: " + remote.label, true)) return false;
        const auto old = favoritePaths;
        favoritePaths.erase(std::remove(favoritePaths.begin(), favoritePaths.end(), remote.path), favoritePaths.end());
        if (!saveFavorites()) { favoritePaths = old; notice("Save failed", "Cannot update IR favorites"); return false; }
        return true;
    }
    if (favoritePaths.size() >= 2048) { notice("Favorites full", "Too many favorites"); return false; }
    // Store the actual .ir data locally so Favorites remains functional offline.
    std::string content;
    if (!readDownloaded(remote.path, content)) {
        const auto result = request("fetch", remote.path);
        if (result.cancelled) return false;
        if (!result.ok) { notice("Cannot favorite", errorMessage(result.output)); return false; }
        content = result.output;
    }
    if (files.getRemoteFromFile(remote.label, content).commands.empty()) {
        notice("Cannot favorite", "This .ir file has no supported commands");
        return false;
    }
    if (!saveDownloaded(remote.path, content)) {
        notice("Save failed", "Cannot cache IR remote for offline use");
        return false;
    }
    auto found = std::find_if(remotes.begin(), remotes.end(), [&](const Remote& r) {
        return r.path == remote.path;
    });
    if (found != remotes.end()) found->saved = true;
    else remotes.push_back({remote.path, true});
    favoritePaths.push_back(remote.path);
    if (!saveFavorites()) {
        favoritePaths.pop_back();
        notice("Save failed", "Cannot update IR favorites");
        return false;
    }
    return true;
}

LinuxIrdbBrowser::Result LinuxIrdbBrowser::request(const std::string& action, const std::string& path, bool refresh) {
    display.displayMessage(action == "catalog" ? "Flipper-IRDB" : "Loading remote",
                           {action == "catalog" ? "Loading the catalog..." : friendly(path.substr(path.find_last_of('/') + 1)),
                            "Please wait"}, true);
    int channel[2];
    if (pipe2(channel, O_CLOEXEC) < 0) return {false, false, "ERROR\tCannot start download"};
    const char* configured = std::getenv("ULTIMATE_REMOTE_IRDB_HELPER");
    const std::string helper = configured && *configured ? configured : "/usr/lib/ultimate-remote/irdb-client.py";
    const pid_t parent = getpid();
    const pid_t child = fork();
    if (child == 0) {
        prctl(PR_SET_PDEATHSIG, SIGTERM);
        if (getppid() != parent) _exit(1);
        dup2(channel[1], STDOUT_FILENO);
        dup2(channel[1], STDERR_FILENO);
        const int nullInput = open("/dev/null", O_RDONLY);
        if (nullInput >= 0) dup2(nullInput, STDIN_FILENO);
        close(channel[0]);
        close(channel[1]);
        execl("/usr/bin/python3", "python3", helper.c_str(), action.c_str(), path.c_str(),
              refresh ? "--refresh" : static_cast<const char*>(nullptr), static_cast<const char*>(nullptr));
        _exit(127);
    }
    close(channel[1]);
    if (child < 0) { close(channel[0]); return {false, false, "ERROR\tCannot start download"}; }
    fcntl(channel[0], F_SETFL, O_NONBLOCK);
    Result result;
    int status = 0;
    const auto start = std::chrono::steady_clock::now();
    while (true) {
        char buffer[8192];
        ssize_t count;
        while ((count = read(channel[0], buffer, sizeof(buffer))) > 0) {
            result.output.append(buffer, static_cast<size_t>(count));
            if (result.output.size() > 16 * 1024 * 1024) break;
        }
        const pid_t done = waitpid(child, &status, WNOHANG);
        if (done == child) {
            // Drain bytes written between the last read and child termination.
            while ((count = read(channel[0], buffer, sizeof(buffer))) > 0 && result.output.size() <= 16 * 1024 * 1024)
                result.output.append(buffer, static_cast<size_t>(count));
            result.ok = WIFEXITED(status) && WEXITSTATUS(status) == 0 && result.output.size() <= 16 * 1024 * 1024;
            break;
        }
        if (done < 0 && errno != EINTR) break;
        const bool timeout = std::chrono::steady_clock::now() - start > std::chrono::seconds(35);
        if (result.output.size() > 16 * 1024 * 1024 || timeout) {
            kill(child, SIGKILL);
            while (waitpid(child, &status, 0) < 0 && errno == EINTR) {}
            result.output = "ERROR\tDownload failed. Check connection and retry";
            break;
        }
        if (back(input.handler())) {
            kill(child, SIGKILL);
            while (waitpid(child, &status, 0) < 0 && errno == EINTR) {}
            result.cancelled = true;
            break;
        }
        usleep(5000);
    }
    close(channel[0]);
    return result;
}

void LinuxIrdbBrowser::notice(const std::string& title, const std::string& message) {
    display.displayMessage(title, {message, "OK to continue"});
    while (true) {
        const char key = input.handler();
        if (key == KEY_OK || back(key)) return;
    }
}

bool LinuxIrdbBrowser::load(bool refresh) {
    const auto result = request("catalog", "", refresh);
    if (result.cancelled) return false;
    if (!result.ok) { notice("IR database unavailable", errorMessage(result.output)); return false; }
    std::istringstream stream(result.output);
    std::string line;
    if (!std::getline(stream, line) || line.compare(0, 3, "OK\t") != 0) {
        notice("IR database unavailable", "Invalid catalog response");
        return false;
    }
    const std::string status = line.substr(3);
    std::vector<Remote> loaded;
    while (std::getline(stream, line)) {
        const auto tab = line.find('\t');
        if (tab != std::string::npos) loaded.push_back({line.substr(0, tab), line.substr(tab + 1) == "Saved"});
    }
    if (loaded.empty()) { notice("IR database unavailable", "The catalog has no remotes"); return false; }
    remotes = std::move(loaded);
    catalogLoaded = true;
    loadDownloaded();
    if (status != "Saved catalog" && status != "Catalog updated") notice("Saved catalog", status);
    return true;
}

std::vector<LinuxIrdbBrowser::GlobalFavorite> LinuxIrdbBrowser::globalFavorites() {
    loadFavorites();
    std::vector<GlobalFavorite> result;
    for (const auto& path : favoritePaths) {
        if (!safeRemotePath(fs::path(path))) continue;
        const fs::path filename(path);
        result.push_back({path, friendly(filename.filename().string())});
    }
    return result;
}

models::FileRemote LinuxIrdbBrowser::openGlobalFavorite(const std::string& path) {
    if (!safeRemotePath(fs::path(path)) || !isFavorite(path)) return {};
    std::string content;
    if (!readDownloaded(path, content)) return {};
    return files.getRemoteFromFile(friendly(fs::path(path).filename().string()), content);
}

bool LinuxIrdbBrowser::removeGlobalFavorite(const std::string& path) {
    if (!isFavorite(path)) return false;
    const auto old = favoritePaths;
    favoritePaths.erase(std::remove(favoritePaths.begin(), favoritePaths.end(), path), favoritePaths.end());
    if (!saveFavorites()) { favoritePaths = old; return false; }
    return true;
}

std::vector<LinuxIrdbBrowser::Entry> LinuxIrdbBrowser::entries(const Page& page) const {
    if (page.source == Page::Source::Home) return {
        {"@downloaded", "Downloaded", "Saved .ir files", true, false},
        {"@online", "Online database", "Browse and download", true, false}
    };
    const std::string prefix = page.path.empty() ? "" : page.path + "/";
    std::map<std::string, size_t> folders;
    std::vector<Entry> found;
    for (const auto& remote : remotes) {
        if (page.source == Page::Source::Downloaded && !remote.saved) continue;
        if (remote.path.compare(0, prefix.size(), prefix) != 0) continue;
        const std::string tail = remote.path.substr(prefix.size());
        const auto slash = tail.find('/');
        if (slash != std::string::npos) ++folders[tail.substr(0, slash)];
        else found.push_back({remote.path, friendly(tail), isFavorite(remote.path) ? "Fav" : (remote.saved ? "Saved" : "Fetch"), false, false});
    }
    for (const auto& folder : folders)
        found.push_back({prefix + folder.first, friendly(folder.first), std::to_string(folder.second) + " IR", true, false});
    std::sort(found.begin(), found.end(), [](const Entry& a, const Entry& b) {
        if (a.folder != b.folder) return a.folder;
        return lower(a.label) < lower(b.label);
    });
    const std::string query = lower(page.query);
    found.erase(std::remove_if(found.begin(), found.end(), [&](const Entry& entry) {
        return lower(entry.label).find(query) == std::string::npos;
    }), found.end());
    if (page.path.empty() && page.query.empty() && page.source == Page::Source::Online)
        found.push_back({"", "Refresh online catalog", "Sync", false, true});
    return found;
}

models::FileRemote LinuxIrdbBrowser::select() {
    inputs::ScopedZBack zBack(input);
    if (pages.empty()) {
        pages.push_back({});
        loadFavorites();
        loadDownloaded();
        // Ensure every favorite appears, even before fetching the remote catalog.
        for (const auto& path : favoritePaths) {
            if (std::find_if(remotes.begin(), remotes.end(), [&](const Remote& r) { return r.path == path; }) == remotes.end())
                remotes.push_back({path, false});
        }
    }
    while (!pages.empty()) {
        auto options = entries(pages.back());
        auto& page = pages.back();
        if (page.selected >= options.size()) page.selected = 0;
        std::vector<std::string> labels, details;
        for (const auto& option : options) { labels.push_back(option.label); details.push_back(option.detail); }
        const std::string title = page.source == Page::Source::Home ? "Online IR" :
            (page.path.empty() ? (page.source == Page::Source::Downloaded ? "Downloaded" : "Online database") : friendly(page.path));
        display.displayTopBar(page.query.empty() ? title : "Find: " + page.query,
                              true, page.source != Page::Source::Home);
        display.displaySelection(labels, page.selected, details);
        char key;
        {
            inputs::ScopedTextInput typing(input);
            do { key = input.handler(); } while (key == KEY_NONE);
        }
        if (back(key)) { pages.pop_back(); continue; }
        if (key == KEY_FAVORITE && page.source != Page::Source::Home && !options.empty() &&
            !options[page.selected].folder && !options[page.selected].refresh) {
            toggleFavorite(options[page.selected]);
            continue;
        }
        if (key == KEY_ARROW_UP && !options.empty())
            page.selected = page.selected > 0 ? page.selected - 1 : options.size() - 1;
        else if (key == KEY_ARROW_DOWN && !options.empty()) page.selected = (page.selected + 1) % options.size();
        else if ((key == KEY_OK) && !options.empty()) {
            const Entry option = options[page.selected];
            if (option.refresh) { load(true); continue; }
            if (page.source == Page::Source::Home) {
                const Page::Source source = option.path == "@downloaded" ? Page::Source::Downloaded : Page::Source::Online;
                if (source == Page::Source::Downloaded) {
                    loadDownloaded();
                } else if (!catalogLoaded && !load()) {
                    continue;
                }
                pages.push_back({"", "", 0, source});
                continue;
            }
            if (option.folder) { pages.push_back({option.path, "", 0, page.source}); continue; }
            std::string irText;
            const bool fromDisk = readDownloaded(option.path, irText);
            if (!fromDisk) {
                const auto result = request("fetch", option.path);
                if (result.cancelled) continue;
                if (!result.ok) { notice("Cannot open remote", errorMessage(result.output)); continue; }
                irText = result.output;
            }
            auto remote = files.getRemoteFromFile(option.label, irText);
            if (remote.commands.empty()) { notice("Cannot open remote", "No supported IR commands in this file"); continue; }
            if (!fromDisk && !saveDownloaded(option.path, irText)) {
                notice("Save failed", "Remote opens, but could not be saved to Downloaded");
            }
            for (auto& item : remotes) if (item.path == option.path) item.saved = true;
            return remote;
        } else if (key == KEY_DEL && page.source != Page::Source::Home) {
            if (!page.query.empty()) page.query.pop_back();
            page.selected = 0;
        } else if (page.source != Page::Source::Home && ((key >= 'a' && key <= 'z') || (key >= '0' && key <= '9') ||
                   (key == KEY_FILTER_SPACE && !page.query.empty()) || key == '-' || key == '_')) {
            if (page.query.size() < 64) page.query += (key == KEY_FILTER_SPACE ? ' ' : key);
            page.selected = 0;
        }
    }
    return {};
}
}
#endif
