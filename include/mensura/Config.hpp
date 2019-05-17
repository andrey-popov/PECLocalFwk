#pragma once

#include <mensura/external/JsonCpp/json.hpp>

#include <filesystem>
#include <initializer_list>
#include <string>


/// A wrapper around Json::Value that simplifies reading of a JSON configuration
class Config
{
public:
    /**
     * \brief Constructs from a path to JSON file
     *
     * The path is resolved using FileInPath. Throws an exception if the parsing of the file fails.
     */
    Config(std::string const &path);

    /// Version of Config(std::string const &) that forwards the given directory to FileInPath
    Config(std::string const &directory, std::string const &path);

    /// Returns resolved path to the configuration file
    std::filesystem::path const &FilePath() const;

    /**
     * \brief Returns node with given path
     *
     * The path is a sequence of names of daughter nodes. Only named nodes (i.e. dictionaries) are
     * supported, whereas arrays are not. If one of the nodes in the path does not exist, an
     * exception is thrown.
     *
     * When no arguments are given, returns the root node.
     */
    Json::Value const &Get(std::initializer_list<std::string> = {}) const;
    
    /**
     * \brief A variant of Get(std::initializer_list<std::string>) that returns the node with the
     * given path with respect to the provided root.
     */
    static Json::Value const &Get(Json::Value const &root, std::initializer_list<std::string> = {});

    /**
     * \brief Deleted overload to flag a subtle error
     *
     * Without this overload, erroneuos code
     *   \code
     *   config.Get("abc");
     *   \endcode
     * compiles fine because the string is converted into Json::Value.
     */
    static Json::Value const &Get(std::string const &,
      std::initializer_list<std::string> = {}) = delete;

private:
    Json::Value root;
    std::filesystem::path resolvedPath;
};

