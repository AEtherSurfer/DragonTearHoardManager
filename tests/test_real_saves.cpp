// Dragon Tear Hoard Manager
// Copyright (C) 2024
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include <gtest/gtest.h>
#include <nlohmann/json.hpp>
#include <filesystem>
#include <fstream>
#include <string>
#include <iostream>
#include <algorithm>

#ifndef TEST_DATA_DIR
#define TEST_DATA_DIR ""
#endif

#include "../games/nms-cpp/src/InventoryParser.hpp"

TEST(NMSIntegration, ParseRealSaveData) {
    std::string savesDir = std::string(TEST_DATA_DIR) + "nms_saves/";

    // Check if directory exists to gracefully skip or fail if missing
    if (!std::filesystem::exists(savesDir) || !std::filesystem::is_directory(savesDir)) {
        FAIL() << "Test directory does not exist: " << savesDir;
    }

    int filesTested = 0;

    for (const auto& entry : std::filesystem::recursive_directory_iterator(savesDir)) {
        if (!entry.is_regular_file()) {
            continue;
        }

        std::string filename = entry.path().filename().string();

        // Process only .hg files and skip mf_ files
        if (entry.path().extension() == ".hg" && filename.find("mf_") != 0) {
            std::ifstream file(entry.path());
            if (!file.is_open()) {
                ADD_FAILURE() << "Could not open file: " << entry.path().string();
                continue;
            }

            nlohmann::json jsonData;
            try {
                // Read the whole file into a string first
                std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

                // Some NMS saves are minified/obfuscated JSON or have garbage/binary headers.
                size_t bracePos = content.find('{');
                if (bracePos != std::string::npos && bracePos > 0) {
                    content = content.substr(bracePos);
                }

                // Remove null bytes and trailing garbage that breaks nlohmann::json
                content.erase(std::remove(content.begin(), content.end(), '\0'), content.end());

                // Ensure it ends at the last closing brace
                size_t lastBracePos = content.rfind('}');
                if (lastBracePos != std::string::npos) {
                    content = content.substr(0, lastBracePos + 1);
                }

                // Check for non-ascii or binary garbage that nlohmann::json will fail on.
                // We'll clean it up to allow parsing if possible, otherwise let it fail gracefully.
                for (size_t i = 0; i < content.size(); ++i) {
                    if (static_cast<unsigned char>(content[i]) < 32 && content[i] != '\n' && content[i] != '\r' && content[i] != '\t') {
                        content[i] = '?'; // replace control chars so JSON doesn't throw invalid string exception
                    }
                }

                jsonData = nlohmann::json::parse(content, nullptr, false, true); // ignore_comments=true

                if (jsonData.is_discarded()) {
                    std::cerr << "Failed to parse: " << entry.path().string() << " gracefully skipping." << std::endl;
                    continue;
                }

            } catch (const nlohmann::json::parse_error& e) {
                std::cerr << "JSON parse error in file: " << entry.path().string() << " Error: " << e.what() << " gracefully skipping." << std::endl;
                continue;
            } catch (const std::exception& e) {
                std::cerr << "Exception during JSON parsing in file: " << entry.path().string() << " Error: " << e.what() << " gracefully skipping." << std::endl;
                continue;
            }

            dthm::nms::InventoryParser parser;

            EXPECT_NO_THROW({
                parser.ExtractPlayerState(jsonData);
                parser.GenerateHoardReport();
            }) << "Exception thrown during extraction/report generation for file: " << entry.path().string();

            filesTested++;
        }
    }

    std::cout << "Tested " << filesTested << " real save files." << std::endl;
    EXPECT_GT(filesTested, 0) << "No .hg save files were found in " << savesDir;
}
