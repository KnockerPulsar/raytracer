#pragma once
#include "../vendor/nlohmann-json/json.hpp"
#include <memory>

using nlohmann::json;


template <typename T> using sPtr = std::shared_ptr<T>; // Shared pointer alias

// make_shared alias
template <typename T, typename Args> inline auto ms(Args &&args...) { return std::make_shared<T>(args); }
