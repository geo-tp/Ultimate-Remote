#ifndef DEFAULT_FAVORITE_REMOTE_DATA_H
#define DEFAULT_FAVORITE_REMOTE_DATA_H


#include <vector>
#include <Models/FavoriteRemote.h>

namespace data {

    // Liste des telecommandes prefs
    const std::vector<models::FavoriteRemote> defaultFavoriteRemotes = {

        models::FavoriteRemote{"Miscellaneous", "On/Off Universal", "1,-X.csv", "On/Off Uni"},

        models::FavoriteRemote{"Sony", "TV", "1,-1.csv", "Sony Uni1"},
        models::FavoriteRemote{"Sony", "Unknown_RM-D7M", "15,-1.csv", "Sony Uni2"},
        models::FavoriteRemote{"Sony", "Unknown_RM-D820", "17,-1.csv", "Sony Uni3"},
        
        models::FavoriteRemote{"Samsung", "Unknown_SAMSUNG", "7,7.csv", "Samsung Uni1"},
        models::FavoriteRemote{"Samsung", "Unknown_SAMSUNG", "5,5.csv", "Samsung Uni2"},
        models::FavoriteRemote{"Samsung", "TV", "7,0.csv", "Samsung Uni3"},

        models::FavoriteRemote{"LG", "TV", "4,-1.csv", "LG Uni 1"},
        models::FavoriteRemote{"LG", "Unknown_6710V00090D", "0,-1.csv", "LG Uni2"},
        models::FavoriteRemote{"LG", "Unknown_LG.6710V00005G", "110,-1.csv", "LG Uni3"},

        models::FavoriteRemote{"Panasonic", "TV", "8,-1.csv", "Panasonic Un1"},
        models::FavoriteRemote{"Panasonic", "Unknown_EUR642195", "160,194.csv", "Panasonic Un2"},
        models::FavoriteRemote{"Panasonic", "VCR", "144,0.csv", "Panasonic Un3"},

        models::FavoriteRemote{"Philips", "Unknown_uDigital", "0,-1.csv", "Philips Uni1"},
        models::FavoriteRemote{"Philips", "Unknown_RC2034302", "0,-1.csv", "Philips Uni2"},
        models::FavoriteRemote{"Philips", "Unknown_CD723", "20,-1.csv", "Philips Uni3"},
        models::FavoriteRemote{"Philips", "DVD Player", "4,-1.csv", "Philips Uni4"},

        models::FavoriteRemote{"Sony", "Playstation2", "58,9.csv", "Playstation2"},
        models::FavoriteRemote{"Microsoft", "Unknown_xbox", "10,-1.csv", "Xbox"},
        models::FavoriteRemote{"Microsoft", "Unknown_Xbox360", "116,15.csv", "Xbox 360"},
        models::FavoriteRemote{"Microsoft", "Xbox One", "128,216.csv", "Xbox One"},

        models::FavoriteRemote{"Amazon", "TV", "2,125.csv", "Amazon Uni1"},

        models::FavoriteRemote{"Sharp", "TV", "1,-1.csv", "Sharp Uni1"},
        
        models::FavoriteRemote{"Toshiba", "TV", "64,-1.csv", "Toshiba Uni1"},
        models::FavoriteRemote{"Toshiba", "VCR", "69,-1.csv", "Toshiba Uni2"},

        models::FavoriteRemote{"Vizio", "TV", "4,-1.csv", "Vizio Uni1"},

        models::FavoriteRemote{"Apple", "Apple TV", "238,135.csv", "Apple Uni1"},

        models::FavoriteRemote{"JVC", "TV", "3,-1.csv", "JVC Uni1"},
        models::FavoriteRemote{"JVC", "Unknown_jvc-lp20465-005-vcr", "67,-1.csv", "JVC Uni2"},

        models::FavoriteRemote{"Pioneer", "CD Player", "162,-1.csv", "Pioneer Uni1"},

        models::FavoriteRemote{"Yamaha", "Unknown_VP59040", "122,-1.csv", "Yamaha Uni1"},
        models::FavoriteRemote{"Yamaha", "Unknown_VQ95010", "121,-1.csv", "Yamaha Uni2"},

        models::FavoriteRemote{"Denon", "DVD Player", "2,-1.csv", "Denon Uni1"},
        models::FavoriteRemote{"Denon", "DAT", "4,-1.csv", "Denon Uni2"},
        models::FavoriteRemote{"Denon", "CD Player", "6,-1.csv", "Denon Uni3"},

        models::FavoriteRemote{"Marantz", "DVD", "4,-1.csv", "Marantz Uni1"},
        models::FavoriteRemote{"Marantz", "Unknown_Marantz-RC-63CD", "20,-1.csv", "Marantz Uni2"},

        models::FavoriteRemote{"Onkyo", "CD Player", "210,109.csv", "Onkyo Uni1"},

        models::FavoriteRemote{"Harman Kardon", "Unknown_DVD", "130,114.csv", "HarKardon Un1"},

        models::FavoriteRemote{"Canon", "Unknown_canon", "133,118.csv", "Canon Uni1"},
        
        models::FavoriteRemote{"Akai", "Unknown_canon", "137,119.csv", "Akai Uni1"},
        
        models::FavoriteRemote{"Fujitsu", "TV", "132,-1.csv", "Fujitsu Uni1"},

        models::FavoriteRemote{"Kenwood", "Unknown_RC-P030", "182,-1.csv", "Kenwood Uni1"},

        models::FavoriteRemote{"NEC", "TV", "4,-1.csv", "NEC Uni1"},
    };
};

#endif
