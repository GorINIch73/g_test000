#pragma once

#include "BaseView.h"
#include <vector>
#include "../Kosgu.h"
#include "../Payment.h"

class KosguView : public BaseView {
public:
    KosguView();
    void Render() override;
    void SetDatabaseManager(DatabaseManager* dbManager) override;
    void SetPdfReporter(PdfReporter* pdfReporter) override;
    std::pair<std::vector<std::string>, std::vector<std::vector<std::string>>> GetDataAsStrings() override;
    const char* GetTitle() override;

private:
    void RefreshData();

    std::vector<Kosgu> kosguEntries;
    Kosgu selectedKosgu;
    int selectedKosguIndex;
    bool showEditModal;
    bool isAdding;
    std::vector<ContractPaymentInfo> payment_info;
    char filterText[256];
    float list_view_height = 200.0f;
    float editor_width = 400.0f;
};
