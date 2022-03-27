#ifndef APTRACER_ENTITIES_ENTITIES_H
#define APTRACER_ENTITIES_ENTITIES_H
namespace APTracer { 
    /**
     * @brief Contains the fundamental types of the project. 
     * 
     * Most of the entities represent interface classes, to be specialized in their respective namespaces.
     * Some entities, such as Vec3, represent basic types used throughout the program.
     */
    namespace Entities {}
}

#include "Ray_t.h"
#include "TransformMatrix_t.h"
#include "Vec3.h"

#endif