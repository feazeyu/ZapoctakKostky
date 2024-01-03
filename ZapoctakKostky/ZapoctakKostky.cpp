#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <deque>
#include <tuple>
#include <stdio.h>
#include <math.h>
#include <map>
using namespace std;


/*
DieResult class - A set of methods to operate with "resultProbabilities", which are chances for each individual possibility to happen.
Numbers are DieResults with probability 1 to be the number they are
*/
class DieResult {
public:
    map<int, int> resultProbabilities;
    vector<int> sides;
    int count;
    int maxRoll;
    long denominator;
    bool artificialDieResult;
    void calculateResults() {
        if (artificialDieResult) {
            cout << "Cannot calculate results for an artificialDieResult" << endl;
            exit;
        }
        int k = count;
        vector<int> n = sides;
        vector<int> variations = {};
        for (int j = 0; j < maxRoll; j++) {
            variations.push_back(0);
        }
        calc(k, n, 0, variations);
    };

    DieResult(vector<int> sides, long s_denominator = 0)
        : sides{ sides },
        denominator{ s_denominator }
    {
        int buffer = 0;
        for (int i : sides) {
            buffer += i;
        }
        maxRoll = buffer;
        count = sides.size();
        for (int i = count; i < maxRoll; i++) {
            resultProbabilities[i] = 0;
        }
        if (denominator == 0) {
            artificialDieResult = false;
            calculateResults();
            if (count == 0) {
                resultProbabilities[0] = 1;
                denominator = 1;
                return;
            }
        }
        else {
            artificialDieResult = true;
        }
    }

    

    void addConstant(int constant) {
        map<int, int> newMap;
        for (const auto& entry : resultProbabilities) {
            int key = entry.first;
            int value = entry.second;
            newMap[key + constant] = value;
        }
        resultProbabilities = newMap;
    }

    void multiplyByConstant(int constant) {
        map<int, int> newMap;
        for (const auto& entry : resultProbabilities) {
            int key = entry.first;
            int value = entry.second;
            newMap[key * constant] = value;
        }
        resultProbabilities = newMap;
    }

    void multiplyProbabilities(int constant) {
        for (auto& probability : resultProbabilities) {
            get<1>(probability) *= constant;
        }
    }

    void logResult() {
        for (const auto& n : resultProbabilities) {
                cout << n.first << " = " << n.second << "/" << denominator << "; " << endl;
        } 
    }
private:
    /*
    Recursive calculation of all possibilities
    */
    void calc(int k, vector<int> n, int i, vector<int>& variations) {
        if (n.size() > 0) {
            for (int j = 1; j < n[i] + 1; j++) {
                variations[i] = j;
                if (i < k - 1) {
                    calc(k, n, i + 1, variations);
                }
                else {
                    int sum = 0;
                    for (int number : variations) {
                        sum += number;
                    }
                    resultProbabilities[sum] += 1;
                    if (!artificialDieResult) {
                        denominator += 1;
                    }
                }
            }
        }
    }

};


class Token {
public:
    enum class Type {
        Unknown,
        Number,
        Operator,
        LeftParen,
        RightParen,
        DieResult,
        Err
    };

    Token(Type type,
        const string& s,
        int precedence = -1,
        bool unary = false
    )
        : type{ type }
        , str(s)
        , precedence{ precedence }
        , unary{ unary }
    {}

    const Type type;
    const string str;
    const int precedence;
    const bool unary;
};

ostream& operator<<(ostream& os, const Token& token) {
    os << token.str;
    return os;
}
// Convert strings from input to tokens
deque<Token> tokenizeExpression(const string& expr) {
    deque<Token> tokens;

    for (const auto* pointer = expr.c_str(); *pointer; ++pointer) {
        if (isblank(*pointer)) {
            // do nothing
        }
        //Special case that handles numbers longer than 1 char
        else if (isdigit(*pointer)) {
            const auto* b = pointer;
            while (isdigit(*pointer)) {
                ++pointer;
            }
            const auto s = string(b, pointer);
            tokens.push_back(Token{ Token::Type::Number, s });
            --pointer;
        }
        else {
            Token::Type token = Token::Type::Unknown;
            int precedence = -1;
            bool unary = false;
            char c = *pointer;
            switch (c) {
            default:    break;
            case '(':   token = Token::Type::LeftParen;break;
            case ')':   token = Token::Type::RightParen;break;
            case '*':   token = Token::Type::Operator;precedence = 3; break;
            case '+':   token = Token::Type::Operator;precedence = 2; break;
            case 'd':   token = Token::Type::Operator;precedence = 7; break;
            case '-':
                if (tokens.empty()
                    || tokens.back().type == Token::Type::Operator
                    || tokens.back().type == Token::Type::LeftParen
                    ) {
                    unary = true;
                    c = 'm';
                    token = Token::Type::Operator;
                    precedence = 6;
                }
                else {
                    token = Token::Type::Operator;
                    precedence = 2;
                }
                break;
            }
            const auto s = string(1, c);
            tokens.push_back(Token{ token, s, precedence, unary });
        }
    }

    return tokens;
}

