#pragma once

#include <string>

struct Payment {
    int id = -1;
    std::string date; // Format YYYY-MM-DD
    std::string doc_number;
    std::string type; // 'income' or 'expense'
    double amount;
    std::string payer;
    std::string payer_inn;
    std::string recipient;
    std::string recipient_inn;
    std::string description;
    int counterparty_id = -1;
    int kosgu_id = -1;
    int contract_id = -1;
    int invoice_id = -1;
};
