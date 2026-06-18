#include <argparse/argparse.hpp>
#include <string>

class CompilerConfig {
public:
    std::string inputFile;
    std::string outputFile;
    bool printTokens = false;

    bool parse(int argc, char* argv[]) {
        argparse::ArgumentParser program("javinha");

        // arquivos de entrada e saída
        program.add_argument("input_file").required();
        program.add_argument("output_file").required();

        // FLAG DE TOKENS
        program.add_argument("--tokens")
            .help("Printa a lista de tokens")
            .flag();

        try {
            program.parse_args(argc, argv);
            inputFile = program.get<std::string>("input_file");
            outputFile = program.get<std::string>("output_file");
            printTokens = program.get<bool>("--tokens");
            return true;
        } catch (const std::exception& err) {
            return false;
        }
    }
};