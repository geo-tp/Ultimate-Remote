/*

############################################################################################################################################
#                                                                                                                                          #
#                                                                                                                                          #
#                   An API that retrieves remote informations such as Manufacturer, Product, Remote, and RemoteCommand.                    #           
#                                                                                                                                          #
#                                                                                                                                          #
############################################################################################################################################

*/


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