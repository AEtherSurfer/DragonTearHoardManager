// Dragon Tear Hoard Manager
// Copyright (C) 2024
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.

#include "SaveManager.hpp"
#include <iostream>
#include <fstream>
#include <vector>
#include <filesystem>

namespace dthm {
namespace nms {

SaveManager::SaveManager() {
}

SaveManager::~SaveManager() {
}

std::string SaveManager::ReadSaveFile(const std::string& filePath) {
    std::ifstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Failed to open save file: " << filePath << std::endl;
        return "";
    }

    file.seekg(0, std::ios::end);
    std::streamsize fsize = file.tellg();
    file.seekg(0, std::ios::beg);

    if (fsize <= 0) {
        return "";
    }

    std::string content(fsize, '\0');
    if (file.read(&content[0], fsize)) {
        return content;
    }
    return "";
}

void SaveManager::BackupAndClearVerification(const std::string& saveDir) {
    namespace fs = std::filesystem;
    fs::path dirPath(saveDir);

    fs::path savePath = dirPath / "save.hg";
    fs::path backupPath = dirPath / "save.hg.bak";
    fs::path mfSavePath = dirPath / "mf_save.hg";

    if (fs::exists(savePath)) {
        try {
            fs::copy_file(savePath, backupPath, fs::copy_options::overwrite_existing);
        } catch (const fs::filesystem_error& e) {
            std::cerr << "Failed to backup save.hg: " << e.what() << std::endl;
        }
    }

    if (fs::exists(mfSavePath)) {
        try {
            fs::remove(mfSavePath);
        } catch (const fs::filesystem_error& e) {
            std::cerr << "Failed to remove mf_save.hg: " << e.what() << std::endl;
        }
    }
}

} // namespace nms
} // namespace dthm
