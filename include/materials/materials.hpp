#ifndef AGPTRACER_MATERIALS_MATERIALS_HPP
#define AGPTRACER_MATERIALS_MATERIALS_HPP

namespace AGPTracer {
    /**
     * @brief Contains different material and medium types that can be used.
     *
     * Materials and mediums describes how some entities interact with light.
     * Materials describe how shapes interact with light when they are intersected by a ray.
     * They can change the ray's colour and mask values, and the ray's origin and direction.
     * Mediums describe how light interacts when being inside a medium. They can change the
     * ray's colour and mask relatively to the distance traveled through the medium. They
     * can also intersect the ray themselves, changing the ray's colour and mask, and its
     * origin and direction. In that case, it reports that it has intersected the ray so
     * the shape intersection can be ignored.
     */
    namespace Materials {
    }
}

#include "Diffuse_t.hpp"

#endif
