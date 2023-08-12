#ifndef AGPTRACER_SHAPES_SHAPES_HPP
#define AGPTRACER_SHAPES_SHAPES_HPP

namespace AGPTracer {
    /**
     * @brief Contains different shape types that can be used.
     *
     * Shapes can be intersected with rays, and can provide geometric information
     * about these intersections. Some shapes support motion blur, and retain information
     * about their past state. Meshes are also contained here. Meshes represent collection
     * of shapes with the same transformation matrix, possibly taking their geometry from
     * the same source.
     */
    namespace Shapes {
    }
}

#include "Triangle_t.hpp"

#endif