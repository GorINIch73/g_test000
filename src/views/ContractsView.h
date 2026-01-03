#pragma once

#include "BaseView.h"
#include <vector>
#include "../Contract.h"
#include "../Counterparty.h"
#include "../Payment.h"

class ContractsView : public BaseView {
public:
    ContractsView();
    void Render() override;
    void SetDatabaseManager(DatabaseManager* dbManager) override;
    void SetPdfReporter(PdfReporter* pdfReporter) override;
    std::pair<std::vector<std::string>, std::vector<std::vector<std::string>>> GetDataAsStrings() override;
    const char* GetTitle() override;

private:
    void RefreshData();
    void RefreshDropdownData();

    std::vector<Contract> contracts;
    Contract selectedContract;
    int selectedContractIndex;
    bool showEditModal;
    bool isAdding;

    std::vector<ContractPaymentInfo> payment_info;
    std::vector<Counterparty> counterpartiesForDropdown;
    char filterText[256];
};
