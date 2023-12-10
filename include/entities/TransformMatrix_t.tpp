#include <algorithm>
#include <cmath>

template<typename T>
constexpr AGPTracer::Entities::TransformMatrix_t<T>::TransformMatrix_t() : matrix_{1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1} {
    buildInverse();
}

template<typename T>
constexpr AGPTracer::Entities::TransformMatrix_t<T>::TransformMatrix_t(T i0, T i1, T i2, T i3, T i4, T i5, T i6, T i7, T i8, T i9, T i10, T i11, T i12, T i13, T i14, T i15) :
        matrix_{i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11, i12, i13, i14, i15} {
    buildInverse();
}

template<typename T>
constexpr AGPTracer::Entities::TransformMatrix_t<T>::TransformMatrix_t(std::array<T, 16> values) : matrix_{values} {
    buildInverse();
}

template<typename T>
template<class T2>
constexpr auto AGPTracer::Entities::TransformMatrix_t<T>::rotateXAxis(T2 angle) -> TransformMatrix_t<T>& {
    const double cosine = sycl::cos(angle);
    const double sine   = sycl::sin(angle);

    const std::array<T, 16> other{1, 0, 0, 0, 0, cosine, sine, 0, 0, -sine, cosine, 0, 0, 0, 0, 1};
    const std::array<T, 16> matrix{matrix_};

    for (unsigned int j = 0; j < 4; j++) {
        for (unsigned int i = 0; i < 4; ++i) {
            matrix_[4 * j + i] = matrix[4 * j] * other[i] + matrix[4 * j + 1] * other[4 + i] + matrix[4 * j + 2] * other[8 + i] + matrix[4 * j + 3] * other[12 + i];
        }
    }

    buildInverse();
    return *this;
}

template<typename T>
template<class T2>
constexpr auto AGPTracer::Entities::TransformMatrix_t<T>::rotateYAxis(T2 angle) -> TransformMatrix_t<T>& {
    const double cosine = sycl::cos(angle);
    const double sine   = sycl::sin(angle);

    const std::array<decltype(sycl::cos(std::declval<T2>())), 16> other{cosine, 0, -sine, 0, 0, 1, 0, 0, sine, 0, cosine, 0, 0, 0, 0, 1};
    const std::array<T, 16> matrix{matrix_};

    for (unsigned int j = 0; j < 4; j++) {
        for (unsigned int i = 0; i < 4; ++i) {
            matrix_[4 * j + i] = matrix[4 * j] * other[i] + matrix[4 * j + 1] * other[4 + i] + matrix[4 * j + 2] * other[8 + i] + matrix[4 * j + 3] * other[12 + i];
        }
    }

    buildInverse();
    return *this;
}

template<typename T>
template<class T2>
constexpr auto AGPTracer::Entities::TransformMatrix_t<T>::rotateZAxis(T2 angle) -> TransformMatrix_t<T>& {
    const double cosine = sycl::cos(angle);
    const double sine   = sycl::sin(angle);

    const std::array<decltype(sycl::cos(std::declval<T2>())), 16> other{cosine, sine, 0, 0, -sine, cosine, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};
    const std::array<T, 16> matrix{matrix_};

    for (unsigned int j = 0; j < 4; j++) {
        for (unsigned int i = 0; i < 4; ++i) {
            matrix_[4 * j + i] = matrix[4 * j] * other[i] + matrix[4 * j + 1] * other[4 + i] + matrix[4 * j + 2] * other[8 + i] + matrix[4 * j + 3] * other[12 + i];
        }
    }

    buildInverse();
    return *this;
}

template<typename T>
template<class T2>
constexpr auto AGPTracer::Entities::TransformMatrix_t<T>::rotateX(T2 angle) -> TransformMatrix_t<T>& {
    const double cosine = sycl::cos(angle);
    const double sine   = sycl::sin(angle);

    const std::array<decltype(sycl::cos(std::declval<T2>())), 16> other{1,
                                                                        0,
                                                                        0,
                                                                        0, /* Dunno if those work, pre-multiplied them*/
                                                                        0,
                                                                        cosine,
                                                                        sine,
                                                                        0,
                                                                        0,
                                                                        -sine,
                                                                        cosine,
                                                                        0,
                                                                        0,
                                                                        matrix_[13] - matrix_[13] * cosine + matrix_[14] * sine,
                                                                        matrix_[14] - matrix_[14] * cosine - matrix_[13] * sine,
                                                                        1};
    const std::array<T, 16> matrix{matrix_};

    for (unsigned int j = 0; j < 4; j++) {
        for (unsigned int i = 0; i < 4; ++i) {
            matrix_[4 * j + i] = matrix[4 * j] * other[i] + matrix[4 * j + 1] * other[4 + i] + matrix[4 * j + 2] * other[8 + i] + matrix[4 * j + 3] * other[12 + i];
        }
    }

    buildInverse();
    return *this;
}

