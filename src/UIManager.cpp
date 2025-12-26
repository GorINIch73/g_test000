
void UIManager::RefreshContractsData() {
    if (dbManager) {
        contracts = dbManager->getContracts();
        selectedContractIndex = -1;
    }
}

void UIManager::RenderContractsWindow() {
    if (!ImGui::Begin("Справочник 'Договоры'", &ShowContractsWindow)) {
        ImGui::End();
        return;
    }
    // TODO: Implement the UI for Contracts
    ImGui::Text("TODO: Implement the UI for Contracts");
    ImGui::End();
}
