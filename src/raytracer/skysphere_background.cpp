#include "skysphere_background.h"
#include "common/Constants.h"

rt::raytracer::SkysphereBackground::SkysphereBackground(::Image const& skysphereImage)
    : skysphereImage(skysphereImage) {}

// Nearest neighbor sampling.
// TODO more sophisticated sampling
auto rt::raytracer::SkysphereBackground::sampleBackground(vec3 direction) -> vec3 {
  // Refer to `EditorUtils::generateSkysphere` for alignment
  float phi = std::atan2(direction.z, direction.x);

  // Shift from [-PI, PI] to [0, 2PI]
  if (phi < 0.0f) {
      phi += 2.0f * rt::constants::pi;
  }

  auto const u = phi / (2.0f * rt::constants::pi);

  // Match the mesh's Latitude: y = cos(theta) -> theta = acos(y)
  auto const v = std::acos(direction.y) / rt::constants::pi;

  auto const x = u * skysphereImage.width;
  auto const y = v * skysphereImage.height;

  return GetImageColor(skysphereImage, static_cast<int>(x), static_cast<int>(y));
}
