#include "print.h"

#include <getopt.h>

int
main(int argc, char *argv[])
{
  if (isatty(STDOUT_FILENO) != 1) {
    throw std::runtime_error("stdout is not a terminal!\n");
    /* skull_emoji */
EXIT:
    return 1;
  }

  std::shared_ptr<FileManager> fileManager = std::make_shared<FileManager>();
  std::unique_ptr<System>      system =
      std::unique_ptr<System>(new System(std::move(fileManager)));
  fileManager = std::shared_ptr<FileManager>(new FileManager());
  std::unique_ptr<Print> print = std::unique_ptr<Print>(
      new Print(std::move(fileManager), std::move(system)));
  fileManager = std::shared_ptr<FileManager>(new FileManager());
  system = std::unique_ptr<System>(new System(std::move(fileManager)));
  fileManager = std::shared_ptr<FileManager>(new FileManager());
  /* this is so stupid ^^^^^^^^ sob_emoji */

  const std::string dir =
      fileManager->getHomeDirectory() + "/.config/snepfetch/";
  fileManager->setRootDirectory(dir);
  if (fileManager->getRootDirectory().empty())
    throw std::runtime_error("failed to set root directory. aborting...");

  static struct option long_options[] = {
      {"ascii", no_argument, 0, 'a'},
      {"image", no_argument, 0, 'i'},
      {"help",  no_argument, 0, 'h'},
      {0,       0,           0, 0  }
  };
  int option_index = 0;
  int c = getopt_long(argc, argv, "aih", long_options, &option_index);
  switch (c) {
  case 'a': {
    const std::string asciiPath = fileManager->getRootDirectory();
    int               width = 0, height = 0;
    system->getTerminalSize(width, height);
    std::system("clear");
    if (!print->ascii(asciiPath + "ascii.txt").empty())
      printf("%s\n", print->ascii(asciiPath + "ascii.txt").c_str());
    else
      goto EXIT;
    print->tabbed(print->welcome(), width, 20);
    print->tabbed(print->systemName(), width, 20);
    print->tabbed(print->windowManager(), width, 20);
    print->tabbed(print->terminal(), width, 20);
    print->tabbed(print->kernel(), width, 20);
    print->tabbed(print->shell(), width, 20);
  } break;
  case 'i': {
    int               width = 0, height = 0;
    const std::string imagePath = fileManager->getRootDirectory();
    system->getTerminalSize(width, height);
    if (print->image(width, height, imagePath) != true)
      goto EXIT;
    print->tabbed(print->welcome(), width, 25);
    print->tabbed(print->systemName(), width, 25);
    print->tabbed(print->windowManager(), width, 25);
    print->tabbed(print->packageManager(), width, 25);
    print->tabbed(print->terminal(), width, 25);
    print->tabbed(print->kernel(), width, 25);
    print->tabbed(print->shell(), width, 25);
    printf("\033[%iB\n", 1);
  } break;
  case 'h':
    printf("Usage:\n	"
           "%s --ascii - prints info along with a neat ascii art.\n	"
           "%s --image - prints info same as --ascii but uses an image instead "
           "of ascii art.\n	"
           "%s --help - prints this message.\n",
           argv[0], argv[0], argv[0]);
    break;
  default: {
    fprintf(stderr,
            "invalid/no argument specified! consider using %s --help.\n",
            argv[0]);
    fflush(stderr);
  } break;
  }
  return 0;
}
