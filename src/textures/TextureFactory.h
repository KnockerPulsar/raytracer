#pragma once
#include "../Defs.h"
#include "CheckerTexture.h"
#include "ImageTexture.h"
#include "NoiseTexture.h"
#include "SolidColor.h"
#include "Texture.h"

#include <raylib.h>

#include <memory>

namespace rt {
  class TextureFactory {
  public:
    static sPtr<Texture> FromJson(json textureJson) {

      std::string texType = textureJson["type"].get<std::string>();

      if (texType == "solid_color") {
        return std::make_shared<SolidColor>(textureJson);
      }

      if (texType == "checker") {
        return std::make_shared<CheckerTexture>(textureJson);
      }

      if (texType == "image") {
        return std::make_shared<ImageTexture>(textureJson);
      }

      if (texType == "noise") {
        return std::make_shared<NoiseTexture>(textureJson);
      }

      return nullptr;
    }
  };
} // namespace rt
