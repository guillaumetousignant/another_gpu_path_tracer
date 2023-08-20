#ifndef AGPTRACER_SKYBOXES_SKYBOXES_HPP
#define AGPTRACER_SKYBOXES_SKYBOXES_HPP

namespace AGPTracer {
    /**
     * @brief Contains different skybox types that can be used.
     *
     * Skyboxes describe how the light get coloured when they miss geometry. It is the
     * background of scenes. They output the background colour depending on a ray's
     * direction. Several types exist, some taking their colour from a texture, a
     * flat colour, light sources, or a combination of these.
     */
    namespace Skyboxes {
    }
}

#include "SkyboxFlat_t.hpp"

#endif
