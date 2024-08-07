#include "ProductService.h"

namespace services {

std::vector<Product> ProductService::getProductBySubstring(const char* substring, const std::vector<Product>& products) {
    std::vector<Product> foundProducts;

    char lowerSubstring[30];
    strncpy(lowerSubstring, substring, sizeof(lowerSubstring));
    lowerSubstring[sizeof(lowerSubstring) - 1] = '\0';
    for (size_t i = 0; i < strlen(lowerSubstring); ++i) {
        lowerSubstring[i] = tolower(lowerSubstring[i]);
    }

    for (const auto& product : products) {
        char lowerProductName[30];
        strncpy(lowerProductName, product.name, sizeof(lowerProductName));
        lowerProductName[sizeof(lowerProductName) - 1] = '\0';
        for (size_t j = 0; j < strlen(lowerProductName); ++j) {
            lowerProductName[j] = tolower(lowerProductName[j]);
        }

        if (strstr(lowerProductName, lowerSubstring) != nullptr) {
            foundProducts.push_back(product);
        }
    }

    return foundProducts;
}

std::vector<Product> ProductService::getManufacturerProducts(const Manufacturer& manufacturer) {
    std::vector<Product> products;
    for (size_t i = 0; i < manufacturer.productCount; ++i) {
        Product product;
        memcpy_P(&product, &manufacturer.products[i], sizeof(Product));
        products.push_back(product);
    }
    return products;
}

Product ProductService::getProduct(const Manufacturer& manufacturer, const std::string& productName) {
    for (size_t i = 0; i < manufacturer.productCount; ++i) {
        Product product;
        memcpy_P(&product, &manufacturer.products[i], sizeof(Product));
        if (strcmp(product.name, productName.c_str()) == 0) {
            return product;
        }
    }
    return ProductRepository::getEmptyProduct();
}

Product ProductService::getEmptyProduct() {
    return productRepository.getEmptyProduct();
}


Product ProductService::getFavoriteRemotesProduct(std::vector<Remote> remotes) {
    static Remote remoteArray[MAX_FAVORITE_REMOTE_SAVED];
    Product favoriteProduct;
    favoriteProduct.remotes = remoteArray;
    favoriteProduct.remoteCount = 0;

    size_t count = 0;

    for (const auto& remote : remotes) {
        if (count >= MAX_FAVORITE_REMOTE_SAVED) {
            break;
        }
        
        if (remote.fileName[0] != '\0') {
            // Copier les infos de la remote
            strncpy(remoteArray[count].fileName, remote.fileName, sizeof(remoteArray[count].fileName) - 1);
            remoteArray[count].fileName[sizeof(remoteArray[count].fileName) - 1] = '\0';
            remoteArray[count].commands = remote.commands;
            remoteArray[count].commandCount = remote.commandCount; 
            count++;
        }
    }

    // Copier le nombre de remotes dans le produit
    favoriteProduct.remoteCount = count;

    // Copier le nom du produit
    strncpy(favoriteProduct.name, "Favorites", sizeof(favoriteProduct.name) - 1);
    favoriteProduct.name[sizeof(favoriteProduct.name) - 1] = '\0';

    return favoriteProduct;
}

}