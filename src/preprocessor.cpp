#include "preprocessor.hpp"
#include <regex>

[[nodiscard]]
std::string jc::pre::remove_comments(std::string&& buffer)
{
  std::string result;
  result.reserve(buffer.size());

  std::regex block_patt("/\\*[\\s\\S]*?\\*/");
  std::regex line_patt("//[^\n]*");

  buffer = std::regex_replace(buffer, block_patt, "");
  buffer = std::regex_replace(buffer, line_patt, "");

  result = std::move(buffer);

  return result;
}

  // ideia: 
  // varrer cada caractere da string:
  // se o caractere atual for um especial,
  // o último do result for um normal e
  // o próximo do buffer for um normal, 
  // então ainda escreve o caractere especial.
[[nodiscard]]
std::string jc::pre::minify(std::string&& buffer) 
{
  u64 buffer_size = buffer.size();
  std::string result;
  result.reserve(buffer_size);
  char current_char;

  for (u64 i = 0; i < buffer_size; i++) 
  {
    current_char = buffer[i]; 
    if (!iscntrl(current_char) && !isspace(current_char)) 
    {
      result += current_char;
      continue;
    }

    if (result.empty()) continue;

    if (i + 1 < buffer_size) 
    {
      if (isalnum(result.back()) &&
          isalnum(buffer[i+1])) 
      {
        result += current_char;
      }
      continue;
    }
  }

  return result;
}
