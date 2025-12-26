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
    ~UIManager();
    void Render();
    void SetDatabaseManager(DatabaseManager* dbManager);
    void SetPdfReporter(PdfReporter* pdfReporter);
    void AddRecentDbPath(const std::string& path);

    std::vector<std::string> recentDbPaths;

    PaymentsView paymentsView;
    KosguView kosguView;
    CounterpartiesView counterpartiesView;
    ContractsView contractsView;
    InvoicesView invoicesView;
    SqlQueryView sqlQueryView;

private:
    void LoadRecentDbPaths();
    void SaveRecentDbPaths();

    DatabaseManager* dbManager;
    PdfReporter* pdfReporter; // Add PdfReporter pointer
};
