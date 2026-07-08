#pragma once
#include "utils.hpp"
#include "symbol_table.hpp"
#include <string>
#include <optional>
#include <vector>

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
			std::weak_ptr<Symbol> target;
			std::weak_ptr<Symbol> argument1;
			std::weak_ptr<Symbol> argument2;
			void print(std::ostream &os = std::cout) const
			{
				auto l1 = target.lock();
				auto l2 = argument1.lock();
				auto l3 = argument2.lock();
				Symbol t, a1, a2;
				if (l1)
					t = *l1.get();
				if (l2)
					a1 = *l2.get();
				if (l3)
					a2 = *l3.get();

				switch (op)
				{
				case OpCode::ADD:
				{
					os << t.name << " = " << a1.name << " + " << a2.name << "\n";
					break;
				}
				case OpCode::SUB:
				{
					os << t.name << " = " << a1.name << " - " << a2.name << "\n";
					break;
				}
				case OpCode::MUL:
				{
					os << t.name << " = " << a1.name << " * " << a2.name << "\n";
					break;
				}
				case OpCode::AND:
				{
					os << t.name << " = " << a1.name << " + " << a2.name << "\n";
					break;
				}
				}
			};
		};

		using TACList = std::vector<TACInstruction>; // represents the instructions of an ast node (method)

		class TACGenerator
		{
		private:
			u32 temp_c = 0;
			u32 label_c = 0;

		public:
			std::weak_ptr<Symbol> next_temp(SymbolTable &sym_table, const std::string &type = "int")
			{
				std::string name = "t" + std::to_string(temp_c++);

				std::shared_ptr<Symbol> entry = std::make_shared<Symbol>();
				entry->name = name;
				entry->category = SymbolCategory::TEMP;
				entry->type = type;
				sym_table.insert(entry->name, type, entry->category);

				return entry;
			}

			std::weak_ptr<Symbol> next_label(SymbolTable &sym_table)
			{
				std::string name = "L" + std::to_string(label_c++);

				std::shared_ptr<Symbol> entry = std::make_shared<Symbol>();
				entry->name = name;
				entry->category = SymbolCategory::LABEL;
				entry->type = "void";

				sym_table.insert(entry->name, "void", entry->category);
				return entry;
			}
		};
	}
}