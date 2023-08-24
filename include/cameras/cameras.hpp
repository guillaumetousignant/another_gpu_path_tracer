#ifndef APTRACER_CAMERAS_CAMERAS_HPP
#define APTRACER_CAMERAS_CAMERAS_HPP

namespace APTracer {
    /**
     * @brief Contains the different camera types that can be used.
     *
     * Cameras create rays according to a projection, and casts them in a scene. Then, the
     * colour of the rays are usually stored in an image buffer. The different cameras have
     * different projections or effects, such as motion blur, depth of field and 3D.
     */
    namespace Cameras {
    }
}

#include "SphericalCamera_t.hpp"

#endif
