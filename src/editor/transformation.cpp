#include "transformation.h"

#include "editor/Utils.h"

#include "imgui.h"

void rt::editor::Transformation::OnImgui() {
  auto const translationChanged =
      ImGui::DragFloat3(("Translation##" + EditorUtils::GetIDFromPointer(this)).c_str(), &translate.x, 0.05f);

  auto euler = toEulerRotation();
  auto const originalEuler = euler;
  auto const rotationChanged =
      ImGui::DragFloat3(("Rotation##" + EditorUtils::GetIDFromPointer(this)).c_str(), &euler.x, 0.05f);

  if (rotationChanged) {
    rotateDelta(euler - originalEuler);
  }

  if (translationChanged || rotationChanged)
    recomputeCaches();
}
