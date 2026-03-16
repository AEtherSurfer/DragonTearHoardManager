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
#include "TearEngine.hpp"

using namespace DragonTear;

TEST(TearEngine, DragonEyeGuardsObjectives) {
    TearEngine engine;
    ItemData item{"ATLAS_PASS", "Atlas Pass v3", "UTILITY", {"SELL", std::nullopt, std::nullopt, std::nullopt, "Usually sell"}};
    PlayerState state{0.5f, false, 1, true}; // isObjectiveItem = true
    EXPECT_EQ(engine.evaluateItem(item, state), Recommendation::KEEP);
}

TEST(TearEngine, GoldenTearForSilicatePowder) {
    TearEngine engine;
    ItemData item{"SILICATE_POWDER", "Silicate Powder", "TERRAIN_WASTE", {"KEEP", std::nullopt, std::nullopt, 1000, "Base building material"}};
    PlayerState state{0.5f, false, 1500, false}; // stack > 1000
    EXPECT_EQ(engine.evaluateItem(item, state), Recommendation::SELL);
}

TEST(TearEngine, RefinersPathWithRefiner) {
    TearEngine engine;
    ItemData item{"RESIDUAL_GOOP", "Residual Goop", "REFINABLE", {"KEEP", std::nullopt, std::nullopt, std::nullopt, "Refine into Nanites"}};
    PlayerState state{0.5f, true, 100, false}; // hasPersonalRefiner = true
    EXPECT_EQ(engine.evaluateItem(item, state), Recommendation::USE);
}

TEST(TearEngine, RefinersPathWithoutRefinerFullInventory) {
    TearEngine engine;
    ItemData item{"RESIDUAL_GOOP", "Residual Goop", "REFINABLE", {"KEEP", std::nullopt, std::nullopt, std::nullopt, "Refine into Nanites"}};
    PlayerState state{0.9f, false, 100, false}; // inventoryFullness > 0.8, hasPersonalRefiner = false
    EXPECT_EQ(engine.evaluateItem(item, state), Recommendation::SELL);
}

TEST(TearEngine, RefinersPathWithoutRefinerSpaceAvailable) {
    TearEngine engine;
    ItemData item{"RESIDUAL_GOOP", "Residual Goop", "REFINABLE", {"KEEP", std::nullopt, std::nullopt, std::nullopt, "Refine into Nanites"}};
    PlayerState state{0.5f, false, 100, false}; // inventoryFullness <= 0.8, hasPersonalRefiner = false
    EXPECT_EQ(engine.evaluateItem(item, state), Recommendation::KEEP);
}
