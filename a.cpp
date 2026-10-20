#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <iomanip>

using namespace std;

// Grammar rules definition
struct Rule {
    char lhs;
    int length;
    string production;
};

map<int, Rule> rules;
map<pair<int, char>, string> actionTable;
map<pair<int, char>, int> gotoTable;

void initializeTables() {
    // Define the grammar rules implied by the table reductions
    rules[1] = {'E', 3, "E -> E+T"};
    rules[2] = {'E', 1, "E -> T"};
    rules[3] = {'T', 3, "T -> T*F"};
    rules[4] = {'T', 1, "T -> F"};
    rules[5] = {'F', 3, "F -> (E)"};
    rules[6] = {'F', 1, "F -> id"};

    // ACTION Table ('i' stands for 'id')
    // State 0
    actionTable[{0, 'i'}] = "s5";
    actionTable[{0, '('}] = "s4";
    // State 1
    actionTable[{1, '+'}] = "s6";
    actionTable[{1, '$'}] = "acc";
    // State 2
    actionTable[{2, '+'}] = "r2";
    actionTable[{2, '*'}] = "s7";
    actionTable[{2, ')'}] = "r2";
    actionTable[{2, '$'}] = "r2";
    // State 3
    actionTable[{3, '+'}] = "r4";
    actionTable[{3, '*'}] = "r4";
    actionTable[{3, ')'}] = "r4";
    actionTable[{3, '$'}] = "r4";
    // State 4
    actionTable[{4, 'i'}] = "s5";
    actionTable[{4, '('}] = "s4";
    // State 5
    actionTable[{5, '+'}] = "r6";
    actionTable[{5, '*'}] = "r6";
    actionTable[{5, ')'}] = "r6";
    actionTable[{5, '$'}] = "r6";
    // State 6
    actionTable[{6, 'i'}] = "s5";
    actionTable[{6, '('}] = "s4";
    // State 7
    actionTable[{7, 'i'}] = "s5";
    actionTable[{7, '('}] = "s4";
    // State 8
    actionTable[{8, '+'}] = "s6";
    actionTable[{8, ')'}] = "s11";
    // State 9
    actionTable[{9, '+'}] = "r1";
    actionTable[{9, '*'}] = "s7";
    actionTable[{9, ')'}] = "r1";
    actionTable[{9, '$'}] = "r1";
    // State 10
    actionTable[{10, '+'}] = "r3";
    actionTable[{10, '*'}] = "r3";
    actionTable[{10, ')'}] = "r3";
    actionTable[{10, '$'}] = "r3";
    // State 11
    actionTable[{11, '+'}] = "r5";
    actionTable[{11, '*'}] = "r5";
    actionTable[{11, ')'}] = "r5";
    actionTable[{11, '$'}] = "r5";

    // GOTO Table
    gotoTable[{0, 'E'}] = 1; gotoTable[{0, 'T'}] = 2; gotoTable[{0, 'F'}] = 3;
    gotoTable[{4, 'E'}] = 8; gotoTable[{4, 'T'}] = 2; gotoTable[{4, 'F'}] = 3;
    gotoTable[{6, 'T'}] = 9; gotoTable[{6, 'F'}] = 3;
    gotoTable[{7, 'F'}] = 10;
}

// Function to visualize the stack
string getStackString(const vector<int>& states, const vector<char>& symbols) {
    string res = "";
    for (size_t i = 0; i < states.size(); ++i) {
        if (i < symbols.size()) {
            if (symbols[i] == 'i') res += "id";
            else res += symbols[i];
        }
        res += to_string(states[i]);
    }
    return res;
}

void parse(string input) {
    if (input.back() != '$') input += "$";

    vector<int> stateStack;
    vector<char> symbolStack;
    
    stateStack.push_back(0); // Initial state
    int idx = 0;

    cout << left << setw(30) << "STACK" << setw(20) << "INPUT" << "ACTION" << endl;
    cout << string(70, '-') << endl;

    while (true) {
        int state = stateStack.back();
        char sym = input[idx];
        
        string stackStr = getStackString(stateStack, symbolStack);
        string inputStr = input.substr(idx);

        // Replace 'i' with 'id' in the input string for display purposes only
        string displayInput = "";
        for (char c : inputStr) {
            if (c == 'i') displayInput += "id";
            else displayInput += c;
        }

        if (actionTable.find({state, sym}) == actionTable.end()) {
            cout << left << setw(30) << stackStr << setw(20) << displayInput << "Error" << endl;
            cout << "\nParsing Failed: Invalid Sequence.\n";
            return;
        }

        string action = actionTable[{state, sym}];

        if (action[0] == 's') { // SHIFT
            int nextState = stoi(action.substr(1));
            cout << left << setw(30) << stackStr << setw(20) << displayInput << "Shift " + action.substr(1) << endl;
            symbolStack.push_back(sym);
            stateStack.push_back(nextState);
            idx++;
        } 
        else if (action[0] == 'r') { // REDUCE
            int ruleNum = stoi(action.substr(1));
            Rule r = rules[ruleNum];
            
            cout << left << setw(30) << stackStr << setw(20) << displayInput << "Reduce " + r.production << endl;

            // Pop elements corresponding to RHS length
            for (int i = 0; i < r.length; ++i) {
                stateStack.pop_back();
                symbolStack.pop_back();
            }

            int topState = stateStack.back();
            symbolStack.push_back(r.lhs);
            
            if (gotoTable.find({topState, r.lhs}) != gotoTable.end()) {
                stateStack.push_back(gotoTable[{topState, r.lhs}]);
            } else {
                cout << "Error: Missing GOTO entry.\n";
                return;
            }
        } 
        else if (action == "acc") { // ACCEPT
            cout << left << setw(30) << stackStr << setw(20) << displayInput << "Accept" << endl;
            cout << "\nParsing Completed Successfully.\n";
            break;
        }
    }
}

int main() {
    initializeTables();
    
    cout << "--- SLR Parser based on Table 1 ---\n";
    cout << "Note: Use 'i' to represent 'id' in your input.\n\n";
    
    string input;
    cout << "Enter input string (e.g., i*i+i): ";
    cin >> input;
    cout << "\n";

    parse(input);

    return 0;
}