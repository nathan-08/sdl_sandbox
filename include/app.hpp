#ifndef APP_HPP
#define APP_HPP
#include <string>

class App
{
  static bool quit;
  static std::string filepath;
  static void execute_command(std::string);
public: 
  static void init();
  static void close();
  static void mainloop();
};

#endif //APP_HPP
