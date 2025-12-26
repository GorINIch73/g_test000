#include "UIManager.h"
#include <iostream>
#include <string> // Added for std::string functions
#include "PdfReporter.h" // Add PdfReporter header
#include <ctime>
#include <iomanip>
#include <sstream>

UIManager::UIManager()
    : dbManager(nullptr) {
    // Конструктор
}

void UIManager::SetDatabaseManager(DatabaseManager* manager) {
    dbManager = manager;
    paymentsView.SetDatabaseManager(manager);
    kosguView.SetDatabaseManager(manager);
    counterpartiesView.SetDatabaseManager(manager);
    contractsView.SetDatabaseManager(manager);
    invoicesView.SetDatabaseManager(manager);
    sqlQueryView.SetDatabaseManager(manager);
}

void UIManager::SetPdfReporter(PdfReporter* reporter) {
    pdfReporter = reporter;
    paymentsView.SetPdfReporter(reporter);
    kosguView.SetPdfReporter(reporter);
    counterpartiesView.SetPdfReporter(reporter);
    contractsView.SetPdfReporter(reporter);
    invoicesView.SetPdfReporter(reporter);
    sqlQueryView.SetPdfReporter(reporter);
}

void UIManager::Render() {
    paymentsView.Render();
    kosguView.Render();
    counterpartiesView.Render();
    contractsView.Render();
    invoicesView.Render();
    sqlQueryView.Render();
}
