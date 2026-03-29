#pragma once
#include "common/Defs.h"
#include "checker_texture.h"
#include "image_texture.h"
#include "noise_texture.h"
#include "solid_color.h"

#include <raylib.h>

#include <memory>

namespace rt::editor {
  class TextureFactory {
  public:
    static sPtr<Texture> FromJson(json textureJson) {

      std::string texType = textureJson["type"].get<std::string>();

      // if (texType == "solid_color") {
      //   return std::make_shared<SolidColor>(textureJson);
      // }
      //
      // if (texType == "checker") {
      //   return std::make_shared<CheckerTexture>(textureJson);
      // }
      //
      // if (texType == "image") {
      //   return std::make_shared<ImageTexture>(textureJson);
      // }
      //
      // if (texType == "noise") {
      //   return std::make_shared<NoiseTexture>(textureJson);
      // }

      return nullptr;
    }
  };
} // namespace rt
