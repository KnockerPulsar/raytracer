#include "mesh.h"

#include "common/raycast.h"
#include "objects/mesh.h"

#include <ranges>
#include <algorithm>

namespace {
  auto loadModel(std::filesystem::path path) -> std::shared_ptr<::Model> {
    return std::shared_ptr<::Model>(new Model{LoadModel(path.c_str())}, [](auto modelPtr) {
      UnloadModel(*modelPtr);
      delete modelPtr;
    });
  }
} // namespace

rt::editor::Mesh::Mesh(std::filesystem::path path) : model(loadModel(path)) { setName(path); }

void rt::editor::Mesh::RasterizeWithoutTransformation(vec3 color) {
  auto const scale = 1.0f;
  DrawModel(*model, Vector3Zero(), scale, color.toRaylibColor());
}

std::optional<float> rt::editor::Mesh::hit(const Ray &worldspaceRay) const {
  if (model->meshes == nullptr || model->meshCount == 0)
    return false;

  auto const localspaceRay = rt::rayWorldToLocal(worldspaceRay, getTransformation());
  auto const meshes = std::span{model->meshes, static_cast<size_t>(model->meshCount)};

  auto const hitMesh = [&](auto const &mesh) -> std::optional<float> {
    auto const collisionResult =
        GetRayCollisionMesh(::Ray{localspaceRay.origin, localspaceRay.direction}, mesh, MatrixIdentity());

    if (!collisionResult.hit)
      return std::optional<float>{};
    return collisionResult.distance;
  };

  auto hitDistances = meshes | std::views::transform(hitMesh) |
                      std::views::filter([](const auto &opt) { return opt.has_value(); }) |
                      std::views::transform([](const auto &opt) { return *opt; });

  auto const minIter = std::ranges::min_element(hitDistances);
  if(minIter == hitDistances.end())
    return {};

  return *minIter;

}

auto rt::editor::Mesh::toRaytracable() const -> std::shared_ptr<rt::Raytracable> {
  return std::make_shared<raytracer::Mesh>(model, getTransformation().toRaytracerTransformation(),
                                           getMaterial()->toRaytracerMaterial());
}
