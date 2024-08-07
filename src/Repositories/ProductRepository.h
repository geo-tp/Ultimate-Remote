/*

############################################################################################################################################
#                                                                                                                                          #
#                                                                                                                                          #
#                   An API that retrieves remote informations such as Manufacturer, Product, Remote, and RemoteCommands.                   #           
#                                                                                                                                          #
#                                                                                                                                          #
############################################################################################################################################

*/


#ifndef PRODUCT_REPOSITORY_H
#define PRODUCT_REPOSITORY_H

#include <vector>
#include <string>
#include <cstring>
#include <algorithm>
#include <M5Cardputer.h>
#include <Preferences.h>
#include <Models/Product.h>
#include <Models/Manufacturer.h>
#include <Models/FavoriteRemote.h>
#include <Repositories/RemoteRepository.h>

using namespace models;

namespace repositories {

    class ProductRepository {
    public:
        ProductRepository();
        ~ProductRepository();

        static Product getEmptyProduct();
    private:
        Preferences preferences; // NVS, used to get/save/delete user favorite remotes
        static const Product emptyProduct;
    };
}


#endif
