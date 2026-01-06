#pragma once
#include "imgui.h"
#include <string>
#include <vector>

namespace CustomWidgets {

struct ComboItem {
    int id;
    std::string name;
};

bool InputText(const char *label, std::string *str,
               ImGuiInputTextFlags flags = 0);
bool InputTextMultiline(const char *label, std::string *str,
                        const ImVec2 &size = ImVec2(0, 0),
                        ImGuiInputTextFlags flags = 0);
bool InputTextMultilineWithWrap(const char *label, std::string *str,
                                const ImVec2 &size = ImVec2(0, 0),
                                ImGuiInputTextFlags flags = 0);

bool ComboWithFilter(const char* label, int& current_id, std::vector<ComboItem>& items, char* search_buffer, int search_buffer_size, ImGuiComboFlags flags);
}
