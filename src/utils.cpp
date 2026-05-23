#include "utils.hpp"
#include <iostream>

void log::debug(const std::string &&msg)
{
#ifdef DEBUG
  std::cout << "[DEBUG] " << msg << "\n";
#endif
}
