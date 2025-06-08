#pragma once
#include "../Perlin.h"
#include "Texture.h"

#include <imgui.h>
#include <raylib.h>

#include <sys/types.h>
#include <iostream>

// Note: raylib doesn't support JPG files by default
// If you want JPG support, you'll have to build raylib with JPG support.
// This can be done by changing raylib/src/config.h, finding a line with
// //#define SUPPORT_FILEFORMAT_JPG      1
// uncommenting it, then rebuilding raylib with it.
// Or you can use PNG, GIF, QOI, DDS and HDR images since they're supported by
// default.

// Another option if you're not using raylib is to use stb_image
// It's already included in raylib so I just used what raylib provided.
namespace rt {
  class Texture;

  class ImageTexture : public Texture {
  public:
    ImageTexture() : img{} {}

    ImageTexture(const char *filename, bool fv = false, bool fh = false) : path(filename), flipH(fh), flipV(fv) {
      ImageFromPath(filename);
    }

    ImageTexture(const json &json) { ImageFromPath(json["path"].get<std::string>().c_str()); }

    void ImageFromPath(const char *filename) {
      int compsPerPixel = bytesPerPixel;
      path              = filename;

      img = LoadImage(filename);
      if (img.data == nullptr) {
        std::cerr << "ERROR: could not load texture image file " << filename << ".\n";
        img.width = img.height = 0;
      }

      bytesPerScanline = bytesPerPixel * img.width;
    }

    ~ImageTexture() {
      // Causes a segfault
      /*UnloadImage(img);*/
    }

    virtual vec3 Value(float u, float v, const vec3 &p) const override {
      // If we have no texture data, then return solid cyan as a debugging aid.
      if (img.data == nullptr)
        return vec3(0, 1, 1);

      // Clamp input texture coordinates to [0,1] x [1,0]

      if (flipV)
        v = 1.0f - Clamp(v, 0.0, 1.0);

      if (flipH)
        u = 1.0f - Clamp(u, 0.0, 1.0);

      int i = u * img.width;
      int j = v * img.height;

      // Clamp integer mapping since actual coords should be < 1.0
      if (i >= img.width)
        i = img.width - 1;
      if (j >= img.height)
        j = img.height - 1;

      // Since we read 8 bit integer values for r, g, and b.
      // Need to convert them to [0,1.0]
      const float    colorScale = 1.0 / 255.0;
      unsigned char *pixel      = (unsigned char *)img.data + j * bytesPerScanline + i * bytesPerPixel;
      return vec3(pixel[0], pixel[1], pixel[2]) * colorScale * multiplier;
    }

    virtual json toJson() const override { return json{{"type", "image"}, {"path", path}}; }

    ::Texture generatePreview(int availableWidth, int availableHeight, float scale) override {
      return Texture::SamplePreview(
          [](float u, float v) { return vec3(u, v, 0); },
          img.width,
          img.height,
          availableWidth,
          availableHeight,
          scale
      );
    }

  private:
    int              bytesPerScanline;
    std::string      path;
    Image            img;
    bool             flipH = false, flipV = false;
    const static int bytesPerPixel = 3;
  };

  inline void to_json(json &j, const ImageTexture &it) { j = it.toJson(); }
} // namespace rt
