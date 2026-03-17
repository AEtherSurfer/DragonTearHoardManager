// Dragon Tear Hoard Manager
// Copyright (C) 2024
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.

#include "WebServer.hpp"
#include <httplib.h>
#include <iostream>
#include <thread>

namespace dthm {
namespace nms {

WebServer::WebServer(int port) : m_port(port) {
    m_server = new httplib::Server();
    m_cachedReport = "{}"; // Default empty JSON
    SetupRoutes();
}

WebServer::~WebServer() {
    Stop();
    delete m_server;
}

void WebServer::SetupRoutes() {
    m_server->Get("/api/report", [this](const httplib::Request& /*req*/, httplib::Response& res) {
        std::lock_guard<std::mutex> lock(m_mutex);
        res.set_content(m_cachedReport, "application/json");
    });

    m_server->Get("/", [this](const httplib::Request& /*req*/, httplib::Response& res) {
        res.set_content(GenerateHtml(), "text/html");
    });
}

void WebServer::UpdateReport(const nlohmann::json& reportJson) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_cachedReport = reportJson.dump();
}

void WebServer::Start() {
    if (!m_server->listen("0.0.0.0", m_port)) {
        std::cerr << "WebServer failed to bind on port " << m_port << std::endl;
    }
}

void WebServer::Stop() {
    m_server->stop();
}

std::string WebServer::GenerateHtml() {
    return R"(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Dragon Tear Hoard Manager</title>
    <style>
        body {
            background-color: #1a1a1a;
            color: #ffffff;
            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
            margin: 0;
            padding: 20px;
        }
        h1 {
            color: #f39c12;
            text-align: center;
        }
        .container {
            display: flex;
            flex-direction: column;
            gap: 20px;
            max-width: 800px;
            margin: 0 auto;
        }
        .card {
            background-color: #2c3e50;
            border-radius: 8px;
            padding: 20px;
            box-shadow: 0 4px 6px rgba(0,0,0,0.3);
        }
        .card-title {
            margin-top: 0;
            border-bottom: 2px solid #34495e;
            padding-bottom: 10px;
        }
        .keep .card-title { color: #e74c3c; } /* Red for keep */
        .sell .card-title { color: #f1c40f; } /* Gold for sell */
        .use .card-title { color: #e67e22; }  /* Orange for use */
        ul {
            list-style-type: none;
            padding: 0;
        }
        li {
            padding: 5px 0;
            border-bottom: 1px solid #34495e;
        }
        li:last-child {
            border-bottom: none;
        }
    </style>
</head>
<body>
    <h1>Dragon Tear Hoard Manager</h1>
    <div class="container">
        <div class="card keep">
            <h2 class="card-title">👁️ THE DRAGON'S EYE (Keep)</h2>
            <ul id="keep-list">Loading...</ul>
        </div>
        <div class="card sell">
            <h2 class="card-title">💧 THE GOLDEN TEAR (Sell/Discard)</h2>
            <ul id="sell-list">Loading...</ul>
        </div>
        <div class="card use">
            <h2 class="card-title">🔥 THE REFINER'S FIRE (Use/Refine)</h2>
            <ul id="use-list">Loading...</ul>
        </div>
    </div>

    <script>
        async function fetchReport() {
            try {
                const response = await fetch('/api/report');
                const data = await response.json();

                const populateList = (id, items) => {
                    const list = document.getElementById(id);
                    list.innerHTML = '';
                    if (!items || items.length === 0) {
                        list.innerHTML = '<li>(None)</li>';
                    } else {
                        items.forEach(item => {
                            const li = document.createElement('li');
                            li.textContent = item;
                            list.appendChild(li);
                        });
                    }
                };

                populateList('keep-list', data.keep);
                populateList('sell-list', data.sell);
                populateList('use-list', data.use);
            } catch (err) {
                console.error('Error fetching report:', err);
            }
        }

        // Fetch immediately, then every 5 seconds
        fetchReport();
        setInterval(fetchReport, 5000);
    </script>
</body>
</html>
    )";
}

} // namespace nms
} // namespace dthm
