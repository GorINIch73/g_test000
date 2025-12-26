#pragma once

#include "imgui.h"
#include <vector>
#include "Kosgu.h"
#include "DatabaseManager.h"
#include "PdfReporter.h"
#include "views/PaymentsView.h"
#include "views/KosguView.h"
#include "views/CounterpartiesView.h"
#include "views/ContractsView.h"
#include "views/InvoicesView.h"
#include "views/SqlQueryView.h"

class UIManager {
public:
    UIManager();
    void Render();
    void SetDatabaseManager(DatabaseManager* dbManager);
    void SetPdfReporter(PdfReporter* pdfReporter);

    PaymentsView paymentsView;
    KosguView kosguView;
    CounterpartiesView counterpartiesView;
    ContractsView contractsView;
    InvoicesView invoicesView;
    SqlQueryView sqlQueryView;

private:
    DatabaseManager* dbManager;
    PdfReporter* pdfReporter; // Add PdfReporter pointer
};
