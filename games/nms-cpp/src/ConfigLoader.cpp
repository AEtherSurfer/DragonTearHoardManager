// Dragon Tear Hoard Manager
// Copyright (C) 2024
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.

#include "ConfigLoader.hpp"
#include <fstream>
#include <sstream>
#include <iostream>

namespace dthm {
namespace nms {

std::string ConfigLoader::LoadAESKey(const std::string& envFilePath) {
    std::ifstream file(envFilePath);
    if (!file.is_open()) {
        std::cerr << "Could not open config file: " << envFilePath << std::endl;
        return "";
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.find("NMS_AES_KEY=") == 0) {
            return line.substr(12);
        }
    }
    return "";
}

} // namespace nms
} // namespace dthm
