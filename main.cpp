#include <cassert>
#include <iostream>
#include <sys/ioctl.h>

void
get_terminal_size(int &width, int &height)
{
  struct winsize w;
  ioctl(fileno(stdout), TIOCGWINSZ, &w);
  width = static_cast<int>(w.ws_col);
  height = static_cast<int>(w.ws_row);
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
  /* returns user's hostname */
}

std::string
get_kernel_ver()
{
  struct utsname buffer;
  return (uname(&buffer) != 0) ? "" : std::string(buffer.release);
  /* returns kernel release version */
}

std::string
get_user_name()
{
  struct passwd *pw = getpwuid(static_cast<uid_t>(geteuid()));
  return (pw != nullptr) ? std::string(pw->pw_name) : "";
  /* returns username */
}

std::string
get_home_dir()
{
  struct passwd *pw = getpwuid(static_cast<uid_t>(geteuid()));
  return (pw != nullptr) ? std::string(pw->pw_dir) : "";
  /* returns home directory */
}

std::string
get_shell()
{
  struct passwd *pw = getpwuid(static_cast<uid_t>(geteuid()));
  return (pw != nullptr) ? std::string(pw->pw_shell) : "";
  /* returns user's shell */
}

std::string
get_terminal_emulator()
{
  const std::string output = getenv("TERM");
  return output.c_str();
  /* returns terminal */
}

std::string
get_file_content(std::istream &s, std::string output)
{
  if (!s.good())
    return "";
  std::string temp{};
  while (getline(s, temp))
    output += temp + '\n';
  return output.c_str();
}

#include <regex>

std::string
get_system_name()
{
  std::string name_cmd = "lsb_release -a";

  FILE *name_pipe = popen(name_cmd.c_str(), "r");
  if (!name_pipe)
    return "";

  std::string name_output;
  while (!feof(name_pipe)) {
    char buffer[128];
    if (fgets(buffer, sizeof(buffer), name_pipe) != NULL)
      name_output += buffer;
  }
  pclose(name_pipe);

  std::regex system_name_pattern(
      "Distributor ID:\\s*(\\w+)"); /* pattern that searches for any
                                       whitespaces/words after 'Distributor
                                       ID'*/
  std::smatch matches;
  if (std::regex_search(name_output, matches, system_name_pattern))
    return matches.str(1);

  return "unknown";
}

std::string
get_window_manager()
{
  std::string check_cmd = "xprop -root _NET_SUPPORTING_WM_CHECK";
  std::string name_cmd = "xprop -id $(xprop -root _NET_SUPPORTING_WM_CHECK | "
                         "awk '{print $NF}') _NET_WM_NAME";

  FILE *check_pipe = popen(check_cmd.c_str(), "r");
  if (!check_pipe)
    return "";

  std::string check_output;
  while (!feof(check_pipe)) {
    char buffer[128];
    if (fgets(buffer, sizeof(buffer), check_pipe) != NULL)
      check_output += buffer;
  }
  pclose(check_pipe);

  std::regex window_id_pattern(
      "_NET_SUPPORTING_WM_CHECK\\(WINDOW\\): window id \\# "
      "0x([a-fA-F0-9]+)"); /* pattern that searches for any lower/upper case
                              words and digits after 'window id #' */
  std::smatch matches;
  if (std::regex_search(check_output, matches, window_id_pattern)) {
    std::string window_id_pattern = matches.str(1);

    FILE *name_pipe = popen(name_cmd.c_str(), "r");
    if (!name_pipe)
      return "";

    std::string name_output;
    while (!feof(name_pipe)) {
      char buffer[128];
      if (fgets(buffer, sizeof(buffer), name_pipe) != NULL)
        name_output += buffer;
    }
    pclose(name_pipe);

    std::regex name_pattern(
        "_NET_WM_NAME\\(UTF8_STRING\\) = \"([^\"]+)\""); /* pattern that matches
                                                            everything within
                                                            '""' after = sign */
    if (std::regex_search(name_output, matches, name_pattern))
      return matches.str(1);
  }

  return "unknown";
}

#include <sys/stat.h>

void
create_directory(const std::string &dir)
{
  struct stat st = {};
  /* 'stat' used to check if directory already exists, if it's not then we
   * proceed to mkdir*/
  if (stat(dir.c_str(), &st) != -1)
    return;

  /* 'mkdir' if directory isn't initialized yet */
  int result = mkdir(dir.c_str(), 0700);
  if (result == -1)
    return;
}

void
print_image(int width, int height, const std::string &image_path)
{
  constexpr int     xoffset = 2, yoffset = 0;
  const std::string command =
      "kitty +kitten icat --align left --place \"" + std::to_string(width) +
      "x" + std::to_string(height) + "@" + std::to_string(xoffset) + "x" +
      std::to_string(yoffset) + "\" \"" + image_path + "\"";

  /* clear everything before calling 'command.c_str()', so we can print our
   * necessary information later*/
  system("clear");
  system(command.c_str());
}

#include <cstring>

void
printf_tabbed(const std::string &str, int scale)
{
  assert(scale >= 0);

  int tabs{};

  tabs = scale / 25;

  std::string output{};

  for (int i = 0; i < tabs; ++i)
    output += "\t";

  output += str;

  printf("%s\n", output.c_str());
}

void
ascii_printf_tabbed(const std::string &str)
{
  printf("\t\t\t\t%s\n", str.c_str());
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
print_system_name()
{
  std::string output{" >"};
  output += " ";
  output += get_system_name();
  return output.c_str();
}

std::string
print_window_manager()
{
  std::string output{" >"};
  output += " ";
  output += get_window_manager();
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
  if (isatty(STDOUT_FILENO) != 1) {
    fprintf(stderr, "stdout is not a terminal!\n");
    fflush(stderr);
    return 1;
  }

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
    ascii_printf_tabbed(print_welcome());
    ascii_printf_tabbed(print_system_name());
    ascii_printf_tabbed(print_window_manager());
    ascii_printf_tabbed(print_terminal());
    ascii_printf_tabbed(print_kernel());
    ascii_printf_tabbed(print_shell());
    ascii_text.close();
  } break;
  case 'i': {
    int               width = 0, height = 0;
    const std::string image_path = dir + "image.png";
    get_terminal_size(width, height);
    print_image(width, height, dir);
    printf_tabbed(print_welcome(), width);
    printf_tabbed(print_system_name(), width);
    printf_tabbed(print_window_manager(), width);
    printf_tabbed(print_terminal(), width);
    printf_tabbed(print_kernel(), width);
    printf_tabbed(print_shell(), width);
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
