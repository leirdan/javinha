#pragma once
#include "utils.hpp"
#include "symbol_table.hpp"
#include <string>
#include <vector>
#include <iostream>

namespace jc
{
	namespace backend
	{

		enum class OpCode : u8
		{
			ADD,
			SUB,
			MUL,
			AND,
			GT,
			ASSIGN,
			NOT,
			LABEL,
			GOTO,
			IF_FALSE,
			PRINT,
			PARAM,
			CALL,
			RETURN,
			ARRAY_LOAD,
			ARRAY_STORE,
			NEW_OBJECT,
			NEW_ARRAY,
			LENGTH
		};

		struct TACInstruction
		{
			OpCode op;
			std::string target;
			std::string argument1;
			std::string argument2;

			void print(std::ostream &os = std::cout) const;
		};

		using TACList = std::vector<TACInstruction>;

		class TACGenerator
		{
		private:
			u32 temp_c = 0;
			u32 label_c = 0;

		public:
			std::string next_temp(SymbolTable &sym_table, const std::string &type = "int");
			std::string next_label(SymbolTable &sym_table);
		};

	} // namespace backend
} // namespace jc