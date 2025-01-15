#include "print.h"

void
Print::tabbed(const std::string &str, int scale, int divider)
{
  const int                tabs = scale / divider;
  std::vector<std::string> temp{};

  for (int i = 0; i < tabs; ++i)
    temp.push_back("\t");
  temp.push_back(str);

  std::string output{};
  for (const auto &it : temp)
    output.append(it);

  printf("%s\n", output.c_str());
}

bool
Print::image(int width, int height, const std::string &imagePath)
{
  if (!m_fileManager->fileExists(imagePath)) {
    fprintf(stderr, "%s does not seem to exist.\n", imagePath.c_str());
    fflush(stderr);
    return false;
  }
  constexpr int     xoffset = 2, yoffset = 0;
  const std::string command =
      "kitty +kitten icat --align left --place \"" + std::to_string(width) +
      "x" + std::to_string(height) + "@" + std::to_string(xoffset) + "x" +
      std::to_string(yoffset) + "\" \"" + imagePath + "\"";

  /* clear everything before calling 'command.c_str()', so we can print our
   * necessary information later*/
  system("clear");
  system(command.c_str());
  return true;
}

std::string
Print::ascii(const std::string &asciiPath)
{
  if (!m_fileManager->fileExists(asciiPath)) {
    fprintf(stderr, "%s does not seem to exist.\n", asciiPath.c_str());
    fflush(stderr);
    return "";
  }
  std::string   output{};
  std::ifstream asciiText{};
  asciiText.open(asciiPath);
  output += m_fileManager->getFileContent(asciiText, output);
  asciiText.close();
  return (!output.empty()) ? output.c_str() : "";
}

std::string
Print::welcome()
{
  std::string output{"Welcome"};
  output += " ";
  output += m_system->getUsername() + "@" + m_system->getNodeName();
  output += "!";
  return output.c_str();
}

std::string
Print::terminal()
{
  std::string output{" >"};
  output += " ";
  output += m_system->getTerminalEmulator();
  return output.c_str();
}

std::string
Print::shell()
{
  std::string output{" >"};
  output += " ";
  output += m_system->getShell();
  return output.c_str();
}

std::string
Print::systemName()
{
  std::string output{" >"};
  output += " ";
  output += m_system->getName();
  return output.c_str();
}


std::string
Print::kernel()
{
  std::string output{" >"};
  output += " ";
  output += m_system->getKernelVersion();
  return output.c_str();
}

std::string
Print::windowManager()
{
  std::string output{" >"};
  output += " ";
  output += m_system->getWindowManager();
  return output.c_str();
}
