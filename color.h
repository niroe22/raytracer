//
// Created by nkls on 04.04.24.
//

#ifndef RAYTRACER_COLOR_H
#define RAYTRACER_COLOR_H

#include "math.h"
#include <iostream>


template <class F, size_t K>
void write_color(std::ostream &out, Vector<F,K> pixel_color) {
    // Write the translated [0,255] value of each color component.
    out << static_cast<int>(255.999 * pixel_color[0]) << ' '
        << static_cast<int>(255.999 * pixel_color[1]) << ' '
        << static_cast<int>(255.999 * pixel_color[2]) << '\n';

}
#endif //RAYTRACER_COLOR_H
