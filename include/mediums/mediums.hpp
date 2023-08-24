#ifndef AGPTRACER_MEDIUMS_MEDIUMS_HPP
#define AGPTRACER_MEDIUMS_MEDIUMS_HPP

namespace AGPTracer {
    /**
     * @brief Contains different medium types that can be used.
     *
     * Mediums describe how light interacts when being inside a medium. They can change the
     * ray's colour and mask relatively to the distance traveled through the medium. They
     * can also intersect the ray themselves, changing the ray's colour and mask, and its
     * origin and direction. In that case, it reports that it has intersected the ray so
     * the shape intersection can be ignored.
     */
    namespace Mediums {
    }
}

#include "NonAbsorber_t.hpp"

#endif
