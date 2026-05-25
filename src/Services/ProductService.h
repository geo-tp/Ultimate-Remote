#ifndef PRODUCT_SERVICE_H
#define PRODUCT_SERVICE_H

#include <vector>
#include <string>
#include <cstring>
#include <algorithm>
#include <pgmspace.h>
#include <Models/Product.h>
#include <Models/Manufacturer.h>
#include <Models/Remote.h>
#include <Repositories/ProductRepository.h>

#define MAX_FAVORITE_REMOTE_SAVED 80

namespace services {

    class ProductService {
    public:
        ProductService(repositories::ProductRepository& repository);
        ~ProductService() = default;

        static std::vector<models::Product> getProductBySubstring(const char* substring, const std::vector<models::Product>& products);
        static std::vector<models::Product> getManufacturerProducts(const models::Manufacturer& manufacturer);
        static models::Product getProduct(const models::Manufacturer& manufacturer, const std::string& productName);
        models::Product getEmptyProduct();
        static models::Product getFavoriteRemotesProduct(std::vector<models::Remote> remotes);
        static models::Product setProductName(models::Product product, std::string name);
    private:
        repositories::ProductRepository productRepository;
    };

}

#endif // PRODUCT_SERVICE_H
