#include "image_texture.h"

#include <filesystem>
#include <iostream>

rt::editor::ImageTexture::ImageTexture(std::filesystem::path imagePath, bool flipVertically, bool flipHorizontally)
    : path(imagePath), flipH(flipVertically), flipV(flipHorizontally) {
  imageFromPath(imagePath);
}

void rt::editor::ImageTexture::imageFromPath(std::filesystem::path imagePath) {
  int compsPerPixel = bytesPerPixel;

  img = LoadImage(imagePath.c_str());
  if (img.data == nullptr) {
    std::cerr << "ERROR: could not load texture image file " << imagePath << ".\n";
    img.width = img.height = 0;
  }

  bytesPerScanline = bytesPerPixel * img.width;
}
