#ifndef PRODUCT_SERVICE_H
#define PRODUCT_SERVICE_H

#include <vector>
#include <string>
#include <cstring>
#include <algorithm>
#include <pgmspace.h>
#include <Models/Product.h>
#include <Models/Manufacturer.h>
#include <Repositories/ProductRepository.h>

#define MAX_FAVORITE_REMOTE_SAVED 80

using namespace models;
using namespace repositories;

namespace services {

    class ProductService {
    public:
        ProductService() = default;
        ~ProductService() = default;

        static std::vector<Product> getProductBySubstring(const char* substring, const std::vector<Product>& products);
        static std::vector<Product> getManufacturerProducts(const Manufacturer& manufacturer);
        static Product getProduct(const Manufacturer& manufacturer, const std::string& productName);
        Product getEmptyProduct();
        static Product getFavoriteRemotesProduct(std::vector<Remote> remotes);
    private:
        ProductRepository productRepository;
    };

}

#endif // PRODUCT_SERVICE_H
