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
    output.append(temp + '\n');
  return output.c_str();
}

#include <regex>

std::string
get_system_name()
{
  const std::string name_cmd = "cat /etc/os-release";

  FILE *name_pipe = popen(name_cmd.c_str(), "r");
  if (!name_pipe)
    return "";

  std::string name_output{};
  while (!feof(name_pipe)) {
    char buffer[128];
    if (fgets(buffer, sizeof(buffer), name_pipe) != NULL)
      name_output += buffer;
  }
  pclose(name_pipe);

  const std::regex system_name_pattern(
      "PRETTY_NAME=\"(.*?)\""); /* pattern that searches for any words in "" */
  std::smatch matches{};

  return (std::regex_search(name_output, matches, system_name_pattern) == true)
             ? matches.str(1)
             : "unknown";
}

std::string
get_window_manager()
{
  const std::string check_cmd = "xprop -root _NET_SUPPORTING_WM_CHECK";
  const std::string name_cmd =
      "xprop -id $(xprop -root _NET_SUPPORTING_WM_CHECK | "
      "awk '{print $NF}') _NET_WM_NAME";

  FILE *check_pipe = popen(check_cmd.c_str(), "r");
  if (!check_pipe)
    return "";

  std::string check_output{};
  while (!feof(check_pipe)) {
    char buffer[128];
    if (fgets(buffer, sizeof(buffer), check_pipe) != NULL)
      check_output += buffer;
  }
  pclose(check_pipe);

  const std::regex window_id_pattern(
      "_NET_SUPPORTING_WM_CHECK\\(WINDOW\\): window id \\# "
      "0x([a-fA-F0-9]+)"); /* pattern that searches for any lower/upper case
                              words and digits after 'window id #' */
  std::smatch matches{};
  if (std::regex_search(check_output, matches, window_id_pattern)) {
    std::string window_id_pattern = matches.str(1);

    FILE *name_pipe = popen(name_cmd.c_str(), "r");
    if (!name_pipe)
      return "";

    std::string name_output{};
    while (!feof(name_pipe)) {
      char buffer[128];
      if (fgets(buffer, sizeof(buffer), name_pipe) != NULL)
        name_output += buffer;
    }
    pclose(name_pipe);

    const std::regex name_pattern(
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
  struct stat st
  {};
  /* 'stat' used to check if directory already exists, if it's not then we
   * proceed to mkdir*/
  if (stat(dir.c_str(), &st) != -1)
    return;

  /* 'mkdir' if directory isn't initialized yet */
  int result = mkdir(dir.c_str(), 0700);
  if (result == -1)
    return;
}

bool
file_exists(const std::string &dir)
{
  struct stat st
  {};
  /* in that 'lstat' used to check if file exists */
  return (lstat(dir.c_str(), &st) != -1) ? true : false;
}

bool
print_image(int width, int height, const std::string &image_path)
{
  if (!file_exists(image_path)) {
    fprintf(stderr, "%s does not seem to exist.\n", image_path.c_str());
    fflush(stderr);
    return false;
  }
  constexpr int     xoffset = 2, yoffset = 0;
  const std::string command =
      "kitty +kitten icat --align left --place \"" + std::to_string(width) +
      "x" + std::to_string(height) + "@" + std::to_string(xoffset) + "x" +
      std::to_string(yoffset) + "\" \"" + image_path + "\"";

  /* clear everything before calling 'command.c_str()', so we can print our
   * necessary information later*/
  system("clear");
  system(command.c_str());
  return true;
}

std::string
print_ascii(const std::string &ascii_path)
{
  if (!file_exists(ascii_path)) {
    fprintf(stderr, "%s does not seem to exist.\n", ascii_path.c_str());
    fflush(stderr);
    return "";
  }
  std::string   output{};
  std::ifstream ascii_text{};
  ascii_text.open(ascii_path);
  output = get_file_content(ascii_text, output);
  ascii_text.close();
  return output.c_str();
}

void
printf_tabbed(const std::string &str, int scale, int divider)
{
  assert(scale >= 0);

  const int                tabs = scale / divider;
  std::vector<std::string> temp{};

  for (int i = 0; i < tabs; ++i)
    temp.push_back("\t");
  temp.push_back(str);

  std::string output{};
  for (const auto &it : temp)
    output.append(it);

  printf("%s\n", output.c_str());
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
EXIT:
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
    const std::string ascii_path = dir + "ascii.txt";
    int               width = 0, height = 0;
    get_terminal_size(width, height);
    system("clear");
    if (!print_ascii(ascii_path).empty())
      printf("%s\n", print_ascii(ascii_path).c_str());
    else
      goto EXIT;
    printf_tabbed(print_welcome(), width, 20);
    printf_tabbed(print_system_name(), width, 20);
    printf_tabbed(print_window_manager(), width, 20);
    printf_tabbed(print_terminal(), width, 20);
    printf_tabbed(print_kernel(), width, 20);
    printf_tabbed(print_shell(), width, 20);
  } break;
  case 'i': {
    int               width = 0, height = 0;
    const std::string image_path = dir + "image.png";
    get_terminal_size(width, height);
    if (print_image(width, height, image_path) != true)
      goto EXIT;
    printf_tabbed(print_welcome(), width, 25);
    printf_tabbed(print_system_name(), width, 25);
    printf_tabbed(print_window_manager(), width, 25);
    printf_tabbed(print_terminal(), width, 25);
    printf_tabbed(print_kernel(), width, 25);
    printf_tabbed(print_shell(), width, 25);
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
