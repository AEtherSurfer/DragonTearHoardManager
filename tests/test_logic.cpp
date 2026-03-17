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
#include <fstream>
#include <string>
#include "TearEngine.hpp"

#ifndef TEST_DATA_DIR
#define TEST_DATA_DIR ""
#endif
#include "../games/nms-cpp/src/InventoryParser.hpp"

using namespace DragonTear;

TEST(TearEngine, DragonEyeGuardsObjectives) {
    TearEngine engine;
    ItemData item{"ATLAS_PASS", "Atlas Pass v3", "UTILITY", {"SELL", std::nullopt, std::nullopt, std::nullopt, "Usually sell"}};
    PlayerState state{0.5f, false, 1, true, 0.0f, 100.0f}; // isObjectiveItem = true
    EXPECT_EQ(engine.evaluateItem(item, state), Recommendation::KEEP);
}

TEST(TearEngine, GoldenTearForSilicatePowder) {
    TearEngine engine;
    ItemData item{"SILICATE_POWDER", "Silicate Powder", "TERRAIN_WASTE", {"KEEP", std::nullopt, std::nullopt, 1000, "Base building material"}};
    PlayerState state{0.5f, false, 1500, false, 0.0f, 100.0f}; // stack > 1000
    EXPECT_EQ(engine.evaluateItem(item, state), Recommendation::SELL);
}

TEST(TearEngine, RefinersPathWithRefiner) {
    TearEngine engine;
    ItemData item{"RESIDUAL_GOOP", "Residual Goop", "REFINABLE", {"KEEP", std::nullopt, std::nullopt, std::nullopt, "Refine into Nanites"}};
    PlayerState state{0.5f, true, 100, false, 0.0f, 100.0f}; // hasPersonalRefiner = true
    EXPECT_EQ(engine.evaluateItem(item, state), Recommendation::USE);
}

TEST(TearEngine, RefinersPathWithoutRefinerFullInventory) {
    TearEngine engine;
    ItemData item{"RESIDUAL_GOOP", "Residual Goop", "REFINABLE", {"KEEP", std::nullopt, std::nullopt, std::nullopt, "Refine into Nanites"}};
    PlayerState state{0.9f, false, 100, false, 0.0f, 100.0f}; // inventoryFullness > 0.8, hasPersonalRefiner = false
    EXPECT_EQ(engine.evaluateItem(item, state), Recommendation::SELL);
}

TEST(TearEngine, RefinersPathWithoutRefinerSpaceAvailable) {
    TearEngine engine;
    ItemData item{"RESIDUAL_GOOP", "Residual Goop", "REFINABLE", {"KEEP", std::nullopt, std::nullopt, std::nullopt, "Refine into Nanites"}};
    PlayerState state{0.5f, false, 100, false, 0.0f, 100.0f}; // inventoryFullness <= 0.8, hasPersonalRefiner = false
    EXPECT_EQ(engine.evaluateItem(item, state), Recommendation::KEEP);
}

TEST(TearEngine, MercenaryLogicHighEfficiencySells) {
    TearEngine engine;
    ItemData item{"LEXINGTON", "M-10AF Lexington", "WEAPON", {"KEEP", std::nullopt, std::nullopt, std::nullopt, ""}, 500.0f, 4.0f};
    PlayerState state{0.5f, false, 1, false, 50.0f, 100.0f};
    // efficiency = 500 / 4 = 125 > 100
    EXPECT_EQ(engine.evaluateItem(item, state), Recommendation::SELL);
}

TEST(TearEngine, MercenaryLogicLowEfficiencyDisassembles) {
    TearEngine engine;
    ItemData item{"LEXINGTON", "M-10AF Lexington", "WEAPON", {"KEEP", std::nullopt, std::nullopt, std::nullopt, ""}, 200.0f, 4.0f};
    PlayerState state{0.5f, false, 1, false, 50.0f, 100.0f};
    // efficiency = 200 / 4 = 50 <= 100 -> disassembles (USE)
    EXPECT_EQ(engine.evaluateItem(item, state), Recommendation::USE);
}

