#pragma once

#include "BaseView.h"
#include <vector>
#include <string>

class SqlQueryView : public BaseView {
public:
    SqlQueryView();
    void Render() override;
    void SetDatabaseManager(DatabaseManager* dbManager) override;
    void SetPdfReporter(PdfReporter* pdfReporter) override;

private:
    DatabaseManager* dbManager;
    PdfReporter* pdfReporter;

    char queryInputBuffer[4096];
    struct QueryResult {
        std::vector<std::string> columns;
        std::vector<std::vector<std::string>> rows;
    } queryResult;
};
