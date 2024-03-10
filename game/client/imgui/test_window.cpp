/**
 * @file test_window.cpp
 * @brief A demo window for reference.
 */

#include "cdll_client_int.h"
#include "convar.h"
#include "imgui_window.h"

DECLARE_IMGUI_WINDOW(test_window, "Test window")
{
	ImGui::Text("Demo text");

	if (ImGui::Button("Print"))
	{
		Msg("Hello ImGui!\n");
	}

	if (ImGui::Button("Impulse 101"))
	{
		engine->ClientCmd("impulse 101");
	}

	return false;
}
