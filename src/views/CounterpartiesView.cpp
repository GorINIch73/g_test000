#include "CounterpartiesView.h"
#include <iostream>

CounterpartiesView::CounterpartiesView()
    : dbManager(nullptr), pdfReporter(nullptr), selectedCounterpartyIndex(-1), showEditModal(false), isAdding(false) {
}

void CounterpartiesView::SetDatabaseManager(DatabaseManager* manager) {
    dbManager = manager;
}

void CounterpartiesView::SetPdfReporter(PdfReporter* reporter) {
    pdfReporter = reporter;
}

void CounterpartiesView::RefreshData() {
    if (dbManager) {
        counterparties = dbManager->getCounterparties();
        selectedCounterpartyIndex = -1;
    }
}

void CounterpartiesView::Render() {
    if (!IsVisible) {
        return;
    }

    if (!ImGui::Begin("Справочник 'Контрагенты'", &IsVisible)) {
        ImGui::End();
        return;
    }

    if (dbManager && counterparties.empty()) {
        RefreshData();
    }

    // Панель управления
    if (ImGui::Button("Добавить")) {
        isAdding = true;
        selectedCounterparty = Counterparty{-1, "", ""};
        showEditModal = true;
    }
    ImGui::SameLine();
    if (ImGui::Button("Изменить")) {
        if (selectedCounterpartyIndex != -1) {
            isAdding = false;
            selectedCounterparty = counterparties[selectedCounterpartyIndex];
            showEditModal = true;
        }
    }
    ImGui::SameLine();
    if (ImGui::Button("Удалить")) {
        if (selectedCounterpartyIndex != -1 && dbManager) {
            dbManager->deleteCounterparty(counterparties[selectedCounterpartyIndex].id);
            RefreshData();
        }
    }
    ImGui::SameLine();
    if (ImGui::Button("Обновить")) {
        RefreshData();
    }

    // Таблица со списком
    if (ImGui::BeginTable("counterparties_table", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable)) {
        ImGui::TableSetupColumn("ID");
        ImGui::TableSetupColumn("Наименование");
        ImGui::TableSetupColumn("ИНН");
        ImGui::TableHeadersRow();

        for (int i = 0; i < counterparties.size(); ++i) {
            ImGui::TableNextRow();
            ImGui::TableNextColumn();

            bool is_selected = (selectedCounterpartyIndex == i);
            if (ImGui::Selectable(std::to_string(counterparties[i].id).c_str(), is_selected, ImGuiSelectableFlags_SpanAllColumns)) {
                selectedCounterpartyIndex = i;
            }
            if (is_selected) {
                 ImGui::SetItemDefaultFocus();
            }

            ImGui::TableNextColumn();
            ImGui::Text("%s", counterparties[i].name.c_str());
            ImGui::TableNextColumn();
            ImGui::Text("%s", counterparties[i].inn.c_str());
        }
        ImGui::EndTable();
    }

    // Модальное окно для редактирования/добавления
    if (showEditModal) {
        ImGui::OpenPopup("EditCounterparty");
    }

    if (ImGui::BeginPopupModal("EditCounterparty", &showEditModal)) {
        char nameBuf[256];
        char innBuf[256];
        
        snprintf(nameBuf, sizeof(nameBuf), "%s", selectedCounterparty.name.c_str());
        snprintf(innBuf, sizeof(innBuf), "%s", selectedCounterparty.inn.c_str());

        if (ImGui::InputText("Наименование", nameBuf, sizeof(nameBuf))) {
            selectedCounterparty.name = nameBuf;
        }
        if (ImGui::InputText("ИНН", innBuf, sizeof(innBuf))) {
            selectedCounterparty.inn = innBuf;
        }

        if (ImGui::Button("Сохранить")) {
            if (dbManager) {
                if (isAdding) {
                    dbManager->addCounterparty(selectedCounterparty);
                } else {
                    dbManager->updateCounterparty(selectedCounterparty);
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
