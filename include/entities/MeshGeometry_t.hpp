#ifndef AGPTRACER_ENTITIES_MESHGEOMETRYUNSTRUCTURED_T_HPP
#define AGPTRACER_ENTITIES_MESHGEOMETRYUNSTRUCTURED_T_HPP

#include "entities/Vec3.hpp"
#include <array>
#include <filesystem>
#include <string>
#include <vector>

namespace AGPTracer { namespace Entities {

    /**
     * @brief The mesh geometry class represents a geometry made up of points and triangular faces.
     *
     * Mesh geometries represent a single geometry without any transformation. Multiple meshes can point to the same mesh geometry
     * while using different transformations, enabling instantiating and saving ressources. This class is constructed from geometry
     * input files. Currently, .obj and .su2 files are supported.
     *
     * @tparam T Floating point datatype to use
     */
    template<typename T = double>
    class MeshGeometry_t {
        public:
            /**
             * @brief Construct a new MeshGeometry_t object from a geometry input file.
             *
             * The input file can have non-triangular faces, as those are triangularised. For .su2 files, the faces in the 'WALL' MARKER_TAG
             * sections will be used.
             *
             * @param filename Path to a geometry file of either .obj or .su2 format.
             */
            MeshGeometry_t(const std::filesystem::path& filename);

            std::vector<Vec3<T>> nodes_; /**< @brief Array of nodes in the mesh.*/
            std::vector<Vec3<T>> normals_; /**< @brief Array of normals in the mesh.*/
            std::vector<std::array<T, 2>> texture_coordinates_; /**< @brief Array of 2D uv coordinates in the mesh.*/
            std::vector<std::string> mat_; /**< @brief Array of strings representing each face's material's name.*/
            std::vector<std::array<size_t, 3>> face_nodes_; /**< @brief Nodes making up each face.*/
            std::vector<std::array<size_t, 3>> face_normals_; /**< @brief Normals of the three corners of each face.*/
            std::vector<std::array<size_t, 3>> face_texture_coordinates_; /**< @brief Texture coordinates of the three corners of each face.*/

        private:
            /**
             * @brief Fills the class' members form a .obj file.
             *
             * @param filename Path to a geometry file in .obj format.
             */
            auto readObj(const std::filesystem::path& filename) -> void;

            /**
             * @brief Fills the class' members form a .su2 file.
             *
             * The faces in the 'WALL' MARKER_TAG sections will be used.
             *
             * @param filename Path to a geometry file in .su2 format.
             */
            auto readSU2(const std::filesystem::path& filename) -> void;

            /**
             * @brief Constructs the face normals from the face points when none are supplied in the file.
             *
             * This will give a facetted look, as the faces will have the same normal at all points.
             *
             * @param normals_to_build Vector indicating which points have missing normals.
             */
            auto build_missing_normals(const std::vector<std::array<bool, 3>>& normals_to_build) -> void;

            /**
             * @brief Constructs the texture coordinates when none are supplied in the file.
             *
             * This will set all missing texture coordinates to [0, 0].
             *
             * @param texture_coordinates_to_build Vector indicating which points have missing texture coordinates.
             */
            auto build_missing_texture_coordinates(const std::vector<std::array<bool, 3>>& texture_coordinates_to_build) -> void;
    };
}}

#include "entities/MeshGeometry_t.tpp"

#endif