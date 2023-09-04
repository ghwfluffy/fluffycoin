#include <fluffycoin/utils/FileTools.h>
#include <fluffycoin/utils/Errno.h>
#include <fluffycoin/log/Log.h>

#include <filesystem>

using namespace fluffycoin;

namespace
{

std::string expandTilde(
    const std::string &path)
{
    if (!path.empty() && path[0] == '~')
    {
        const char *home = getenv("HOME");
        if (home && *home)
            return std::string(home) + path.substr(1);
    }
    return path;
}

}

bool FileTools::exists(const std::string &path)
{
    return std::filesystem::exists(path);
}

bool FileTools::isFile(const std::string &path)
{
    return std::filesystem::is_regular_file(path);
}

bool FileTools::isDir(const std::string &path)
{
    return std::filesystem::is_directory(path);
}

namespace
{

template<typename Output>
bool readT(
    const std::string &path,
    Output &data)
{
    FILE *fp = fopen(expandTilde(path).c_str(), "rb");
    if (!fp)
    {
        log::error("Failed to open file '{}' for reading: {}.", path, Errno::error());
        return false;
    }

    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    if (size < 0)
    {
        fclose(fp);
        log::error("Failed to get size of file '{}': {}.", path, Errno::error());
        return false;
    }
    fseek(fp, 0, SEEK_SET);

    size_t sizet = static_cast<size_t>(size);
    data.resize(sizet);

    size_t read = fread(data.data(), 1, sizet, fp);

    fclose(fp);

    return read == sizet;
}

}

bool FileTools::read(
    const std::string &path,
    std::string &data)
{
    return ::readT(path, data);
}

bool FileTools::read(
    const std::string &path,
    BinData &data)
{
    return ::readT(path, data);
}

bool FileTools::write(
    const std::string &path,
    const std::string &data)
{
    return write(path, reinterpret_cast<const unsigned char *>(data.data()), data.length());
}

bool FileTools::write(
    const std::string &path,
    const BinData &data)
{
    return write(path, data.data(), data.length());
}

bool FileTools::write(
    const std::string &path,
    const unsigned char *data,
    size_t len)
{
    FILE *fp = fopen(expandTilde(path).c_str(), "wb");
    if (!fp)
    {
        log::error("Failed to open file '{}' for writing: {}.", path, Errno::error());
        return false;
    }

    bool ret = true;
    size_t write = fwrite(data, 1, len, fp);
    if (write != len)
    {
        ret = false;
        log::error("Failed to write to file '{}' ({}/{} written): {}.",
            path, write, len, Errno::error());
    }

    fclose(fp);
    return ret;
}

bool FileTools::createDir(
    const std::string &path,
    std::filesystem::perms perms)
{
    std::string fullPath = expandTilde(path);

    try {
        std::filesystem::create_directory(fullPath);
    } catch (const std::exception &e) {
        log::error("Failed to create directory '{}': {}", path, e.what());
        return false;
    }

    try {
        std::filesystem::permissions(fullPath, perms);
    } catch (const std::exception &e) {
        log::error("Failed to set permissions on directory '{}': {}", path, e.what());
        return false;
    }

    return true;
}
