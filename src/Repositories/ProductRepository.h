#ifndef PRODUCT_REPOSITORY_H
#define PRODUCT_REPOSITORY_H

#include <vector>
#include <string>
#include <cstring>
#include <algorithm>
#include <Preferences.h>
#include <Models/Product.h>
#include <Models/Manufacturer.h>
#include <Models/FavoriteRemote.h>

namespace repositories {

    class ProductRepository {
    public:
        ProductRepository();
        ~ProductRepository();

        static models::Product getEmptyProduct();
    private:
        Preferences preferences; // NVS, used to get/save/delete user favorite remotes
        static const models::Product emptyProduct;
    };
}


#endif
