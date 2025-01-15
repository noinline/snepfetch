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
             ? throw std::runtime_error("failed to get node name.\n")
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
