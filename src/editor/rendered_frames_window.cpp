#include "rendered_frames_window.h"

#include "editor/Utils.h"
#include "imgui.h"

#include <chrono>
#include <format>
#include <string_view>

namespace {
  void centerImage(std::string_view id, ImVec2 const &size, rt::raytracer::Frame const &frame) {
    // 1. Pass the unique ID to the child window
    if (ImGui::BeginChild(id.data(), size, ImGuiChildFlags_None, ImGuiWindowFlags_NoScrollbar)) {
      ImVec2 availSpace = ImGui::GetContentRegionAvail();
      auto const frameSize  = ImVec2(frame.texture.width, frame.texture.height);
      auto const fittedArea = EditorUtils::FitIntoArea(availSpace, frameSize);

      float offsetX = (availSpace.x - fittedArea.x) * 0.5f;
      // 2. Fixed typo (changed fittedArea.x to fittedArea.y)
      float offsetY = (availSpace.y - fittedArea.y) * 0.5f;

      // 3. SetCursorPos inside a child window is strictly local.
      // No need to add GetCursorPos() to it.
      ImGui::SetCursorPos(ImVec2(offsetX, offsetY));

      ImGui::Image(reinterpret_cast<ImTextureID>(&frame.texture.id), fittedArea, ImVec2{0, 1}, ImVec2{1, 0});
    }
    ImGui::EndChild();
  }

  template <typename... Args> auto formatWithNull(std::format_string<Args...> fmt, Args &&...args) -> const char * {
    static std::array<char, 32> buf;
    auto const [out, _] = std::format_to_n(buf.data(), buf.size(), fmt, args...);
    *out                = '\0';

    return buf.data();
  }
}

void rt::editor::RenderedFramesWindow::draw() {
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0, 0});
    ImGui::Begin("Rendered Frames");
    if(!renderedFrames.empty())
    {
      // TODO if we close the application with this window open, the next
      // launch shouldn't open onto it
      // TODO list of renders in memory, with the ability to single/batch
      // delete/save. Should show some data such as the render time.

      auto constexpr flags = ImGuiTableFlags_Resizable | ImGuiTableFlags_BordersInnerV;
      if (ImGui::BeginTable("MainLayout", 2, flags, ImGui::GetContentRegionAvail())) {

        ImGui::TableSetupColumn("ImageCol", ImGuiTableColumnFlags_WidthStretch, 0.7f);
        ImGui::TableSetupColumn("ListCol", ImGuiTableColumnFlags_WidthStretch, 0.3f);
        ImGui::TableNextRow();

        ImGui::TableSetColumnIndex(0);
        centerImage("MainPreviewImage", ImVec2{0, 0}, renderedFrames[selectedItem.value_or(0)].frame);

        ImGui::TableSetColumnIndex(1);
        if (ImGui::BeginChild("ListRegion", ImVec2(0, 0), ImGuiChildFlags_None)) {
          // One master table for the entire list
          if (ImGui::BeginTable("MainListTable", 2, ImGuiTableFlags_SizingStretchProp)) {
            ImGui::TableSetupColumn("ImageCol", ImGuiTableColumnFlags_WidthStretch, 0.3f);
            ImGui::TableSetupColumn("DataCol", ImGuiTableColumnFlags_WidthStretch, 0.7f);
            auto const rowHeight = 70.0f;

            for (auto i = 0u; i < renderedFrames.size(); i++) {
              ImGui::TableNextRow(ImGuiTableRowFlags_None, rowHeight);
              ImGui::TableNextColumn();

              float startY = ImGui::GetCursorPosY();

              if (ImGui::Selectable(formatWithNull("##item_{}", i), selectedItem == i,
                                    ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowOverlap,
                                    ImVec2(0, rowHeight))) {
                selectedItem = i;
              }

              ImGui::SetCursorPosY(startY);
              auto const& frame = renderedFrames[i];
              centerImage(formatWithNull("list_image_{}", i), ImVec2{0, rowHeight}, frame.frame);

              ImGui::TableNextColumn();

              ImGui::BeginGroup();
              ImGui::Text("%s", std::format("{:%Y-%m-%d %H:%M:%S}", frame.renderFinishTime).c_str());
              ImGui::Text("%dx%d", frame.frame.texture.width, frame.frame.texture.height);
              ImGui::Text("%ldms", frame.frame.renderTime.count());
              ImGui::EndGroup();
            }
            ImGui::EndTable();
          }
          ImGui::EndChild();
        }
        ImGui::EndTable();
      }
    }
    ImGui::End();
    ImGui::PopStyleVar();
}

void rt::editor::RenderedFramesWindow::onFrameRendered(raytracer::Frame const &frame) {
  renderedFrames.push_back(Frame{frame, std::chrono::system_clock::now()});
  selectedItem = renderedFrames.size() - 1;
}

// void rt::raytracer::Raytracer::Autosave() {
//   std::stringstream ss;
//
//   auto t  = std::time(nullptr);
//   auto tm = *std::localtime(&t);
//   auto const imageSize = ard.config.imageSize;
//   ss << "screenshots/" << std::put_time(&tm, "%d-%m-%Y %H-%M-%S") << "_" << imageSize.x << "x"
//      << imageSize.y << "_" << ard.config.numSamples << "_" << ard.config.maxDepth
//      << ".bmp";
//
//   std::cout << "Finished render: " << ss.str() << '\n';
//
//   Image raytraced = LoadImageFromTexture(ard.raytraceRT.texture);
//
//   // 4 components for RGBA;
//   // Couldn't get raylibs ExportImage to work with BMP. It did work with PNG however
//   stbi_write_bmp(ss.str().c_str(), raytraced.width, raytraced.height, 4, raytraced.data);
//
//   UnloadImage(raytraced);
// }
