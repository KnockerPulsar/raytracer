#pragma once

#include "editor/IImguiDrawable.h"
#include "preview_texture.h"

namespace rt {
  namespace raytracer { class Texture; }

  namespace editor {
    class Texture : public IImguiDrawable {
    public:
      void OnImgui() override {
        previewTexture.possiblyGeneratePreview(*this);
        previewTexture.OnImgui();
      }

      virtual auto toRaytracerTexture() const -> std::shared_ptr<raytracer::Texture> {
        if (auto const previewRtTexture = previewTexture.getRaytracerTexture())
          return previewRtTexture;

        return createRaytracerTexture();
      }

      virtual auto createRaytracerTexture() const -> std::shared_ptr<raytracer::Texture> = 0;

      void invalidatePreview() { previewTexture.invalidate(); }

    private:
      PreviewTexture previewTexture;
    };
  } // namespace editor
} // namespace rt