template<typename T>
template<class T2>
constexpr auto AGPTracer::Entities::TransformMatrix_t<T>::rotateY(T2 angle) -> TransformMatrix_t<T>& {
    const double cosine = sycl::cos(angle);
    const double sine   = sycl::sin(angle);

    const std::array<decltype(sycl::cos(std::declval<T2>())), 16> other{
        cosine, 0, -sine, 0, 0, 1, 0, 0, sine, 0, cosine, 0, matrix_[12] - matrix_[12] * cosine - matrix_[14] * sine, 0, matrix_[14] - matrix_[14] * cosine + matrix_[12] * sine, 1};
    const std::array<T, 16> matrix{matrix_};

    for (unsigned int j = 0; j < 4; j++) {
        for (unsigned int i = 0; i < 4; ++i) {
            matrix_[4 * j + i] = matrix[4 * j] * other[i] + matrix[4 * j + 1] * other[4 + i] + matrix[4 * j + 2] * other[8 + i] + matrix[4 * j + 3] * other[12 + i];
        }
    }

    buildInverse();
    return *this;
}

template<typename T>
template<class T2>
constexpr auto AGPTracer::Entities::TransformMatrix_t<T>::rotateZ(T2 angle) -> TransformMatrix_t<T>& {
    const double cosine = sycl::cos(angle);
    const double sine   = sycl::sin(angle);

    const std::array<decltype(sycl::cos(std::declval<T2>())), 16> other{
        cosine, sine, 0, 0, -sine, cosine, 0, 0, 0, 0, 1, 0, matrix_[12] - matrix_[12] * cosine + matrix_[13] * sine, matrix_[13] - matrix_[13] * cosine - matrix_[12] * sine, 0, 1};
    const std::array<T, 16> matrix{matrix_};

    for (unsigned int j = 0; j < 4; j++) {
        for (unsigned int i = 0; i < 4; ++i) {
            matrix_[4 * j + i] = matrix[4 * j] * other[i] + matrix[4 * j + 1] * other[4 + i] + matrix[4 * j + 2] * other[8 + i] + matrix[4 * j + 3] * other[12 + i];
        }
    }

    buildInverse();
    return *this;
}

template<typename T>
template<class T2, class T3>
auto AGPTracer::Entities::TransformMatrix_t<T>::rotateAxis(const AGPTracer::Entities::Vec3<T3>& vec, T2 angle) -> TransformMatrix_t<T>& { // In c++26 sqrt is constexpr
    const double cosine = sycl::cos(angle);
    const double sine   = sycl::sin(angle);
    const double t      = 1.0 - cosine;

    const AGPTracer::Entities::Vec3<T3> vec2 = vec.normalize(); // Dunno if needed
    const std::array<decltype(std::declval<T3>() * sycl::cos(std::declval<T2>())), 16> other{vec2[0] * vec2[0] * t + cosine,
                                                                                             vec2[1] * vec2[0] * t - vec2[2] * sine,
                                                                                             vec2[2] * vec2[0] * t + vec2[1] * sine,
                                                                                             0,
                                                                                             vec2[0] * vec2[1] * t + vec2[2] * sine,
                                                                                             vec2[1] * vec2[1] * t + cosine,
                                                                                             vec2[2] * vec2[1] * t - vec2[0] * sine,
                                                                                             0,
                                                                                             vec2[0] * vec2[2] * t - vec2[1] * sine,
                                                                                             vec2[1] * vec2[2] * t + vec2[0] * sine,
                                                                                             vec2[2] * vec2[2] * t + cosine,
                                                                                             0,
                                                                                             0,
                                                                                             0,
                                                                                             0,
                                                                                             1};
    const std::array<T, 16> matrix{matrix_};

    for (unsigned int j = 0; j < 4; j++) {
        for (unsigned int i = 0; i < 4; ++i) {
            matrix_[4 * j + i] = matrix[4 * j] * other[i] + matrix[4 * j + 1] * other[4 + i] + matrix[4 * j + 2] * other[8 + i] + matrix[4 * j + 3] * other[12 + i];
        }
    }

    buildInverse();
    return *this;
}

