#pragma once

#include "BaseView.h"
#include <vector>
#include <string>
#include "../Payment.h"
#include "../Counterparty.h"
#include "../Kosgu.h"
#include "../PaymentDetail.h"
#include "../Contract.h"
#include "../Invoice.h"
#include "imgui.h"
#include "CustomWidgets.h"

class PaymentsView : public BaseView {
public:
    PaymentsView();
    void Render() override;
    void SetDatabaseManager(DatabaseManager* dbManager) override;
    void SetPdfReporter(PdfReporter* pdfReporter) override;
    std::pair<std::vector<std::string>, std::vector<std::vector<std::string>>> GetDataAsStrings() override;
    const char* GetTitle() override;

private:
    void RefreshData();
    void RefreshDropdownData();

    std::vector<Payment> payments;
    Payment selectedPayment;
    int selectedPaymentIndex;
    bool isAdding;

    std::string descriptionBuffer;

    std::vector<PaymentDetail> paymentDetails;
    PaymentDetail selectedDetail;
    int selectedDetailIndex;
    bool isAddingDetail;

    std::vector<Counterparty> counterpartiesForDropdown;
    std::vector<Kosgu> kosguForDropdown;
    std::vector<Contract> contractsForDropdown;
    std::vector<Invoice> invoicesForDropdown;
    char filterText[256];
    float list_view_height = 200.0f;
    float editor_width = 400.0f;
};
