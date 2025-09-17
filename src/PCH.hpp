#pragma once
#define IMGUI_DEFINE_MATH_OPERATORS
#include <PreInclude.hpp>
#include <filesystem>
#include <iostream>
#include <string>
#include <fstream>
#include <initializer_list>
#include <thread>
#include <unordered_map>
#include <map>
#include <memory>
#include <urlmon.h>
#include <ranges>
#include <vector>
#include <cstdint>
#include <cstddef>
#include <utility>
#include <cassert>
#include <future>
#include <type_traits>
#include <algorithm>
#include <iterator>
#include <regex>
#include <stack>
#include <libhat.hpp>
#include <glm/glm/glm.hpp>
#include <glm/glm/ext/matrix_transform.hpp>
#include <entt/entt.hpp>

// Heavy DirectX headers frequently used across the codebase
#include <d3d11.h>
#include <d2d1.h>
#include <dwrite.h>
#include <wincodec.h>
#include <winrt/base.h>
#include <winrt/Windows.Foundation.h>

// ImGui headers frequently used
#include <imgui/imgui.h>

#include "Assets/Assets.hpp"
#include "Events/EventManager.hpp"
#include "Events/Events.hpp"

#include <lib/json/json.hpp>

#include "GUI/Engine/Engine.hpp"