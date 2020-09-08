#include "app.hpp"
#include <iostream>

int main( int argc, char *argv[] )
{
  std::cout << "init app" << std::endl;
  try
  {
    App::init();
    App::mainloop();
  }
  catch (std::exception& e)
  {
    std::cout << "caught exception: " << e.what() << std::endl;
  }

  std::cout << "closing application\n";
  App::close();
  return EXIT_SUCCESS;
}

