// Dragon Tear Hoard Manager
// Copyright (C) 2024
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.

#include "InventoryParser.hpp"
#include "SaveManager.hpp"
#include <nlohmann/json.hpp>
#include <iostream>

namespace dthm {
namespace nms {

InventoryParser::InventoryParser() {
}

InventoryParser::~InventoryParser() {
}

std::string InventoryParser::ExtractPlayerState(const std::string& filePath) {
    SaveManager saveManager;

    std::string unencryptedJson = saveManager.ReadSaveFile(filePath);

    if (unencryptedJson.empty()) {
        std::cerr << "Save file is empty or could not be read." << std::endl;
        return "";
    }

    try {
        nlohmann::json j = nlohmann::json::parse(unencryptedJson);
        // Here we would normally parse the JSON to extract the player state
        // But for this task, we just wire the output.
        return j.dump();
    } catch (const nlohmann::json::parse_error& e) {
        std::cerr << "JSON parsing error: " << e.what() << "\n"
                  << "Exception id: " << e.id << "\n"
                  << "Byte position of error: " << e.byte << std::endl;
        return "";
    }
}

} // namespace nms
} // namespace dthm
