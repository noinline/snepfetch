#include <fstream>
#include <pwd.h>
#include <regex>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/utsname.h>

class FileManager
{
  public:
  std::string getHomeDirectory(void);
  std::string getFileContent(std::istream &s, std::string output);
  bool setDirectory(const std::string &dir);
  bool fileExists(const std::string &dir);
};
