#include <iostream>
#include <fstream>
#include <sstream>
#include "types.hpp"
#include "preprocessor.hpp"

using namespace jc;

int main(int argc, char *argv[]) 
{
  std::string program_buff;

  if (argc != 3) 
  {
    std::cerr << "please provide the correct arguments.\n";
    return EXIT_FAILURE;
  }
  char* input_file = argv[1];
  char* output_file = argv[2];

  std::fstream f_stream(input_file);
  if (!f_stream.is_open()) 
  { 
    std::cerr << "file not found\n";
    return EXIT_FAILURE;
  }

  // TODO: improve the efficiency of this
  std::stringstream stream;
  stream << f_stream.rdbuf();
  program_buff = stream.str();
  f_stream.close();

  // TODO: add some logs
  std::string buffer1 = pre::remove_comments(std::move(program_buff));
  std::string new_buffer = pre::minify(std::move(buffer1));
  u64 size = new_buffer.length();
  std::ofstream o_stream(output_file);
  for (u64 i = 0; i < size; i++) 
  {
    o_stream << new_buffer[i];
  }

  return 0;
}
