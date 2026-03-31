#include "preprocessor.hpp"
#include <regex>

[[nodiscard]]
std::string jc::pre::remove_comments(std::string&& buffer)
{
  u64 buffer_size = buffer.size();
  std::string result;
  result.reserve(buffer_size);

  std::regex block_patt("/\\*[\\s\\S]*?\\*/");
  std::regex line_patt("//[^\n]*");

  buffer = std::regex_replace(buffer, block_patt, "");
  buffer = std::regex_replace(buffer, line_patt, "");

  result = std::move(buffer);

  return result;
}
