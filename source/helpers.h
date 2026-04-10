#pragma once

#ifndef HELPERS_H
#define HELPERS_H

#include "libs.h"

#define drawSquare(hub75, size, x, y, r, g, b)\
    for(int i = 0; i < size; i++)\
    {\
        for(int j = 0; j < size; j++)\
        {\
            hub75.set_pixel(x + j, y + i, r, g, b);\
        }\
    }
    
#endif