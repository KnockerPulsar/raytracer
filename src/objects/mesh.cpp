#include "mesh.h"

#include "common/raycast.h"

namespace {
  using Triangle = rt::raytracer::Mesh::Triangle;

  auto loadTriangles(::Model const& model) -> std::vector<Triangle> {
    auto const meshes         = std::span{model.meshes, static_cast<size_t>(model.meshCount)};
    auto const triangleCount = std::ranges::fold_left(
        meshes, 0u, [](auto const accumulator, auto const &mesh) { return accumulator + mesh.triangleCount; });

    std::vector<Triangle> triangles;
    triangles.reserve(triangleCount);
    for (auto const mesh : meshes) {
      auto const *vertexData = reinterpret_cast<vec3 *>(mesh.vertices);

      // TODO UVs
      // indexed mesh. Number of indices == number of triangles * 3
      if (mesh.indices != nullptr) {

        auto const indexCount = mesh.triangleCount * 3;
        auto const indices     = std::span{mesh.indices, static_cast<size_t>(indexCount)};

        for (auto i = 0u; i < indexCount; i += 3) {
          vec3 v0 = vertexData[indices[i + 0]];
          vec3 v1 = vertexData[indices[i + 1]];
          vec3 v2 = vertexData[indices[i + 2]];

          // TODO are raylib normals per vertex or per triangle?
          vec3 v01 = (v1 - v0).Normalize();
          vec3 v02 = (v2 - v0).Normalize();
          vec3 n   = vec3::CrsProd(v01, v02).Normalize();

          triangles.push_back(Triangle{v0, v1, v2, n});
        }
      } else {
        for (auto i = 0u; i < mesh.vertexCount; i += 3) {
          vec3 v0 = vertexData[i + 0];
          vec3 v1 = vertexData[i + 1];
          vec3 v2 = vertexData[i + 2];

          vec3 v01 = (v1 - v0).Normalize();
          vec3 v02 = (v2 - v0).Normalize();
          vec3 n   = vec3::CrsProd(v01, v02).Normalize();

          triangles.push_back(Triangle{v0, v1, v2, n});
        }
      }
    }

    return triangles;
  }

  std::optional<float> raycastTriangle(const rt::Ray &localspaceRay, vec3 v0, vec3 v1, vec3 v2,
                                                           float tMin, float tMax) {
    vec3  v0v1 = v1 - v0;
    vec3  v0v2 = v2 - v0;
    vec3  pvec = vec3::CrsProd(localspaceRay.direction, v0v2);
    float det  = vec3::DotProd(v0v1, pvec);

    // ray and triangle are parallel if det is close to 0
    if (fabs(det) < 1e-6)
      return {};

    float invDet = 1 / det;

    vec3  tvec = localspaceRay.origin - v0;
    float u    = vec3::DotProd(tvec, pvec) * invDet;
    if (u < 0 || u > 1)
      return {};

    vec3  qvec = vec3::CrsProd(tvec, v0v1);
    float v    = vec3::DotProd(localspaceRay.direction, qvec) * invDet;
    if (v < 0 || u + v > 1)
      return {};

    float t = vec3::DotProd(v0v2, qvec) * invDet;

    if (t < tMin && t > tMax) {
      return {};
    }
    return t;
  }

  auto localspaceBoundingBox(std::vector<Triangle> const &triangles) {
    auto const &infinity = rt::constants::infinity;
    float       minX    = infinity;
    float       minY    = infinity;
    float       minZ    = infinity;
    float       maxX    = -infinity;
    float       maxY    = -infinity;
    float       maxZ    = -infinity;

    for (auto const &triangle : triangles) {
      minX = std::min(minX, std::min({triangle.v0.x, triangle.v1.x, triangle.v2.x}));
      minY = std::min(minY, std::min({triangle.v0.y, triangle.v1.y, triangle.v2.y}));
      minZ = std::min(minZ, std::min({triangle.v0.z, triangle.v1.z, triangle.v2.z}));

      maxX = std::max(maxX, std::max({triangle.v0.x, triangle.v1.x, triangle.v2.x}));
      maxY = std::max(maxY, std::max({triangle.v0.y, triangle.v1.y, triangle.v2.y}));
      maxZ = std::max(maxZ, std::max({triangle.v0.z, triangle.v1.z, triangle.v2.z}));
    }

    auto const min = vec3{minX, minY, minZ};
    auto const max = vec3{maxX, maxY, maxZ};

    return rt::AABB{min, max};
  }
} // namespace

namespace rt::raytracer {
  Model::Model(std::shared_ptr<::Model> const &model) : triangles(loadTriangles(*model)) {}

  Mesh::Mesh(std::shared_ptr<::Model> const &model, Transformation const &transformation,
             sPtr<raytracer::Material> const &material)
      : Model(model), Hittable(transformation, material, localspaceBoundingBox(triangles)) {}

  bool Mesh::Hit(const rt::Ray &r, float t_min, float t_max, rt::HitRecord &rec) const {
    // TODO: Looks like BVHNode (and maybe HittableList) depend on leaf nodes
    // checking their own bounding boxes.They could probably be moved to be
    // stored in `Hittable`, each primitive can then initialize it accordingly.
    // That then can be checked in `HitTransformed`?
    //
    // Might be a bit difficult since bounding boxes should react to the
    // objects being transformed.
    if(!boundingBox.Hit(r, t_min, t_max))
      return false;

    struct HitResult {
      float t;
      vec3 n;
    };

    std::optional<HitResult> closestHitResult;
    std::ranges::for_each(triangles, [&](auto const &triangle) {
      if (auto const hitDistance = raycastTriangle(r, triangle.v0, triangle.v1, triangle.v2, t_min, t_max)) {
        closestHitResult = {*hitDistance, triangle.normal};
      }
    });

    if (!closestHitResult)
      return false;

    vec3 p     = r.At(closestHitResult->t);
    rec.p      = p;
    rec.normal = closestHitResult->n;

    // TODO UV interpolation
    // source:
    // https://www.gamedev.net/forums/topic/513814-raytracing-and-texture-mapping-triangles/
    // vec3 properUVs = v0.uvw + (v1.uvw - v0.uvw) * u + (v2.uvw - v0.uvw) * v;
    //
    // rec.u       = properUVs.x;
    // rec.v       = properUVs.y;
    rec.u = 0;
    rec.v = 0;
    rec.mat_ptr = material;
    rec.t       = closestHitResult->t;
    rec.set_face_normal(r, closestHitResult->n);
    rec.closestHit = (Hittable *)this;

    return true;
  }
} // namespace rt
