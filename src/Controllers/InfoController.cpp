#include "InfoController.h"
#include <Inputs/InputKeys.h>
#include <Inputs/ScopedZBack.h>
#include <Data/AppInfo.h>

namespace controllers {
void InfoController::handle() {
    const auto pages = data::getAppInfoPages(manufacturerRepository.getEmbeddedRemoteCount(),
                                            manufacturerRepository.getEmbeddedManufacturerCount());
    inputs::ScopedZBack zBack(input);
    size_t page = 0;
    while (true) {
        display.displayInfoPage(pages[page].title, pages[page].lines, page, pages.size());
        char key;
        do { key = input.handler(); } while (key == KEY_NONE);
        if (key == KEY_RETURN || key == 'z') break;
        if (key == KEY_OK || key == KEY_ARROW_DOWN || key == KEY_ARROW_RIGHT || key == 'c')
            page = (page + 1) % pages.size();
        else if (key == KEY_ARROW_UP || key == KEY_ARROW_LEFT)
            page = page ? page - 1 : pages.size() - 1;
    }
    selectionContext.setIsModeSelected(false);
}

}
