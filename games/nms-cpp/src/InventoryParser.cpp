// Dragon Tear Hoard Manager
// Copyright (C) 2024
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.

#include "InventoryParser.hpp"
#include "SaveManager.hpp"

namespace dthm {
namespace nms {

InventoryParser::InventoryParser() {
}

InventoryParser::~InventoryParser() {
}

std::string InventoryParser::ExtractPlayerState(const std::string& filePath) {
    SaveManager saveManager;
    std::string unencryptedJson = saveManager.DecryptSaveFile(filePath);

    // Here we would normally parse the JSON to extract the player state
    // But for this task, we just wire the output.

    return unencryptedJson;
}

} // namespace nms
} // namespace dthm
