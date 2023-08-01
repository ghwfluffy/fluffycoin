#pragma once

#include <fluffycoin/utils/BinData.h>

#include <string>

#include <filesystem>

namespace fluffycoin
{

/**
 * Perform file system operations
 */
namespace FileTools
{
    bool exists(const std::string &path);
    bool isFile(const std::string &path);
    bool isDir(const std::string &path);

    bool read(
        const std::string &path,
        std::string &data);

    bool write(
        const std::string &path,
        const std::string &data);

    bool write(
        const std::string &path,
        const BinData &data);

    bool write(
        const std::string &path,
        const unsigned char *data,
        size_t len);

    bool createDir(
        const std::string &path,
        std::filesystem::perms perms =
            (std::filesystem::perms::owner_read |
            std::filesystem::perms::owner_write |
            std::filesystem::perms::owner_exec |
            std::filesystem::perms::group_read |
            std::filesystem::perms::group_write |
            std::filesystem::perms::group_exec |
            std::filesystem::perms::others_read |
            std::filesystem::perms::others_exec));
}

}
