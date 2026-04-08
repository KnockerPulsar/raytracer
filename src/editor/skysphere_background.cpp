#include "skysphere_background.h"

#include "editor/Utils.h"
#include "raytracer/skysphere_background.h"

namespace {
  Model GenerateSkysphereModel(float radius, int rings, int slices) {
    // 1. Generate a standard sphere mesh
    Mesh mesh = EditorUtils::generateSkysphere(radius, rings, slices);

    return LoadModelFromMesh(mesh);
  }
} // namespace

rt::editor::SkysphereBackground::SkysphereBackground(std::filesystem::path const &texturePath)
    : skysphereeTexture(LoadTexture(texturePath.c_str())), skysphereModel(GenerateSkysphereModel(100.0f, 64, 64)) {
      skysphereModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = skysphereeTexture;
    }

rt::editor::SkysphereBackground::~SkysphereBackground() {
  UnloadModel(skysphereModel);
  UnloadTexture(skysphereeTexture);
}

void rt::editor::SkysphereBackground::OnImgui() {
  // TODO
}

void rt::editor::SkysphereBackground::drawBackground(vec3 lookFrom) const {
  rlDisableBackfaceCulling();
  rlDisableDepthMask();
  DrawModel(skysphereModel, lookFrom, 1.0f, WHITE);
  rlEnableDepthMask();
  rlEnableBackfaceCulling();
}

auto rt::editor::SkysphereBackground::toRaytracerBackground() const -> std::unique_ptr<raytracer::Background> {
  return std::make_unique<raytracer::SkysphereBackground>(LoadImageFromTexture(skysphereeTexture));
}
