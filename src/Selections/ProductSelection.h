#ifndef PRODUCT_SELECTION_H
#define PRODUCT_SELECTION_H

#include <Views/CardputerView.h>
#include <Inputs/CardputerInput.h>
#include <Contexts/GlobalContext.h>
#include <Models/Product.h>

namespace selections {

class ProductSelection {
public:
    ProductSelection(views::CardputerView& display, inputs::CardputerInput& input);
    models::Product select(const std::vector<models::Product>& products, const std::string& manufacturerName, uint16_t& selectionIndex, models::Product emptyProduct);
private:
    views::CardputerView& display;
    inputs::CardputerInput& input;
    int16_t lastIndex;
};

}

#endif // MODE_SELECTION_H
