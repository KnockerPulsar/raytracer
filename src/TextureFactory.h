#pragma once
#include "Defs.h"
#include "ImageTexture.h"
#include "SolidColor.h"
#include "Texture.h"
#include "CheckerTexture.h"
#include "Vec3.h"
#include <memory>

namespace raytracer {
  class TextureFactory {
  public:
    static sPtr<Texture> FromJson(json textureJson) {

      string texType = textureJson["type"].get<std::string>();

      if (texType == "solid_color") {
        Vec3 color = Vec3::FromJson(textureJson["color"]);
        return std::make_shared<SolidColor>(color);
      }

      if(texType == "checker")
      {
        float scale = textureJson["scale"].get<float>();
        Vec3 even = Vec3::FromJson(textureJson["even"]);
        Vec3 odd = Vec3::FromJson(textureJson["odd"]);
        return std::make_shared<CheckerTexture>(even,odd, scale);
      }

      if(texType == "image")
      {
        string path = textureJson["path"];
        return std::make_shared<ImageTexture>(path.c_str());
      }

      return nullptr;
    }
  };
} // namespace raytracer