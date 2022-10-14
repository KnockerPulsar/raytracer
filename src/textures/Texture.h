#pragma once
#include "../IImguiDrawable.h"
#include "../data_structures/vec3.h"
#include "editor/Utils.h"
#include "imgui.h"
#include <cstddef>
#include <functional>
#include <limits>
#include <optional>
#include <raylib.h>

#define PREVIEW_WIDTH 100
#define PREVIEW_HEIGHT 100

#include "GroupPanel.h"

namespace rt {

  enum TextureTypes { SolidColorTex, ImageTex, Noise, Checker, TextureTypesCount };

  class Texture : public IImguiDrawable {
  protected:
    float multiplier = 1.0f;

    Texture2D previewTexture = Texture2D{
        .id      = std::numeric_limits<unsigned int>::max(),
        .width   = -1,
        .height  = -1,
        .mipmaps = -1,
        .format  = -1,
    };

    int   previewWidth = PREVIEW_WIDTH, previewHeight = PREVIEW_HEIGHT;
    float previewScale = 1;

  public:
    virtual vec3 Value(float u, float v, const vec3 &p) const = 0;
    virtual json toJson() const                               = 0;
    virtual void GetTexture(const json &j)                    = 0;
    virtual void setIntensity(float i) { multiplier = i; }

    virtual ~Texture() { UnloadTexture(previewTexture); }

    virtual void generatePreview() {}

    virtual void OnBaseImgui() {
      Texture::previewOrGenerate(); 
      OnDerivedImgui();
    }

  protected:
    void generatePreviewUtil(std::function<vec3(float, float)> pointSampler) {
      vec3 *previewData = new vec3[previewWidth * previewHeight];
      for (int y = 0; y < previewHeight; ++y) {
        for (int x = 0; x < previewWidth; ++x) {
          float u = float(x) / (previewWidth * previewScale);
          float v = float(y) / (previewHeight * previewScale);

          previewData[x + y * previewWidth] = Value(u, v, pointSampler(u, v));
        }
      }

      generatePreviewTexture(previewData, previewWidth, previewHeight);
    }

    void previewOrGenerate() {
      if (previewTexture.width != -1 && previewTexture.height != -1) {
        ImGui::BeginGroupPanel("Texture preview");

        ImGui::Dummy({-1, 10});

        float aspectRatio = float(previewTexture.width) / previewTexture.height;


        ImVec2 avail = ImGui::GetContentRegionAvail();
        ImVec2 contentMin = ImGui::GetWindowContentRegionMin();

        float drawX = (avail.x - previewWidth) * 0.5f;
        float drawY = (avail.y - previewHeight) * 0.5f;

        ImGui::SetCursorPos(ImVec2{drawX, drawY});
        ImGui::Image(&previewTexture.id, {float(previewWidth), float(previewHeight)});

        Texture::previewSettingsImgui();
        
        ImGui::EndGroupPanel();
      } else
        generatePreview();
    }

    void previewSettingsImgui() {

      ImGui::DragInt2("Preview resolution", &previewWidth);
      ImGui::DragFloat("Preview scale", &previewScale);

      if (ImGui::Button("Regen preview"))
        generatePreview();
    }

    virtual void generatePreviewTexture(vec3 *data, int previewWidth, int previewHeight) {
      Image previewImage = {
          .data    = data,
          .width   = previewWidth,
          .height  = previewHeight,
          .mipmaps = 1,
          .format  = PIXELFORMAT_UNCOMPRESSED_R32G32B32};

      previewTexture = LoadTextureFromImage(previewImage);

      UnloadImage(previewImage);
    }
  };
} // namespace rt