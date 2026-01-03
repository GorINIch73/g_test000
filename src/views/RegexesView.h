#pragma once

#include "BaseView.h"
#include <vector>
#include "../Regex.h"

class RegexesView : public BaseView {
public:
    RegexesView();
    void Render() override;
    void SetDatabaseManager(DatabaseManager* dbManager) override;
    void SetPdfReporter(PdfReporter* pdfReporter) override;
    std::pair<std::vector<std::string>, std::vector<std::vector<std::string>>> GetDataAsStrings() override;
    const char* GetTitle() override;

private:
    void RefreshData();

    std::vector<Regex> regexes;
    Regex selectedRegex;
    int selectedRegexIndex;
    bool isAdding;
    char filterText[256];
};
