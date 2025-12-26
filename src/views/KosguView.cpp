#include "KosguView.h"
#include <iostream>

KosguView::KosguView()
    : dbManager(nullptr), pdfReporter(nullptr), selectedKosguIndex(-1), showEditModal(false), isAdding(false) {
}

void KosguView::SetDatabaseManager(DatabaseManager* manager) {
    dbManager = manager;
}

void KosguView::SetPdfReporter(PdfReporter* reporter) {
    pdfReporter = reporter;
}

void KosguView::RefreshData() {
    if (dbManager) {
        kosguEntries = dbManager->getKosguEntries();
        selectedKosguIndex = -1;
    }
}

void KosguView::Render() {
    if (!IsVisible) {
        return;
    }

    if (!ImGui::Begin("Справочник КОСГУ", &IsVisible)) {
        ImGui::End();
        return;
    }

    if (dbManager && kosguEntries.empty()) {
        RefreshData();
    }

    // Панель управления
    if (ImGui::Button("Добавить")) {
        isAdding = true;
        selectedKosgu = Kosgu{-1, "", ""};
        showEditModal = true;
    }
    ImGui::SameLine();
    if (ImGui::Button("Изменить")) {
        if (selectedKosguIndex != -1) {
            isAdding = false;
            selectedKosgu = kosguEntries[selectedKosguIndex];
            showEditModal = true;
        }
    }
    ImGui::SameLine();
    if (ImGui::Button("Удалить")) {
        if (selectedKosguIndex != -1 && dbManager) {
            dbManager->deleteKosguEntry(kosguEntries[selectedKosguIndex].id);
            RefreshData();
        }
    }
    ImGui::SameLine();
    if (ImGui::Button("Обновить")) {
        RefreshData();
    }
    ImGui::SameLine();
    if (ImGui::Button("Экспорт в PDF")) {
        if (pdfReporter && dbManager && dbManager->is_open()) {
            std::vector<std::string> columns = {"ID", "Код", "Наименование"};
            std::vector<std::vector<std::string>> rows;
            for (const auto& entry : kosguEntries) {
                rows.push_back({std::to_string(entry.id), entry.code, entry.name});
            }
            pdfReporter->generatePdfFromTable("kosgu_report.pdf", "Справочник КОСГУ", columns, rows);
        } else {
            std::cerr << "Cannot export to PDF: No database open or PdfReporter not set." << std::endl;
        }
    }

    if (ImGui::BeginTable("kosgu_table", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable)) {
        ImGui::TableSetupColumn("ID");
        ImGui::TableSetupColumn("Код");
        ImGui::TableSetupColumn("Наименование");
        ImGui::TableHeadersRow();

        for (int i = 0; i < kosguEntries.size(); ++i) {
            ImGui::TableNextRow();
            ImGui::TableNextColumn();

            bool is_selected = (selectedKosguIndex == i);
            if (ImGui::Selectable(std::to_string(kosguEntries[i].id).c_str(), is_selected, ImGuiSelectableFlags_SpanAllColumns)) {
                selectedKosguIndex = i;
            }
            if (is_selected) {
                 ImGui::SetItemDefaultFocus();
            }

            ImGui::TableNextColumn();
            ImGui::Text("%s", kosguEntries[i].code.c_str());
            ImGui::TableNextColumn();
            ImGui::Text("%s", kosguEntries[i].name.c_str());
        }
        ImGui::EndTable();
    }

    if (showEditModal) {
        ImGui::OpenPopup("EditKosgu");
    }

    if (ImGui::BeginPopupModal("EditKosgu", &showEditModal)) {
        char codeBuf[256];
        char nameBuf[256];
        if (isAdding) {
            snprintf(codeBuf, sizeof(codeBuf), "%s", "");
            snprintf(nameBuf, sizeof(nameBuf), "%s", "");
        } else {
            snprintf(codeBuf, sizeof(codeBuf), "%s", selectedKosgu.code.c_str());
            snprintf(nameBuf, sizeof(nameBuf), "%s", selectedKosgu.name.c_str());
        }

        ImGui::InputText("Код", codeBuf, sizeof(codeBuf));
        ImGui::InputText("Наименование", nameBuf, sizeof(nameBuf));

        if (ImGui::Button("Сохранить")) {
            if (dbManager) {
                selectedKosgu.code = codeBuf;
                selectedKosgu.name = nameBuf;
                if (isAdding) {
                    dbManager->addKosguEntry(selectedKosgu);
                } else {
                    dbManager->updateKosguEntry(selectedKosgu);
                }
                RefreshData();
            }
            showEditModal = false;
        }
        ImGui::SameLine();
        if (ImGui::Button("Отмена")) {
            showEditModal = false;
        }

        ImGui::EndPopup();
    }

    ImGui::End();
}
