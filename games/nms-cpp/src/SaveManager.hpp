// Dragon Tear Hoard Manager
// Copyright (C) 2024
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.

#pragma once

#include <string>

namespace dthm {
namespace nms {

class SaveManager {
public:
    SaveManager();
    ~SaveManager();

    std::string DecryptSaveFile(const std::string& filePath);

private:
    std::string aesKey_;
};

} // namespace nms
} // namespace dthm
