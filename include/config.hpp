#include <argparse/argparse.hpp>
#include <string>

class CompilerConfig {
public:
    std::string inputFile;
    std::string outputFile;
    bool printTokens = false;
    bool firstLexicalError = false;

    bool parse(int argc, char* argv[]) {
        argparse::ArgumentParser program("javinha");

        // arquivos de entrada e saída
        program.add_argument("input_file").required();
        program.add_argument("output_file").required();

        // FLAG DE TOKENS
        program.add_argument("--tokens")
            .help("Printa a lista de tokens")
            .flag();

        // FLAG PARA QUE O PROCESSAMENTO PARE NO PRIMEIRO ERRO LÉXICO
        program.add_argument("--first_lexical_error")
            .help("O processamento para no primeiro erro léxico")
            .flag();

        try {
            program.parse_args(argc, argv);
            inputFile = program.get<std::string>("input_file");
            outputFile = program.get<std::string>("output_file");
            printTokens = program.get<bool>("--tokens");
            firstLexicalError = program.get<bool>("--first_lexical_error");
            return true;
        } catch (const std::exception& err) {
            return false;
        }
    }
};