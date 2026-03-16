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

#pragma once

#include <string>
#include <optional>
#include "DragonLogger.hpp"

namespace DragonTear {

enum class Recommendation {
    KEEP,
    SELL,
    USE
};

struct PlayerState {
    float inventoryFullness; // 0.0 to 1.0
    bool hasPersonalRefiner;
    int currentStackSize;
    bool isObjectiveItem;
};

struct TearLogic {
    std::string primary;
    std::optional<std::string> condition;
    std::optional<std::string> fallback;
    std::optional<int> threshold_stack;
    std::string note;
};

struct ItemData {
    std::string id;
    std::string name;
    std::string category;
    TearLogic tear_logic;
    float value = 0.0f;
    float weight = 0.0f;

    float getEfficiencyRatio() const {
        if (weight <= 0.0f) {
            return value;
        }
        return value / weight;
    }
};

class TearEngine {
public:
    TearEngine() {}

    Recommendation evaluateItem(const ItemData& item, const PlayerState& state) const {
        // "Dragon's Eye" watchful over mission-critical items
        if (state.isObjectiveItem) {
            DragonLogger::logKeep(item.id, "Mission-critical objective item detected.");
            return Recommendation::KEEP;
        }

        if (item.category == "REFINABLE") {
            if (state.hasPersonalRefiner) {
                DragonLogger::logUse(item.id, "Refiner available for processing.");
                return Recommendation::USE;
            } else if (state.inventoryFullness > 0.8f) {
                DragonLogger::logSell(item.id, "No refiner and inventory > 80%. Shedding a Golden Tear.");
                return Recommendation::SELL;
            } else {
                DragonLogger::logKeep(item.id, "No refiner but inventory has space. Keeping for now.");
                return Recommendation::KEEP;
            }
        }

        if (item.tear_logic.threshold_stack.has_value()) {
            if (state.currentStackSize > item.tear_logic.threshold_stack.value()) {
                DragonLogger::logSell(item.id, "Stack size (" + std::to_string(state.currentStackSize) +
                                               ") exceeds threshold (" + std::to_string(item.tear_logic.threshold_stack.value()) + ").");
                return Recommendation::SELL;
            } else if (item.category == "CURRENCY_ITEM" || item.category == "TERRAIN_WASTE") {
                DragonLogger::logKeep(item.id, "Stack size is below threshold.");
                return Recommendation::KEEP;
            }
        }

        if (item.category == "TRADE_GOOD") {
            DragonLogger::logSell(item.id, "Pure liquid asset. No crafting utility.");
            return Recommendation::SELL;
        }

        if (item.category == "HIGH_VALUE") {
            DragonLogger::logKeep(item.id, "High-value asset detected.");
            return Recommendation::KEEP;
        }

        // The Mercenary logic: Disassemble vs. Sell
        if (item.category == "WEAPON" || item.category == "APPAREL") {
            float efficiency = item.getEfficiencyRatio();
            // Arbitrary threshold: If value-per-weight is high, we sell for Eddies
            if (efficiency > 100.0f) {
                DragonLogger::logSell(item.id, "High efficiency (" + std::to_string(efficiency) + "). Better sold for Eddies.");
                return Recommendation::SELL;
            } else {
                DragonLogger::logUse(item.id, "Low efficiency (" + std::to_string(efficiency) + "). Better disassembled into components.");
                return Recommendation::USE;
            }
        }

        // Default behavior based on tear logic
        if (item.tear_logic.primary == "KEEP") {
            DragonLogger::logKeep(item.id, item.tear_logic.note);
            return Recommendation::KEEP;
        } else if (item.tear_logic.primary == "SELL") {
            DragonLogger::logSell(item.id, item.tear_logic.note);
            return Recommendation::SELL;
        } else if (item.tear_logic.primary == "USE") {
            DragonLogger::logUse(item.id, item.tear_logic.note);
            return Recommendation::USE;
        }

        DragonLogger::logKeep(item.id, "Default fallback. Dragon's Eye watchful.");
        return Recommendation::KEEP;
    }
};

} // namespace DragonTear
