// Dragon Tear Hoard Manager
// Copyright (C) 2024
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.

#pragma once

#include <string>
#include <vector>
#include <optional>
#include <nlohmann/json.hpp>
#include "TearEngine.hpp"

namespace dthm {
namespace nms {

struct HoardReport {
    std::vector<std::string> keep;
    std::vector<std::string> sell;
    std::vector<std::string> use;
    int redundantTechFound = 0;
    long long units = 0;
    long long nanites = 0;
    long long quicksilver = 0;
};

class InventoryParser {
public:
    InventoryParser();
    ~InventoryParser();

    std::string ExtractPlayerState(const std::string& filePath);
    void ParseItemMapping(const std::string& mappingFilePath);
    HoardReport GenerateHoardReport();

    // For testing and internal use
    void ExtractPlayerState(const nlohmann::json& saveData);
    const DragonTear::PlayerState& GetPlayerState() const { return m_playerState; }

    int GetPopulatedSlots() const { return m_populatedSlots; }
    int GetTotalSlots() const { return m_totalSlots; }

    // Decompress an NMS save file (LZ4 blocks) if compressed
    static std::string DecompressSaveData(const std::vector<uint8_t>& data);

private:
    DragonTear::PlayerState m_playerState;
    std::vector<DragonTear::ItemData> m_itemMapping;
    std::vector<std::pair<std::string, int>> m_inventorySlots; // NMS ID, Amount
    int m_populatedSlots = 0;
    int m_totalSlots = 0;

    long long m_units = 0;
    long long m_nanites = 0;
    long long m_quicksilver = 0;
};

} // namespace nms
} // namespace dthm
