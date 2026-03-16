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

#include <iostream>
#include <fstream>
#include <string>

namespace DragonTear {

class DragonLogger {
public:
    static void init(const std::string& logFile) {
        logFilePath_ = logFile;
        // Clear old log
        std::ofstream ofs(logFilePath_, std::ofstream::out | std::ofstream::trunc);
    }

    static void logKeep(const std::string& item, const std::string& reason) {
        log("[DTHM] Dragon's Eye watchful on " + item + ": " + reason);
    }

    static void logSell(const std::string& item, const std::string& reason) {
        log("[DTHM] Shedding a Golden Tear for " + item + ": " + reason);
    }

    static void logUse(const std::string& item, const std::string& reason) {
        log("[DTHM] The Dragon consumes " + item + ": " + reason);
    }

    static void log(const std::string& message) {
        std::ofstream ofs(logFilePath_, std::ofstream::out | std::ofstream::app);
        if (ofs.is_open()) {
            ofs << message << "\n";
            ofs.close();
        } else {
            std::cerr << "Failed to open log file: " << logFilePath_ << "\n";
        }
    }

private:
    inline static std::string logFilePath_ = "dthm_dragon.log";
};

} // namespace DragonTear
