#include "hittable.h"

void rt::editor::Hittable::setTranslation(vec3 translation) { transformation.setTranslation(translation); }
vec3 rt::editor::Hittable::getTranslation() const { return transformation.getTranslation(); }

void rt::editor::Hittable::rotateDelta(vec3 rotationDelta) { transformation.rotateDelta(rotationDelta); }
vec3 rt::editor::Hittable::getRotationEuler() const { return transformation.getRotationEuler(); }

void rt::editor::Hittable::setName(std::string newName) { name = newName; }
auto rt::editor::Hittable::getName() const -> std::string { return name; }
