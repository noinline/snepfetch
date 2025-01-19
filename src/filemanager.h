#pragma once
#include "includes.h"

class FileManager
{
public:
  std::string getHomeDirectory();
  std::string getFileContent(std::istream &s, std::string output);
  std::string getRootDirectory();
  bool        setRootDirectory(const std::string &dir);
  bool        fileExists(const std::string &dir);

private:
  std::string m_rootDirectory{};
};
