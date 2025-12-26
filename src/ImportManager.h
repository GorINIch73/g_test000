#pragma once

#include <string>
#include <vector>
#include "DatabaseManager.h"

class ImportManager {
public:
    ImportManager();

    // Imports payments from a TSV file.
    // Returns true on success, false on failure.
    bool ImportPaymentsFromTsv(const std::string& filepath, DatabaseManager* dbManager);

private:
    // Helper to parse a single line of TSV data into a Payment object
    Payment parsePaymentLine(const std::string& line);
    
    // Helper to extract counterparty details from payer/recipient
    Counterparty extractCounterparty(const std::string& name, const std::string& inn);
};
