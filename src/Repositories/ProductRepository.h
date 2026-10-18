#ifndef PRODUCT_REPOSITORY_H
#define PRODUCT_REPOSITORY_H

#include <vector>
#include <string>
#include <cstring>
#include <algorithm>
#include <Models/Product.h>
#include <Models/Manufacturer.h>
#include <Models/FavoriteRemote.h>


using namespace models;

namespace repositories {

    class ProductRepository {
    public:
        ProductRepository();
        ~ProductRepository();

        static Product getEmptyProduct();
    private:
        static const Product emptyProduct;
    };
}


#endif
