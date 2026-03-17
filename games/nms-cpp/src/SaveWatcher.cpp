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

SaveWatcher::SaveWatcher(const std::string& filePath)
    : m_filePath(filePath)
{
    try {
        if (std::filesystem::exists(m_filePath)) {
            m_lastWriteTime = std::filesystem::last_write_time(m_filePath);
        } else {
            std::cerr << "Warning: SaveWatcher started but file does not exist yet: " << m_filePath << std::endl;
        }
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Error checking save file: " << e.what() << std::endl;
    }
}

SaveWatcher::~SaveWatcher() {
    Stop();
}

void SaveWatcher::OnChange(std::function<void()> callback) {
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
            if (std::filesystem::exists(m_filePath)) {
                auto currentWriteTime = std::filesystem::last_write_time(m_filePath);
                if (currentWriteTime != m_lastWriteTime) {
                    m_lastWriteTime = currentWriteTime;

                    std::cout << "[SaveWatcher] Detected change in " << m_filePath << std::endl;
                    if (m_callback) {
                        m_callback();
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
