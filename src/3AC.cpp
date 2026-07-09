#include "3AC.hpp"

namespace jc {
    namespace backend {

        void TACInstruction::print(std::ostream &os) const {
            switch (op) {
                case OpCode::ADD: os << target << " = " << argument1 << " + " << argument2 << "\n"; break;
                case OpCode::SUB: os << target << " = " << argument1 << " - " << argument2 << "\n"; break;
                case OpCode::MUL: os << target << " = " << argument1 << " * " << argument2 << "\n"; break;
                case OpCode::AND: os << target << " = " << argument1 << " && " << argument2 << "\n"; break;
                case OpCode::GT:  os << target << " = " << argument1 << " > " << argument2 << "\n"; break;
                case OpCode::ASSIGN: os << target << " = " << argument1 << "\n"; break;
                case OpCode::NOT: os << target << " = !" << argument1 << "\n"; break;
                case OpCode::LABEL: os << target << ":\n"; break;
                case OpCode::GOTO: os << "goto " << target << "\n"; break;
                case OpCode::IF_FALSE: os << "ifFalse " << argument1 << " goto " << target << "\n"; break;
                case OpCode::PRINT: os << "print " << argument1 << "\n"; break;
                case OpCode::PARAM: os << "param " << argument1 << "\n"; break;
                case OpCode::CALL: 
                    if (!target.empty()) os << target << " = ";
                    os << "call " << argument1 << ", " << argument2 << "\n"; 
                    break;
                case OpCode::RETURN: os << "return " << argument1 << "\n"; break;
                case OpCode::ARRAY_LOAD: os << target << " = " << argument1 << "[" << argument2 << "]\n"; break;
                case OpCode::ARRAY_STORE: os << target << "[" << argument1 << "] = " << argument2 << "\n"; break;
                case OpCode::NEW_ARRAY: os << target << " = new int[" << argument1 << "]\n"; break;
                case OpCode::NEW_OBJECT: os << target << " = new " << argument1 << "\n"; break;
                case OpCode::LENGTH: os << target << " = length " << argument1 << "\n"; break;
            }
        }

        std::string TACGenerator::next_temp(SymbolTable &sym_table, const std::string &type) {
            std::string name = "t" + std::to_string(temp_c++);
            sym_table.insert(name, type, SymbolCategory::TEMP);
            return name;
        }

        std::string TACGenerator::next_label(SymbolTable &sym_table) {
            std::string name = "L" + std::to_string(label_c++);
            sym_table.insert(name, "void", SymbolCategory::LABEL);
            return name;
        }

    } // namespace backend
} // namespace jc