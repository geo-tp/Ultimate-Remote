#ifndef PRODUCT_SELECTION_H
#define PRODUCT_SELECTION_H

#include <Interfaces/IDisplayView.h>
#include <Interfaces/IInputProvider.h>
#include <Models/Product.h>
#include <Contexts/GlobalContext.h>

using namespace contexts;
using namespace models;

namespace selections {

class ProductSelection {
public:
    ProductSelection(interfaces::IDisplayView& display, interfaces::IInputProvider& input);
    Product select(const std::vector<Product>& products, const std::string& manufacturerName, uint16_t& selectionIndex, Product emptyProduct);
private:
    interfaces::IDisplayView& display;
    interfaces::IInputProvider& input;
    int16_t lastIndex;
};

}

#endif // MODE_SELECTION_H