int GCD(long long a, long long b) {
    if (b == 0)
        return a;
    return GCD(b, a % b);
}

int LCM(vector<long long>& input)
{
    int result = input[0];
    for (size_t i = 1; i < input.size(); i++) {
        result = (((input[i] * result)) / (GCD(input[i], result)));
    }
    return result;
}

//Converts denominators of two results to equal each other, then adds the probabilities
DieResult combineResult(DieResult&& a, DieResult&& b) {
    vector<long long> denominators = { a.denominator, b.denominator };
    if (a.denominator == 0 || b.denominator == 0) {
        printf("The resulting die could have negative sides, and you cant roll a negative sided die. So... don't do that :)");
        exit(500);
    };
    int lcm = LCM(denominators);
    int coefficientA = lcm / a.denominator;
    int coefficientB = lcm / b.denominator;

    a.multiplyProbabilities(coefficientA);
    b.multiplyProbabilities(coefficientB);
    for (auto& result : b.resultProbabilities) {
        a.resultProbabilities[get<0>(result)] += get<1>(result);
    }
    a.denominator = lcm;

    return a;
};

DieResult addResult(DieResult &a, DieResult &b) {
    int newDenominator = a.denominator * b.denominator;
    DieResult newResult = DieResult({}, newDenominator);
    for (tuple<int,int> resultA : a.resultProbabilities) {
        for (tuple<int, int> resultB : b.resultProbabilities) {
            int keyA = get<0>(resultA);
            int keyB = get<0>(resultB);
            int valueA = get<1>(resultA);
            int valueB = get<1>(resultB);
            newResult.resultProbabilities[keyA + keyB] += valueA * valueB;
        }
    }
    return newResult;
};

DieResult subtractResult(DieResult& a, DieResult& b) {
    int newDenominator = a.denominator * b.denominator;
    DieResult newResult = DieResult({}, newDenominator);
    for (tuple<int, int> resultA : a.resultProbabilities) {
        for (tuple<int, int> resultB : b.resultProbabilities) {
            int keyA = get<0>(resultA);
            int keyB = get<0>(resultB);
            int valueA = get<1>(resultA);
            int valueB = get<1>(resultB);
            newResult.resultProbabilities[keyA - keyB] += valueA * valueB;
        }
    }
    return newResult;
};

DieResult multiplyResult(DieResult& a, DieResult& b) {
    int newDenominator = a.denominator * b.denominator;
    DieResult newResult = DieResult({}, newDenominator);
    for (tuple<int, int> resultA : a.resultProbabilities) {
        for (tuple<int, int> resultB : b.resultProbabilities) {
            int keyA = get<0>(resultA);
            int keyB = get<0>(resultB);
            int valueA = get<1>(resultA);
            int valueB = get<1>(resultB);
            newResult.resultProbabilities[keyA * keyB] += valueA * valueB;
        }
    }
    return newResult;
}

DieResult intToDieResult(int input) {
    DieResult newResult = DieResult({}, 1);
    newResult.resultProbabilities[input] = 1;
    return newResult;
}

