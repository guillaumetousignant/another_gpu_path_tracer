#ifndef AGPTRACER_CAMERAS_SPHERICALCAMERA_T_HPP
#define AGPTRACER_CAMERAS_SPHERICALCAMERA_T_HPP

#include "entities/Image.hpp"
#include "entities/MediumList_t.hpp"
#include "entities/RandomGenerator_t.hpp"
#include "entities/Scene_t.hpp"
#include "entities/Shape.hpp"
#include "entities/Skybox.hpp"
#include "entities/TransformMatrix_t.hpp"
#include "entities/Vec3.hpp"
#include "images/SimpleImage_t.hpp"
#include "skyboxes/SkyboxFlat_t.hpp"
#include <array>
#include <filesystem>
#include <list>
#include <random>
#include <sycl/sycl.hpp>

namespace AGPTracer { namespace Cameras {

    /**
     * @brief The SphericalCamera_t class describes a camera that uses a spherical projection.
     *
     * It uses a spherical projection [r, theta, phi], where rays are equally spaced
     * in the theta and phi directions. This reduces warping at higher fields of view,
     * and permits the usage of arbitrarily high fields of view.
     * This camera stores the result from its rays in a single image buffer, and has no
     * other effects like motion blur or aperture.
     *
     * @tparam T Floating point datatype to use
     * @tparam K Skybox type
     * @tparam I Image type
     * @tparam N Number of mediums in the camera and ray's medium list
     */
    template<typename T = double, template<typename> typename K = Skyboxes::SkyboxFlat_t, template<typename> typename I = Images::SimpleImage_t, size_t N = 16>
    requires Entities::Skybox<K, T>&& Entities::Image<I, T> class SphericalCamera_t {
        public:
            /**
             * @brief Construct a new SphericalCamera_t object.
             *
             * @param transformation Transformation matrix used by the camera. Sets the camera's origin and direction when created and updated.
             * @param filename Filename used by the camera when saving an image. Not used by all cameras, and some might modify it or save multiple pictures.
             * @param up Vector pointing up. Used to set the roll of the camera.
             * @param fov Array containing field of view of camera [vertical, horizontal].
             * @param subpix Array containing the number of subpixels per pixel, [vertical, horizontal], for antialiasing purposes. Usually [1, 1].
             * @param medium_list Initial list of materials in which the camera is placed. Should have at least two copies of an "outside" medium not assigned to any object (issue #25).
             * @param skybox Skybox that will be intersected if no shape is hit by the rays.
             * @param max_bounces Maximum intersections with shapes and bounces on materials a ray can do before it is terminated. Actual number may be less.
             * @param gammaind Gamma of the saved picture. A value of 1 should be used for usual cases.
             * @param image Image buffer into which the resulting image will be stored.
             */
            SphericalCamera_t(Entities::TransformMatrix_t<T> transformation,
                              std::filesystem::path filename,
                              Entities::Vec3<T> up,
                              std::array<T, 2> fov,
                              std::array<unsigned int, 2> subpix,
                              Entities::MediumList_t<N> medium_list,
                              K<T> skybox,
                              unsigned int max_bounces,
                              T gammaind,
                              I<T> image);

            Entities::TransformMatrix_t<T> transformation_; /**< @brief Transformation matrix used by the camera. Sets the camera's origin and direction when created and updated.*/
            std::filesystem::path
                filename_; /**< @brief Filename used by the camera when saving an image. Not used by all cameras, and some might modify it or save multiple pictures with variations of the name.*/
            std::array<T, 2> fov_; /**< @brief Array containing field of view of camera [vertical, horizontal]. Will have different units depending on the projection used.*/
            std::array<T, 2> fov_buffer_; /**< @brief Stores the field of view until the camera is updated.*/
            std::array<unsigned int, 2>
                subpix_; /**< @brief Array containing the number of subpixels per pixel, [vertical, horizontal], for antialiasing purposes. Not very useful because sample jittering when there are
                            multiple samples per pixel removes aliasing. May be useful when there are few samples per pixel and location of the samples mush be controlled.*/
            Entities::MediumList_t<N> medium_list_; /**< @brief List of materials in which the camera is placed. Active medium is first element. Should have at least two copies of an "outside" medium
                      // not assigned to any object (issue #25).*/
            K<T> skybox_; /**< @brief Skybox that will be intersected if no shape is hit by the rays.*/
            unsigned int max_bounces_; /**< @brief Maximum intersections with shapes and bounces on materials a ray can do before it is terminated. Actual number may be less.*/
            Entities::Vec3<T> direction_; /**< @brief Direction in which the camera points. Changed by modifying the camera's transformation matrix.*/
            Entities::Vec3<T> origin_; /**< @brief Position of the camera. Changed by modifying the camera's transformation matrix.*/
            T gammaind_; /**< @brief Gamma of the saved picture. A value of 1 should be used for usual cases.*/
            Entities::Vec3<T> up_; /**< @brief Vector pointing up. Used to set the roll of the camera. Changed by setUp.*/
            Entities::Vec3<T> up_buffer_; /**< @brief Stores the up vector until the camera is updated.*/
            I<T> image_; /**< @brief Image buffer into which the image is stored.*/

            /**
             * @brief Updates the camera's members.
             *
             * This is used to set the new direction, origin, up, and other variables. Should be called once per frame, before rendering. This is how the changes to the transformation matrix and
             * functions like setUp take effect.
             */
            auto update() -> void;

            /**
             * @brief Sends rays through the scene, to generate an image.
             *
             * The camera will generate rays according to a spherical projection, and cast them through the provided scene.
             * The resulting colour is written to the image buffer. This will generate one image.
             *
             * @tparam R Random generator type to use
             * @tparam S Shape type to use
             * @param queue Device queue to use to run computations
             * @param random_generator Random generator used to get random numbers
             * @param scene Scene that will be used to find what each ray hits.
             */
            template<class R, template<typename> typename S>
            requires Entities::Shape<S, T> auto raytrace(sycl::queue& queue, Entities::RandomGenerator_t<T, R>& random_generator, Entities::Scene_t<T, S>& scene) -> void;

            /**
             * @brief Raytraces the scene multiple times to get more samples per pixel.
             *
             * This function will raytrace the same scene multiple times in order to accumulate more samples. This will reduce noise.
             *
             * @tparam R Random generator type to use
             * @tparam S Shape type to use
             * @param queue Device queue to use to run computations
             * @param random_generator Random generator used to get random numbers
             * @param scene Scene that will be used to find what each ray hits.
             * @param n_iter Number of times the scene will be raytraced.
             */
            template<class R, template<typename> typename S>
            requires Entities::Shape<S, T> auto accumulate(sycl::queue& queue, Entities::RandomGenerator_t<T, R>& random_generator, Entities::Scene_t<T, S>& scene, unsigned int n_iter) -> void;

            /**
             * @brief Raytraces the scene indefinitely to get more samples per pixel.
             *
             * This function will raytrace the same scene indefinitely in order to accumulate more samples. This will reduce noise.
             *
             * @tparam R Random generator type to use
             * @tparam S Shape type to use
             * @param queue Device queue to use to run computations
             * @param random_generator Random generator used to get random numbers
             * @param scene Scene that will be used to find what each ray hits.
             */
            template<class R, template<typename> typename S>
            requires Entities::Shape<S, T> auto accumulate(sycl::queue& queue, Entities::RandomGenerator_t<T, R>& random_generator, Entities::Scene_t<T, S>& scene) -> void;

            /**
             * @brief Raytraces the scene multiple times to get more samples per pixel, saving the image every so often.
             *
             * This function will raytrace the same scene multiple times in order to accumulate more samples. This will reduce noise. It will also save the generated image at an interval.
             *
             * @tparam R Random generator type to use
             * @tparam S Shape type to use
             * @param queue Device queue to use to run computations
             * @param random_generator Random generator used to get random numbers
             * @param scene Scene that will be used to find what each ray hits.
             * @param n_iter Number of times the scene will be raytraced.
             * @param interval Saves the image every x frames by calling write().
             */
            template<class R, template<typename> typename S>
            requires Entities::Shape<S, T> auto
            accumulateWrite(sycl::queue& queue, Entities::RandomGenerator_t<T, R>& random_generator, Entities::Scene_t<T, S>& scene, unsigned int n_iter, unsigned int interval) -> void;

            /**
             * @brief Raytraces the scene indefinitely to get more samples per pixel, saving the image every so often.
             *
             * This function will raytrace the same scene indefinitely in order to accumulate more samples. This will reduce noise. It will also save the generated image at an interval.
             *
             * @tparam R Random generator type to use
             * @tparam S Shape type to use
             * @param queue Device queue to use to run computations
             * @param random_generator Random generator used to get random numbers
             * @param scene Scene that will be used to find what each ray hits.
             * @param interval Saves the image every x frames by calling write().
             */
            template<class R, template<typename> typename S>
            requires Entities::Shape<S, T> auto accumulateWrite(sycl::queue& queue, Entities::RandomGenerator_t<T, R>& random_generator, Entities::Scene_t<T, S>& scene, unsigned int interval) -> void;

            /**
             * @brief Raytraces the scene indefinitely to get more samples per pixel, saving the image frame.
             *
             * This function will raytrace the same scene indefinitely in order to accumulate more samples. This will reduce noise. It will also save the generated image every frame.
             *
             * @tparam R Random generator type to use
             * @tparam S Shape type to use
             * @param queue Device queue to use to run computations
             * @param random_generator Random generator used to get random numbers
             * @param scene Scene that will be used to find what each ray hits.
             */
            template<class R, template<typename> typename S>
            requires Entities::Shape<S, T> auto accumulateWrite(sycl::queue& queue, Entities::RandomGenerator_t<T, R>& random_generator, Entities::Scene_t<T, S>& scene) -> void;

            /**
             * @brief Sets the focus distance of the camera to a specific distance.
             *
             * This is used to set the focal plane distance of the camera to a given value. Defaults to doing nothing, as cameras use the pinhole model by default, where everything is in focus.
             * The focal plane's shape will vary based on the projection used.
             *
             * @param focus_distance How far will the focal plane be.
             */
            auto focus(T focus_distance) -> void{};

            /**
             * @brief Sets the focus distance of the camera to put an object in focus.
             *
             * The scene will be intersected at a specific position of the frame, and the hit distance returned will be used to set the focus distance. This effectively puts that object in focus.
             * defaults to doing nothing, as cameras use the pinhole model by default, where everything is in focus. Will also intersect invisible objects, and won't propagate through mirrors and
             * such. The focal plane's shape will vary based on the projection used.
             *
             * @tparam S Shape type to use
             * @param queue Device queue to use to run computations
             * @param scene Scene that will be used to find what object the ray hits and its distance.
             * @param position Where in the frame will the ray be sent. [horizontal, vertical], both from 0 to 1, starting from bottom left.
             */
            template<template<typename> typename S>
            requires Entities::Shape<S, T> auto autoFocus(sycl::queue& queue, Entities::Scene_t<T, S>& scene, std::array<T, 2> position) -> void{};

            /**
             * @brief Set the up vector of the camera.
             *
             * The actual up will be changed on next update.
             *
             * @param new_up New up vector for the camera.
             */
            auto setUp(Entities::Vec3<T> new_up) -> void;

            /**
             * @brief Zooms the camera's field of view by a factor.
             *
             * This is used to change the camera's field of view by a factor.
             *
             * @param factor Factor by which to zoom the camera.
             */
            auto zoom(T factor) -> void;

            /**
             * @brief Sets the camera's field of view to a new value.
             *
             * This is used to change the camera's field of view to a new value.
             *
             * @param fov New field of view of the camera.
             */
            auto zoom(std::array<T, 2> fov) -> void;

            /**
             * @brief Writes the image buffer to disk with the provided name.
             *
             * This will write the camera's image to disk. It uses the input name.
             * This calls the image buffer's write function. Directory must exist.
             *
             * @param file_name Filename used to write the images.
             */
            auto write(const std::filesystem::path& file_name) -> void;

            /**
             * @brief Writes the image buffer to disk with the camera's filename.
             *
             * This will write the camera's image to disk. It uses the camera's filename_.
             * This calls the image buffer's write function. Directory must exist.
             */
            auto write() -> void;

            /**
             * @brief Resets the camera's image buffer, for when the scene or camera has changed.
             *
             * This will discard all accumulated samples and start accumulation from scratch. Calls the image buffer's
             * reset function.
             */
            auto reset() -> void;
    };
}}

#include "cameras/SphericalCamera_t.tpp"

#endif
