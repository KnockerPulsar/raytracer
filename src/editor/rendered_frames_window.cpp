#include "rendered_frames_window.h"

#include "editor/Utils.h"
#include "imgui.h"

#include <ImGuiFileDialog.h>
#include <raylib.h>

#include <chrono>
#include <filesystem>
#include <format>
#include <print>
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

  auto getUniquePath(std::filesystem::path directory, std::string filename) -> std::filesystem::path {
    auto const *extension = "bmp";
    auto        path      = directory / std::format("{}.{}", filename, extension);

    if (std::filesystem::exists(path)) {
      std::println("File '{}' already exists, appending a number to prevent overwriting", path.string());

      auto dupes = 1;
      path       = directory / std::format("{}-{}.{}", filename, dupes, extension);
      while (std::filesystem::exists(path)) {
        path = std::format("{}/{}-{}.{}", directory.string(), filename, dupes++, extension);
      }
    }
    return path;
  }
} // namespace

void rt::editor::RenderedFramesWindow::draw() {
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0, 0});

    ImGui::Begin("Rendered Frames");
    if (!renderedFrames.empty()) {
      renderMainLayout();
    }

    ImGui::End();
    ImGui::PopStyleVar();
}

void rt::editor::RenderedFramesWindow::onFrameRendered(raytracer::Frame const &frame) {
  renderedFrames.push_back(Frame{frame, std::chrono::system_clock::now()});
  focusOnItem(renderedFrames.size() - 1);
}

void rt::editor::RenderedFramesWindow::focusOnItem(int itemIndex) {
  focusedItem = itemIndex;
  selectedItems.clear();
  selectedItems.insert(itemIndex);
}

void rt::editor::RenderedFramesWindow::eraseSelected() {
  // Erase from end to start to avoid iterator invalidation
  for (auto iter = selectedItems.crbegin(); iter != selectedItems.crend(); ++iter)
    renderedFrames.erase(renderedFrames.begin() + *iter);

  selectedItems.clear();
  focusedItem.reset();
}

void rt::editor::RenderedFramesWindow::saveSelected(std::filesystem::path directory) {
  stbi_flip_vertically_on_write(1);
  for (auto const index : selectedItems) {
    auto const &frame     = renderedFrames.at(index);
    auto const &rtFrame   = frame.frame;
    auto const  imageSize = {rtFrame.texture.width, rtFrame.texture.height};
    auto const *extension = "bmp";

    auto const timeRounded = std::chrono::time_point_cast<std::chrono::seconds>(frame.renderFinishTime);
    auto const localTime   = std::chrono::current_zone()->to_local(timeRounded);

    auto const filename = std::format("{:%d-%m-%Y-%H-%M-%S}_{}x{}_{}_{}", localTime, rtFrame.texture.width,
                                      rtFrame.texture.height, rtFrame.numSamples, rtFrame.maxDepth);

    auto path = getUniquePath(directory, filename);

    struct AutoUnloadingImage {
      ::Image const img;
      AutoUnloadingImage(::Texture texture) : img{LoadImageFromTexture(texture)} {}
      ~AutoUnloadingImage() { UnloadImage(img); };
    };

    AutoUnloadingImage raytraced = AutoUnloadingImage(rtFrame.texture);
    if (stbi_write_bmp(path.c_str(), raytraced.img.width, raytraced.img.height, 4, raytraced.img.data) == 0) {
      auto const failureReason = errno != 0 ? std::format("System Error: {}", std::strerror(errno)) : "Unknown reason";
      std::println("Failed to write frame at path '{}'. Reason: {}", path.string(), failureReason);
      continue;
    }

    std::println("Successfully wrote frame to path '{}'", path.string());
  }
}
void rt::editor::RenderedFramesWindow::selectRange(uint64_t untilItem) {

  // Select from the focused item up or down the list to the clicked item.
  auto const direction = *focusedItem > untilItem ? -1 : 1;
  auto const start     = *focusedItem;
  auto const end       = untilItem + direction;

  selectedItems.clear();
  for (auto item = start; item != end; item += direction) {
    selectedItems.insert(item);
  }
}

void rt::editor::RenderedFramesWindow::toggleSelection(uint64_t item) {
  // Add/remove just this item to/from the selection
  if (selectedItems.contains(item))
    selectedItems.erase(item);
  else
    selectedItems.insert(item);
}

