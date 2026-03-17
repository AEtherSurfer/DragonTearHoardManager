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
#include <unordered_map>

namespace dthm {
namespace nms {

class SaveWatcher {
public:
    SaveWatcher(const std::string& saveDirectory);
    ~SaveWatcher();

    void OnChange(std::function<void(const std::string&)> callback);
    void Start();
    void Stop();

private:
    void WatchLoop();

    std::string m_saveDirectory;
    std::function<void(const std::string&)> m_callback;
    std::thread m_watcherThread;
    std::atomic<bool> m_running{false};
    std::unordered_map<std::string, std::filesystem::file_time_type> m_fileTimestamps;
};

} // namespace nms
} // namespace dthm
