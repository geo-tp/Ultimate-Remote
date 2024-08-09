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
        case NECext: return "NECx2";
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

ProtocolEnum ProtocolRepository::getProtocolEnum(const std::string& protocolStr) {
    if (protocolStr == "nec1") return _48_NEC1;
    if (protocolStr == "nec2") return _48_NEC2;
    if (protocolStr == "aiwa") return AIWA;
    if (protocolStr == "akai") return AKAI;
    if (protocolStr == "apple") return _APPLE;
    if (protocolStr == "barco") return BARCO;
    if (protocolStr == "blaupunkt") return BLAUPUNKT;
    if (protocolStr == "bose") return BOSE;
    if (protocolStr == "canalsat") return CANALSAT;
    if (protocolStr == "denon") return _DENON;
    if (protocolStr == "Denon-K") return DENON_K;
    if (protocolStr == "denon1") return DENON_1_;
    if (protocolStr == "denon2") return DENON_2_;
    if (protocolStr == "Dgtec") return DGTEC;
    if (protocolStr == "directv") return DIRECTV;
    if (protocolStr == "DishPlayer_Network") return DISH_NETWORK;
    if (protocolStr == "emerson") return EMERSON;
    if (protocolStr == "f12") return F12;
    if (protocolStr == "fujitsu") return FUJITSU;
    if (protocolStr == "GI4dtv") return G_I_4DTV;
    if (protocolStr == "GI_cable") return G_I_CABLE;
    if (protocolStr == "gxb") return GXB;
    if (protocolStr == "grundig16") return GRUNDIG16;
    if (protocolStr == "grundig16_30") return GRUNDIG16_30;
    if (protocolStr == "jvc") return _JVC;
    if (protocolStr == "jvc_two_frames") return JVC_2_;
    if (protocolStr == "Jerrold") return JERROLD;
    if (protocolStr == "kathrein") return KATHREIN;
    if (protocolStr == "konka") return KONKA;
    if (protocolStr == "logitech") return LOGITECH;
    if (protocolStr == "lumagen") return LUMAGEN;
    if (protocolStr == "mce") return MCE;
    if (protocolStr == "Mitsubishi") return MITSUBISHI;
    if (protocolStr == "nec") return _NEC;
    if (protocolStr == "nec1_f16") return NEC1_F16;
    if (protocolStr == "nec1_rnc") return NEC1_RNC;
    if (protocolStr == "nec1_y1") return NEC1_Y1;
    if (protocolStr == "nec1_y2") return NEC1_Y2;
    if (protocolStr == "nec1_y3") return NEC1_Y3;
    if (protocolStr == "nec2_f16") return NEC2_F16;
    if (protocolStr == "NECx1") return NECX1;
    if (protocolStr == "NECx2") return NECX2;
    if (protocolStr == "NECext") return NECX2;
    if (protocolStr == "nrc16") return NRC16;
    if (protocolStr == "nrc17") return NRC17;
    if (protocolStr == "Nokia32") return NOKIA;
    if (protocolStr == "pacemss") return PACEMSS;
    if (protocolStr == "panasonic") return _PANASONIC;
    if (protocolStr == "panasonic2") return PANASONIC2;
    if (protocolStr == "panasonic_old") return PANASONIC_OLD;
    if (protocolStr == "pioneer") return PIONEER;
    if (protocolStr == "Proton") return PROTON;
    if (protocolStr == "rc5") return _RC5;
    if (protocolStr == "rc5odd") return RC5_7F;
    if (protocolStr == "rc6") return _RC6;
    if (protocolStr == "rc6-6-20") return RC6_6_20;
    if (protocolStr == "rca") return RCA;
    if (protocolStr == "recs80_45") return RECS80;
    if (protocolStr == "replay") return REPLAY;
    if (protocolStr == "Samsung") return _SAMSUNG;
    if (protocolStr == "Samsung20") return SAMSUNG20;
    if (protocolStr == "Samsung36") return SAMSUNG36;
    if (protocolStr == "scatl_6") return SCATL_6;
    if (protocolStr == "sejin_1_38") return SEJIN_1_38;
    if (protocolStr == "sejin_1_56") return SEJIN_1_56;
    if (protocolStr == "sharp") return _SHARP;
    if (protocolStr == "SIRC") return SONY12;
    if (protocolStr == "sony12") return SONY12;
    if (protocolStr == "sony15") return SONY15;
    if (protocolStr == "sony20") return SONY20;
    if (protocolStr == "streamzap") return STREAMZAP;
    if (protocolStr == "Teac-K") return TEAC_K;
    if (protocolStr == "Thomson") return THOMSON;
    if (protocolStr == "Tivo-Nec1") return TIVO_UNIT;
    if (protocolStr == "velleman") return VELLEMAN;
    if (protocolStr == "XMP") return XMP;
    if (protocolStr == "zaptor_56") return ZAPTOR_56;
    if (protocolStr == "zenith") return ZENITH;
    if (protocolStr == "DishPlayer_Network") return DISH_NETWO;
    if (protocolStr == "Kaseikyo") return _KASEIKYO;
    return _NEC2; // Default or fallback
}


}