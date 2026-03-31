#pragma once

#include "editor/hittable.h"
#include "editor/IRasterizable.h"

namespace rt::editor {
  class Object : public Hittable, public IRasterizable {};
}