template<typename T>
template<class T2, class T3>
auto AGPTracer::Entities::TransformMatrix_t<T>::rotate(const AGPTracer::Entities::Vec3<T3>& vec, T2 angle) -> TransformMatrix_t<T>& { // In c++26 sqrt is constexpr
    const double cosine = sycl::cos(angle);
    const double sine   = sycl::sin(angle);
    const double t      = 1.0 - cosine;

    const AGPTracer::Entities::Vec3<T3> vec2 = vec.normalize(); // Dunno if needed
    const std::array<decltype(std::declval<T3>() * sycl::cos(std::declval<T2>())), 16> other{
        vec2[0] * vec2[0] * t + cosine,
        vec2[1] * vec2[0] * t - vec2[2] * sine,
        vec2[2] * vec2[0] * t + vec2[1] * sine,
        0,
        vec2[0] * vec2[1] * t + vec2[2] * sine,
        vec2[1] * vec2[1] * t + cosine,
        vec2[2] * vec2[1] * t - vec2[0] * sine,
        0,
        vec2[0] * vec2[2] * t - vec2[1] * sine,
        vec2[1] * vec2[2] * t + vec2[0] * sine,
        vec2[2] * vec2[2] * t + cosine,
        0,
        matrix_[12] - matrix_[12] * (vec2[0] * vec2[0] * t + cosine) - matrix_[13] * (vec2[0] * vec2[1] * t + vec2[2] * sine) - matrix_[14] * (vec2[0] * vec2[2] * t - vec2[1] * sine),
        matrix_[13] - matrix_[12] * (vec2[1] * vec2[0] * t - vec2[2] * sine) - matrix_[13] * (vec2[1] * vec2[1] * t + cosine) - matrix_[14] * (vec2[1] * vec2[2] * t + vec2[0] * sine),
        matrix_[14] - matrix_[12] * (vec2[2] * vec2[0] * t + vec2[1] * sine) - matrix_[13] * (vec2[2] * vec2[1] * t - vec2[0] * sine) - matrix_[14] * (vec2[2] * vec2[2] * t + cosine),
        1};
    // Wow just wow, such a line to write. I assume this is super slow
    const std::array<T, 16> matrix{matrix_};

    for (unsigned int j = 0; j < 4; j++) {
        for (unsigned int i = 0; i < 4; ++i) {
            matrix_[4 * j + i] = matrix[4 * j] * other[i] + matrix[4 * j + 1] * other[4 + i] + matrix[4 * j + 2] * other[8 + i] + matrix[4 * j + 3] * other[12 + i];
        }
    }

    buildInverse();
    return *this;
}

template<typename T>
template<class T2>
constexpr auto AGPTracer::Entities::TransformMatrix_t<T>::translate(const AGPTracer::Entities::Vec3<T2>& vec) -> TransformMatrix_t<T>& {
    const std::array<T2, 16> other{1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, vec[0], vec[1], vec[2], 1};
    const std::array<T, 16> matrix{matrix_};

    for (unsigned int j = 0; j < 4; j++) {
        for (unsigned int i = 0; i < 4; ++i) {
            matrix_[4 * j + i] = matrix[4 * j] * other[i] + matrix[4 * j + 1] * other[4 + i] + matrix[4 * j + 2] * other[8 + i] + matrix[4 * j + 3] * other[12 + i];
        }
    }

    buildInverse();
    return *this;
}

template<typename T>
template<class T2>
constexpr auto AGPTracer::Entities::TransformMatrix_t<T>::scaleAxis(const AGPTracer::Entities::Vec3<T2>& vec) -> TransformMatrix_t<T>& {
    const std::array<T2, 16> other{vec[0], 0, 0, 0, 0, vec[1], 0, 0, 0, 0, vec[2], 0, 0, 0, 0, 1};
    const std::array<T, 16> matrix{matrix_};

    for (unsigned int j = 0; j < 4; j++) { // This can be optimised
        for (unsigned int i = 0; i < 4; ++i) {
            matrix_[4 * j + i] = matrix[4 * j] * other[i] + matrix[4 * j + 1] * other[4 + i] + matrix[4 * j + 2] * other[8 + i] + matrix[4 * j + 3] * other[12 + i];
        }
    }

    buildInverse();
    return *this;
}

