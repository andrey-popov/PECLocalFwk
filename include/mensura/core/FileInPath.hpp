/**
 * \file FileInPath.hpp
 * 
 * Module defines tools to resolve relative file paths w.r.t. standard locations of the package.
 */

#pragma once

#include <string>


/**
 * \class FileInPath
 * \brief Allows to resolve a (possibly) relative file path w.r.t. to several possible locations
 * 
 * A relative path is first resolved w.r.t. to $MENSURA_INSTALL/data/, then w.r.t. the current
 * directory. Supports also an absolute path.
 */
class FileInPath
{
public:
    /**
     * \brief Constructor
     * 
     * Reads the values of the MENSURA_INSTALL environmental variable and stores it in
     * intallPath. If the variable is not set, an exception is thrown.
     */
    FileInPath();

public:
    /**
     * \brief Resolves the file path
     * 
     * If an absolute path is given, the method returns it unchanged after verifying that such
     * file exists. If the path is not absolute, the method tries to resolve it first w.r.t.
     * $MENSURA_INSTALL/data/ then w.r.t. the current directory. If the path is resolved, which
     * means it points to an existing file, an equivalent absolute path is returned. Otherwise
     * an exception is thrown.
     */
    std::string Resolve(std::string const &path) const;
    
    /**
     * \brief Resolves the file path
     * 
     * Does precisely the same as Resolve(string const &), but tries to resolve a relative path
     * w.r.t. $MENSURA_INSTALL/data/$prefix/ instead of $MENSURA_INSTALL/data/.
     */
    std::string Resolve(std::string const &prefix, std::string const &path) const;

private:
    /**
     * \brief Path in which the framework is intalled
     * 
     * Read from the environmental variable MENSURA_INSTALL. The path stored in this variable
     * terminates with '/'.
     */
    std::string installPath;
};
