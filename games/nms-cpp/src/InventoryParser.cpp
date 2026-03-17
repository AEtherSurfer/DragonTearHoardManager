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
#include <fstream>
#include <algorithm>

namespace dthm {
namespace nms {

InventoryParser::InventoryParser() {
    // Default initialization
    m_playerState.inventoryFullness = 0.0f;
    m_playerState.hasPersonalRefiner = false;
    m_playerState.currentStackSize = 0;
    m_playerState.isObjectiveItem = false;
    m_playerState.currentWeight = 0.0f;
    m_playerState.maxWeight = 0.0f;
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
        ExtractPlayerState(j);
        return j.dump();
    } catch (const nlohmann::json::parse_error& e) {
        std::cerr << "JSON parsing error: " << e.what() << "\n"
                  << "Exception id: " << e.id << "\n"
                  << "Byte position of error: " << e.byte << std::endl;
        return "";
    }
}

void InventoryParser::ExtractPlayerState(const nlohmann::json& saveData) {
    m_inventorySlots.clear();
    m_playerState.equippedTech.clear();

    try {
        if (saveData.contains("PlayerStateData") && saveData["PlayerStateData"].contains("Inventory_Personal")) {
            const auto& inventory = saveData["PlayerStateData"]["Inventory_Personal"];

            int totalSlots = 0;
            if (inventory.contains("ValidSlotIndices")) {
                totalSlots = inventory["ValidSlotIndices"].size();
            }

            int populatedSlots = 0;
            if (inventory.contains("Slots")) {
                for (const auto& slot : inventory["Slots"]) {
                    populatedSlots++;
                    if (slot.contains("Id")) {
                        std::string id = slot["Id"].get<std::string>();
                        // Strip prefix like ^
                        if (!id.empty() && id[0] == '^') {
                            id = id.substr(1);
                        }

                        int amount = 1;
                        if (slot.contains("Amount")) {
                            amount = slot["Amount"].get<int>();
                        }

                        m_inventorySlots.push_back({id, amount});

                        // Check if it's technology
                        if (slot.contains("Type") && slot["Type"].contains("InventoryType")) {
                            std::string invType = slot["Type"]["InventoryType"].get<std::string>();
                            if (invType == "Technology") {
                                DragonTear::InstalledTech tech;
                                tech.techType = "Unknown";
                                tech.classTier = 1;
                                tech.qualityScore = 0.0f;

                                // Parse mapping to find correct values at extraction time
                                auto it = std::find_if(m_itemMapping.begin(), m_itemMapping.end(),
                                    [&id](const DragonTear::ItemData& d) {
                                        if (d.id == id) return true;
                                        size_t last_underscore = id.find_last_of('_');
                                        if (last_underscore != std::string::npos && last_underscore == id.length() - 2) {
                                            char tierChar = id.back();
                                            if (tierChar == 'C' || tierChar == 'B' || tierChar == 'A' || tierChar == 'S' || tierChar == 'X') {
                                                return d.id == id.substr(0, last_underscore);
                                            }
                                        }
                                        return false;
                                    });

                                if (it != m_itemMapping.end() && it->category == "TECH_PACKAGE" && it->technology_type.has_value()) {
                                    tech.techType = it->technology_type.value();
                                    if (it->class_tier.has_value()) {
                                        tech.classTier = it->class_tier.value();
                                    } else {
                                        size_t last_underscore = id.find_last_of('_');
                                        if (last_underscore != std::string::npos && last_underscore == id.length() - 2) {
                                            char tierChar = id.back();
                                            if (tierChar == 'C') tech.classTier = 1;
                                            else if (tierChar == 'B') tech.classTier = 2;
                                            else if (tierChar == 'A') tech.classTier = 3;
                                            else if (tierChar == 'S') tech.classTier = 4;
                                            else if (tierChar == 'X') tech.classTier = 5;
                                        }
                                    }
                                }
                                m_playerState.equippedTech.push_back(tech);
                            }
                        }
                    }
                }
            }

            m_totalSlots = totalSlots;
            m_populatedSlots = populatedSlots;

            if (totalSlots > 0) {
                m_playerState.inventoryFullness = static_cast<float>(populatedSlots) / totalSlots;
            } else {
                m_playerState.inventoryFullness = 0.0f;
            }
        }
    } catch (const nlohmann::json::exception& e) {
        std::cerr << "Error extracting player state: " << e.what() << std::endl;
    }
}

void InventoryParser::ParseItemMapping(const std::string& mappingFilePath) {
    std::ifstream file(mappingFilePath);
    if (!file.is_open()) {
        std::cerr << "Failed to open mapping file: " << mappingFilePath << std::endl;
        return;
    }

    try {
        nlohmann::json j;
        file >> j;

        if (j.contains("nms_items")) {
            for (const auto& itemJson : j["nms_items"]) {
                DragonTear::ItemData item;
                item.id = itemJson.value("id", "");
                item.name = itemJson.value("name", "");
                item.category = itemJson.value("category", "");

                if (itemJson.contains("tear_logic")) {
                    const auto& logicJson = itemJson["tear_logic"];
                    item.tear_logic.primary = logicJson.value("primary", "KEEP");
                    if (logicJson.contains("condition")) item.tear_logic.condition = logicJson["condition"].get<std::string>();
                    if (logicJson.contains("fallback")) item.tear_logic.fallback = logicJson["fallback"].get<std::string>();
                    if (logicJson.contains("threshold_stack")) item.tear_logic.threshold_stack = logicJson["threshold_stack"].get<int>();
                    item.tear_logic.note = logicJson.value("note", "");
                }

                if (itemJson.contains("technology_type")) item.technology_type = itemJson["technology_type"].get<std::string>();
                if (itemJson.contains("class_tier")) item.class_tier = itemJson["class_tier"].get<int>();
                item.value = itemJson.value("value", 0.0f);
                item.weight = itemJson.value("weight", 0.0f);

                m_itemMapping.push_back(item);
            }
        }
    } catch (const nlohmann::json::exception& e) {
        std::cerr << "JSON parsing error in mapping file: " << e.what() << std::endl;
    }
}

HoardReport InventoryParser::GenerateHoardReport() {
    HoardReport report;
    DragonTear::TearEngine engine;

    for (const auto& slot : m_inventorySlots) {
        std::string itemId = slot.first;
        int amount = slot.second;

        // Find item in mapping
        auto it = std::find_if(m_itemMapping.begin(), m_itemMapping.end(),
            [&itemId](const DragonTear::ItemData& d) {
                if (d.id == itemId) return true;
                // Only fallback if the item is explicitly a procedural upgrade module ending in _C, _B, _A, _S, or _X
                size_t last_underscore = itemId.find_last_of('_');
                if (last_underscore != std::string::npos && last_underscore == itemId.length() - 2) {
                    char tierChar = itemId.back();
                    if (tierChar == 'C' || tierChar == 'B' || tierChar == 'A' || tierChar == 'S' || tierChar == 'X') {
                        return d.id == itemId.substr(0, last_underscore);
                    }
                }
                return false;
            });

        DragonTear::ItemData currentItem;
        if (it != m_itemMapping.end()) {
            currentItem = *it;
            // Handle some specific mapping like appending class if missing
            if (itemId != currentItem.id) {
                 currentItem.id = itemId;
            }
        } else {
            // Default unknown item
            currentItem.id = itemId;
            currentItem.name = itemId;
            currentItem.category = "UNKNOWN";
            currentItem.tear_logic.primary = "KEEP";
            currentItem.tear_logic.note = "Unknown item. Default keep.";
        }

        DragonTear::PlayerState state = m_playerState;
        state.currentStackSize = amount;

        // Evaluate item
        DragonTear::Recommendation rec = engine.evaluateItem(currentItem, state);

        std::string entry = std::to_string(amount) + "x " + currentItem.name + " (" + itemId + ")";

        if (rec == DragonTear::Recommendation::KEEP) {
            report.keep.push_back(entry);
        } else if (rec == DragonTear::Recommendation::SELL) {
            report.sell.push_back(entry);
            if (currentItem.category == "TECH_PACKAGE") {
                report.redundantTechFound++;
            }
        } else if (rec == DragonTear::Recommendation::USE) {
            report.use.push_back(entry);
        }
    }

    return report;
}

} // namespace nms
} // namespace dthm
