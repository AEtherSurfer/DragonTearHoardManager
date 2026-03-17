// Dragon Tear Hoard Manager
// Copyright (C) 2024
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.

#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <nlohmann/json.hpp>
#include "InventoryParser.hpp"
#include "SaveWatcher.hpp"
#include "WebServer.hpp"

using namespace dthm::nms;

void printHeader() {
    std::cout << "=======================================\n";
    std::cout << "      Dragon Tear Hoard Manager        \n";
    std::cout << "=======================================\n";
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <path_to_save.json> [path_to_item_mapping.json]\n";
        return 1;
    }

    std::string saveFilePath = argv[1];
    std::string mappingFilePath = "games/nms/item_mapping.json"; // Default fallback
    if (argc >= 3) {
        mappingFilePath = argv[2];
    }

    printHeader();
    std::cout << "[DTHM] Dragon's Eye open. Web server listening on http://0.0.0.0:8080. Waiting for save file updates...\n";

    InventoryParser parser;
    std::cout << "Loading item mapping from " << mappingFilePath << "...\n";
    parser.ParseItemMapping(mappingFilePath);

    WebServer server(8080);

    // Initial load
    std::cout << "Parsing initial save file " << saveFilePath << "...\n";
    parser.ExtractPlayerState(saveFilePath);
    auto report = parser.GenerateHoardReport();

    // Package into JSON
    nlohmann::json reportJson;
    reportJson["keep"] = report.keep;
    reportJson["sell"] = report.sell;
    reportJson["use"] = report.use;
    server.UpdateReport(reportJson);

    // Setup watcher
    SaveWatcher watcher(saveFilePath);
    watcher.OnChange([&]() {
        parser.ExtractPlayerState(saveFilePath);
        auto newReport = parser.GenerateHoardReport();

        nlohmann::json newReportJson;
        newReportJson["keep"] = newReport.keep;
        newReportJson["sell"] = newReport.sell;
        newReportJson["use"] = newReport.use;
        server.UpdateReport(newReportJson);
    });

    // Start background tasks
    watcher.Start();

    // Start server on main thread (blocking)
    server.Start();

    return 0;
}
