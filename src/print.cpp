#include "print.h"

void
Print::tabbed(const std::string &str, int scale, int divider)
{
  int                      tabs = scale / divider;
  std::vector<std::string> temp{};

  if (scale > 98)
    tabs -= 1;
  else if (scale <= 49)
    tabs += 2;
  else if (scale >= 49)
    tabs += 1;

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
  if (!m_fileManager->fileExists(imagePath + "image.png"))
    throw std::runtime_error("image.png does not seem to exist.\n");
  else if (width < 46)
    throw std::runtime_error("not enough space to correctly output image.png");

  const std::string picture = imagePath + "image.png";
  const std::string pictureFinal = imagePath + "image_temp.png";
  const std::string magick =
      "magick " + picture + " -resize 340x640 " + pictureFinal;
  const std::string tempClean = "rm " + pictureFinal;
  constexpr int     offsetX = 2, offsetY = 0;
  const std::string command =
      "kitty +kitten icat --align left --place \"" + std::to_string(width) +
      "x" + std::to_string(height) + "@" + std::to_string(offsetX) + "x" +
      std::to_string(offsetY) + "\" \"" + pictureFinal + "\"";

  /* clear everything before calling 'command', so we can print our
   * necessary information later*/
  m_system->executeCommand("clear");

  /* create a temp image */
  if (!m_fileManager->fileExists(pictureFinal))
    m_system->executeCommand(magick);

  /* call main 'command' */
  m_system->executeCommand(command);

  /* cleanup 'magick' stuff */
  m_system->executeCommand(tempClean);
  return true;
}

std::string
Print::ascii(const std::string &asciiPath)
{
  if (!m_fileManager->fileExists(asciiPath))
    throw std::runtime_error("ascii.txt does not seem to exist.\n");

  std::string   output{};
  std::ifstream asciiText{};
  asciiText.open(asciiPath);
  output += m_fileManager->getFileContent(asciiText, output);
  asciiText.close();
  return (!output.empty()) ? output.c_str() : "";
}

std::string
Print::welcome(void)
{
  std::string output{"Welcome"};
  output += " ";
  output += m_system->getUsername() + "@" + m_system->getNodeName();
  output += "!";
  return output.c_str();
}

std::string
Print::terminal(void)
{
  std::string output{" >"};
  output += " ";
  output += m_system->getTerminalEmulator();
  return output.c_str();
}

std::string
Print::shell(void)
{
  std::string output{" >"};
  output += " ";
  output += m_system->getShell();
  return output.c_str();
}

std::string
Print::systemName(void)
{
  std::string output{" >"};
  output += " ";
  output += m_system->getName();
  return output.c_str();
}

std::string
Print::kernel(void)
{
  std::string output{" >"};
  output += " ";
  output += m_system->getKernelVersion();
  return output.c_str();
}

std::string
Print::windowManager(void)
{
  std::string output{" >"};
  output += " ";
  output += m_system->getWindowManager();
  return output.c_str();
}

std::string
Print::packageManager(void)
{
  std::string output{" >"};
  output += " ";
  output += m_system->getPackageManager();
  output += " (";
  output += m_system->getPackageCount();
  output += ")";
  return output.c_str();
}
