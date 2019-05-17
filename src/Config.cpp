#include <mensura/Config.hpp>

#include <mensura/FileInPath.hpp>

#include <fstream>
#include <sstream>
#include <stdexcept>


Config::Config(std::string const &path):
    Config("", path)
{}


Config::Config(std::string const &directory, std::string const &path)
{
    resolvedPath = FileInPath::Resolve(directory, path);
    std::ifstream dbFile{resolvedPath, std::ifstream::binary};
    
    try
    {
        dbFile >> root;
    }
    catch (Json::Exception const &)
    {
        std::ostringstream message;
        message << "Failed to parse file " << resolvedPath << " as JSON.";
        // There is no need to include the exact error message from the source exception as it is
        // printed automatically.
        throw std::runtime_error(message.str());
    }

    dbFile.close();
}


std::filesystem::path const &Config::FilePath() const
{
    return resolvedPath;
}


Json::Value const &Config::Get(std::initializer_list<std::string> keys) const
{
    return Get(root, keys);
}


Json::Value const &Config::Get(Json::Value const &root, std::initializer_list<std::string> keys)
{
    Json::Value const *node = &root;
    int curIndex = 0;

    for (auto const &key: keys)
    {
        if (not node->isMember(key))
        {
            std::ostringstream message;
            message << "Node with path ";
            int i = 0;

            for (auto const &key: keys)
            {
                message << "[\"" << key << "\"]";
                ++i;

                if (i > curIndex)
                    break;
            }

            message << " is not found.";
            throw std::runtime_error(message.str());
        }

        node = &(*node)[key];
        ++curIndex;
    }

    return *node;
}

