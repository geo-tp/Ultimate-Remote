#include "ProductSelection.h"

namespace selections {

ProductSelection::ProductSelection(CardputerView& display, CardputerInput& input)
    : display(display), input(input), lastIndex(-1) {}

Product ProductSelection::select(const std::vector<Product>& products, const std::string& manufacturerName, uint16_t& selectionIndex, Product emptyProduct) {
    char key = KEY_NONE;
    lastIndex = -1;

    display.displayTopBar(manufacturerName, true);
    std::vector<std::string> productNames = utils::StringUtils::extractFieldNames(products, "name");

    while (key != KEY_OK) {
        if (lastIndex != selectionIndex) {
            display.displaySelection(productNames, selectionIndex);
            lastIndex = selectionIndex;
        }

        key = input.handler();

        switch (key) {
            case KEY_NONE:
                break;

            case KEY_ARROW_DOWN:
                if (selectionIndex < products.size() - 1) {
                    selectionIndex++;
                } else {
                    selectionIndex = 0;
                }
                break;

            case KEY_ARROW_UP:
                if (selectionIndex > 0) {
                    selectionIndex--;
                } else {
                    selectionIndex = products.size() - 1;
                }
                break;

            case KEY_RETURN:
                return emptyProduct;

            default:
                break;
        }
    }

    return products[selectionIndex];
}

}
