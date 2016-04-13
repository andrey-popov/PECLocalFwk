#include <mensura/core/FileInPath.hpp>

#include <boost/algorithm/string/predicate.hpp>
#include <boost/filesystem.hpp>

#include <cstdlib>
#include <stdexcept>


using namespace std;


FileInPath::FileInPath()
{
    // Read the install path from the environment
    char const *path = getenv("PEC_FWK_INSTALL");
    
    if (not path)
        throw runtime_error("FileInPath::FileInPath: Mandatory environment variable "
         "PEC_FWK_INSTALL is not defined.");
    
    
    installPath = path;
    
    if (installPath.length() > 0 and installPath[installPath.length() - 1] != '/')
        installPath += '/';
}


string FileInPath::Resolve(string const &path) const
{
    return Resolve("", path);
}


string FileInPath::Resolve(string const &prefix, string const &path) const
{
    // Check if it is an absolute path
    if (path.length() > 0 and path[0] == '/')
    {
        // Make sure the requested file exists
        if (not boost::filesystem::exists(path))
            throw runtime_error(string("FileInPath::Resolve: ") + "File \"" + path +
             "\" was not found.");
        
        return path;
    }
    
    
    // Make sure the prefix ends up with a '/'
    string canonicalPrefix(prefix);
    
    if (canonicalPrefix.length() > 0 and canonicalPrefix[canonicalPrefix.length() - 1] != '/')
        canonicalPrefix += '/';
    
    
    // Try to resolve the path w.r.t. $PEC_FWK_INSTALL/data/
    string tryPath = installPath + "/data/" + canonicalPrefix + path;
    
    if (boost::filesystem::exists(tryPath))
        return tryPath;
    
    
    // Try to resolve it w.r.t. to the working directory
    if (boost::filesystem::exists(path))
    {
        return boost::filesystem::current_path().native() + path;
    }
    
    
    // If the workflow has reached this point, the path has not been resolved
    throw runtime_error(string("FileInPath: ") + "Cannot resolve path \"" + path + "\".");
}
