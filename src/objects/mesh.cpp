#include "mesh.h"

#include "common/Constants.h"
#include "common/raycast.h"

#include <algorithm>
#include <ranges>

namespace {
  using Triangle = rt::raytracer::Mesh::Triangle;

  auto loadTriangles(::Model const &model) -> std::vector<Triangle> {
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

  bool raycastTriangle(const rt::Ray &localspaceRay, const vec3 &v0, const vec3 &v1, const vec3 &v2, float tMin,
                       float tMax, float &hitDistance) {
    vec3  v0v1 = v1 - v0;
    vec3  v0v2 = v2 - v0;
    vec3  pvec = vec3::CrsProd(localspaceRay.direction, v0v2);
    float det  = vec3::DotProd(v0v1, pvec);

    // ray and triangle are parallel if det is close to 0
    if (fabs(det) < rt::constants::epsilon)
      return false;

    float invDet = 1 / det;

    vec3  tvec = localspaceRay.origin - v0;
    float u    = vec3::DotProd(tvec, pvec) * invDet;
    if (u < 0 || u > 1)
      return false;

    vec3  qvec = vec3::CrsProd(tvec, v0v1);
    float v    = vec3::DotProd(localspaceRay.direction, qvec) * invDet;
    if (v < 0 || u + v > 1)
      return false;

    float t = vec3::DotProd(v0v2, qvec) * invDet;

    if (t < tMin || t > tMax) {
      return false;
    }

    hitDistance = t;
    return true;
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

    auto const componentWiseMin = [](std::initializer_list<vec3> vertices) {
      return vec3{
        std::min(vertices, [](const vec3& a, const vec3& b) { return a.x < b.x; }).x,
        std::min(vertices, [](const vec3& a, const vec3& b) { return a.y < b.y; }).y,
        std::min(vertices, [](const vec3& a, const vec3& b) { return a.z < b.z; }).z,
      };
    };

    auto const componentWiseMax = [](std::initializer_list<vec3> vertices) {
      return vec3{
        std::max(vertices, [](const vec3& a, const vec3& b) { return a.x < b.x; }).x,
        std::max(vertices, [](const vec3& a, const vec3& b) { return a.y < b.y; }).y,
        std::max(vertices, [](const vec3& a, const vec3& b) { return a.z < b.z; }).z,
      };
    };

  rt::AABB getAABB(const Triangle& triangle)
  {
    auto const min = componentWiseMin({triangle.v0, triangle.v1, triangle.v2});
    auto const max = componentWiseMax({triangle.v0, triangle.v1, triangle.v2});

    return rt::AABB{min - rt::constants::epsilon, max + rt::constants::epsilon};
  }
} // namespace

namespace rt::raytracer {
  struct Mesh::MeshBVHNode {
    MeshBVHNode(std::span<Triangle> triangles)
    {
      auto const computeBoundingBox = [](auto const &triangles) {
        auto min = vec3(rt::constants::infinity);
        auto max = -min;
        for (auto const &triangle : triangles) {
          auto const boundingBox = getAABB(triangle);
          min                    = componentWiseMin({min, boundingBox.min});
          max                    = componentWiseMax({max, boundingBox.max});
        }

        return AABB{min, max};
      };

      boundingBox = computeBoundingBox(triangles);

      auto constexpr maxTrianglesPerLeaf = 4;
      if (triangles.size() <= maxTrianglesPerLeaf) {
        this->triangles = triangles;
        return;
      }

      auto const longestAxis = [&]() {
        auto xSpan = boundingBox.max.x - boundingBox.min.x;
        auto ySpan = boundingBox.max.y - boundingBox.min.y;
        auto zSpan = boundingBox.max.z - boundingBox.min.z;

        auto const longestSpan = std::max({xSpan, ySpan, zSpan});

        if (longestSpan == xSpan) {
          return 0;
        } else if (longestSpan == ySpan) {
          return 1;
        } else {
          return 2;
        }
      }();

      auto const triangleCentroid = [](const Triangle &triangle, auto const axis) {
        auto const component = [](vec3 v, auto const axis) {
          switch (axis) {
          case 0:
            return v.x;
          case 1:
            return v.y;
          case 2:
            return v.z;
          default:
            return 0.0f;
          }
        };
        return (component(triangle.v0, axis) + component(triangle.v1, axis) + component(triangle.v2, axis)) / 3.0f;
      };

      std::ranges::sort(triangles, [&](auto const &a, auto const &b) {
        return triangleCentroid(a, longestAxis) < triangleCentroid(b, longestAxis);
      });

      auto const mid = triangles.size() / 2;
      left           = std::make_unique<MeshBVHNode>(triangles.subspan(0, mid));
      right          = std::make_unique<MeshBVHNode>(triangles.subspan(mid));
      auto const bb = AABB::SurroundingBox(left->boundingBox, right->boundingBox);
    }

    bool hit(const rt::Ray& r, float t_min, float t_max, rt::HitRecord& rec) const {
      if (!boundingBox.Hit(r, t_min, t_max))
        return false;

      if (!triangles.empty()) {
        bool anyHit = false;
        auto closestT = t_max;

        for (const auto& triangle : triangles) {
          float hitDistance = rt::constants::infinity;
          if (raycastTriangle(r, triangle.v0, triangle.v1, triangle.v2, t_min, closestT, hitDistance)) {
            anyHit = true;
            closestT = hitDistance;
            rec.t = hitDistance;
            rec.normal = triangle.normal;
          }
        }

        return anyHit;
      }

      bool hitLeft  = left->hit(r, t_min, t_max, rec);
      bool hitRight = right->hit(r, t_min, hitLeft ? rec.t : t_max, rec);

      return hitLeft || hitRight;
    }


    std::unique_ptr<MeshBVHNode> left{nullptr};
    std::unique_ptr<MeshBVHNode> right{nullptr};

    rt::AABB boundingBox;
    std::span<Triangle> triangles;
  };

  Model::Model(std::shared_ptr<::Model> const &model) : triangles(loadTriangles(*model)) {}


  void Model::transform(const Transformation &transformation) {
    std::ranges::transform(triangles, triangles.begin(), [&](auto const &triangle) {
      auto const v0 = transformation.Apply(triangle.v0);
      auto const v1 = transformation.Apply(triangle.v1);
      auto const v2 = transformation.Apply(triangle.v2);

      vec3 v01 = (v1 - v0).Normalize();
      vec3 v02 = (v2 - v0).Normalize();
      vec3 n   = vec3::CrsProd(v01, v02).Normalize();

      return Triangle{v0, v1, v2, n};
    });
  }

  // Loads the model in localspace first, computes the AABB (which gets
  // transforme by the transformation) into worldspace in `Hittable`, then
  // bakes the transformation into the triangles.
  Mesh::Mesh(std::shared_ptr<::Model> const &model, Transformation const &transformation,
             sPtr<raytracer::Material> const &material)
      : Model(model), Raytracable(transformation, material, localspaceBoundingBox(triangles)) {
    transform(transformation);
    bvhRoot = std::make_unique<MeshBVHNode>(std::span{triangles});
  }

  Mesh::~Mesh() = default;

  bool Mesh::Hit(const rt::Ray &worldspaceRay, float t_min, float t_max, rt::HitRecord &rec) const {
    if (!bvhRoot->hit(worldspaceRay, t_min, t_max, rec))
      return {};

    // The BVH sets the normal and the hit distance (t)
    rec.p = worldspaceRay.At(rec.t);

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

    return true;
  }
} // namespace rt
