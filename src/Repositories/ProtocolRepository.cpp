#include "ProtocolRepository.h"

namespace repositories {

const Protocol* ProtocolRepository::getAllProtocols() {
    return protocolDefinitions;
}

Protocol ProtocolRepository::getProtocolByName(const std::string& name) {
    for (const auto& protocol : protocolDefinitions) {
        if (protocol.name == name) {
            return protocol;
        }
    }
    return {"", ""}; // Return an empty protocol if not found
}

const char* ProtocolRepository::getProtocolString(ProtocolEnum protocol) {
    switch (protocol) {
        case _48_NEC1: return "nec1";
        case _48_NEC2: return "nec2";
        case AIWA: return "aiwa";
        case AKAI: return "akai";
        case _APPLE: return "apple";
        case BARCO: return "barco";
        case BLAUPUNKT: return "blaupunkt";
        case BOSE: return "bose";
        case CANALSAT: return "canalsat";
        case _DENON: return "denon";
        case DENON_K: return "Denon-K";
        case DENON_1_: return "denon";
        case DENON_2_: return "denon";
        case DGTEC: return "Dgtec";
        case DIRECTV: return "directv";
        case DISH_NETWORK: return "DishPlayer_Network";
        case DISHPLAYER: return "DishPlayer_Network";
        case EMERSON: return "emerson";
        case F12: return "f12";
        case FUJITSU: return "fujitsu";
        case G_I_4DTV: return "GI4dtv";
        case G_I_CABLE: return "GI_cable";
        case GXB: return "gxb";
        case GRUNDIG16: return "grundig16";
        case GRUNDIG16_30: return "grundig16_30";
        case _JVC: return "jvc";
        case JVC_48: return "jvc";
        case JVC_2_: return "jvc_two_frames";
        case JERROLD: return "Jerrold";
        case KATHREIN: return "kathrein";
        case KONKA: return "konka";
        case LOGITECH: return "logitech";
        case LUMAGEN: return "lumagen";
        case MCE: return "mce";
        case MITSUBISHI: return "Mitsubishi";
        case _NEC: return "nec";
        case NEC1: return "nec1";
        case NEC1_F16: return "nec1_f16";
        case NEC1_RNC: return "nec1_rnc";
        case NEC1_Y1: return "nec1_y1";
        case NEC1_Y2: return "nec1_y2";
        case NEC1_Y3: return "nec1_y3";
        case _NEC2: return "nec2";
        case NEC2_F16: return "nec2_f16";
        case NECX1: return "NECx1";
        case NECX2: return "NECx2";
        case NRC16: return "nrc16";
        case NRC17: return "nrc17";
        case NOKIA: return "Nokia32";
        case NOKIA12: return "Nokia32single";
        case NOKIA32: return "Nokia32";
        case PACEMSS: return "pacemss";
        case _PANASONIC: return "panasonic";
        case PANASONIC2: return "panasonic2";
        case PANASONIC_OLD: return "panasonic_old";
        case PIONEER: return "pioneer";
        case PROTON: return "Proton";
        case _RC5: return "rc5";
        case RC5_7F: return "rc5odd";
        case _RC6: return "rc6";
        case RC6_6_20: return "rc6-6-20";
        case RCA: return "rca";
        case RCA_OLD_: return "rca";
        case RCA_38: return "rca";
        case RCA_38_OLD_: return "rca";
        case RECS80: return "recs80_45";
        case REPLAY: return "replay";
        case SAMSUNG20: return "Samsung20";
        case SAMSUNG36: return "Samsung36";
        case SCATL_6: return "scatl_6";
        case SEJIN_1_38: return "sejin_1_38";
        case SEJIN_1_56: return "sejin_1_56";
        case _SHARP: return "sharp";
        case SHARPDVD: return "sharp";
        case SHARP_1_: return "sharp";
        case SHARP_2_: return "sharp";
        case SONY12: return "sony12";
        case SONY15: return "sony15";
        case SONY20: return "sony20";
        case STREAMZAP: return "streamzap";
        case TEAC_K: return "Teac-K";
        case THOMSON: return "Thomson";
        case THOMSON7: return "Thomson";
        case TIVO_UNIT_0: return "Tivo-Nec1";
        case TIVO_UNIT_1: return "Tivo-Nec1";
        case TIVO_UNIT_2: return "Tivo-Nec1";
        case TIVO_UNIT_3: return "Tivo-Nec1";
        case TIVO_UNIT_4: return "Tivo-Nec1";
        case TIVO_UNIT_5: return "Tivo-Nec1";
        case TIVO_UNIT_6: return "Tivo-Nec1";
        case TIVO_UNIT_7: return "Tivo-Nec1";
        case TIVO_UNIT_8: return "Tivo-Nec1";
        case TIVO_UNIT_9: return "Tivo-Nec1";
        case VELLEMAN: return "velleman";
        case XMP: return "XMP";
        case XMP_1: return "XMP";
        case XMP_2: return "XMP";
        case ZAPTOR_56: return "zaptor_56";
        case ZENITH: return "zenith";
        case DISH_NETWO: return "DishPlayer_Network";
        case PANASONIC_: return "panasonic";
        case TIVO_UNIT: return "Tivo-Nec1";
        case RCA_38_OLD: return "rca";
        case _KASEIKYO: return "Kaseikyo";
        default: return "nec2"; // // Most common, NEC2 for reliabity which continuously sends the full command frame
    }

}

}