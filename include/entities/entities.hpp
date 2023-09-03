#ifndef AGPTRACER_ENTITIES_ENTITIES_HPP
#define AGPTRACER_ENTITIES_ENTITIES_HPP
namespace AGPTracer {
    /**
     * @brief Contains the fundamental types of the project.
     *
     * Most of the entities represent interface classes, to be specialized in their respective namespaces.
     * Some entities, such as Vec3, represent basic types used throughout the program.
     */
    namespace Entities {
    }
}

#include "Camera.hpp"
#include "Material.hpp"
#include "Medium.hpp"
#include "MediumList_t.hpp"
#include "MeshGeometry_t.hpp"
#include "RandomGenerator_t.hpp"
#include "RandomNumberGenerator_t.hpp"
#include "Ray_t.hpp"
#include "Scene_t.hpp"
#include "Shape.hpp"
#include "Skybox.hpp"
#include "Texture_t.hpp"
#include "TransformMatrix_t.hpp"
#include "Translucent.hpp"
#include "UniformDistribution_t.hpp"
#include "Vec3.hpp"

#endif