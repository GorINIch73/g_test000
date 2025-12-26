#pragma once

#include "imgui.h"
#include "../DatabaseManager.h"
#include "../PdfReporter.h"

class BaseView {
public:
    virtual ~BaseView() = default;
    virtual void Render() = 0;
    virtual void SetDatabaseManager(DatabaseManager* dbManager) = 0;
    virtual void SetPdfReporter(PdfReporter* pdfReporter) = 0;

    bool IsVisible = false;
};
