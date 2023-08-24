template<typename T>
AGPTracer::Mediums::NonAbsorber_t<T>::NonAbsorber_t(T ind, unsigned int priority) : ind_(ind), priority_(priority) {}

template<typename T>
template<class R, size_t N>
auto AGPTracer::Mediums::NonAbsorber_t<T>::scatter(R& /*rng*/, std::uniform_real_distribution<T>& /*unif*/, AGPTracer::Entities::Ray_t<T, N>& /*ray*/) const -> bool {
    return false;
}
