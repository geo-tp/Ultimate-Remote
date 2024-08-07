#ifndef MANUFACTURER_MODEL_H
#define MANUFACTURER_MODEL_H

#include <stddef.h>
#include "Models/Product.h"

namespace models {

    typedef struct {
        char name[24];
        const Product *products;
        size_t productCount;
    } Manufacturer;

    // Définition des opérateurs == pour comparaison struct
    inline bool operator==(const Manufacturer& lhs, const Manufacturer& rhs) {
        return (strcmp(lhs.name, rhs.name) == 0) && (lhs.products == rhs.products) && (lhs.productCount == rhs.productCount);
    }

    // Définition des opérateurs != pour comparaison struct
    inline bool operator!=(const Manufacturer& lhs, const Manufacturer& rhs) {
        return !(lhs == rhs);
    }

}

#endif