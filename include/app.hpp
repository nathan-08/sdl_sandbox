#ifndef APP_HPP
#define APP_HPP
#include <string>

class App
{
  static bool quit;
  static std::string filepath;
  static void execute_command(std::string);
  static bool evaluate_quick_buf(std::string, SDL_Keymod);
public: 
  static void init();
  static void close();
  static void mainloop();
};

#endif //APP_HPP


// I have modified this file
