#pragma once

#include <string>
#include <vector>

class PdfReporter {
public:
    PdfReporter();

    // Placeholder method to generate a PDF from tabular data
    bool generatePdfFromTable(
        const std::string& filename,
        const std::string& title,
        const std::vector<std::string>& columns,
        const std::vector<std::vector<std::string>>& rows
    );
};
