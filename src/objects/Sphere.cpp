#include "Sphere.h"
#include "../AABB.h"
#include "../Ray.h"
#include "../materials/Material.h"
#include <cmath>
#include <memory>
#include <raylib.h>
#include <raymath.h>

namespace rt {
  Sphere::Sphere(float r) : radius(r) {}
  Sphere::Sphere(float r, std::shared_ptr<Material> m) : Sphere(r) { material = m; }

  bool Sphere::Hit(const Ray &r, float t_min, float t_max, HitRecord &rec) const {

    // Spheres are centered at zero by default
    // We transform the ray hitting the object so no need to move it.
    auto center = vec3::Zero();

    // Vector between ray origin and sphere center
    vec3  oc    = r.origin - center;
    float a     = r.direction.SqrLen();
    float halfB = vec3::DotProd(oc, r.direction);
    float c     = oc.Len() * oc.Len() - radius * radius;

    float discriminant = halfB * halfB - a * c;
    if (discriminant < 0)
      return false;

    float sqrtDisc = sqrt(discriminant);

    float root = (-halfB - sqrtDisc) / a;
    if (root < t_min || t_max < root) {
      root = (-halfB + sqrtDisc) / a;
      if (root < t_min || t_max < root)
        return false;
    }

    rec.t                    = root;
    rec.p                    = r.At(rec.t);
    const vec3 outwardNormal = (rec.p - center) / radius;
    rec.set_face_normal(r, outwardNormal);
    GetSphereUV(outwardNormal, rec.u, rec.v);
    rec.mat_ptr    = material;
    rec.closestHit = (Hittable *)this;

    return true;
  }

  bool Sphere::BoundingBox(float t0, float t1, AABB &outputBox) const {
    outputBox = transformation.regenAABB(AABB(-vec3(radius), vec3(radius)));

    return true;
  }

  json Sphere::toJsonSpecific() const { return json{{"type", "sphere"}, {"radius", radius}}; }

  void Sphere::Rasterize(vec3 color) {
    // RasterizeTransformed takes care of the transformation and rotation
    DrawSphere(vec3::Zero(), radius, color.toRaylibColor(255));
  }

  void Sphere::GetSphereUV(const vec3 &p, float &u, float &v) {
    float theta = std::acos(-p.y);
    float phi   = std::atan2(-p.z, p.x) + PI;

    v = theta / PI;
    u = phi / (2 * PI);
  }
} // namespace rt
