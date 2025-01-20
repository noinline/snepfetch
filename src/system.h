#pragma once
#include "filemanager.h"
#include "includes.h"

class System
{
public:
  System(std::shared_ptr<FileManager> fileManager)
      : m_fileManager(std::move(fileManager))
  {}
  std::string getName();
  std::string getNodeName();
  std::string getUsername();
  std::string getShell();
  std::string getKernelVersion();
  std::string getWindowManager();
  std::string getPackageManager();
  std::string getPackageCount();
  std::string getTerminalEmulator();
  void        getTerminalSize(int &width, int &height);
  void        executeCommand(const std::string &command);

private:
  std::shared_ptr<FileManager> m_fileManager{};
};
