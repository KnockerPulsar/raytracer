#pragma once

#include "../IImguiDrawable.h"
#include "../data_structures/vec3.h"
#include "GroupPanel.h"
#include "editor/PreviewTexture.h"

#include <optional>
#include <raylib.h>
#include <imgui.h>

#include <functional>

namespace rt {
  class Texture : public IImguiDrawable {
  protected:
    float multiplier = 1.0f;

    std::optional<editor::PreviewTexture> previewTexture;

  public:
    Texture &operator=(Texture const &other) {
      previewTexture = std::nullopt;
      multiplier     = other.multiplier;

      return *this;
    }

    virtual vec3 Value(float u, float v, const vec3 &p) const = 0;
    virtual json toJson() const                               = 0;
    virtual void GetTexture(const json &j)                    = 0;
    virtual void setIntensity(float i) { multiplier = i; }

    virtual ~Texture() = default;

    virtual ::Texture generatePreview(int availableWidth, int availableHeight, float scale = 1.0f) {
      // Infinite textures can pass nullopt as the source width an height to indicate infinite size
      return Texture::SamplePreview(
          [](float u, float v) { return vec3(u, v, 0); },
          std::nullopt,
          std::nullopt,
          availableWidth,
          availableHeight,
          scale
      );
    }

  protected:
    ::Texture SamplePreview(
        std::function<vec3(float, float)> pointSampler,
        std::optional<int> sourceWidth, std::optional<int> sourceHeight,
        int destinationWidth, int destinationHeight,
        float scale
    ) const {
      auto const width = static_cast<int>(sourceWidth.value_or(destinationWidth) * scale);
      auto const height = static_cast<int>(sourceHeight.value_or(destinationHeight) * scale);

      auto *previewData = new vec3[width * height];
      for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
          float u = float(x) / width;
          float v = float(y) / height;

          previewData[x + (y * width)] = Value(u, v, pointSampler(u, v));
        }
      }

      Image previewImage = {
          .data    = previewData,
          .width   = width,
          .height  = height,
          .mipmaps = 1,
          .format  = PIXELFORMAT_UNCOMPRESSED_R32G32B32
      };

      auto const texture = LoadTextureFromImage(previewImage);
      UnloadImage(previewImage);

      return texture;
    }

    void previewOrGenerate() {
      ImGui::BeginGroupPanel("Texture preview");
      {
        ImGui::Dummy({-1, 10});

        auto regionMax     = ImGui::GetWindowContentRegionMax();
        auto regionMin     = ImGui::GetWindowContentRegionMin();
        auto availableArea = ImVec2{regionMax.x - regionMin.x, regionMax.y - regionMin.y};

        if (!previewTexture || previewTexture->ShouldResize(availableArea))
          previewTexture.emplace(generatePreview(availableArea.x, availableArea.y, 0.1f), availableArea.x, availableArea.y);

        ImGui::Image(
            previewTexture->GetImTextureID(), ImVec2(previewTexture->previewWidth, previewTexture->previewHeight)
        );
      }
      ImGui::EndGroupPanel();
    }
  };
} // namespace rt
