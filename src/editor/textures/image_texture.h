#pragma once

#include "texture.h"

namespace rt::editor {
  class ImageTexture : public Texture {
  public:
    ImageTexture(std::filesystem::path imagePath, bool flipVertically = false, bool flipHorizontally = false);

    auto createRaytracerTexture() const -> std::shared_ptr<raytracer::Texture> override;

  private:
    void imageFromPath(std::filesystem::path imagePath);

    int                   bytesPerScanline;
    std::filesystem::path path;
    Image                 img;
    bool                  flipH = false, flipV = false;
    const static int      bytesPerPixel = 3;
  };

  // inline void to_json(json &j, const ImageTexture &it) { j = it.toJson(); }
}
