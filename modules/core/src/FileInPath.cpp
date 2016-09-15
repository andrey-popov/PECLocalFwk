#include <mensura/core/FileInPath.hpp>

#include <boost/algorithm/string/predicate.hpp>
#include <boost/filesystem.hpp>

#include <cstdlib>
#include <sstream>
#include <stdexcept>


FileInPath::FileInPath()
{
    // Read the install path from the environment
    char const *path = std::getenv("MENSURA_INSTALL");
    
    if (not path)
        throw std::runtime_error("FileInPath::FileInPath: Mandatory environment variable "
         "MENSURA_INSTALL is not defined.");
    
    
    // Save the data/ subdirectory as a default location. Cannot use method AddLocation here since
    //it will indirectly call the constructor
    std::string installPath(path);
    
    if (not boost::ends_with(installPath, "/"))
        installPath += '/';
    
    locations.emplace_back(installPath + "data/");
}


void FileInPath::AddLocation(std::string path)
{
    if (not boost::ends_with(path, "/"))
        path += '/';
    
    GetInstance().locations.emplace_back(path);
}


std::string FileInPath::Resolve(std::string subDir, std::string const &path)
{
    namespace fs = boost::filesystem;
    
    // Check if an absolute path is provided. In this case the first argument is ignored and the
    //path is returned as is
    if (boost::starts_with(path, "/"))
    {
        // Make sure the requested file exists
        if (not fs::exists(path) or not fs::is_regular(path))
        {
            std::ostringstream message;
            message << "FileInPath::Resolve: Requested file with absolute path \"" <<
              path << "\" does not exist or is not a regular file.";
            throw std::runtime_error(message.str());
        }
        
        return path;
    }
    
    
    // Make sure subDir ends with '/'
    if (subDir != "" and not boost::ends_with(subDir, "/"))
        subDir += '/';
    
    
    // Loop over all possible locations, giving preference to ones added later
    auto const &locations = GetInstance().locations;
    
    for (auto locationIt = locations.rbegin(); locationIt != locations.rend(); ++locationIt)
    {
        std::string tryPath;
        
        // Try to resolve the path using the provided subdirectory
        if (subDir != "")
        {
            tryPath = *locationIt + subDir + path;
            
            if (fs::exists(tryPath) and fs::is_regular(tryPath))
                return tryPath;
        }
        
        // Try to resolve the path ignoring the subdir
        tryPath = *locationIt + path;
        
        if (fs::exists(tryPath) and fs::is_regular(tryPath))
            return tryPath;
    }
    
    
    // If none of the above succeeds, try to resolve the path with respect to the currect working
    //directory, with and without the subdirectory
    if (subDir != "")
    {
        std::string tryPath = subDir + path;
        
        if (fs::exists(tryPath) and fs::is_regular(tryPath))
            return fs::current_path().native() + "/" + tryPath;
    }
    
    if (fs::exists(path) and fs::is_regular(path))
        return fs::current_path().native() + "/" + path;
    
    
    // If the workflow has reached this point, the path has not been resolved
    std::ostringstream message;
    message << "FileInPath::Resolve: Failed to resolve path \"" << path <<
      "\" within (optional) subdirectory \"" << subDir <<
      "\". Following locations have been tried (with and without the subdirectory):\n";
    
    for (auto locationIt = locations.rbegin(); locationIt != locations.rend(); ++locationIt)
        message << "  " << *locationIt << '\n';
    
    message << "  " << fs::current_path().native() << '\n';
    throw std::runtime_error(message.str());
}


std::string FileInPath::Resolve(std::string const &path)
{
    return Resolve("", path);
}


FileInPath &FileInPath::GetInstance()
{
    static FileInPath instance;
    return instance;
}
