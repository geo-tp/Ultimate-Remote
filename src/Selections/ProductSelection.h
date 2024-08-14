#ifndef PRODUCT_SELECTION_H
#define PRODUCT_SELECTION_H

#include <Views/CardputerView.h>
#include <Inputs/CardputerInput.h>
#include <Contexts/GlobalContext.h>

using namespace views;
using namespace inputs;
using namespace contexts;

namespace selections {

class ProductSelection {
public:
    ProductSelection(CardputerView& display, CardputerInput& input);
    Product select(const std::vector<Product>& products, const std::string& manufacturerName, uint16_t& selectionIndex, Product emptyProduct);
private:
    GlobalContext& context = GlobalContext::getInstance();
    CardputerView& display;
    CardputerInput& input;
    uint16_t selectionIndex;
    int16_t lastIndex;
};

}

#endif // MODE_SELECTION_H
