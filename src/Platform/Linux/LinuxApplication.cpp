#ifndef ARDUINO
#include <Contexts/GlobalContext.h>
#include <Data/AppVersion.h>
#include <Dispatchers/AppDispatcher.h>
#include <Platform/Linux/LinuxFramebufferView.h>
#include <Platform/Linux/LinuxKeyboardInput.h>
#include <Platform/Linux/KernelLircInfraredSender.h>
#include <Platform/Linux/StubInfraredSender.h>
#include <Platform/Linux/LinuxAppPlatform.h>
#include <cstdlib>
#include <csignal>
#include <cstring>
#include <execinfo.h>
#include <iostream>
#include <memory>
#include <vector>
#include <unistd.h>

using namespace dispatchers;
namespace platform::linux {
namespace {

void crashHandler(int signalNumber) {
    const char* title = signalNumber == SIGBUS
        ? "\n[crash] SIGBUS caught\n"
        : "\n[crash] SIGSEGV caught\n";
    ssize_t ignored = write(STDERR_FILENO, title, std::strlen(title));
    (void)ignored;

    void* frames[32];
    int frameCount = backtrace(frames, 32);
    backtrace_symbols_fd(frames, frameCount, STDERR_FILENO);
    _exit(128 + signalNumber);
}

void installCrashDiagnostics() {
    std::signal(SIGSEGV, crashHandler);
    std::signal(SIGBUS, crashHandler);
}

std::vector<uint32_t> makeNecTestPattern() {
    std::vector<uint32_t> raw;
    raw.reserve(67);
    raw.push_back(9000);
    raw.push_back(4500);

    auto appendByte = [&](uint8_t value) {
        for (int bit = 0; bit < 8; ++bit) {
            bool one = ((value >> bit) & 0x01) != 0;
            raw.push_back(560);
            raw.push_back(one ? 1690 : 560);
        }
    };

    appendByte(0x00);
    appendByte(0xff);
    appendByte(0x45);
    appendByte(0xba);
    raw.push_back(560);
    return raw;
}

bool configureLinuxContext(int argc, char** argv) {
    contexts::GlobalContext& context = contexts::GlobalContext::getInstance();
    context.loadSettings();

    if (const char* pin = std::getenv("ULTIMATE_REMOTE_IR_TX_PIN")) {
        context.setIrTxPin(std::atoi(pin));
    }
    if (const char* device = std::getenv("ULTIMATE_REMOTE_LIRC_DEVICE")) {
        context.setLircDevicePath(device);
    }

    for (int i = 1; i < argc; ++i) {
        if (std::strcmp(argv[i], "--ir-tx-pin") == 0 && i + 1 < argc) {
            context.setIrTxPin(std::atoi(argv[++i]));
        } else if (std::strcmp(argv[i], "--lirc-device") == 0 && i + 1 < argc) {
            context.setLircDevicePath(argv[++i]);
        } else if (std::strcmp(argv[i], "--input-device") == 0 && i + 1 < argc) {
            setenv("ULTIMATE_REMOTE_INPUT_DEVICE", argv[++i], 1);
        } else if (std::strcmp(argv[i], "--input-debug") == 0) {
            setenv("ULTIMATE_REMOTE_INPUT_DEBUG", "1", 1);
        } else if (std::strcmp(argv[i], "--ir-test") == 0) {
            setenv("ULTIMATE_REMOTE_IR_TEST", "1", 1);
        } else if (std::strcmp(argv[i], "--version") == 0) {
            std::cout << "Ultimate Remote " << data::AppVersion << "\n";
            return false;
        } else if (std::strcmp(argv[i], "--help") == 0) {
            std::cout << "Ultimate Remote " << data::AppVersion << "\n";
            std::cout << "Usage: ultimate-remote [--ir-tx-pin GPIO] [--lirc-device /dev/lircX] [--input-device /dev/input/eventX] [--input-debug] [--ir-test] [--version]\n";
            std::cout << "Env: ULTIMATE_REMOTE_IR_TX_PIN, ULTIMATE_REMOTE_LIRC_DEVICE, ULTIMATE_REMOTE_IR_STUB=1, ULTIMATE_REMOTE_INPUT_DEVICE, ULTIMATE_REMOTE_IR_DEBUG=1\n";
            return false;
        }
    }

    return true;
}

}

int applicationMain(int argc, char** argv) {
    installCrashDiagnostics();

    if (!configureLinuxContext(argc, argv)) {
        return 0;
    }
    contexts::GlobalContext& context = contexts::GlobalContext::getInstance();

    std::unique_ptr<interfaces::IInfraredSender> infraredSender;

    const char* useStub = std::getenv("ULTIMATE_REMOTE_IR_STUB");
    if (useStub && std::strcmp(useStub, "0") != 0) {
        infraredSender = std::make_unique<platform::linux::StubInfraredSender>();
    } else {
        infraredSender = std::make_unique<platform::linux::KernelLircInfraredSender>(context.getLircDevicePath());
    }

    const char* irTest = std::getenv("ULTIMATE_REMOTE_IR_TEST");
    if (irTest && std::strcmp(irTest, "0") != 0) {
        std::cerr << "[startup] IR test mode\n";
        infraredSender->begin(context.getIrTxPin());
        std::vector<uint32_t> raw = makeNecTestPattern();
        bool sent = infraredSender->sendRaw(raw.data(), raw.size(), 38000);
        std::cerr << "[IR test] " << (sent ? "sent NEC test frame" : "failed to send NEC test frame") << "\n";
        return sent ? 0 : 1;
    }

    platform::linux::LinuxFramebufferView display;
    platform::linux::LinuxKeyboardInput input;
    input.setScreenBlankingCallback([&display](bool sleep) {
        return display.setScreenBlanked(sleep);
    });

    platform::linux::LinuxAppPlatform platformActions;
    std::cerr << "[startup] dispatcher create\n";
    auto dispatcher = std::make_unique<AppDispatcher>(display, input, *infraredSender, platformActions);
    std::cerr << "[startup] dispatcher setup\n";
    dispatcher->setup();
    std::cerr << "[startup] dispatcher run\n";
    dispatcher->run();
    return 0;
}

}
#endif
