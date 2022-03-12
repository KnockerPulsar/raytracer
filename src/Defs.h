#pragma once
#include <memory>
#include "../vendor/nlohmann-json/json.hpp"

using nlohmann::json;

template <typename T> using sPtr = std::shared_ptr<T>;
