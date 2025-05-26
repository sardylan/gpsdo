#include "ui.hpp"

#include <iostream>

#include "version.hpp"

void uiHeader() {
    std::cout << APPLICATION_NAME << std::endl;
    std::cout << "Version: " << APPLICATION_VERSION << std::endl;
    std::cout << "Built on " << APPLICATION_BUILD << std::endl;

#ifdef APPLICATION_GIT_SHA1
    std::cout << "Commit: " << APPLICATION_GIT_SHA1 << std::endl;
#endif
}
