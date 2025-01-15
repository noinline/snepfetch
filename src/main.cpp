#include <iostream>
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
  std::shared_ptr<System> system = std::make_shared<System>();
    std::unique_ptr<Print> print = std::unique_ptr<Print>(
        new Print(std::move(fileManager), std::move(system)));
    fileManager = std::shared_ptr<FileManager>(new FileManager());
    system = std::shared_ptr<System>(new System());
  const std::string dir = fileManager->getHomeDirectory() + "/.config/snepfetch/";
  fileManager->setDirectory(dir);

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
    const std::string asciiPath = dir + "ascii.txt";
    int               width = 0, height = 0;
    system->getTerminalSize(width, height);
    std::system("clear");
    if (!print->ascii(asciiPath).empty())
      printf("%s\n", print->ascii(asciiPath).c_str());
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
    const std::string image_path = dir + "image.png";
    system->getTerminalSize(width, height);
    if (print->image(width, height, image_path) != true)
      goto EXIT;
    print->tabbed(print->welcome(), width, 25);
    print->tabbed(print->systemName(), width, 25);
    print->tabbed(print->windowManager(), width, 25);
    print->tabbed(print->terminal(), width, 25);
    print->tabbed(print->kernel(), width, 25);
    print->tabbed(print->shell(), width, 25);
    printf("\033[%iB\n", 1);
  } break;
  case 'h':
    printf("Usage:\n	"
           "%s --ascii - prints info along with a neat ascii art.\n	"
           "%s --image - same as --ascii but uses image.\n",
           argv[0], argv[0]);
    break;
  default: {
    fprintf(stderr,
            "Invalid/No argument specified! Consider using %s --help.\n",
            argv[0]);
    fflush(stderr);
  } break;
  }
  return 0;
}
