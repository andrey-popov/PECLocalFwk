#pragma once

#include <string>
#include <vector>


/**
 * \class FileInPath
 * \brief Allows to resolve a (possibly) relative file path w.r.t. to several possible locations
 * 
 * One location, $MENSURA_INSTALL/data/, is included by default. User can add other locations if
 * needed.
 * 
 * This class is a singleton, and thus user cannot constract an instance of it. Instead, all
 * functionality is implemented in static methods.
 */
class FileInPath
{
public:
    /// Copy constructor is distabled because this is a singleton
    FileInPath(FileInPath const &) = delete;
    
    /// Assignment operator is disabled because this is a singleton
    FileInPath &operator=(FileInPath const &) = delete;
    
private:
    /**
     * \brief Constructor
     * 
     * The constructor is private because the class is a singletop. It reads value of environmental
     * variable MENSURA_INSTALL and saves $MENSURA_INSTALL/data/ as the first location. If the
     * variable is not set, an exception is thrown.
     */
    FileInPath();

public:
    /**
     * \brief Adds a new location in which files with be searched
     * 
     * The new location takes preference over all paths added previously. This method is not
     * guaranteed to be thread-safe.
     */
    static void AddLocation(std::string path);
    
    /**
     * \brief Resolves a path, allowing for an optional subdirectory
     * 
     * If the path starts with '/', it is treated as an absolute path and returned unchanched after
     * verifying that such file exists. If the path is not absolute, method tries to resolve it
     * with respect to all defined locations, in a reversed order of their definition. For each
     * location, provided subdirectory is first added to it, and the resolution is attempted. If
     * such file is not found, the subdirectory is omitted, and the resolution is attempted again.
     * Finally, the file is searched for in the current working directory (the one in which the
     * executable is being run), with and without the subdirectory. If all attempts to find the
     * file fail, an exception is thrown.
     */
    static std::string Resolve(std::string subDir, std::string const &path);
    
    /**
     * \brief Resolves a path
     * 
     * Works in the same way as the above version but does not include the additional subdirectory.
     */
    static std::string Resolve(std::string const &path);
    
private:
    /// Returns the only instance of this singleton
    static FileInPath &GetInstance();

private:
    /**
     * \brief Locations with respect to which paths are resolved
     * 
     * All paths stored in this collection terminate with '/'.
     */
    std::vector<std::string> locations;
};