template<typename T>
template<class T2>
constexpr auto AGPTracer::Entities::TransformMatrix_t<T>::scaleAxis(T2 fac) -> TransformMatrix_t<T>& {
    const std::array<T2, 16> other{fac, 0, 0, 0, 0, fac, 0, 0, 0, 0, fac, 0, 0, 0, 0, 1};
    const std::array<T, 16> matrix{matrix_};

    for (unsigned int j = 0; j < 4; j++) { // This can be optimised
        for (unsigned int i = 0; i < 4; ++i) {
            matrix_[4 * j + i] = matrix[4 * j] * other[i] + matrix[4 * j + 1] * other[4 + i] + matrix[4 * j + 2] * other[8 + i] + matrix[4 * j + 3] * other[12 + i];
        }
    }

    buildInverse();
    return *this;
}

template<typename T>
template<class T2>
constexpr auto AGPTracer::Entities::TransformMatrix_t<T>::scale(const AGPTracer::Entities::Vec3<T2>& vec) -> TransformMatrix_t<T>& {
    const std::array<decltype(std::declval<T>() - std::declval<T2>() * std::declval<T>()), 16> other{
        vec[0], 0, 0, 0, 0, vec[1], 0, 0, 0, 0, vec[2], 0, matrix_[12] - vec[0] * matrix_[12], matrix_[13] - vec[1] * matrix_[13], matrix_[14] - vec[2] * matrix_[14], 1};
    const std::array<T, 16> matrix{matrix_};

    for (unsigned int j = 0; j < 4; j++) { // This can be optimised
        for (unsigned int i = 0; i < 4; ++i) {
            matrix_[4 * j + i] = matrix[4 * j] * other[i] + matrix[4 * j + 1] * other[4 + i] + matrix[4 * j + 2] * other[8 + i] + matrix[4 * j + 3] * other[12 + i];
        }
    }

    buildInverse();
    return *this;
}

template<typename T>
template<class T2>
constexpr auto AGPTracer::Entities::TransformMatrix_t<T>::scale(T2 fac) -> TransformMatrix_t<T>& {
    const std::array<decltype(std::declval<T>() - std::declval<T2>() * std::declval<T>()), 16> other{
        fac, 0, 0, 0, 0, fac, 0, 0, 0, 0, fac, 0, matrix_[12] - fac * matrix_[12], matrix_[13] - fac * matrix_[13], matrix_[14] - fac * matrix_[14], 1};
    const std::array<T, 16> matrix{matrix_};

    for (unsigned int j = 0; j < 4; j++) { // This can be optimised
        for (unsigned int i = 0; i < 4; ++i) {
            matrix_[4 * j + i] = matrix[4 * j] * other[i] + matrix[4 * j + 1] * other[4 + i] + matrix[4 * j + 2] * other[8 + i] + matrix[4 * j + 3] * other[12 + i];
        }
    }

    buildInverse();
    return *this;
}

template<typename T>
template<class T2>
constexpr auto AGPTracer::Entities::TransformMatrix_t<T>::reflect(const AGPTracer::Entities::Vec3<T2>& vec)
    -> TransformMatrix_t<T>& { // Needs a 4 component vec, or a distance factor. not used for now
    /*std::array<T2, 16> other {1 - 2 * vec[0] * vec[0], -2 * vec[0] * vec[1],
    -2 * vec[0] * vec[2], -2 * vec[0] * vec[3], -2 * vec[0] * vec[1], 1 - 2 *
    vec[1] * vec[1], -2 * vec[1] * vec[2], -2 * vec[1] * vec[3], -2 * vec[0] *
    vec[2], -2 * vec[1] * vec[2], 1 - 2 * vec[2] * vec[2], -2 * vec[2] * vec[3],
                                    0, 0, 0, 1};

    for (unsigned int j = 0; j < 4; j++) { // This can be optimised
        for (unsigned int i = 0; i < 4; ++i) {
            matrix_[4*j + i] =    matrix_[4*j] * other[i]
                                + matrix_[4*j + 1] * other[4 + i]
                                + matrix_[4*j + 2] * other[8 + i]
                                + matrix_[4*j + 3] * other[12 + i];
        }
    }*/

    buildInverse();
    return *this;
}

