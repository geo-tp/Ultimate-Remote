#include <Services/FileService.h>
#include <Encoders/FlipperEncoder.h>
#include <cassert>
#include <iostream>
#include <fstream>
#include <sstream>
#include <numeric>

using namespace services;
using encoders::encodeFlipperCommand;
int main(int argc, char** argv) {
    repositories::ProtocolRepository protocols;
    FileService files(protocols);
    const std::string header = "Filetype: IR signals file\nVersion: 1\n";
    auto parse = [&](const std::string& body) { return files.getRemoteFromFile("test", header + body); };
    auto parsed = [&](const std::string& protocol, const std::string& addr, const std::string& code) {
        return parse("name: Power\ntype: parsed\nprotocol: " + protocol + "\naddress: " + addr + "\ncommand: " + code + "\n").commands.at(0);
    };
    assert(!files.validateInfraredFile("Filetype: wrong\nVersion: 1\n"));
    assert(!files.validateInfraredFile("Filetype: IR signals file\n"));
    assert(files.getRemoteFromFile("bad", "Filetype: wrong\nVersion: 1\nname: Power\n").commands.empty());
    auto raw = parse("name: Raw\ntype: raw\nfrequency: 38000\nduty_cycle: 0.5\ndata: 9000 4500 560 70000 560\n").commands.at(0);
    auto copy = raw;
    auto wave = encodeFlipperCommand(copy, false);
    assert(wave.pulses == std::vector<uint32_t>({9000,4500,560,70000,560}));
    assert(wave.frequencyHz == 38000 && wave.dutyPercent == 50);
    assert(copy == raw);
    copy.rawData.back() = 600;
    assert(copy != raw);
    for (const std::string data : {"0", "-1", "9999999999999999999999", "abc", "1000001"}) {
        auto bad = parse("name: Bad\ntype: raw\nfrequency: 38000\nduty_cycle: .33\ndata: " + data + "\n").commands.at(0);
        assert(!bad.error.empty() && encodeFlipperCommand(bad, false).pulses.empty());
    }
    auto missing = parse("name: Missing\ntype: raw\n").commands.at(0);
    assert(!missing.error.empty());
    assert(!parsed("Unknown", "00 00 00 00", "00 00 00 00").error.empty());
    assert(!parsed("NEC", "00 01 00 00", "00 00 00 00").error.empty());
    assert(!parsed("NEC", "garbage", "00 00 00 00").error.empty());
    auto nec = parsed("NEC", "04 00 00 00", "08 00 00 00");
    wave = encodeFlipperCommand(nec, false);
    assert(wave.pulses.size() == 67 && wave.pulses[0] == 9000 && wave.pulses[1] == 4500);
    uint32_t decoded = 0;
    for (int bit = 0; bit < 32; ++bit) {
        assert(wave.pulses[2 + bit * 2] == 560);
        if (wave.pulses[3 + bit * 2] == 1690) decoded |= uint32_t(1) << bit;
    }
    assert(decoded == 0xf708fb04u);
    auto extended = parsed("NECext", "34 12 00 00", "CD AB 00 00");
    assert(extended.address == 0x1234 && extended.function == 0xabcd);
    wave = encodeFlipperCommand(extended, false);
    decoded = 0;
    for (int bit = 0; bit < 32; ++bit) if (wave.pulses[3 + bit * 2] == 1690) decoded |= uint32_t(1) << bit;
    assert(decoded == 0xabcd1234u);
    wave = encodeFlipperCommand(parsed("Samsung32", "07 00 00 00", "02 00 00 00"), false);
    assert(wave.pulses.size() == 67 && wave.pulses[0] == 4500 && wave.pulses[2] == 550);
    decoded = 0;
    for (int bit = 0; bit < 32; ++bit) if (wave.pulses[3 + bit * 2] == 1650) decoded |= uint32_t(1) << bit;
    assert(decoded == 0xfd020707u);
    wave = encodeFlipperCommand(parsed("SIRC", "01 00 00 00", "15 00 00 00"), false);
    assert(wave.frequencyHz == 40000 && wave.pulses.size() == 77);
    assert(std::accumulate(wave.pulses.begin(), wave.pulses.begin() + 26, 0u) == 45000);
    auto rc5 = parsed("RC5", "05 00 00 00", "12 00 00 00");
    assert(encodeFlipperCommand(rc5, false).pulses != encodeFlipperCommand(rc5, true).pulses);
    assert(encodeFlipperCommand(rc5, false).frequencyHz == 36000);
    // RC5X extends the six-bit RC5 command field to seven bits.
    assert(!parsed("RC5", "05 00 00 00", "40 00 00 00").error.empty());
    assert(parsed("RC5X", "05 00 00 00", "40 00 00 00").error.empty());
    assert(parsed("RC5X", "05 00 00 00", "7F 00 00 00").error.empty());
    assert(!parsed("RC5X", "05 00 00 00", "80 00 00 00").error.empty());
    auto rc6 = parsed("RC6", "05 00 00 00", "12 00 00 00");
    wave = encodeFlipperCommand(rc6, false);
    assert(wave.pulses[0] == 2666 && wave.pulses[1] == 889 && wave.frequencyHz == 36000);
    assert(wave.pulses != encodeFlipperCommand(rc6, true).pulses);
    auto nec42 = parsed("NEC42ext", "56 34 12 00", "CD AB 00 00");
    wave = encodeFlipperCommand(nec42, false);
    assert(wave.pulses.size() == 87);
    uint64_t decoded42 = 0;
    for (int bit = 0; bit < 42; ++bit) if (wave.pulses[3 + bit * 2] == 1690) decoded42 |= uint64_t(1) << bit;
    assert(decoded42 == (uint64_t(0xabcd) << 26 | 0x123456));
    if (argc > 1) {
        std::ifstream source(argv[1]);
        std::ostringstream content;
        content << source.rdbuf();
        const auto remote = files.getRemoteFromFile("live sample", content.str());
        assert(!remote.commands.empty());
        for (const auto& command : remote.commands) {
            assert(command.error.empty());
            assert(!encodeFlipperCommand(command, false).pulses.empty());
        }
        std::cout << "Live Flipper-IRDB sample parsed/encoded: " << remote.commands.size() << " commands.\n";
    }
    std::cout << "Flipper parsing, bounds, raw duration/duty, NEC/NECext/Samsung/SIRC waveforms and toggle checks passed.\n";
}
