#include "mesh.h"


namespace rt {
  Mesh::Mesh(std::filesystem::path path)
      : Hittable{std::format("Mesh: {}", path.string())}, model{LoadModel(path.c_str())},
        triangles{load_triangles(model)}
  {}

  Mesh::~Mesh() { UnloadModel(model); }

  auto Mesh::load_triangles(Model model) -> std::vector<Triangle> {
    auto const meshes         = std::span{model.meshes, static_cast<size_t>(model.meshCount)};
    auto const triangle_count = std::ranges::fold_left(
        meshes, 0u, [](auto const accumulator, auto const &mesh) { return accumulator + mesh.triangleCount; });

    std::vector<Triangle> triangles;
    triangles.reserve(triangle_count);
    for (auto const mesh : meshes) {
      auto const *vertex_data = reinterpret_cast<vec3 *>(mesh.vertices);

      // TODO UVs
      // indexed mesh. Number of indices == number of triangles * 3
      if (mesh.indices != nullptr) {

        auto const index_count = mesh.triangleCount * 3;
        auto const indices     = std::span{mesh.indices, static_cast<size_t>(index_count)};

        for (auto i = 0u; i < index_count; i += 3) {
          vec3 v0 = vertex_data[indices[i + 0]];
          vec3 v1 = vertex_data[indices[i + 1]];
          vec3 v2 = vertex_data[indices[i + 2]];

          // TODO are raylib normals per vertex or per triangle?
          vec3 v01 = (v1 - v0).Normalize();
          vec3 v02 = (v2 - v0).Normalize();
          vec3 n   = vec3::CrsProd(v01, v02).Normalize();

          triangles.push_back(Triangle{v0, v1, v2, n});
        }
      } else {
        for (auto i = 0u; i < mesh.vertexCount; i += 3) {
          vec3 v0 = vertex_data[i + 0];
          vec3 v1 = vertex_data[i + 1];
          vec3 v2 = vertex_data[i + 2];

          vec3 v01 = (v1 - v0).Normalize();
          vec3 v02 = (v2 - v0).Normalize();
          vec3 n   = vec3::CrsProd(v01, v02).Normalize();

          triangles.push_back(Triangle{v0, v1, v2, n});
        }
      }
    }

    return triangles;
  }

  bool Mesh::Hit(const Ray &r, float t_min, float t_max, HitRecord &rec) const {
    // TODO: Looks like BVHNode (and maybe HittableList) depend on leaf nodes
    // checking their own bounding boxes.They could probably be moved to be
    // stored in `Hittable`, each primitive can then initialize it accordingly.
    // That then can be checked in `HitTransformed`?
    //
    // Might be a bit difficult since bounding boxes should react to the
    // objects being transformed.
    if(!local_space_bb.Hit(r, t_min, t_max))
      return false;

    auto any_hit = false;
    std::ranges::for_each(triangles,
                          [&](auto const &triangle) { any_hit |= triangle_hit(triangle, this, r, t_min, t_max, rec); });

    return any_hit;
  }

  bool Mesh::BoundingBox(float t0, float t1, AABB &world_space_bb) const {
    float min_x = constants::infinity;
    float min_y = constants::infinity;
    float min_z = constants::infinity;
    float max_x = -constants::infinity;
    float max_y = -constants::infinity;
    float max_z = -constants::infinity;

    for (auto const &triangle : triangles) {
      min_x = std::min(min_x, std::min({triangle.v0.x, triangle.v1.x, triangle.v2.x}));
      min_y = std::min(min_y, std::min({triangle.v0.y, triangle.v1.y, triangle.v2.y}));
      min_z = std::min(min_z, std::min({triangle.v0.z, triangle.v1.z, triangle.v2.z}));

      max_x = std::max(max_x, std::max({triangle.v0.x, triangle.v1.x, triangle.v2.x}));
      max_y = std::max(max_y, std::max({triangle.v0.y, triangle.v1.y, triangle.v2.y}));
      max_z = std::max(max_z, std::max({triangle.v0.z, triangle.v1.z, triangle.v2.z}));
    }

    auto const min = vec3{min_x, min_y, min_z};
    auto const max = vec3{max_x, max_y, max_z};

    local_space_bb = AABB{min, max};
    world_space_bb = AABB{transformation.Apply(min), transformation.Apply(max)};

    return true;
  }

  void Mesh::Rasterize(vec3 color) {
    // Before calling this method, we push the object's transform onto raylib's
    // internal transformation stack.
    DrawModel(model, Vector3Zero(), 1.0f, color.toRaylibColor(255));
  }

  bool Mesh::triangle_hit(Triangle const &tri, Mesh const* hittable, const Ray &r, float t_min, float t_max, HitRecord &rec) {
    auto const [v0, v1, v2, normal] = tri;

    vec3  v0v1 = v1 - v0;
    vec3  v0v2 = v2 - v0;
    vec3  pvec = vec3::CrsProd(r.direction, v0v2);
    float det  = vec3::DotProd(v0v1, pvec);

    // ray and triangle are parallel if det is close to 0
    if (fabs(det) < 1e-6)
      return false;

    float invDet = 1 / det;

    vec3  tvec = r.origin - v0;
    float u    = vec3::DotProd(tvec, pvec) * invDet;
    if (u < 0 || u > 1)
      return false;

    vec3  qvec = vec3::CrsProd(tvec, v0v1);
    float v    = vec3::DotProd(r.direction, qvec) * invDet;
    if (v < 0 || u + v > 1)
      return false;

    float t = vec3::DotProd(v0v2, qvec) * invDet;

    if (t > t_min && t < t_max) {
      vec3 p     = r.At(t);
      rec.p      = p;
      rec.normal = normal;

      // UV interpolation
      // source:
      // https://www.gamedev.net/forums/topic/513814-raytracing-and-texture-mapping-triangles/
      // vec3 properUVs = v0.uvw + (v1.uvw - v0.uvw) * u + (v2.uvw - v0.uvw) * v;
      //
      // rec.u       = properUVs.x;
      // rec.v       = properUVs.y;
      rec.u = 0;
      rec.v = 0;
      rec.mat_ptr = hittable->material;
      rec.t       = t;
      rec.set_face_normal(r, normal);
      rec.closestHit = (Hittable *)hittable;

      return true;
    }

    return false;
  }
} // namespace rt
