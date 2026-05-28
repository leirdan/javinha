#include "preprocessor.hpp"
#include <regex>

[[nodiscard]]
std::string jc::pre::remove_comments(std::string &&buffer)
{
  int i = 0;
  while (i + 1 < buffer.size())
  {
    if (buffer[i] == '/' && buffer[i + 1] == '*')
    {
      int end = buffer.find("*/", i + 2);
      if (end == std::string::npos) break;
      for (int j = i; j < end + 2; j++)
        if (buffer[j] != '\n') buffer[j] = ' ';
      i = end + 2;
    } else i++;
  }

  std::regex line_patt("//[^\n]*");
  buffer = std::regex_replace(buffer, line_patt, "");

  return buffer;
}

[[nodiscard]]
std::string jc::pre::minify(std::string &&buffer)
{
  u64 buffer_size = buffer.size();
  std::string result;
  result.reserve(buffer_size);
  char current_char;

  for (u64 i = 0; i < buffer_size; i++)
  {
    current_char = buffer[i];
    if (current_char == '\n')
    {
      result += current_char;
      continue;
    }

    if (!iscntrl(current_char) && !isspace(current_char))
    {
      result += current_char;
      continue;
    }

    if (result.empty()) continue;

    if (i + 1 < buffer_size)
    {
      if (isalnum(result.back()) &&
          isalnum(buffer[i + 1]))
      {
        result += current_char;
      }
    }
  }

  return result;
}