template<typename T>
template<class T2>
constexpr auto AGPTracer::Entities::TransformMatrix_t<T>::shear(const AGPTracer::Entities::Vec3<T2>& vec) -> TransformMatrix_t<T>& { // who the hell uses this
    buildInverse();
    return *this;
}

template<typename T>
constexpr auto AGPTracer::Entities::TransformMatrix_t<T>::transpose() -> TransformMatrix_t<T>& {
    matrix_ = {matrix_[0],
               matrix_[4],
               matrix_[8],
               matrix_[12],
               matrix_[1],
               matrix_[5],
               matrix_[9],
               matrix_[13],
               matrix_[2],
               matrix_[6],
               matrix_[10],
               matrix_[14],
               matrix_[3],
               matrix_[7],
               matrix_[11],
               matrix_[15]};

    buildInverse();
    return *this;
}

template<typename T>
constexpr auto AGPTracer::Entities::TransformMatrix_t<T>::invert() -> TransformMatrix_t<T>& { // oh boï
    // See https://graphics.stanford.edu/~mdfisher/Code/Engine/Matrix4.cpp.html

    const std::array<T, 16> transpose{matrix_[0],
                                      matrix_[4],
                                      matrix_[8],
                                      matrix_[12],
                                      matrix_[1],
                                      matrix_[5],
                                      matrix_[9],
                                      matrix_[13],
                                      matrix_[2],
                                      matrix_[6],
                                      matrix_[10],
                                      matrix_[14],
                                      matrix_[3],
                                      matrix_[7],
                                      matrix_[11],
                                      matrix_[15]};

    /* temp array for pairs */
    std::array<T, 12> tmp{transpose[10] * transpose[15],
                          transpose[11] * transpose[14],
                          transpose[9] * transpose[15],
                          transpose[11] * transpose[13],
                          transpose[9] * transpose[14],
                          transpose[10] * transpose[13],
                          transpose[8] * transpose[15],
                          transpose[11] * transpose[12],
                          transpose[8] * transpose[14],
                          transpose[10] * transpose[12],
                          transpose[8] * transpose[13],
                          transpose[9] * transpose[12]};

    // ij order might be messed up for these. If it is, order it: 0 4 8 12 1 5
    // 9 13.
    matrix_[0] = tmp[0] * transpose[5] + tmp[3] * transpose[6] + tmp[4] * transpose[7];
    matrix_[0] -= tmp[1] * transpose[5] + tmp[2] * transpose[6] + tmp[5] * transpose[7];
    matrix_[1] = tmp[1] * transpose[4] + tmp[6] * transpose[6] + tmp[9] * transpose[7];
    matrix_[1] -= tmp[0] * transpose[4] + tmp[7] * transpose[6] + tmp[8] * transpose[7];
    matrix_[2] = tmp[2] * transpose[4] + tmp[7] * transpose[5] + tmp[10] * transpose[7];
    matrix_[2] -= tmp[3] * transpose[4] + tmp[6] * transpose[5] + tmp[11] * transpose[7];
    matrix_[3] = tmp[5] * transpose[4] + tmp[8] * transpose[5] + tmp[11] * transpose[6];
    matrix_[3] -= tmp[4] * transpose[4] + tmp[9] * transpose[5] + tmp[10] * transpose[6];
    matrix_[4] = tmp[1] * transpose[1] + tmp[2] * transpose[2] + tmp[5] * transpose[3];
    matrix_[4] -= tmp[0] * transpose[1] + tmp[3] * transpose[2] + tmp[4] * transpose[3];
    matrix_[5] = tmp[0] * transpose[0] + tmp[7] * transpose[2] + tmp[8] * transpose[3];
    matrix_[5] -= tmp[1] * transpose[0] + tmp[6] * transpose[2] + tmp[9] * transpose[3];
    matrix_[6] = tmp[3] * transpose[0] + tmp[6] * transpose[1] + tmp[11] * transpose[3];
    matrix_[6] -= tmp[2] * transpose[0] + tmp[7] * transpose[1] + tmp[10] * transpose[3];
    matrix_[7] = tmp[4] * transpose[0] + tmp[9] * transpose[1] + tmp[10] * transpose[2];
    matrix_[7] -= tmp[5] * transpose[0] + tmp[8] * transpose[1] + tmp[11] * transpose[2];

    tmp[0] = transpose[2] * transpose[7];
    tmp[1] = transpose[3] * transpose[6];
    tmp[2] = transpose[1] * transpose[7];
    tmp[3] = transpose[3] * transpose[5];
    tmp[4] = transpose[1] * transpose[6];
    tmp[5] = transpose[2] * transpose[5];

    tmp[6]  = transpose[0] * transpose[7];
    tmp[7]  = transpose[3] * transpose[4];
    tmp[8]  = transpose[0] * transpose[6];
    tmp[9]  = transpose[2] * transpose[4];
    tmp[10] = transpose[0] * transpose[5];
    tmp[11] = transpose[1] * transpose[4];

    matrix_[8] = tmp[0] * transpose[13] + tmp[3] * transpose[14] + tmp[4] * transpose[15];
    matrix_[8] -= tmp[1] * transpose[13] + tmp[2] * transpose[14] + tmp[5] * transpose[15];
    matrix_[9] = tmp[1] * transpose[12] + tmp[6] * transpose[14] + tmp[9] * transpose[15];
    matrix_[9] -= tmp[0] * transpose[12] + tmp[7] * transpose[14] + tmp[8] * transpose[15];
    matrix_[10] = tmp[2] * transpose[12] + tmp[7] * transpose[13] + tmp[10] * transpose[15];
    matrix_[10] -= tmp[3] * transpose[12] + tmp[6] * transpose[13] + tmp[11] * transpose[15];
    matrix_[11] = tmp[5] * transpose[12] + tmp[8] * transpose[13] + tmp[11] * transpose[14];
    matrix_[11] -= tmp[4] * transpose[12] + tmp[9] * transpose[13] + tmp[10] * transpose[14];
    matrix_[12] = tmp[2] * transpose[10] + tmp[5] * transpose[11] + tmp[1] * transpose[9];
    matrix_[12] -= tmp[4] * transpose[11] + tmp[0] * transpose[9] + tmp[3] * transpose[10];
    matrix_[13] = tmp[8] * transpose[11] + tmp[0] * transpose[8] + tmp[7] * transpose[10];
    matrix_[13] -= tmp[6] * transpose[10] + tmp[9] * transpose[11] + tmp[1] * transpose[8];
    matrix_[14] = tmp[6] * transpose[9] + tmp[11] * transpose[11] + tmp[3] * transpose[8];
    matrix_[14] -= tmp[10] * transpose[11] + tmp[2] * transpose[8] + tmp[7] * transpose[9];
    matrix_[15] = tmp[10] * transpose[10] + tmp[4] * transpose[8] + tmp[9] * transpose[9];
    matrix_[15] -= tmp[8] * transpose[9] + tmp[11] * transpose[10] + tmp[5] * transpose[8];

    /* determinant */
    const T det = 1 / (transpose[0] * matrix_[0] + transpose[1] * matrix_[1] + transpose[2] * matrix_[2] + transpose[3] * matrix_[3]);

    for (auto& element: matrix_) {
        element *= det;
    }

    buildInverse();
    return *this;
}

