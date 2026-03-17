// Dragon Tear Hoard Manager
// Copyright (C) 2024
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.

#pragma once

#include <string>
#include <mutex>
#include <nlohmann/json.hpp>

// Forward declaration
namespace httplib {
    class Server;
}

namespace dthm {
namespace nms {

class WebServer {
public:
    WebServer(int port);
    ~WebServer();

    void Start();
    void Stop();

    void UpdateReport(const nlohmann::json& reportJson);

private:
    void SetupRoutes();
    std::string GenerateHtml();

    int m_port;
    httplib::Server* m_server;
    std::string m_cachedReport;
    std::mutex m_mutex;
};

} // namespace nms
} // namespace dthm
