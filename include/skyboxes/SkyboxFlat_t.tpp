template<typename T>
AGPTracer::Skyboxes::SkyboxFlat_t<T>::SkyboxFlat_t(const AGPTracer::Entities::Vec3<T>& background) : background_(background) {}

template<typename T>
auto AGPTracer::Skyboxes::SkyboxFlat_t<T>::get(const AGPTracer::Entities::Vec3<T>& /*xyz*/) const -> AGPTracer::Entities::Vec3<T> {
    return background_;
}
