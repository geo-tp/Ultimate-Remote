#ifndef PRODUCT_CONTROLLER_H
#define PRODUCT_CONTROLLER_H

#include <Interfaces/IDisplayView.h>
#include <Interfaces/IInputProvider.h>
#include <Services/ProductService.h>
#include <Contexts/GlobalContext.h>
#include <Contexts/SelectionContext.h>
#include <Selections/ProductSelection.h>
#include <Models/Product.h>

using namespace services;
using namespace contexts;
using namespace selections;
using namespace models;

namespace controllers {

class ProductController {
public:
    ProductController(interfaces::IDisplayView& display, interfaces::IInputProvider& input, ProductService& productService,
                      ProductSelection& productSelection);

    void handleProductSelection();

private:
    interfaces::IDisplayView& display;
    interfaces::IInputProvider& input;
    ProductService& productService;
    ProductSelection& productSelection;
    SelectionContext& selectionContext = SelectionContext::getInstance();
};

} // namespace controllers

#endif // PRODUCT_CONTROLLER_H
