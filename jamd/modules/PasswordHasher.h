#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <random>
#include <argon2.h>

#include "../../ext/alias.h"
#include "manager/ConfigFileManager/ConfigFileManager.h"

class PasswordHasher
{
public:
    // 生成带盐的Argon2哈希
    static String hashPassword(const String& password, const int salt_length = 16)
    {
        const Vector<uint8_t> salt = generateSalt(salt_length);

        const auto [
            timecost,
            memcost,
            parallelism,
            length] = jamd::managers::ConfigFileManager::Instance().daemonConfig.security.argon2;

        // 输出缓冲区
        Vector<uint8_t> hash(length);

        // 执行Argon2id哈希
        const int result = argon2id_hash_raw(
            timecost, memcost, parallelism,
            password.data(), password.length(),
            salt.data(), salt.size(),
            hash.data(), length
        );

        if (result != ARGON2_OK)
        {
            throw std::runtime_error("Argon2 hashing failed: " + String(argon2_error_message(result)));
        }

        return "$argon2id$v=19$m=" + std::to_string(memcost) +
            ",t=" + std::to_string(timecost) +
            ",p=" + std::to_string(parallelism) + "$" +
            base64Encode(salt) + "$" +
            base64Encode(hash);
    }

    // 验证密码
    static bool verifyPassword(const String& password, const String& hashed)
    {
        size_t pos = hashed.find('$', 1) + 1;
        const String params = hashed.substr(pos, hashed.find('$', pos) - pos);
        pos = hashed.find('$', pos) + 1;
        const String salt_b64 = hashed.substr(pos, hashed.find('$', pos) - pos);
        pos = hashed.find('$', pos) + 1;
        const String stored_hash_b64 = hashed.substr(pos);

        // 解码盐和哈希
        const Vector<uint8_t> salt = base64Decode(salt_b64);
        const Vector<uint8_t> stored_hash = base64Decode(stored_hash_b64);

        // 重新计算哈希
        Vector<uint8_t> computed_hash(stored_hash.size());
        const int result = argon2id_hash_raw(
            extractParam(params, "t"),
            extractParam(params, "m"),
            extractParam(params, "p"),
            password.data(), password.length(),
            salt.data(), salt.size(),
            computed_hash.data(), computed_hash.size()
        );

        if (result != ARGON2_OK) return false;

        // 安全比较（防时序攻击）
        return constantTimeCompare(computed_hash, stored_hash);
    }

private:
    // 生成密码学安全的随机盐
    static Vector<uint8_t> generateSalt(const size_t length)
    {
        Vector<uint8_t> salt(length);
        std::random_device rd;
        std::uniform_int_distribution<uint8_t> dist(0, 255);
        for (auto& byte : salt)
        {
            byte = dist(rd);
        }
        return salt;
    }

    // 恒定时间比较（防时序攻击）
    static bool constantTimeCompare(const Vector<uint8_t>& a, const Vector<uint8_t>& b)
    {
        if (a.size() != b.size()) return false;
        uint8_t result = 0;
        for (size_t i = 0; i < a.size(); ++i)
        {
            result |= a[i] ^ b[i];
        }
        return result == 0;
    }

    static String base64Encode(const Vector<uint8_t>& data)
    {
        const auto base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
        String ret;
        int i = 0;
        uint8_t char_array_3[3], char_array_4[4];

        for (const auto byte : data)
        {
            char_array_3[i++] = byte;
            if (i == 3)
            {
                char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
                char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
                char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
                char_array_4[3] = char_array_3[2] & 0x3f;
                for (i = 0; i < 4; i++) ret += base64_chars[char_array_4[i]];
                i = 0;
            }
        }

        if (i)
        {
            int j = 0;
            for (j = i; j < 3; j++) char_array_3[j] = '\0';
            char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
            char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
            char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
            for (j = 0; j < i + 1; j++) ret += base64_chars[char_array_4[j]];
            while (i++ < 3) ret += '=';
        }
        return ret;
    }

    static Vector<uint8_t> base64Decode(const String& encoded)
    {
        const String base64_chars =
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz"
            "0123456789+/";

        Vector<uint8_t> decoded;
        int i = 0, j = 0, in_len = encoded.size();
        uint8_t char_array_4[4], char_array_3[3];

        while (in_len-- && encoded[j] != '=' && !isspace(encoded[j]))
        {
            char_array_4[i++] = encoded[j++];
            if (i == 4)
            {
                for (i = 0; i < 4; i++)
                {
                    const size_t pos = base64_chars.find(char_array_4[i]);
                    if (pos == String::npos)
                    {
                        throw std::runtime_error("Invalid base64 character");
                    }
                    char_array_4[i] = pos;
                }

                char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
                char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
                char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

                for (i = 0; i < 3; i++)
                {
                    decoded.push_back(char_array_3[i]);
                }
                i = 0;
            }
        }

        if (i)
        {
            for (int k = i; k < 4; k++)
            {
                char_array_4[k] = 0;
            }

            for (int k = 0; k < 4; k++)
            {
                const size_t pos = base64_chars.find(char_array_4[k]);
                if (pos == String::npos && k < i)
                {
                    throw std::runtime_error("Invalid base64 character");
                }
                char_array_4[k] = (pos == String::npos) ? 0 : pos;
            }

            char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
            char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
            char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

            for (int k = 0; k < i - 1; k++)
            {
                decoded.push_back(char_array_3[k]);
            }
        }

        return decoded;
    }

    // 从参数字符串提取数值
    static uint32_t extractParam(const String& params, const String& key)
    {
        size_t start_pos = params.find(key + "=");
        if (start_pos == String::npos)
        {
            throw std::runtime_error("Parameter not found: " + key);
        }

        start_pos += key.length() + 1; // 移动到值开始位置
        const size_t end_pos = params.find(',', start_pos);

        String value_str;
        if (end_pos == String::npos)
        {
            value_str = params.substr(start_pos);
        }
        else
        {
            value_str = params.substr(start_pos, end_pos - start_pos);
        }

        try
        {
            return std::stoul(value_str);
        }
        catch (const std::exception& _)
        {
            throw std::runtime_error("Invalid parameter value for " + key + ": " + value_str);
        }
    }
};
