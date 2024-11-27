#include "ProductController.h"

namespace controllers {

ProductController::ProductController(CardputerView& display, CardputerInput& input, ProductService& productService,
                                     ProductSelection& productSelection)
    : display(display), input(input), productService(productService), productSelection(productSelection) {}

void ProductController::handleProductSelection() {
    std::vector<Product> products = productService.getManufacturerProducts(selectionContext.getCurrentSelectedManufacturer());
    Product emptyProduct = productService.getEmptyProduct();

    uint16_t currentProductIndex = selectionContext.getCurrentProductIndex();
    Product selectedProduct = productSelection.select(products, selectionContext.getCurrentSelectedManufacturer().name,
                                                      currentProductIndex, emptyProduct);

    if (selectedProduct == emptyProduct) {
        selectionContext.setIsManufacturerSelected(false);
        selectionContext.setCurrentProductIndex(0);
    } else {
        selectionContext.setIsProductSelected(true);
        selectionContext.setCurrentSelectedProduct(selectedProduct);
        selectionContext.setCurrentProductIndex(currentProductIndex);
    }
}

} // namespace controllers
