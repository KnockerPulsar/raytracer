#include "mesh.h"

rt::editor::Mesh::Mesh(std::filesystem::path path) : model(LoadModel(path.string().c_str())) {}

void rt::editor::Mesh::Rasterize(vec3 color) {
  auto const scale = 1.0f;
  DrawModel(model, getTranslation(), scale, color.toRaylibColor());
}
