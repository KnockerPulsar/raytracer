#pragma once

#include "editor/IImguiDrawable.h"
#include "editor/Utils.h"
#include "preview_texture.h"

namespace rt {
  namespace raytracer { class Texture; }

  namespace editor {
    class Texture : public IImguiDrawable {
    public:
      bool OnImgui() override {
        previewTexture.possiblyGeneratePreview(*this);
        return previewTexture.OnImgui();
      }

      virtual auto toRaytracerTexture() const -> std::shared_ptr<raytracer::Texture> {
        if (auto const previewRtTexture = previewTexture.getRaytracerTexture())
          return previewRtTexture;

        return createRaytracerTexture();
      }

      virtual auto createRaytracerTexture() const -> std::shared_ptr<raytracer::Texture> = 0;

      void invalidatePreview() { previewTexture.invalidate(); }

      template <typename Fn> bool previewHeader(char const *name, Fn &&fn) {
        auto changed = false;

        std::array<char, 256> buf{'\0'};
        std::format_to(buf.data(), "{}##{}", name, EditorUtils::GetIDFromPointer(this));

        if (ImGui::TreeNodeEx(buf.data(), ImGuiTreeNodeFlags_CollapsingHeader | ImGuiTreeNodeFlags_DefaultOpen)) {
          Texture::OnImgui();

          changed = fn();
          if (changed)
            invalidatePreview();

          ImGui::Spacing();
        }

        return changed;
      }

    private:
      PreviewTexture previewTexture;
    };
  } // namespace editor
} // namespace rt