//Shunting yard algorithm, https://en.wikipedia.org/wiki/Shunting_yard_algorithm
deque<Token> shuntingYard(const deque<Token>& tokens) {
    deque<Token> queue;
    vector<Token> operatorStack;

    for (auto token : tokens) {
        switch (token.type) {
        case Token::Type::Number:
            queue.push_back(token);
            break;
        case Token::Type::Operator:
        {
            const auto o1 = token;
            while (!operatorStack.empty()) {
                const auto o2 = operatorStack.back();
                if (o1.precedence <= o2.precedence) {
                    operatorStack.pop_back();
                    queue.push_back(o2);
                    continue;
                }
                
                break;
            }
            operatorStack.push_back(o1);
        }
        break;
        case Token::Type::LeftParen:
            operatorStack.push_back(token);
            break;
        case Token::Type::RightParen:
        {
            bool match = false;
            while (!operatorStack.empty() && operatorStack.back().type != Token::Type::LeftParen) {
                queue.push_back(operatorStack.back());
                operatorStack.pop_back();
                match = true;
            }

            if (!match && operatorStack.empty()) {
                return {};
            }
            operatorStack.pop_back();
        }
        break;
        default:
            return { Token{Token::Type::Err, "Invalid Input"}};
        }
    }

    while (!operatorStack.empty()) {
        if (operatorStack.back().type == Token::Type::LeftParen) {
            return {};
        }
        queue.push_back(move(operatorStack.back()));
        operatorStack.pop_back();
    }
    return queue;
}


DieResult calculate(const string& expr) {
    const auto tokens = tokenizeExpression(expr);
    auto queue = shuntingYard(tokens);
    vector<DieResult> stack;
    while (!queue.empty()) {

        const auto token = queue.front();
        queue.pop_front();
        switch (token.type) {
        case Token::Type::Number:
            stack.push_back(intToDieResult(stoi(token.str)));
            break;

        case Token::Type::Operator:
        {
            if (token.unary) {
                if (stack.empty()) {
                    queue.push_back(Token{ Token::Type::Err, "Invalid Input" });
                    break;
                }
                auto rhs = stack.back();
                stack.pop_back();
                switch (token.str[0]) {
                default:
                    queue.push_back(Token{ Token::Type::Err, "Operator Error" });
                    break;
                case 'm':                   // Special operator name for unary '-'
                    rhs.multiplyByConstant(-1);
                    stack.push_back(rhs);
                    break;
                }
            }
            else {
                // binary operators
                if (stack.empty()) {
                    queue.push_back(Token{ Token::Type::Err, "Invalid Input" });
                    break;
                }
                auto rhs = stack.back();
                stack.pop_back();
                if (stack.empty()) {
                    queue.push_back(Token{ Token::Type::Err, "Invalid Input" });
                    break;
                }
                auto lhs = stack.back();
                stack.pop_back();

                switch (token.str[0]) {
                default:
                    queue.push_back(Token{ Token::Type::Err, "Operator Error" });
                    break;
                case '*':
                    stack.push_back(multiplyResult(lhs, rhs));
                    break;
                case '+':
                    stack.push_back(addResult(lhs, rhs));
                    break;
                case '-':
                    stack.push_back(subtractResult(lhs, rhs));
                    break;
                case 'd':
                    vector<DieResult> tempResults;
                    for (auto& resultLhs : lhs.resultProbabilities) {
                        for (auto& resultRhs : rhs.resultProbabilities) {
                            vector<int> sides;
                            for (int i = 0; i < get<0>(resultLhs); i++) {
                                sides.push_back(get<0>(resultRhs));
                            }
                            DieResult tempResult = DieResult(sides);
                            tempResult.multiplyProbabilities(get<1>(resultLhs));
                            tempResult.denominator *= lhs.denominator * rhs.denominator;
                            tempResults.push_back(tempResult);
                            if (tempResult.resultProbabilities.size() == 0) {
                                queue.push_back(Token{ Token::Type::Err, "Cannot have 0 or negative sided die" });
                            }
                        }
                    }
                    DieResult finalResult = tempResults[0];
                    for (size_t i = 1; i < tempResults.size(); i++) {
                        finalResult = combineResult(move(finalResult), move(tempResults[i]));
                    }
                    stack.push_back(finalResult);
                    break;
                }
            }
        }
        break;

        default:
            cout << (token.str) << endl;
            return DieResult({ 0 });
        }
    }

    return stack.back();
}

int main() {
    cout << "Input a die equation to find outcome probabilities!" << endl;
    cout << "Input help for valid commands" << endl;
    string input;
    while (getline(cin, input)) {
        if (input == "q" || input == "exit") {
            break;
        }
        else if (input == "help") {
            cout << "help: Show this dialogue" << endl;
            cout << "q | exit: Quit the program" << endl;
            continue;
        }
        DieResult testResult = calculate(input);
        testResult.logResult();
    }
} 