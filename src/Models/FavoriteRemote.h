#ifndef FAVORITE_REMOTE_MODEL_H
#define FAVORITE_REMOTE_MODEL_H

#include <string.h>

namespace models {

    typedef struct {
        char manufacturerName[24];
        char productName[30];
        char remoteFileName[14];
        char favoriteName[14];
    } FavoriteRemote;

    inline bool operator==(const FavoriteRemote& lhs, const FavoriteRemote& rhs) {
        return strcmp(lhs.manufacturerName, rhs.manufacturerName) == 0 &&
            strcmp(lhs.productName, rhs.productName) == 0 &&
            strcmp(lhs.remoteFileName, rhs.remoteFileName) == 0 &&
            strcmp(lhs.favoriteName, rhs.favoriteName) == 0;
    }

    inline bool operator!=(const FavoriteRemote& lhs, const FavoriteRemote& rhs) {
        return !(lhs == rhs);
    }

}

#endif