void rt::editor::RenderedFramesWindow::renderMainLayout() {
  // TODO if we close the application with this window open, the next
  // launch shouldn't open onto it

  auto constexpr flags = ImGuiTableFlags_Resizable | ImGuiTableFlags_BordersInnerV;
  if (ImGui::BeginTable("MainLayout", 2, flags, ImGui::GetContentRegionAvail())) {

    ImGui::TableSetupColumn("ImageCol", ImGuiTableColumnFlags_WidthStretch, 0.7f);
    ImGui::TableSetupColumn("ListCol", ImGuiTableColumnFlags_WidthStretch, 0.3f);
    ImGui::TableNextRow();

    ImGui::TableSetColumnIndex(0);
    centerImage("##MainPreviewImage", ImVec2{0, 0}, renderedFrames[focusedItem.value_or(0)].frame);

    ImGui::TableSetColumnIndex(1);
    renderSidePanel();

    ImGui::EndTable();
  }
}

void rt::editor::RenderedFramesWindow::renderSidePanel() {
  // Reserve enough space for the buttons to be visible without scrolling
  auto const footerHeight = ImGui::GetFrameHeightWithSpacing();
  if (ImGui::BeginChild("ListRegion", ImVec2(0, -footerHeight), ImGuiChildFlags_None)) {
    renderFrameList();
  }

  renderFrameListFooter();
}

void rt::editor::RenderedFramesWindow::renderFrameList() {
  // One master table for the entire list
  if (ImGui::BeginTable("MainListTable", 2, ImGuiTableFlags_SizingStretchProp)) {
    ImGui::TableSetupColumn("ImageCol", ImGuiTableColumnFlags_WidthStretch, 0.3f);
    ImGui::TableSetupColumn("DataCol", ImGuiTableColumnFlags_WidthStretch, 0.7f);

    auto const       rowHeight = 70.0f;
    ImGuiListClipper clipper;
    clipper.Begin(renderedFrames.size(), rowHeight);

    while (clipper.Step()) {
      for (auto i = clipper.DisplayStart; i < clipper.DisplayEnd; i++) {
        renderFrameListElement(i, rowHeight);
      }
    }

    ImGui::EndTable();
  }
  ImGui::EndChild();
}

void rt::editor::RenderedFramesWindow::renderFrameListFooter() {
  auto const spacing     = ImGui::GetStyle().ItemSpacing.x;
  auto const buttonWidth = (ImGui::GetContentRegionAvail().x - spacing) / 2.0f;

  if (ImGui::Button("Save", ImVec2(buttonWidth, 0))) {
    ImGuiFileDialog::Instance()->OpenDialog(
        "SaveSelected", "Choose a directory to save selected renders", nullptr,
        IGFD::FileDialogConfig{.countSelectionMax = 0, .flags = ImGuiFileDialogFlags_Modal});
  }

  if (ImGuiFileDialog::Instance()->Display("SaveSelected")) {
    if (ImGuiFileDialog::Instance()->IsOk()) {
      auto const directory = std::filesystem::path{ImGuiFileDialog::Instance()->GetCurrentPath()};
      saveSelected(directory);
    }

    ImGuiFileDialog::Instance()->Close();
  }

  ImGui::SameLine(0, spacing / 2.0);
  if (ImGui::Button("Delete", ImVec2(buttonWidth, 0))) {
    eraseSelected();
  }
}

void rt::editor::RenderedFramesWindow::renderFrameListElement(uint64_t i, float rowHeight) {
  ImGui::PushID(i);
  ImGui::TableNextRow(ImGuiTableRowFlags_None, rowHeight);
  ImGui::TableNextColumn();

  float startY = ImGui::GetCursorPosY();

  if (ImGui::Selectable("##item", selectedItems.contains(i),
                        ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowOverlap,
                        ImVec2(0, rowHeight))) {
    if (ImGui::IsKeyDown(ImGuiKey_ModShift)) {
      selectRange(i);
    } else if (ImGui::IsKeyDown(ImGuiMod_Ctrl)) {
      toggleSelection(i);
    } else {
      focusOnItem(i);
    }
  }

  ImGui::SetCursorPosY(startY);
  auto const &frame = renderedFrames[i];
  centerImage("##list_image", ImVec2{0, rowHeight}, frame.frame);

  ImGui::TableNextColumn();

  ImGui::BeginGroup();
  auto const timeRounded = std::chrono::time_point_cast<std::chrono::seconds>(frame.renderFinishTime);
  auto const localTime   = std::chrono::current_zone()->to_local(timeRounded);
  // TODO more information (samples per pixel, depth, seed, ...)
  ImGui::Text("%s", std::format("{:%d-%m-%Y-%H-%M-%S}", localTime).c_str());
  ImGui::Text("%dx%d", frame.frame.texture.width, frame.frame.texture.height);
  ImGui::Text("%ldms", frame.frame.renderTime.count());
  ImGui::EndGroup();
  ImGui::PopID();
}
