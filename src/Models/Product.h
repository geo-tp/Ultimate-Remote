#ifndef PRODUCT_MODEL_H
#define PRODUCT_MODEL_H

#include <stddef.h>
#include "Remote.h"

namespace models {

    typedef struct {
        char name[30];
        const Remote *remotes;
        size_t remoteCount;
    } Product;


    inline bool operator==(const Product& lhs, const Product& rhs) {
        return (strcmp(lhs.name, rhs.name) == 0) && (lhs.remotes == rhs.remotes) && (lhs.remoteCount == rhs.remoteCount);
    }


    inline bool operator!=(const Product& lhs, const Product& rhs) {
        return !(lhs == rhs);
    }

}

#endif
    

