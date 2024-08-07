#include "ProductRepository.h"

namespace repositories {

const Product ProductRepository::emptyProduct = {"", nullptr, 0};

ProductRepository::ProductRepository() {
}

ProductRepository::~ProductRepository() {
}

Product ProductRepository::getEmptyProduct() {
    return emptyProduct;
}

}