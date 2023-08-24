template<typename T>
AGPTracer::Mediums::NonAbsorber_t<T>::NonAbsorber_t(T ind, unsigned int priority) : ind_(ind), priority_(priority) {}

template<typename T>
template<size_t N>
auto AGPTracer::Mediums::NonAbsorber_t<T>::scatter(AGPTracer::Entities::Ray_t<T, N>& /*ray*/) -> bool {
    return false;
}
