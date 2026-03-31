#pragma once
#include "types.hpp"

namespace jc 
{
  namespace pre 
  {
    /**
     * \brief Method responsible for remove comments from source file.
     *
     * During the preprocessor phase the compiler must eliminate
     * all the comments written by the programmer due to its nature.
     * To do so, JC implements a regex mechanism to replace all types 
     * of comments for empty spaces.
     *
     * @param buffer String containing the source code. Must be passed
     * through move semantics to avoid unnecessary copies.
    */
    std::string remove_comments(std::string&& buffer);
  }
}
