#pragma once
#include <iostream>

#define REPORT_ICE(MSG)                                                                                                \
    do                                                                                                                 \
    {                                                                                                                  \
        std::cerr << "Internal compiler error occured: \n\tfile: " << __FILE__ << "\n\tFunction: " << __FUNCTION__     \
                  << "\n\tLine: " << __LINE__ << "\n\tMessage: " << MSG << '\n';                                       \
        std::exit(2);                                                                                                  \
    } while (0)