TEST(TearEngine, MercenaryLogicHighEncumbranceRaisesThreshold) {
    TearEngine engine;
    // Efficiency is 125. Normally this sells (threshold 100).
    ItemData item{"LEXINGTON", "M-10AF Lexington", "WEAPON", {"KEEP", std::nullopt, std::nullopt, std::nullopt, ""}, 500.0f, 4.0f};
    // Encumbrance is 90 / 100 = 0.9 (> 0.8). Threshold becomes 150.
    PlayerState state{0.5f, false, 1, false, 90.0f, 100.0f};
    // efficiency 125 <= 150 -> disassembles (USE)
    EXPECT_EQ(engine.evaluateItem(item, state), Recommendation::USE);
}

TEST(TearEngine, MercenaryLogicZeroWeightHandled) {
    TearEngine engine;
    ItemData item{"LIGHT_JACKET", "Light Jacket", "APPAREL", {"KEEP", std::nullopt, std::nullopt, std::nullopt, ""}, 50.0f, 0.0f};
    PlayerState state{0.5f, false, 1, false, 50.0f, 100.0f};
    // efficiency = 50 / 0 handled as 50 <= 100 -> disassembles (USE)
    EXPECT_EQ(engine.evaluateItem(item, state), Recommendation::USE);
}

TEST(InventoryParserIntegration, ParsesMockEndgameSave) {
    dthm::nms::InventoryParser parser;

    std::string path = std::string(TEST_DATA_DIR) + "mock_endgame_save.json";
    std::ifstream file(path);
    if (!file.is_open()) {
        FAIL() << "Could not open " << path;
    }

    nlohmann::json mockJson;
    file >> mockJson;

    parser.ExtractPlayerState(mockJson);

    auto state = parser.GetPlayerState();
    EXPECT_FLOAT_EQ(state.inventoryFullness, 2.0f / 3.0f); // 2 populated slots out of 3 valid

    auto report = parser.GenerateHoardReport();
    EXPECT_GE(report.keep.size() + report.sell.size() + report.use.size(), 2);
}

TEST(InventoryParserIntegration, HandlesEmptyOrMalformedSave) {
    dthm::nms::InventoryParser parser;
    nlohmann::json emptyJson;
    EXPECT_NO_THROW({
        parser.ExtractPlayerState(emptyJson);
    });

    auto state = parser.GetPlayerState();
    EXPECT_FLOAT_EQ(state.inventoryFullness, 0.0f);
}

// TechAuditTests
TEST(TearEngine, TechAuditUpgradeDetection) {
    TearEngine engine;
    ItemData item{"HYPERDRIVE_UPGRADE_S", "S-Class Hyperdrive Upgrade", "TECH_PACKAGE", {"KEEP", std::nullopt, std::nullopt, std::nullopt, "Upgrade"}, 100.0f, 0.0f, "Hyperdrive", 4};
    PlayerState state;
    state.isObjectiveItem = false;
    state.equippedTech = {
        {"Hyperdrive", 1, 0.0f},
        {"Hyperdrive", 1, 0.0f},
        {"Hyperdrive", 1, 0.0f}
    };
    EXPECT_EQ(engine.evaluateItem(item, state), Recommendation::KEEP);
}

TEST(TearEngine, TechAuditRedundancy) {
    TearEngine engine;
    ItemData item{"SHIELD_UPGRADE_B", "B-Class Shield Upgrade", "TECH_PACKAGE", {"KEEP", std::nullopt, std::nullopt, std::nullopt, "Upgrade"}, 100.0f, 0.0f, "Shield", 2};
    PlayerState state;
    state.isObjectiveItem = false;
    state.equippedTech = {
        {"Shield", 4, 0.0f},
        {"Shield", 4, 0.0f},
        {"Shield", 4, 0.0f}
    };
    EXPECT_EQ(engine.evaluateItem(item, state), Recommendation::SELL);
}

TEST(TearEngine, TechAuditObjectiveProtection) {
    TearEngine engine;
    ItemData item{"SPECIAL_TECH", "Objective Tech", "TECH_PACKAGE", {"SELL", std::nullopt, std::nullopt, std::nullopt, "Sell"}, 100.0f, 0.0f, "Hyperdrive", 1};
    PlayerState state;
    state.isObjectiveItem = true;
    state.equippedTech = {
        {"Hyperdrive", 4, 0.0f},
        {"Hyperdrive", 4, 0.0f},
        {"Hyperdrive", 4, 0.0f}
    };
    // Should be kept because it's an objective item
    EXPECT_EQ(engine.evaluateItem(item, state), Recommendation::KEEP);
}
