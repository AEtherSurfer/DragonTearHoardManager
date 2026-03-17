// Dragon Tear Hoard Manager
// Copyright (C) 2024
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.

#include "SaveManager.hpp"
#include "ConfigLoader.hpp"

#include <iostream>
#include <fstream>
#include <vector>
#include <openssl/evp.h>
#include <openssl/err.h>

namespace dthm {
namespace nms {

SaveManager::SaveManager() {
    aesKey_ = ConfigLoader::LoadAESKey();
}

SaveManager::~SaveManager() {
    // Cleanup if necessary
}

std::string SaveManager::DecryptSaveFile(const std::string& filePath) {
    std::ifstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Failed to open save file: " << filePath << std::endl;
        return "";
    }

    // Reset file pointer
    file.seekg(0, std::ios::end);
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    if (size == 0) {
        return "";
    }

    // Read first character to check if it's already unencrypted JSON
    char firstChar;
    if (!file.read(&firstChar, 1)) {
        return "";
    }

    if (firstChar == '{') {
        // It's plain JSON
        file.seekg(0, std::ios::beg);
        std::string content(size, '\0');
        if (file.read(&content[0], size)) {
            return content;
        }
        return "";
    }
    file.seekg(0, std::ios::beg);

    // It's encrypted, need to decrypt using OpenSSL
    if (aesKey_.empty()) {
        std::cerr << "No AES key provided for decryption." << std::endl;
        return "";
    }

    if (aesKey_.length() < 32) {
        std::cerr << "AES key is too short. Expected at least 32 bytes for AES-256." << std::endl;
        return "";
    }

    // Read the whole file
    std::vector<unsigned char> fullContent(size);
    if (!file.read(reinterpret_cast<char*>(fullContent.data()), size)) {
        return "";
    }

    // NMS save files typically start with a magic header before the AES payload.
    // Usually, there is a magic number (4 bytes), followed by the uncompressed length (4 bytes),
    // and potentially padding or other metadata. In some common PC saves, the header is exactly 0x600 bytes.
    // But to be safer, let's scan for a known pattern or use 0x600 if size permits.
    // According to known structures, some saves start directly with the ciphertext.
    // Let's use 0x600 as the typical offset for NMS PC saves.
    int headerOffset = 0;
    if (size > 0x600) {
        headerOffset = 0x600;
    }

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        std::cerr << "Failed to create EVP_CIPHER_CTX" << std::endl;
        return "";
    }

    // AES-256-ECB or CBC? NMS is known to use AES-256-ECB. Wait, ECB doesn't use IV.
    // Let's use AES-256-ECB. If CBC, we'd need an IV. Most NMS modding tools use AES-256-ECB for saves.
    // Let's try EVP_aes_256_ecb(). Wait, NMS might use AES-256-CBC with a specific IV.
    // Actually, it's typically AES-128-ECB or AES-256-ECB depending on the version.
    // Let's use AES-256-ECB as requested: "AES-256 Decryption".

    // Assuming ECB for simplicity unless specified. Let's provide ECB.
    if (1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_ecb(), NULL, reinterpret_cast<const unsigned char*>(aesKey_.c_str()), NULL)) {
        std::cerr << "Failed to initialize decryption" << std::endl;
        EVP_CIPHER_CTX_free(ctx);
        return "";
    }

    std::vector<unsigned char> plainText(fullContent.size() - headerOffset + EVP_MAX_BLOCK_LENGTH);
    int len = 0;
    int plainTextLen = 0;

    if (1 != EVP_DecryptUpdate(ctx, plainText.data(), &len, fullContent.data() + headerOffset, fullContent.size() - headerOffset)) {
        std::cerr << "Failed to decrypt" << std::endl;
        EVP_CIPHER_CTX_free(ctx);
        return "";
    }
    plainTextLen = len;

    if (1 != EVP_DecryptFinal_ex(ctx, plainText.data() + len, &len)) {
        std::cerr << "Failed to finalize decryption" << std::endl;
        EVP_CIPHER_CTX_free(ctx);
        ERR_clear_error();
        return "";
    }
    plainTextLen += len;

    EVP_CIPHER_CTX_free(ctx);

    return std::string(reinterpret_cast<char*>(plainText.data()), plainTextLen);
}

} // namespace nms
} // namespace dthm
