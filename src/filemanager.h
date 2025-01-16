#include <fstream>
#include <pwd.h>
#include <regex>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/utsname.h>
#include <unistd.h>

class FileManager
{
public:
  std::string       getHomeDirectory(void);
  std::string       getFileContent(std::istream &s, std::string output);
  bool              setRootDirectory(const std::string &dir);
  const std::string getRootDirectory();
  bool              fileExists(const std::string &dir);

private:
  std::string m_rootDirectory{};
};
