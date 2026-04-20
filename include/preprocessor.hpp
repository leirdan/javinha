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

    /** 
     * \brief Method responsible for minify the source code.
     *
     * During the preprocessor phase the compiler must remove all the
     * special characters and empty spaces without breaking the valid tokens.
     * For instance, the compiler must perform the following transformation:
     * From "class     A  { private     int [] b  }"
     * to "class A{private int[]b}"
     *
     * @param buffer String containing the source code without the comments.
     * Must be passed through move semantics to avoid unnecessary copies.
    */
    std::string minify(std::string&& buffer);
  }
}