template<typename T>
constexpr auto AGPTracer::Entities::TransformMatrix_t<T>::neg() -> TransformMatrix_t<T>& {
    for (auto& element: matrix_) {
        element *= -1.0;
    }

    buildInverse();
    return *this;
}

template<typename T>
template<class T2>
constexpr auto AGPTracer::Entities::TransformMatrix_t<T>::multVec(const AGPTracer::Entities::Vec3<T2>& vec) const -> AGPTracer::Entities::Vec3<T2> {
    std::array<decltype(std::declval<T2>() * std::declval<T>()), 4> vec2;
    for (unsigned int i = 0; i < 4; ++i) {
        vec2[i] = vec[0] * matrix_[i] + vec[1] * matrix_[i + 4] + vec[2] * matrix_[i + 8] + matrix_[i + 12];
    }
    return AGPTracer::Entities::Vec3<T2>(vec2[0], vec2[1], vec2[2]) / vec2[3];
}

template<typename T>
template<class T2>
auto AGPTracer::Entities::TransformMatrix_t<T>::multDir(const AGPTracer::Entities::Vec3<T2>& vec) const
    -> AGPTracer::Entities::Vec3<T2> { // Different than matlab, check. // In c++26 sqrt is constexpr
    // https://graphics.stanford.edu/~mdfisher/Code/Engine/Matrix4.cpp.html checks
    // for negative afterwards also
    std::array<decltype(std::declval<T2>() * std::declval<T>()), 4> vec2;
    for (unsigned int i = 0; i < 4; ++i) {
        vec2[i] = vec[0] * matrix_inverse_[i] + vec[1] * matrix_inverse_[i + 4] + vec[2] * matrix_inverse_[i + 8];
    }
    return AGPTracer::Entities::Vec3<T2>(vec2[0], vec2[1], vec2[2]).normalize_inplace(); /// vec2[3]; // vec2[3] will probably always be 0.
                                                                                         /// website puts at 1 then. if vec2[3] < 0, inverts
                                                                                         /// result.
    // Will maybe have to normalise this. (was not normalised)
}

