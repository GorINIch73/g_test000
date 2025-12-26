#include "ContractsView.h"
#include <iostream>

ContractsView::ContractsView()
    : dbManager(nullptr), pdfReporter(nullptr), selectedContractIndex(-1), showEditModal(false), isAdding(false) {
}

void ContractsView::SetDatabaseManager(DatabaseManager* manager) {
    dbManager = manager;
}

void ContractsView::SetPdfReporter(PdfReporter* reporter) {
    pdfReporter = reporter;
}

void ContractsView::RefreshData() {
    if (dbManager) {
        contracts = dbManager->getContracts();
        selectedContractIndex = -1;
    }
}

void ContractsView::RefreshDropdownData() {
    if (dbManager) {
        counterpartiesForDropdown = dbManager->getCounterparties();
    }
}

void ContractsView::Render() {
    if (!IsVisible) {
        return;
    }

    if (!ImGui::Begin("Справочник 'Договоры'", &IsVisible)) {
        ImGui::End();
        return;
    }

    if (dbManager && contracts.empty()) {
        RefreshData();
        RefreshDropdownData();
    }

    // Панель управления
    if (ImGui::Button("Добавить")) {
        isAdding = true;
        selectedContract = Contract{-1, "", "", -1};
        showEditModal = true;
    }
    ImGui::SameLine();
    if (ImGui::Button("Изменить")) {
        if (selectedContractIndex != -1) {
            isAdding = false;
            selectedContract = contracts[selectedContractIndex];
            showEditModal = true;
        }
    }
    ImGui::SameLine();
    if (ImGui::Button("Удалить")) {
        if (selectedContractIndex != -1 && dbManager) {
            dbManager->deleteContract(contracts[selectedContractIndex].id);
            RefreshData();
        }
    }
    ImGui::SameLine();
    if (ImGui::Button("Обновить")) {
        RefreshData();
        RefreshDropdownData();
    }
    ImGui::SameLine();
    if (ImGui::Button("Экспорт в PDF")) {
        if (pdfReporter && dbManager && dbManager->is_open()) {
            std::vector<std::string> columns = {"ID", "Номер", "Дата", "Контрагент ID"};
            std::vector<std::vector<std::string>> rows;
            for (const auto& entry : contracts) {
                rows.push_back({std::to_string(entry.id), entry.number, entry.date, std::to_string(entry.counterparty_id)});
            }
            pdfReporter->generatePdfFromTable("contracts_report.pdf", "Справочник 'Договоры'", columns, rows);
        } else {
            std::cerr << "Cannot export to PDF: No database open or PdfReporter not set." << std::endl;
        }
    }

    // Таблица со списком
    if (ImGui::BeginTable("contracts_table", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable)) {
        ImGui::TableSetupColumn("ID");
        ImGui::TableSetupColumn("Номер");
        ImGui::TableSetupColumn("Дата");
        ImGui::TableSetupColumn("Контрагент");
        ImGui::TableHeadersRow();

        auto getCounterpartyName = [&](int id) -> const char* {
            for (const auto& cp : counterpartiesForDropdown) {
                if (cp.id == id) {
                    return cp.name.c_str();
                }
            }
            return "N/A";
        };

        for (int i = 0; i < contracts.size(); ++i) {
            ImGui::TableNextRow();
            ImGui::TableNextColumn();

            bool is_selected = (selectedContractIndex == i);
            if (ImGui::Selectable(std::to_string(contracts[i].id).c_str(), is_selected, ImGuiSelectableFlags_SpanAllColumns)) {
                selectedContractIndex = i;
            }
            if (is_selected) {
                 ImGui::SetItemDefaultFocus();
            }

            ImGui::TableNextColumn();
            ImGui::Text("%s", contracts[i].number.c_str());
            ImGui::TableNextColumn();
            ImGui::Text("%s", contracts[i].date.c_str());
            ImGui::TableNextColumn();
            ImGui::Text("%s", getCounterpartyName(contracts[i].counterparty_id));
        }
        ImGui::EndTable();
    }

    // Модальное окно для редактирования/добавления
    if (showEditModal) {
        ImGui::OpenPopup("EditContract");
    }

    if (ImGui::BeginPopupModal("EditContract", &showEditModal)) {
        char numberBuf[256];
        char dateBuf[12];
        
        snprintf(numberBuf, sizeof(numberBuf), "%s", selectedContract.number.c_str());
        snprintf(dateBuf, sizeof(dateBuf), "%s", selectedContract.date.c_str());

        if (ImGui::InputText("Номер", numberBuf, sizeof(numberBuf))) {
            selectedContract.number = numberBuf;
        }
        if (ImGui::InputText("Дата", dateBuf, sizeof(dateBuf))) {
            selectedContract.date = dateBuf;
        }
        
        if (!counterpartiesForDropdown.empty()) {
            auto getCounterpartyName = [&](int id) -> const char* {
                for (const auto& cp : counterpartiesForDropdown) {
                    if (cp.id == id) {
                        return cp.name.c_str();
                    }
                }
                return "N/A";
            };
            const char* currentCounterpartyName = getCounterpartyName(selectedContract.counterparty_id);
            
            if (ImGui::BeginCombo("Контрагент", currentCounterpartyName)) {
                for (const auto& cp : counterpartiesForDropdown) {
                    bool isSelected = (cp.id == selectedContract.counterparty_id);
                    if (ImGui::Selectable(cp.name.c_str(), isSelected)) {
                        selectedContract.counterparty_id = cp.id;
                    }
                    if (isSelected) {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }
        }

        if (ImGui::Button("Сохранить")) {
            if (dbManager) {
                if (isAdding) {
                    dbManager->addContract(selectedContract);
                } else {
                    dbManager->updateContract(selectedContract);
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
