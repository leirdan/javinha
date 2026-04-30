#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <functional>

// grammar representation
using Production = std::vector<std::string>;
using Grammar = std::unordered_map<std::string, std::vector<Production>>;

// earley item
struct State {
    std::string lhs;
    Production rhs;
    int dot;    // position of the dot in rhs
    int start;  // input position where this state started

    State(std::string lhs, Production rhs, int dot, int start)
        : lhs(std::move(lhs)), rhs(std::move(rhs)), dot(dot), start(start) {}

    bool isComplete() const {
        return dot >= (int)rhs.size();
    }

    const std::string* nextSymbol() const {
        if (dot < (int)rhs.size()) return &rhs[dot];
        return nullptr;
    }

    State advance() const {
        return State(lhs, rhs, dot + 1, start);
    }

    bool operator==(const State& other) const {
        return lhs == other.lhs &&
               dot == other.dot &&
               start == other.start &&
               rhs == other.rhs;
    }

    std::string toString() const {
        std::string sb = lhs + " -> ";
        for (int i = 0; i < (int)rhs.size(); i++) {
            if (i == dot) sb += "• ";
            sb += rhs[i] + ' ';
        }
        if (dot == (int)rhs.size()) sb += "•";
        sb += " [" + std::to_string(start) + "]";
        return sb;
    }
};

struct StateHash {
    std::size_t operator()(const State& s) const {
        std::size_t h = std::hash<std::string>{}(s.lhs);
        h ^= std::hash<int>{}(s.dot)   + 0x9e3779b9 + (h << 6) + (h >> 2);
        h ^= std::hash<int>{}(s.start) + 0x9e3779b9 + (h << 6) + (h >> 2);
        for (const auto& sym : s.rhs) {
            h ^= std::hash<std::string>{}(sym) + 0x9e3779b9 + (h << 6) + (h >> 2);
        }
        return h;
    }
};

using StateSet = std::unordered_set<State, StateHash>;

class EarleyParser {
    Grammar grammar;
    std::string startSymbol;

public:
    EarleyParser(Grammar grammar, std::string startSymbol)
        : grammar(std::move(grammar)), startSymbol(std::move(startSymbol)) {}

    bool parse(const std::string& input) {
        int n = (int)input.size();
        std::vector<StateSet> chart(n + 1);

        // initial state
        chart[0].insert(State(startSymbol, {}, 0, 0));

        for (int i = 0; i <= n; i++) {
            bool added;
            do {
                added = false;
                // copy current set to safely iterate while modifying
                StateSet current = chart[i];
                for (const State& state : current) {
                    if (!state.isComplete()) {
                        const std::string* sym = state.nextSymbol();
                        auto grammarIt = grammar.find(*sym);
                        if (grammarIt != grammar.end()) {
                            // predict a sym that is a nonterminal
                            for (const Production& prod : grammarIt->second) {
                                State newState(*sym, prod, 0, i);
                                if (chart[i].insert(newState).second) added = true;
                            }
                        } else {
                            // scan a sym that is a terminal
                            if (i < n && *sym == std::string(1, input[i])) {
                                State newState = state.advance();
                                if (chart[i + 1].insert(newState).second) added = true;
                            }
                        }
                    } else {
                        // complete step
                        StateSet startChart = chart[state.start]; // copy to iterate safely
                        for (const State& s : startChart) {
                            if (!s.isComplete()) {
                                const std::string* nextSym = s.nextSymbol();
                                if (nextSym && *nextSym == state.lhs) {
                                    State newState(s.lhs, s.rhs, s.dot + 1, s.start);
                                    if (chart[i].insert(newState).second) added = true;
                                }
                            }
                        }
                    }
                }
            } while (added);
        }

        // accept if there is a completed startSymbol state covering the entire input
        for (const State& s : chart[n]) {
            if (s.lhs == startSymbol && s.isComplete() && s.start == 0) {
                return true;
            }
        }
        return false;
    }
};

int main() {
    
    // nothing... yet!

    return 0;
}