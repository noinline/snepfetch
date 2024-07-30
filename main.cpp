#include <iostream>
#include <sys/ioctl.h>

void
get_terminal_size(int &width, int &height)
{
  struct winsize w;
  ioctl(fileno(stdout), TIOCGWINSZ, &w);
  width = (int) (w.ws_col);
  height = (int) (w.ws_row);
}

#include <fstream>
#include <pwd.h>
#include <unistd.h>

std::string
get_user_name()
{
  struct passwd *pw = getpwuid((uid_t) geteuid());
  return (pw != nullptr) ? std::string(pw->pw_name) : "";
}

std::string
get_home_dir()
{
struct passwd *pw = getpwuid((uid_t)geteuid());
return (pw != nullptr) ? std::string(pw->pw_dir) : "";
}

#include <vector>

std::string
get_file_content(std::istream &s, std::string output)
{
  if (!s.good())
    return "";

  std::vector<std::string> line;
  while (s >> output)
    line.push_back(output);

  return output.c_str();
}

#include <sys/stat.h>

void create_directory(std::string dir)
{
    struct stat st = {0};
    if (stat(dir.c_str(), &st) != -1) 
        return;

    int result = mkdir(dir.c_str(), 0700); 
    if (result == -1)
        return;
}

void
print_image(int width, int height, std::string image_path)
{
  constexpr int xoffset = 0, yoffset = 0;
  get_terminal_size(width, height);
  const std::string command =
      "kitty +kitten icat --align left --place \"" + std::to_string(width) +
      "x" + std::to_string(height) + "@" + std::to_string(xoffset) + "x" +
      std::to_string(yoffset) + "\" \"" + image_path + "\"";

  system("clear");
  system(command.c_str());
}

#include <getopt.h>

int
main(int argc, char *argv[])
{ 
  const std::string dir = get_home_dir() + "/.config/snepfetch/";
  create_directory(dir);

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
    std::ifstream ascii_text;
    std::string   output;
    ascii_text.open(dir + "ascii.txt");
    output = get_file_content(ascii_text, output);
    printf("%s", output.c_str());
    ascii_text.close();
  } break;
  case 'i': {
    int width = 0, height = 0;
    const std::string image_path = dir + "chibi.png";
    print_image(width, height, dir);
    printf("\033[%iB", 8);
  } break;
  case 'h':
    printf("Usage:\n	"
           "%s --ascii\n",
           argv[0]);
    break;
  default:
    printf("Invalid command! Consider using %s --help.\n", argv[0]);
    break;
  }
  return 0;
}
