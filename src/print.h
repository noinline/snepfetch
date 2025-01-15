#include "includes.h"

class Print
{
  public:
  Print(std::shared_ptr<FileManager> fileManager,
        std::shared_ptr<System>      system)
      : m_fileManager(std::move(fileManager)), m_system(std::move(system))
  {}
  void        tabbed(const std::string &str, int scale, int divider);
  bool        image(int width, int height, const std::string &imagePath);
  std::string ascii(const std::string &asciiPath);
  std::string welcome();
  std::string terminal();
  std::string systemName();
  std::string shell();
  std::string kernel();
  std::string windowManager();

private:
  std::shared_ptr<FileManager> m_fileManager{};
  std::shared_ptr<System>      m_system{};
};
