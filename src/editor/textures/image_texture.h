#pragma once

#include "texture.h"

namespace rt::editor {
  class ImageTexture : public Texture {
  public:
    ImageTexture(std::filesystem::path imagePath);
  };

  // inline void to_json(json &j, const ImageTexture &it) { j = it.toJson(); }
}
