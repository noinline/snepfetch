#include "filemanager.h"

std::string
FileManager::getHomeDirectory(void)
{
  struct passwd *pwd = getpwuid(static_cast<uid_t>(geteuid()));
  return (pwd != nullptr)
             ? std::string(pwd->pw_dir)
             : throw std::runtime_error("failed to get home directory.\n");
}

std::string
FileManager::getFileContent(std::istream &s, std::string output)
{
  if (!s.good())
    return "";
  std::string temp{};
  while (getline(s, temp))
    output.append(temp + '\n');
  return output.c_str();
}

std::string
FileManager::getRootDirectory()
{
  return m_rootDirectory;
}

bool
FileManager::setRootDirectory(const std::string &dir)
{
  /* this-> looks ugly too but it's ok */
  struct stat st
  {};
  m_rootDirectory += dir.c_str();
  if (stat(dir.c_str(), &st) == -1) {
    if (mkdir(dir.c_str(), 0700) == -1)
      return false;
    return true;
  }
  return false;
}

bool
FileManager::fileExists(const std::string &dir)
{
  struct stat st
  {};
  /* 'lstat' used to check if file exists */
  return (lstat(dir.c_str(), &st) != -1) ? true : false;
}
