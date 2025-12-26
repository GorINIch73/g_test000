#include "ImportManager.h"
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm> // For std::remove
#include <regex>     // For regex parsing
#include <iostream>  // For std::cerr

ImportManager::ImportManager() {}

// Helper function to trim whitespace and quotes from a string
std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\n\r\"");
    if (std::string::npos == first) {
        return str;
    }
    size_t last = str.find_last_not_of(" \t\n\r\"");
    return str.substr(first, (last - first + 1));
}

// Dummy implementation for parsing a single line of TSV data into a Payment object
// This is a placeholder and needs to be adapted to the actual TSV format
Payment ImportManager::parsePaymentLine(const std::string& line) {
    Payment payment;
    std::stringstream ss(line);
    std::string token;
    std::vector<std::string> tokens;

    // Split by tab delimiter
    while (std::getline(ss, token, '\t')) {
        tokens.push_back(trim(token));
    }

    // Actual TSV column mapping from платежки 2024г.tsv:
    // 0: Состояние (Status)
    // 1: Номер документа (Document Number)
    // 2: Дата документа (Document Date)
    // 3: Сумма (Amount)
    // 4: Л/с в ФО (Account in FO) - Ignored for payment object
    // 5: Наименование (Name) - Payer/Recipient Name
    // 6: Назначение платежа (Payment Purpose) - Description, where Contract/Invoice info is
    // 7: Тип БК и направление (Type BK and Direction) - Used to confirm payment type
    // 8: Дата принятия (Acceptance Date) - Ignored for payment object

    if (tokens.size() >= 9) {
        // --- Map basic payment details ---
        std::string status = tokens[0]; // "Принят" or "Расход"
        if (status == "Принят") {
            payment.type = "income";
        } else if (status == "Расход"){ // Corrected from "Расходная (платеж)" as token[0] contains "Расход"
             payment.type = "expense";
        } else {
             payment.type = "unknown";
        }

        payment.doc_number = tokens[1];
        payment.date = tokens[2];
        try {
            std::string amount_str = tokens[3];
            std::replace(amount_str.begin(), amount_str.end(), ',', '.');
            payment.amount = std::stod(amount_str);
        } catch (const std::exception& e) {
            std::cerr << "Error parsing amount: " << e.what() << " for value " << tokens[3] << std::endl;
            payment.amount = 0.0;
        }
        
        // --- Determine Payer/Recipient Name based on payment type ---
        // For income, 'Наименование' (tokens[5]) is the Payer.
        // For expense, 'Наименование' (tokens[5]) is the Recipient.
        if (payment.type == "income") {
            payment.payer = tokens[5];
            payment.recipient = ""; // No explicit recipient from TSV for income
        } else if (payment.type == "expense") {
            payment.payer = "";     // No explicit payer from TSV for expense
            payment.recipient = tokens[5];
        }

        payment.description = tokens[6]; // This is 'Назначение платежа'

        // --- Extract INN from description using regex ---
        // Example Russian INN: 10 or 12 digits
        // std::regex inn_regex("\\b(\\d{10}|\\d{12})\\b"); // 10 or 12 digits
        // std::smatch inn_matches;
        // if (std::regex_search(payment.description, inn_matches, inn_regex)) {
        //     if (inn_matches.size() > 1) {
        //         // Assign INN based on payment type for consistency, if found in description
        //         if (payment.type == "income") {
        //             payment.payer_inn = inn_matches[1].str();
        //         } else if (payment.type == "expense") {
        //             payment.recipient_inn = inn_matches[1].str();
        //         }
        //     }
        // }
    } else {
        std::cerr << "Warning: TSV line has fewer than 9 columns: " << line << std::endl;
    }

    return payment;
}

// Helper to extract counterparty details from payer/recipient
// This function is now simplified, as we get the INN directly.
Counterparty ImportManager::extractCounterparty(const std::string& name, const std::string& inn) {
    Counterparty counterparty;
    counterparty.name = trim(name);
    counterparty.inn = trim(inn);
    return counterparty;
}


