// Dragon Tear Hoard Manager
// Copyright (C) 2024
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.

#include "SaveWatcher.hpp"
#include <iostream>
#include <chrono>

namespace dthm {
namespace nms {

SaveWatcher::SaveWatcher(const std::string& saveDirectory)
    : m_saveDirectory(saveDirectory)
{
    try {
        if (std::filesystem::exists(m_saveDirectory) && std::filesystem::is_directory(m_saveDirectory)) {
            for (const auto& entry : std::filesystem::recursive_directory_iterator(m_saveDirectory)) {
                if (entry.is_regular_file()) {
                    std::string path = entry.path().string();
                    std::string filename = entry.path().filename().string();
                    if (filename.length() >= 3 && filename.substr(filename.length() - 3) == ".hg" &&
                        filename.substr(0, 3) != "mf_") {
                        m_fileTimestamps[path] = std::filesystem::last_write_time(path);
                    }
                }
            }
        } else {
            std::cerr << "Warning: SaveWatcher started but directory does not exist yet: " << m_saveDirectory << std::endl;
        }
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Error checking save directory: " << e.what() << std::endl;
    }
}

SaveWatcher::~SaveWatcher() {
    Stop();
}

void SaveWatcher::OnChange(std::function<void(const std::string&)> callback) {
    m_callback = std::move(callback);
}

void SaveWatcher::Start() {
    if (!m_running.exchange(true)) {
        m_watcherThread = std::thread(&SaveWatcher::WatchLoop, this);
    }
}

void SaveWatcher::Stop() {
    if (m_running.exchange(false)) {
        if (m_watcherThread.joinable()) {
            m_watcherThread.join();
        }
    }
}

void SaveWatcher::WatchLoop() {
    while (m_running) {
        std::this_thread::sleep_for(std::chrono::seconds(2));

        try {
            if (std::filesystem::exists(m_saveDirectory) && std::filesystem::is_directory(m_saveDirectory)) {
                for (const auto& entry : std::filesystem::recursive_directory_iterator(m_saveDirectory)) {
                    if (entry.is_regular_file()) {
                        std::string path = entry.path().string();
                        std::string filename = entry.path().filename().string();

                        if (filename.length() >= 3 && filename.substr(filename.length() - 3) == ".hg" &&
                            filename.substr(0, 3) != "mf_") {

                            auto currentWriteTime = std::filesystem::last_write_time(path);

                            bool isNewOrChanged = false;
                            auto it = m_fileTimestamps.find(path);
                            if (it == m_fileTimestamps.end()) {
                                isNewOrChanged = true;
                            } else if (currentWriteTime > it->second) {
                                isNewOrChanged = true;
                            }

                            if (isNewOrChanged) {
                                m_fileTimestamps[path] = currentWriteTime;
                                if (m_callback) {
                                    m_callback(path);
                                }
                            }
                        }
                    }
                }
            }
        } catch (const std::filesystem::filesystem_error& e) {
            std::cerr << "SaveWatcher error: " << e.what() << std::endl;
        }
    }
}

} // namespace nms
} // namespace dthm
