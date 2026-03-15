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

namespace DragonTear {

enum class Recommendation {
    KEEP,
    SELL,
    USE
};

class TearEngine {
public:
    TearEngine(float keepThreshold, float sellThreshold)
        : keepThreshold_(keepThreshold), sellThreshold_(sellThreshold) {}

    // Calculate Tear Threshold based on rarity, market value, and utility
    Recommendation calculateRecommendation(float rarity, float marketValue, float utility) const {
        float score = (rarity * 0.4f) + (marketValue * 0.4f) + (utility * 0.2f);

        // Dragon Scale logic
        if (score > keepThreshold_) {
            // The Dragon's Eye stays open
            return Recommendation::KEEP;
        } else if (score < sellThreshold_) {
            // Shed a GOLDEN TEAR
            return Recommendation::SELL;
        } else {
            return Recommendation::USE;
        }
    }

private:
    float keepThreshold_;
    float sellThreshold_;
};

} // namespace DragonTear
