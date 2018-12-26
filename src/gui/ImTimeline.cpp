#include "imgui.h"
#include "imgui_internal.h"

#include "ImTimeline.h"

namespace ImTimeline
{
	// centalize this, as we're also using it in ImSequencer.cpp
	ImVec2 operator+(const ImVec2& lhs, const ImVec2& rhs)
	{
		return ImVec2(lhs.x + rhs.x, lhs.y + rhs.y);
	}

    bool Timeline()
    {
        bool retval = false;
        ImGuiIO &io = ImGui::GetIO();

        ImGui::BeginGroup();
        ImDrawList* drawList = ImGui::GetWindowDrawList();
		ImVec2 canvasPosition = ImGui::GetCursorScreenPos();
        ImVec2 canvasSize = ImGui::GetContentRegionAvail();

        ImRect regionRect(canvasPosition, canvasPosition + canvasSize);

        if (regionRect.Contains(io.MousePos))
        {
            drawList->AddRectFilled(canvasPosition, canvasPosition + canvasSize, ImGui::GetColorU32(ImVec4(0.1f, 0.1f, 0.1f, 1.0f)));
			ImGui::Text("cursor in window, (%f, %f), (%f, %f)", canvasPosition.x, canvasPosition.y, canvasSize.x, canvasSize.y);
		}

        ImGui::EndGroup();
        return retval;
    }
}