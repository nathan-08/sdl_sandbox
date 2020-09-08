#include "Clock.hpp"

std::string Clock::getTime()
{
  char buff[20];

  time_t now = time( nullptr );
  tm *time_data = localtime( &now );
  strftime( buff, 20, "%a %I:%M %p", time_data );

  return std::string( buff );
}
