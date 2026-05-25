#ifndef PRODUCT_CONTROLLER_H
#define PRODUCT_CONTROLLER_H

#include <Views/CardputerView.h>
#include <Inputs/CardputerInput.h>
#include <Services/ProductService.h>
#include <Contexts/GlobalContext.h>
#include <Contexts/SelectionContext.h>
#include <Selections/ProductSelection.h>
#include <Models/Product.h>

namespace controllers {

class ProductController {
public:
    ProductController(views::CardputerView& display, inputs::CardputerInput& input, services::ProductService& productService,
                      selections::ProductSelection& productSelection);

    void handleProductSelection();

private:
    views::CardputerView& display;
    inputs::CardputerInput& input;
    services::ProductService& productService;
    selections::ProductSelection& productSelection;
    contexts::SelectionContext& selectionContext = contexts::SelectionContext::getInstance();
};

} // namespace controllers

#endif // PRODUCT_CONTROLLER_H
