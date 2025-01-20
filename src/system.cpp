#include "system.h"

std::string
System::getName(void)
{
  const std::string name = "cat /etc/os-release";

  FILE *namePipe = popen(name.c_str(), "r");
  if (!namePipe)
    return "";

  std::string output{};
  while (!feof(namePipe)) {
    char buffer[128];
    if (fgets(buffer, sizeof(buffer), namePipe) != NULL)
      output += buffer;
  }
  pclose(namePipe);

  const std::regex namePattern(
      "PRETTY_NAME=\"(.*?)\""); /* pattern that searches for any words in "" */
  std::smatch matches{};

  return (std::regex_search(output, matches, namePattern) == true)
             ? matches.str(1)
             : "unknown";
}

std::string
System::getNodeName(void)
{
  struct utsname buffer;
  return (uname(&buffer) != 0)
             ? throw std::runtime_error("failed to get nodename.\n")
             : std::string(buffer.nodename);
  /* returns user's hostname */
}

std::string
System::getUsername(void)
{
  struct passwd *pw = getpwuid(static_cast<uid_t>(geteuid()));
  return (pw != nullptr)
             ? std::string(pw->pw_name)
             : throw std::runtime_error("failed to get username.\n");
  ;
  /* returns username */
}

std::string
System::getShell(void)
{
  struct passwd *pw = getpwuid(static_cast<uid_t>(geteuid()));
  return (pw != nullptr) ? std::string(pw->pw_shell)
                         : throw std::runtime_error("failed to get shell.\n");
  /* returns user's shell */
}

std::string
System::getKernelVersion(void)
{
  struct utsname buffer;
  return (uname(&buffer) != 0)
             ? throw std::runtime_error("failed to get kernel version.\n")
             : std::string(buffer.release);
  /* returns kernel release version */
}

std::string
System::getWindowManager(void)
{
  const std::string check = "xprop -root _NET_SUPPORTING_WM_CHECK";
  const std::string name = "xprop -id $(xprop -root _NET_SUPPORTING_WM_CHECK | "
                           "awk '{print $NF}') _NET_WM_NAME";

  FILE *checkPipe = popen(check.c_str(), "r");
  if (!checkPipe)
    return "";

  std::string checkOutput{};
  while (!feof(checkPipe)) {
    char buffer[128];
    if (fgets(buffer, sizeof(buffer), checkPipe) != NULL)
      checkOutput += buffer;
  }
  pclose(checkPipe);

  const std::regex windowPattern(
      "_NET_SUPPORTING_WM_CHECK\\(WINDOW\\): window id \\# "
      "0x([a-fA-F0-9]+)"); /* pattern that searches for any lower/upper case
                              words and digits after 'window id #' */
  std::smatch matches{};
  if (std::regex_search(checkOutput, matches, windowPattern)) {
    std::string windowPattern = matches.str(1); /* not an std::regex variable */

    FILE *namePipe = popen(name.c_str(), "r");
    if (!namePipe)
      return "";

    std::string nameOutput{};
    while (!feof(namePipe)) {
      char buffer[128];
      if (fgets(buffer, sizeof(buffer), namePipe) != NULL)
        nameOutput += buffer;
    }
    pclose(namePipe);

    const std::regex namePattern(
        "_NET_WM_NAME\\(UTF8_STRING\\) = \"([^\"]+)\""); /* pattern that matches
                                                            everything within
                                                            '""' after = sign */
    if (std::regex_search(nameOutput, matches, namePattern))
      return matches.str(1);
  }

  return "unknown";
}

std::string
System::getPackageManager(void)
{
  const char *packageManagers[] = {
      "/usr/bin/apt",    "/usr/bin/apk",  "/usr/bin/qlist",
      "/usr/bin/snap",   "/usr/bin/guix", "/usr/bin/nix-store",
      "/usr/bin/pacman", "/usr/bin/pkg",  "/usr/bin/port",
      "/usr/bin/brew",   "/usr/bin/rpm",  "/usr/bin/xbps-query"};

  const char *names[] = {"apt",       "apk",    "emerge", "snap", "guix",
                         "nix",       "pacman", "pkg",    "port", "brew-cellar",
                         "brew-cask", "rpm",    "xbps"};

  std::string output{};
  std::string packageManager{};
  for (size_t i = 0; i < sizeof(packageManagers) / sizeof(packageManagers[0]);
       ++i)
  {
    if (!m_fileManager->fileExists(packageManagers[i]))
      continue;

    packageManager += packageManagers[i];

    if (packageManager.find(names[i]) != std::string::npos)
      output += names[i];
  }

  return (!output.empty()) ? output.c_str() : "unknown";
}

std::string
System::getPackageCount(void)
{
  const char *listCommands[] = {
      "apt list --installed | wc -l\n",
      "apk info | wc -l\n",
      "qlist -I | wc -l\n",
      "flatpak list | wc -l\n",
      "snap list | wc -l\n",
      "guix package --list-installed | wc -l\n",
      "nix-store -q --requisites /run/current-system/sw | wc -l\n",
      "pacman -Qq | wc -l\n",
      "pkg info | wc -l\n",
      "port installed | tail +2 | wc -l\n",
      "brew --cellar | wc -l\n",
      "brew --caskroom | wc -l\n",
      "rpm -qa --last | wc -l\n",
      "xbps-query -l | wc -l\n"};

  const std::string packageManager = getPackageManager();
  std::string tempCommand{}, command{}, output{}, temp{};
  for (size_t i = 0; i < sizeof(listCommands) / sizeof(listCommands[0]); ++i) {
    tempCommand += listCommands[i];

    if (tempCommand.find(packageManager) != std::string::npos)
    {
      command += listCommands[i];
      break;
    }
    else
      continue;
  }

  FILE *pipe = popen(command.c_str(), "r");
  if (!pipe)
    return "";

  while (!feof(pipe)) {
    char buffer[128];
    if (fgets(buffer, sizeof(buffer), pipe) != NULL)
      temp += buffer;
  }
  pclose(pipe);

    output += std::to_string(std::stoi(temp));
  return (!output.empty()) ? output.c_str() : "unknown";
}

std::string
System::getTerminalEmulator(void)
{
  const std::string output = getenv("TERM");
  return (!output.empty())
             ? output.c_str()
             : throw std::runtime_error("failed to get terminal emulator.\n");
  /* returns terminal */
}

void
System::getTerminalSize(int &width, int &height)
{
  return [&width, &height]() {
    struct winsize window;
    ioctl(fileno(stdout), TIOCGWINSZ, &window);
    if (window.ws_col == 0 && window.ws_row == 0)
      throw std::runtime_error("failed to get terminal size.\n");
    width = static_cast<int>(window.ws_col);
    height = static_cast<int>(window.ws_row);
  }();
}

void
System::executeCommand(const std::string &command)
{
  return [&command]() {
    try {
      int exitStatus = system(command.c_str());
      if (exitStatus != 0)
        throw std::runtime_error(
            command + " failed with exit code: " + std::to_string(exitStatus));
    } catch (const std::exception &e) {
      std::runtime_error(e.what());
    }
  }();
}