// Helper function to convert DD.MM.YYYY to YYYY-MM-DD
std::string convertDateToDBFormat(const std::string& date_ddmmyyyy) {
    if (date_ddmmyyyy.length() == 10) {
        return date_ddmmyyyy.substr(6, 4) + "-" + date_ddmmyyyy.substr(3, 2) + "-" + date_ddmmyyyy.substr(0, 2);
    }
    return date_ddmmyyyy; // Return as is if format is unexpected
}

bool ImportManager::ImportPaymentsFromTsv(const std::string& filepath, DatabaseManager* dbManager) {
    if (!dbManager) {
        std::cerr << "DatabaseManager is null." << std::endl;
        return false;
    }

    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Failed to open TSV file: " << filepath << std::endl;
        return false;
    }

    std::string line;
    std::getline(file, line); // Skip header line

    // Regex patterns for contract and invoice
    std::regex contract_regex("по контракту\\s*([^\\s]+)\\s*(\\d{2}\\.\\d{2}\\.\\d{4})");
    std::regex invoice_regex("(?:док\\.о пр-ке пост\\.товаров|акт об оказ\\.услуг|тов\\.накладная|счет на оплату|№)\\s*([^\\s]+)\\s*от\\s*(\\d{2}\\.\\d{2}\\.\\d{4})");

    while (std::getline(file, line)) {
        Payment payment = parsePaymentLine(line);
        if (payment.date.empty() || payment.amount == 0.0) { // Basic validation
            std::cerr << "Skipping invalid payment line: " << line << std::endl;
            continue;
        }

        // --- Обработка контрагентов ---
        Counterparty payer_counterparty = extractCounterparty(payment.payer, payment.payer_inn);
        Counterparty recipient_counterparty = extractCounterparty(payment.recipient, payment.recipient_inn);

        // Handle Payer Counterparty
        int payer_id = -1;
        if (!payer_counterparty.name.empty()) {
            // 1. Try to find by name and INN if INN is available
            if (!payer_counterparty.inn.empty()) {
                payer_id = dbManager->getCounterpartyIdByNameInn(payer_counterparty.name, payer_counterparty.inn);
            }
            
            // 2. If not found or INN was empty, try to find by name only (for NULL INN entries)
            if (payer_id == -1) { // If still not found by Name+INN, or if INN was empty
                payer_id = dbManager->getCounterpartyIdByName(payer_counterparty.name);
            }

            // 3. If still not found, add a new counterparty
            if (payer_id == -1) {
                if (dbManager->addCounterparty(payer_counterparty)) {
                    payer_id = payer_counterparty.id;
                } else {
                    std::cerr << "Failed to add new payer counterparty: " << payer_counterparty.name << std::endl;
                }
            }
        }
        
        // Handle Recipient Counterparty
        int recipient_id = -1; // Declare recipient_id here
        if (!recipient_counterparty.name.empty()) {
            // 1. Try to find by name and INN if INN is available
            if (!recipient_counterparty.inn.empty()) {
                recipient_id = dbManager->getCounterpartyIdByNameInn(recipient_counterparty.name, recipient_counterparty.inn);
            }

            // 2. If not found or INN was empty, try to find by name only (for NULL INN entries)
            if (recipient_id == -1) { // If still not found by Name+INN, or if INN was empty
                recipient_id = dbManager->getCounterpartyIdByName(recipient_counterparty.name);
            }

            // 3. If still not found, add a new counterparty
            if (recipient_id == -1) {
                if (dbManager->addCounterparty(recipient_counterparty)) {
                    recipient_id = recipient_counterparty.id;
                } else {
                    std::cerr << "Failed to add new recipient counterparty: " << recipient_counterparty.name << std::endl;
                }
            }
        }

        // Decide which counterparty to link to the payment itself
        // Based on type: income payment links to payer, expense payment links to recipient
        if (payment.type == "income") {
            payment.counterparty_id = payer_id;
        } else if (payment.type == "expense") {
            payment.counterparty_id = recipient_id; 
        } else {
            // Default or unknown type handling
            payment.counterparty_id = payer_id; 
        }

        int contract_counterparty_id = -1; // Determine counterparty for contract
        if (payment.type == "income") {
            contract_counterparty_id = payer_id;
        } else if (payment.type == "expense") {
            contract_counterparty_id = recipient_id;
        }
        
        // --- Парсинг и обработка договоров ---
        std::smatch contract_matches;
        int current_contract_id = -1; // To link invoice if found
        if (std::regex_search(payment.description, contract_matches, contract_regex)) {
            if (contract_matches.size() == 3) { // Full match + 2 capture groups
                std::string contract_number = contract_matches[1].str();
                std::string contract_date_ddmmyyyy = contract_matches[2].str();
                std::string contract_date_db_format = convertDateToDBFormat(contract_date_ddmmyyyy);

                // int counterparty_for_contract = -1; // This variable is now redundant
                // if (payment.type == "income") {
                //     counterparty_for_contract = payer_id;
                // } else if (payment.type == "expense") {
                //     counterparty_for_contract = recipient_id_for_contract; // Old name
                // }

                int found_contract_id = dbManager->getContractIdByNumberDate(contract_number, contract_date_db_format);
                if (found_contract_id == -1) {
                    Contract contract_obj;
                    contract_obj.number = contract_number;
                    contract_obj.date = contract_date_db_format;
                    contract_obj.counterparty_id = contract_counterparty_id; // Use the correctly determined ID
                    if (dbManager->addContract(contract_obj)) {
                        current_contract_id = contract_obj.id;
                        std::cerr << "Added Contract: Number=" << contract_number << ", Date=" << contract_date_db_format << ", ID=" << current_contract_id << std::endl;
                    } else {
                        std::cerr << "Failed to add contract: " << contract_number << " from " << line << std::endl;
                    }
                } else {
                    current_contract_id = found_contract_id;
                    std::cerr << "Found existing Contract: Number=" << contract_number << ", Date=" << contract_date_db_format << ", ID=" << current_contract_id << std::endl;
                }
                payment.contract_id = current_contract_id;
            }
        }

        // --- Парсинг и обработка накладных ---
        std::smatch invoice_matches;
        if (std::regex_search(payment.description, invoice_matches, invoice_regex)) {
            if (invoice_matches.size() == 3) { // Full match + 2 capture groups
                std::string invoice_number = invoice_matches[1].str();
                std::string invoice_date_ddmmyyyy = invoice_matches[2].str();
                std::string invoice_date_db_format = convertDateToDBFormat(invoice_date_ddmmyyyy);

                int found_invoice_id = dbManager->getInvoiceIdByNumberDate(invoice_number, invoice_date_db_format);
                if (found_invoice_id == -1) {
                    Invoice invoice_obj;
                    invoice_obj.number = invoice_number;
                    invoice_obj.date = invoice_date_db_format;
                    invoice_obj.contract_id = current_contract_id; // Link to contract if found
                    if (dbManager->addInvoice(invoice_obj)) {
                        payment.invoice_id = invoice_obj.id;
                        std::cerr << "Added Invoice: Number=" << invoice_number << ", Date=" << invoice_date_db_format << ", ID=" << payment.invoice_id << ", Contract ID=" << current_contract_id << std::endl;
                    } else {
                        std::cerr << "Failed to add invoice: " << invoice_number << " from " << line << std::endl;
                    }
                } else {
                    payment.invoice_id = found_invoice_id;
                    std::cerr << "Found existing Invoice: Number=" << invoice_number << ", Date=" << invoice_date_db_format << ", ID=" << payment.invoice_id << ", Contract ID=" << current_contract_id << std::endl;
                }
            }
        }

        // --- Добавление платежа ---
        if (!dbManager->addPayment(payment)) {
            std::cerr << "Failed to add payment from line: " << line << std::endl;
            std::cerr << "Payment details: date=" << payment.date << ", doc=" << payment.doc_number 
                      << ", type=" << payment.type << ", amount=" << payment.amount 
                      << ", payer=" << payment.payer << ", recipient=" << payment.recipient << std::endl;
        }
    }

    file.close();
    return true;
}
