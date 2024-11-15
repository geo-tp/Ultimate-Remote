#ifndef PRODUCT_CONTROLLER_H
#define PRODUCT_CONTROLLER_H

#include <Views/CardputerView.h>
#include <Inputs/CardputerInput.h>
#include <Services/ProductService.h>
#include <Contexts/GlobalContext.h>
#include <Contexts/SelectionContext.h>
#include <Selections/ProductSelection.h>
#include <Models/Product.h>

using namespace views;
using namespace inputs;
using namespace services;
using namespace contexts;
using namespace selections;
using namespace models;

namespace controllers {

class ProductController {
public:
    ProductController(CardputerView& display, CardputerInput& input, ProductService& productService,
                      GlobalContext& globalContext, SelectionContext& selectionContext, ProductSelection& productSelection);

    void handleProductSelection();

private:
    CardputerView& display;
    CardputerInput& input;
    ProductService& productService;
    ProductSelection& productSelection;
    GlobalContext& globalContext;
    SelectionContext& selectionContext;
};

} // namespace controllers

#endif // PRODUCT_CONTROLLER_H
