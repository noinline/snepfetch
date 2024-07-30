#include <exception>
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
#include <sys/utsname.h>
#include <unistd.h>

std::string
get_node_name()
{
  struct utsname buffer;
  return (uname(&buffer) != 0) ? "" : std::string(buffer.nodename);
}

std::string
get_kernel_ver()
{
  struct utsname buffer;
  return (uname(&buffer) != 0) ? "" : std::string(buffer.release);
}

std::string
get_user_name()
{
  struct passwd *pw = getpwuid(static_cast<uid_t>(geteuid()));
  return (pw != nullptr) ? std::string(pw->pw_name) : "";
}

std::string
get_home_dir()
{
  struct passwd *pw = getpwuid(static_cast<uid_t>(geteuid()));
  return (pw != nullptr) ? std::string(pw->pw_dir) : "";
}

std::string
get_shell()
{
  struct passwd *pw = getpwuid(static_cast<uid_t>(geteuid()));
  return (pw != nullptr) ? std::string(pw->pw_shell) : "";
}

std::string
get_terminal_emulator()
{
  std::string output = getenv("TERM");
  return output.c_str();
}

std::string
get_file_content(std::istream &s, std::string output)
{
  if (!s.good())
    return "";
  std::string temp{""};
  while (getline(s, temp))
    output += temp + '\n';
  return output.c_str();
}

#include <sys/stat.h>

void
create_directory(std::string dir)
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
  constexpr int xoffset = 2, yoffset = 0;
  get_terminal_size(width, height);
  const std::string command =
      "kitty +kitten icat --align left --place \"" + std::to_string(width) +
      "x" + std::to_string(height) + "@" + std::to_string(xoffset) + "x" +
      std::to_string(yoffset) + "\" \"" + image_path + "\"";

  system("clear");
  system(command.c_str());
}
void
printf_tabbed(std::string str)
{
  printf(" 			%s\n", str.c_str());
}

std::string
print_welcome()
{
  std::string output{"Welcome"};
  output += " ";
  output += get_user_name() + "@" + get_node_name();
  output += "!";
  return output.c_str();
}

std::string
print_terminal()
{
  std::string output{" >"};
  output += " ";
  output += get_terminal_emulator();
  return output.c_str();
}

std::string
print_shell()
{
  std::string output{" >"};
  output += " ";
  output += get_shell();
  return output.c_str();
}

std::string
print_kernel()
{
  std::string output{" >"};
  output += " ";
  output += get_kernel_ver();
  return output.c_str();
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
    system("clear");
    printf("%s\n", output.c_str());
    ascii_text.close();
  } break;
  case 'i': {
    int               width = 0, height = 0;
    const std::string image_path = dir + "chibi.png";
    print_image(width, height, dir);
    printf_tabbed(print_welcome());
    printf_tabbed(print_terminal());
    printf_tabbed(print_kernel());
    printf_tabbed(print_shell());
    printf("\033[%iB\n", 5);
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