template<typename T>
auto AGPTracer::Entities::TransformMatrix_t<T>::getScale() const -> T { // In c++26 sqrt is constexpr
    const T norm0 = AGPTracer::Entities::Vec3<T>(matrix_[0], matrix_[1], matrix_[2]).magnitude();
    const T norm1 = AGPTracer::Entities::Vec3<T>(matrix_[4], matrix_[5], matrix_[6]).magnitude();
    const T norm2 = AGPTracer::Entities::Vec3<T>(matrix_[7], matrix_[8], matrix_[9]).magnitude();
    return std::max(std::max(norm0, norm1), norm2);
}

template<typename T>
constexpr auto AGPTracer::Entities::TransformMatrix_t<T>::buildInverse() -> void {
    // See https://graphics.stanford.edu/~mdfisher/Code/Engine/Matrix4.cpp.html

    /* temp array for pairs */
    std::array<T, 12> tmp{matrix_[10] * matrix_[15],
                          matrix_[14] * matrix_[11],
                          matrix_[6] * matrix_[15],
                          matrix_[14] * matrix_[7],
                          matrix_[6] * matrix_[11],
                          matrix_[10] * matrix_[7],
                          matrix_[2] * matrix_[15],
                          matrix_[14] * matrix_[3],
                          matrix_[2] * matrix_[11],
                          matrix_[10] * matrix_[3],
                          matrix_[2] * matrix_[7],
                          matrix_[6] * matrix_[3]};

    // ij order might be messed up for these. If it is, order it: 0 4 8 12 1 5
    // 9 13.
    matrix_inverse_[0] = tmp[0] * matrix_[5] + tmp[3] * matrix_[9] + tmp[4] * matrix_[13];
    matrix_inverse_[0] -= tmp[1] * matrix_[5] + tmp[2] * matrix_[9] + tmp[5] * matrix_[13];
    matrix_inverse_[4] = tmp[1] * matrix_[1] + tmp[6] * matrix_[9] + tmp[9] * matrix_[13];
    matrix_inverse_[4] -= tmp[0] * matrix_[1] + tmp[7] * matrix_[9] + tmp[8] * matrix_[13];
    matrix_inverse_[8] = tmp[2] * matrix_[1] + tmp[7] * matrix_[5] + tmp[10] * matrix_[13];
    matrix_inverse_[8] -= tmp[3] * matrix_[1] + tmp[6] * matrix_[5] + tmp[11] * matrix_[13];
    matrix_inverse_[12] = tmp[5] * matrix_[1] + tmp[8] * matrix_[5] + tmp[11] * matrix_[9];
    matrix_inverse_[12] -= tmp[4] * matrix_[1] + tmp[9] * matrix_[5] + tmp[10] * matrix_[9];
    matrix_inverse_[1] = tmp[1] * matrix_[4] + tmp[2] * matrix_[8] + tmp[5] * matrix_[12];
    matrix_inverse_[1] -= tmp[0] * matrix_[4] + tmp[3] * matrix_[8] + tmp[4] * matrix_[12];
    matrix_inverse_[5] = tmp[0] * matrix_[0] + tmp[7] * matrix_[8] + tmp[8] * matrix_[12];
    matrix_inverse_[5] -= tmp[1] * matrix_[0] + tmp[6] * matrix_[8] + tmp[9] * matrix_[12];
    matrix_inverse_[9] = tmp[3] * matrix_[0] + tmp[6] * matrix_[4] + tmp[11] * matrix_[12];
    matrix_inverse_[9] -= tmp[2] * matrix_[0] + tmp[7] * matrix_[4] + tmp[10] * matrix_[12];
    matrix_inverse_[13] = tmp[4] * matrix_[0] + tmp[9] * matrix_[4] + tmp[10] * matrix_[8];
    matrix_inverse_[13] -= tmp[5] * matrix_[0] + tmp[8] * matrix_[4] + tmp[11] * matrix_[8];

    tmp[0] = matrix_[8] * matrix_[13];
    tmp[1] = matrix_[12] * matrix_[9];
    tmp[2] = matrix_[4] * matrix_[13];
    tmp[3] = matrix_[12] * matrix_[5];
    tmp[4] = matrix_[4] * matrix_[9];
    tmp[5] = matrix_[8] * matrix_[5];

    tmp[6]  = matrix_[0] * matrix_[13];
    tmp[7]  = matrix_[12] * matrix_[1];
    tmp[8]  = matrix_[0] * matrix_[9];
    tmp[9]  = matrix_[8] * matrix_[1];
    tmp[10] = matrix_[0] * matrix_[5];
    tmp[11] = matrix_[4] * matrix_[1];

    matrix_inverse_[2] = tmp[0] * matrix_[7] + tmp[3] * matrix_[11] + tmp[4] * matrix_[15];
    matrix_inverse_[2] -= tmp[1] * matrix_[7] + tmp[2] * matrix_[11] + tmp[5] * matrix_[15];
    matrix_inverse_[6] = tmp[1] * matrix_[3] + tmp[6] * matrix_[11] + tmp[9] * matrix_[15];
    matrix_inverse_[6] -= tmp[0] * matrix_[3] + tmp[7] * matrix_[11] + tmp[8] * matrix_[15];
    matrix_inverse_[10] = tmp[2] * matrix_[3] + tmp[7] * matrix_[7] + tmp[10] * matrix_[15];
    matrix_inverse_[10] -= tmp[3] * matrix_[3] + tmp[6] * matrix_[7] + tmp[11] * matrix_[15];
    matrix_inverse_[14] = tmp[5] * matrix_[3] + tmp[8] * matrix_[7] + tmp[11] * matrix_[11];
    matrix_inverse_[14] -= tmp[4] * matrix_[3] + tmp[9] * matrix_[7] + tmp[10] * matrix_[11];
    matrix_inverse_[3] = tmp[2] * matrix_[10] + tmp[5] * matrix_[14] + tmp[1] * matrix_[6];
    matrix_inverse_[3] -= tmp[4] * matrix_[14] + tmp[0] * matrix_[6] + tmp[3] * matrix_[10];
    matrix_inverse_[7] = tmp[8] * matrix_[14] + tmp[0] * matrix_[2] + tmp[7] * matrix_[10];
    matrix_inverse_[7] -= tmp[6] * matrix_[10] + tmp[9] * matrix_[14] + tmp[1] * matrix_[2];
    matrix_inverse_[11] = tmp[6] * matrix_[6] + tmp[11] * matrix_[14] + tmp[3] * matrix_[2];
    matrix_inverse_[11] -= tmp[10] * matrix_[14] + tmp[2] * matrix_[2] + tmp[7] * matrix_[6];
    matrix_inverse_[15] = tmp[10] * matrix_[10] + tmp[4] * matrix_[2] + tmp[9] * matrix_[6];
    matrix_inverse_[15] -= tmp[8] * matrix_[6] + tmp[11] * matrix_[10] + tmp[5] * matrix_[2];

    /* determinant */
    const T det = 1.0 / (matrix_[0] * matrix_inverse_[0] + matrix_[4] * matrix_inverse_[4] + matrix_[8] * matrix_inverse_[8] + matrix_[12] * matrix_inverse_[12]);

    for (auto& element: matrix_inverse_) {
        element *= det;
    }
}
