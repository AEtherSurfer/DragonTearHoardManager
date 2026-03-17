#pragma once

#include <string>

namespace dthm {
namespace nms {

class ConfigLoader {
public:
    static std::string LoadAESKey(const std::string& envFilePath = "dthm_keys.env");
};

} // namespace nms
} // namespace dthm
