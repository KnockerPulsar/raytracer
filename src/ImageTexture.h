#pragma once
#include "Perlin.h"
#include "Texture.h"
#include <iostream>
#include <raylib.h>
#include <sys/types.h>


// Note: raylib doesn't support JPG files by default
// If you want JPG support, you'll have to build raylib with JPG support.
// This can be done by changing raylib/src/config.h, finding a line with 
// //#define SUPPORT_FILEFORMAT_JPG      1
// uncommenting it, then rebuilding raylib with it.
// Or you can use PNG, GIF, QOI, DDS and HDR images since they're supported by default.

// Another option if you're not using raylib is to use stb_image
// It's already included in raylib so I just used what raylib provided.
namespace raytracer {
  class Texture;

  class ImageTexture : public Texture {
  public:
    const static int bytesPerPixel = 3;

    ImageTexture() : img{} {}

    ImageTexture(const char *filename) {
      int compsPerPixel = bytesPerPixel;

      // data =
      //     LoadImage(filename, &width, &height, &compsPerPixel,
      //     compsPerPixel);
      img = LoadImage(filename);

      if (img.data == nullptr) {
        std::cerr << "ERROR: could not load texture image file " << filename
                  << ".\n";
        img.width = img.height = 0;
      }

      bytesPerScanline = bytesPerPixel * img.width;
    }

    ~ImageTexture() { UnloadImage(img); }

    virtual Vec3 Value(float u, float v, const Vec3 &p) const override {
      // If we have no texture data, then return solid cyan as a debugging aid.
      if (img.data == nullptr)
        return Vec3(0, 1, 1);

      // Clamp input texture coordinates to [0,1] x [1,0]
      u = Clamp(u, 0.0, 1.0);
      v = 1.0f - Clamp(v, 0.0, 1.0); // Flip v to image coords

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
      unsigned char *pixel =
          (unsigned char *)img.data + j * bytesPerScanline + i * bytesPerPixel;
      return Vec3(pixel[0], pixel[1], pixel[2]) * colorScale;
    }

  private:
    int   bytesPerScanline;
    Image img;
  };
} // namespace raytracer