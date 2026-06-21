#pragma once

#include <argparse/argparse.hpp>
#include <string>

class CompilerConfig
{
public:
    std::string inputFile;
    std::string outputFile;
    bool printTokens = false;
    bool firstLexicalError = false;
    bool printAst = false;
    bool printSymbolTable = false;
    bool debug = false;

    bool parse(int argc, char *argv[])
    {
        argparse::ArgumentParser program("javinha");

        // arquivos de entrada e saída
        program.add_argument("input_file").required();
        program.add_argument("output_file").required();

        // flags
        program.add_argument("--tokens")
            .help("Printa a lista de tokens")
            .flag();

        program.add_argument("--first_lexical_error")
            .help("O processamento para no primeiro erro léxico")
            .flag();

        program.add_argument("--ast")
            .help("Printa a árvore sintática abstrata")
            .flag();

        program.add_argument("--symbol_table")
            .help("Printa a table de símbolos")
            .flag();

        program.add_argument("--debug")
            .help("Printa mensagens de log durante a compilação")
            .flag();

        try
        {
            program.parse_args(argc, argv);
            inputFile = program.get<std::string>("input_file");
            outputFile = program.get<std::string>("output_file");
            printTokens = program.get<bool>("--tokens");
            firstLexicalError = program.get<bool>("--first_lexical_error");
            printAst = program.get<bool>("--ast");
            printSymbolTable = program.get<bool>("--symbol_table");
            debug = program.get<bool>("--debug");
            return true;
        }
        catch (const std::exception &err)
        {
            return false;
        }
    }
};