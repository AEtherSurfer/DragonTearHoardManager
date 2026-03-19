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
    std::string targetDir;
    std::string mappingFilePath = "games/nms/item_mapping.json"; // Default fallback

    if (argc > 1) {
        targetDir = argv[1];
    } else {
        const char* homeDir = getenv("HOME");
        if (homeDir) {
            targetDir = std::string(homeDir) + "/.local/share/Steam/steamapps/compatdata/275850/pfx/drive_c/users/steamuser/AppData/Roaming/HelloGames/NMS/";
        } else {
            std::cerr << "Error: HOME environment variable not found. Please provide save directory as an argument.\n";
            return 1;
        }
    }

    if (argc >= 3) {
        mappingFilePath = argv[2];
    }

    if (!std::filesystem::exists(targetDir)) {
        std::cerr << "Error: Save directory does not exist: " << targetDir << "\n";
        return 1;
    }

    printHeader();
    std::cout << "[DTHM] Dragon's Eye open. Web server listening on http://0.0.0.0:8080. Waiting for save file updates in " << targetDir << "...\n";

    InventoryParser parser;
    std::cout << "Loading item mapping from " << mappingFilePath << "...\n";
    parser.ParseItemMapping(mappingFilePath);

    WebServer server(8080);
    std::mutex reportMutex;

    auto updateReport = [&](const std::string& filePath, std::filesystem::file_time_type lastWriteTime) {
        std::lock_guard<std::mutex> lock(reportMutex);
        parser.ExtractPlayerState(filePath);
        auto newReport = parser.GenerateHoardReport();

        nlohmann::json newReportJson;
        newReportJson["keep"] = newReport.keep;
        newReportJson["sell"] = newReport.sell;
        newReportJson["use"] = newReport.use;
        newReportJson["units"] = newReport.units;
        newReportJson["nanites"] = newReport.nanites;
        newReportJson["quicksilver"] = newReport.quicksilver;

        auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
            lastWriteTime - std::filesystem::file_time_type::clock::now() + std::chrono::system_clock::now()
        );
        std::time_t tt = std::chrono::system_clock::to_time_t(sctp);
        newReportJson["lastSaveTime"] = static_cast<long long>(tt);

        server.UpdateReport(newReportJson);
    };

    // Initial load
    std::string newestFile = "";
    std::filesystem::file_time_type newestTime = std::filesystem::file_time_type::min();

    for (const auto& entry : std::filesystem::recursive_directory_iterator(targetDir)) {
        if (entry.is_regular_file()) {
            std::string filename = entry.path().filename().string();
            if (filename.length() >= 3 && filename.substr(filename.length() - 3) == ".hg" && filename.substr(0, 3) != "mf_") {
                auto time = std::filesystem::last_write_time(entry.path());
                if (time > newestTime) {
                    newestTime = time;
                    newestFile = entry.path().string();
                }
            }
        }
    }

    if (!newestFile.empty()) {
        std::cout << "Parsing initial save file " << newestFile << "...\n";
        updateReport(newestFile, newestTime);
    }

    // Setup watcher
    SaveWatcher watcher(targetDir);
    watcher.OnChange([&](const std::string& changedFilePath) {
        std::string filename = std::filesystem::path(changedFilePath).filename().string();
        std::cout << "[DTHM] Save file updated detected: " << filename << ". Regenerating Hoard Report...\n";
        std::filesystem::file_time_type writeTime;
        try {
            writeTime = std::filesystem::last_write_time(changedFilePath);
        } catch (...) {
            writeTime = std::filesystem::file_time_type::clock::now();
        }
        updateReport(changedFilePath, writeTime);
    });

    // Start background tasks
    watcher.Start();

    // Start server on main thread (blocking)
    server.Start();

    return 0;
}
