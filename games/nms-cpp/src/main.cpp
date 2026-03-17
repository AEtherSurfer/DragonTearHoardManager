// Dragon Tear Hoard Manager
// Copyright (C) 2024
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.

#include <iostream>
#include <string>
#include "InventoryParser.hpp"

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

    InventoryParser parser;
    std::cout << "Loading item mapping from " << mappingFilePath << "...\n";
    parser.ParseItemMapping(mappingFilePath);

    std::cout << "Parsing save file " << saveFilePath << "...\n";
    parser.ExtractPlayerState(saveFilePath);

    auto state = parser.GetPlayerState();
    auto report = parser.GenerateHoardReport();

    std::cout << "\n---------------------------------------\n";
    std::cout << "\n\033[1;36m👁️ THE DRAGON'S EYE (Keep)\033[0m\n";
    if (report.keep.empty()) {
        std::cout << "  (None)\n";
    } else {
        for (const auto& item : report.keep) {
            std::cout << "  " << item << "\n";
        }
    }

    std::cout << "\n\033[1;33m💧 THE GOLDEN TEAR (Sell/Discard)\033[0m\n";
    if (report.sell.empty()) {
        std::cout << "  (None)\n";
    } else {
        for (const auto& item : report.sell) {
            std::cout << "  " << item << "\n";
        }
    }

    std::cout << "\n\033[1;31m🔥 THE REFINER'S FIRE (Use/Refine)\033[0m\n";
    if (report.use.empty()) {
        std::cout << "  (None)\n";
    } else {
        for (const auto& item : report.use) {
            std::cout << "  " << item << "\n";
        }
    }

    std::cout << "\n---------------------------------------\n";
    std::cout << "Summary:\n";
    std::cout << "Inventory Fullness: " << parser.GetPopulatedSlots() << "/" << parser.GetTotalSlots() << " Slots.\n";
    std::cout << "Redundant Tech Found: " << report.redundantTechFound << ".\n";

    return 0;
}
