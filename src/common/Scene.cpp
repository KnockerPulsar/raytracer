#include "Scene.h"
#include "common/BVHNode.h"
#include "Camera.h"
#include "Defs.h"
#include "Hittable.h"
#include "HittableBuilder.h"
#include "HittableList.h"
#include "Ray.h"
#include "Transformation.h"
#include "Util.h"
#include "bvh_tree.h"
#include "data_structures/vec3.h"

#include "materials/Dielectric.h"
#include "materials/DiffuseLight.h"
#include "materials/Lambertian.h"
#include "materials/Material.h"
#include "materials/Metal.h"

// #include "objects/MovingSphere.h"
// #include "objects/ObjectFactory.h"
#include "objects/Plane.h"
#include "objects/Sphere.h"
#include "objects/Triangle.h"

#include "objects/mesh.h"
#include "textures/CheckerTexture.h"
#include "textures/ImageTexture.h"
#include "textures/NoiseTexture.h"

#include <raylib.h>

#include <fstream>
#include <memory>
#include <ostream>
#include <vector>

using nlohmann::json;

namespace rt::raytracer {
  std::vector<std::shared_ptr<Hittable>> Scene::GetObjects() const { return worldRoot->get_hittables(); }
} // namespace rt
