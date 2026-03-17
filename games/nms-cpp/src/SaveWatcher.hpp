// Dragon Tear Hoard Manager
// Copyright (C) 2024
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.

#pragma once

#include <string>
#include <functional>
#include <thread>
#include <atomic>
#include <filesystem>

namespace dthm {
namespace nms {

class SaveWatcher {
public:
    SaveWatcher(const std::string& filePath);
    ~SaveWatcher();

    void OnChange(std::function<void()> callback);
    void Start();
    void Stop();

private:
    void WatchLoop();

    std::string m_filePath;
    std::function<void()> m_callback;
    std::thread m_watcherThread;
    std::atomic<bool> m_running{false};
    std::filesystem::file_time_type m_lastWriteTime;
};

} // namespace nms
} // namespace dthm
