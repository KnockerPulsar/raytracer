#include "rendered_frames_window.h"

#include "editor/Utils.h"

void rt::editor::RenderedFramesWindow::draw() {
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0, 0});
    ImGui::Begin("Rendered Frames");
    if(!renderedFrames.empty())
    {
      // TODO remove IState and commit "Roll raytraced image viewer into editor"
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
        // Use a child window so it occupies the exact column space without overflowing
        if (ImGui::BeginChild("ImageRegion", ImVec2(0, 0), ImGuiChildFlags_None, ImGuiWindowFlags_NoScrollbar)) {
          ImVec2 availSpace = ImGui::GetContentRegionAvail();

          auto const &selectedFrame = renderedFrames[selectedItem.value_or(0)];
          auto const  frameSize     = ImVec2(selectedFrame.texture.width, selectedFrame.texture.height);
          auto const  fittedArea    = EditorUtils::FitIntoArea(availSpace, frameSize);

          // Optional: Center the image in the available space
          float offsetX = (availSpace.x - fittedArea.x) * 0.5f;
          float offsetY = (availSpace.y - fittedArea.x) * 0.5f;
          ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + offsetX, ImGui::GetCursorPosY() + offsetY));

          // Draw the image
          ImGui::Image(reinterpret_cast<ImTextureID>(&selectedFrame.texture.id), fittedArea, ImVec2{0, 1},
                       ImVec2{1, 0});
        }
        ImGui::EndChild();

        ImGui::TableSetColumnIndex(1);
        // Use a child window to create an independent scrolling region
        if (ImGui::BeginChild("ListRegion", ImVec2(0, 0), ImGuiChildFlags_None)) {
          std::array<char, 32> buf;
          for (auto i = 0u; i < renderedFrames.size(); i++) {

            auto const formatResult = std::format_to_n(buf.data(), buf.size(), "Item {}", i);
            buf[formatResult.size] = '\0';

            if (ImGui::Selectable(buf.data(), selectedItem == i)) {
              selectedItem = i;
              // Handle selection logic here
            }
          }
        }
        ImGui::EndChild();

        ImGui::EndTable();
      }
    }
    ImGui::End();
    ImGui::PopStyleVar();
}

void rt::editor::RenderedFramesWindow::onFrameRendered(raytracer::Frame const &frame) {
  renderedFrames.push_back(frame);
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
