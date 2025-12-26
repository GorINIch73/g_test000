#include "UIManager.h"
#include <iostream>
#include <string> // Added for std::string functions
#include "PdfReporter.h" // Add PdfReporter header
#include <ctime>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <algorithm>

const size_t MAX_RECENT_PATHS = 10;
const std::string RECENT_PATHS_FILE = ".recent_dbs.txt";

UIManager::UIManager()
    : dbManager(nullptr) {
    // Конструктор
    LoadRecentDbPaths();
}

UIManager::~UIManager() {
    SaveRecentDbPaths();
}

void UIManager::AddRecentDbPath(const std::string& path) {
    // Удаляем старую запись, если она есть
    recentDbPaths.erase(std::remove(recentDbPaths.begin(), recentDbPaths.end(), path), recentDbPaths.end());

    // Добавляем новый путь в начало
    recentDbPaths.insert(recentDbPaths.begin(), path);

    // Ограничиваем размер списка
    if (recentDbPaths.size() > MAX_RECENT_PATHS) {
        recentDbPaths.resize(MAX_RECENT_PATHS);
    }
}

void UIManager::LoadRecentDbPaths() {
    std::ifstream file(RECENT_PATHS_FILE);
    if (!file.is_open()) {
        return;
    }
    std::string path;
    while (std::getline(file, path)) {
        if (!path.empty()) {
            recentDbPaths.push_back(path);
        }
    }
}

void UIManager::SaveRecentDbPaths() {
    std::ofstream file(RECENT_PATHS_FILE);
    if (!file.is_open()) {
        return;
    }
    for (const auto& path : recentDbPaths) {
        file << path << std::endl;
    }
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
