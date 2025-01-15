#include <pwd.h>
#include <regex>
#include <string>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/utsname.h>
#include <unistd.h>

class System
{
public:
  std::string getName();
  std::string getNodeName();
  std::string getUsername();
  std::string getShell();
  std::string getKernelVersion();
  std::string getWindowManager();
  std::string getTerminalEmulator();
  void        getTerminalSize(int &width, int &height);
};
