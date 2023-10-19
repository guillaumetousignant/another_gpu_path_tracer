#ifndef AGPTRACER_ENTITIES_MEDIUMLIST_T_HPP
#define AGPTRACER_ENTITIES_MEDIUMLIST_T_HPP

#include "entities/Translucent.hpp"
#include <array>
#include <sycl/sycl.hpp>

namespace AGPTracer::Entities {
    /**
     * @brief The medium list class represent a list of mediums into which a ray is travelling.
     *
     * The first one is the current one.
     *
     * @tparam N Maximum length of the medium list
     */
    template<size_t N = 16>
    class MediumList_t {
        public:
            /**
             * @brief Constructs a new MediumList_t object.
             */
            constexpr MediumList_t() : n_mediums_{0}, mediums_{} {};

            /**
             * @brief Constructs a new MediumList_t object from an array of mediums.
             *
             * @param n_mediums Number of mediums in the list
             * @param mediums List of mediums
             */
            constexpr explicit MediumList_t(size_t n_mediums, std::array<size_t, N> mediums) : n_mediums_{n_mediums}, mediums_{mediums} {};

            size_t n_mediums_; /**< @brief Number of mediums currently in the list*/
            std::array<size_t, N> mediums_; /**< @brief List of materials in which the ray travels. The first one is the current one.*/

            /**
             * @brief Adds a medium to a  list of mediums, according to the medium's priority.
             *
             * Adds a medium to a list of mediums. The input medium will be added before the first
             * medium from the list which has a priority equal or superior to the input medium's
             * priority. If it becomes the first element of the list, it becomes the active medium.
             *
             * @tparam T Floating point datatype
             * @tparam D Medium type
             * @param[in] accessor Accessor to a buffer of mediums
             * @param[in] medium Medium to be added to list of mediums.
             */
            template<class T, template<typename> typename D>
            requires Entities::Translucent<D, T> auto add_to_mediums(sycl::accessor<D<T>, 1, sycl::access::mode::read>& accessor, size_t medium) -> void {
                for (size_t i = 0; i < n_mediums_; ++i) {
                    if (accessor[mediums_[i]].priority_ <= accessor[medium].priority_) {
                        for (size_t j = i; j < std::min(n_mediums_, mediums_.size() - 1); ++j) {
                            mediums_[j + 1] = mediums_[j];
                        }
                        mediums_[i] = medium;
                        n_mediums_  = std::min(n_mediums_ + 1, mediums_.size());
                        return;
                    }
                }
                if (n_mediums_ < mediums_.size()) {
                    mediums_[n_mediums_] = medium;
                    ++n_mediums_;
                }
            };

            /**
             * @brief Removes the first instance of the input medium in the list of mediums.
             *
             * Removes a medium from a list of mediums. Will remove the first instance of the
             * medium. The first instance means the instance with the highest priority, so the
             * priority value closest to 0.
             *
             * @param[in] medium Medium to be removed from the list of mediums.
             */
            auto remove_from_mediums(size_t medium) -> void {
                for (size_t i = 0; i < n_mediums_; ++i) {
                    if (mediums_[i] == medium) {
                        for (size_t j = i; j < n_mediums_ - 1; ++j) {
                            mediums_[j] = mediums_[j + 1];
                        }
                        --n_mediums_;
                        return;
                    }
                }
            };
    };
}

#endif
