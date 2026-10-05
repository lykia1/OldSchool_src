#include "imgui_additional.h"

bool ImGui::SimpleCombo(std::string label, const std::vector<std::string> valueList, int* selectedIndex, ImGuiComboFlags flags)
{
	bool selection_changed = false;
	if (ImGui::BeginCombo(label.c_str(), valueList[*selectedIndex].c_str(), flags))
	{
		for (int n = 0; n < valueList.size(); n++)
		{
			bool is_selected = (*selectedIndex == n);
			if (ImGui::Selectable(valueList[n].c_str(), is_selected))
			{
				*selectedIndex = n;
				selection_changed = true;
				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}
	return selection_changed;
}
