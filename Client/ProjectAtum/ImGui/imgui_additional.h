#pragma once
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include "imgui_internal.h"

#include <string>
#include <vector>

namespace ImGui
{
    // helper to simplify and shorten the use of a simple dropdown box
    // returns true if the selection has changed
    // NOTE: dont add more elements in valueList than selectedIndex can hold 
    bool SimpleCombo(std::string label, const std::vector<std::string> valueList, int* selectedIndex, ImGuiComboFlags flags = ImGuiColorEditFlags_None);
}