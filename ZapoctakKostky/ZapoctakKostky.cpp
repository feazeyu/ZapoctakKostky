#include "main.h"
using namespace std;

long long factorial(long long n) {
    if (n == 0) {
        return 1;
    }
    return n * factorial(n - 1);
}


/*
DieResult class - A set of methods to operate with "resultProbabilities", which are chances for each individual possibility to happen.
Numbers are DieResults with probability 1 to be the number they are
*/
DieResult DieResult::Create(int dieCount, DieResult sideSize) {
    DieResult dieResult;
    dieResult.maxRoll = sideSize.resultProbabilities.rbegin()->first * dieCount;
    for (int i = dieCount; i <= dieResult.maxRoll; i++) {
        dieResult.resultProbabilities[i] = 0;
    }
    if (dieResult.denominator == 0) {
        dieResult.artificialDieResult = false;
        dieResult = dieResult.calculateResults(dieCount, sideSize);
        if (dieCount == 0) {
            dieResult.resultProbabilities[0] = 1;
            dieResult.denominator = 1;
            return dieResult;
        }
    }
    else {
        dieResult.artificialDieResult = true;
    }
    return dieResult;
}

DieResult DieResult::Create(DieResult dieCount, int sideSize) {
    DieResult dieResult;
    dieResult.maxRoll = sideSize * dieCount.resultProbabilities.rbegin()->first;
    for (int i = dieCount.resultProbabilities.begin()->first; i <= dieResult.maxRoll; i++) {
        dieResult.resultProbabilities[i] = 0;
    }
    if (dieResult.denominator == 0) {
        dieResult.artificialDieResult = false;
        dieResult = dieResult.calculateResults(dieCount, sideSize);
    }
    else {
        dieResult.artificialDieResult = true;
    }
    return dieResult;
}

DieResult DieResult::Create(DieResult dieCount, DieResult sideSize) {
    DieResult dieResult;
    dieResult.maxRoll = sideSize.resultProbabilities.rbegin()->first * dieCount.resultProbabilities.rbegin()->first;
    for (int i = dieCount.resultProbabilities.begin()->first; i <= dieResult.maxRoll; i++) {
        dieResult.resultProbabilities[i] = 0;
    }
    if (dieResult.denominator == 0) {
        dieResult.artificialDieResult = false;
        dieResult = dieResult.calculateResults(dieCount, sideSize);
    }
    else {
        dieResult.artificialDieResult = true;
    }
    return dieResult;
}

DieResult DieResult::calculateResults(int dieCount, DieResult sideSize) {
    if (artificialDieResult) {
        cout << "Cannot calculate results for an artificialDieResult" << endl;
        exit(0);
    }
    vector<int> variations = {};
    DieResult dieResult;
    bool haveFirst = false;
    for (auto&& pair : sideSize.resultProbabilities) {
        DieResult newDieResult = SimpleDieResult::Create(dieCount, (int)get<0>(pair));
        newDieResult.multiplyProbabilities(get<1>(pair));
        newDieResult.denominator *= sideSize.denominator;
        if (!haveFirst) {
            dieResult = newDieResult;
            haveFirst = true;
        }
        else {
            dieResult = combineResult(move(dieResult), move(newDieResult));
        }
    }

    return dieResult;
}

DieResult DieResult::calculateResults(DieResult dieCount, int sideSize) {
    if (artificialDieResult) {
        cout << "Cannot calculate results for an artificialDieResult" << endl;
        exit(0);
    }
    vector<int> variations = {};
    DieResult dieResult;
    bool haveFirst = false;
    for (auto&& pair : dieCount.resultProbabilities) {
        DieResult newDieResult = SimpleDieResult::Create((int)get<0>(pair), sideSize);
        newDieResult.multiplyProbabilities(get<1>(pair));
        newDieResult.denominator *= dieCount.denominator;
        if (!haveFirst) {
            dieResult = newDieResult;
            haveFirst = true;
        }
        else {
            dieResult = combineResult(std::move(dieResult), move(newDieResult));
        }
    }
    return dieResult;
}

DieResult DieResult::calculateResults(DieResult dieCount, DieResult sideSize) {
    if (artificialDieResult) {
        cout << "Cannot calculate results for an artificialDieResult" << endl;
        exit(0);
    }
    vector<int> variations = {};
    DieResult dieResult;
    bool haveFirst = false;
    for (auto&& pair : dieCount.resultProbabilities) {
        DieResult newDieResult = DieResult::Create((int)get<0>(pair), sideSize);
        newDieResult.multiplyProbabilities(get<1>(pair));
        newDieResult.denominator *= dieCount.denominator;
        if (!haveFirst) {
            dieResult = newDieResult;
            haveFirst = true;
        }
        else {
            dieResult = combineResult(std::move(dieResult), move(newDieResult));
        }
    }
    return dieResult;
}

void DieResult::addConstant(int constant) {
    map<long long, long long> newMap;
    for (const auto& entry : resultProbabilities) {
        long long key = entry.first;
        long long value = entry.second;
        newMap[key + constant] = value;
    }
    resultProbabilities = newMap;
}

void DieResult::multiplyByConstant(int constant) {
    map<long long, long long> newMap;
    for (const auto& entry : resultProbabilities) {
        long long key = entry.first;
        long long value = entry.second;
        newMap[key * constant] = value;
    }
    resultProbabilities = newMap;
}

void DieResult::multiplyProbabilities(int constant) {
    for (auto& probability : resultProbabilities) {
        get<1>(probability) *= constant;
    }
}

void DieResult::logResult() {
    for (const auto& n : resultProbabilities) {
        cout << n.first << " = " << n.second << "/" << denominator << "; " << endl;
    }
}

void DieResult::processVariation(int initialSum, vector<int>& variations) {
    int variationSum = 0; //TODO
    for (size_t k = 0; k < variations.size(); k++) {
        variationSum += variations[k] * k;
    }
    long long topOfMultinomialCoefficient = factorial(initialSum);
    long long bottomOfMultinomialCoefficient = 1;
    for (size_t j = 0; j < variations.size(); j++) {
        bottomOfMultinomialCoefficient *= factorial(variations[j]);
    }
    resultProbabilities[variationSum] += (topOfMultinomialCoefficient / bottomOfMultinomialCoefficient);
    return;
}
/*
Recursive calculation of all possibilities
*/
void DieResult::calc(int targetSum, size_t i, vector<int>& variations, int initialSum) {
    if (i == variations.size() - 1 || targetSum == 0) {
        variations[i] = targetSum;
        //Mam valid variaci
        processVariation(initialSum, variations);
        return;
    }
    for (int currentValue = 0; currentValue <= targetSum; currentValue++) {
        variations[i] = currentValue;
        calc(targetSum - currentValue, i + 1, variations, initialSum);
    }
}

SimpleDieResult SimpleDieResult::Create(int dieCount, int sideSize)
{
    SimpleDieResult simpleDieResult;
    simpleDieResult.dieCount = dieCount;
    simpleDieResult.sideSize = sideSize;
    simpleDieResult.maxRoll = sideSize * dieCount;
    for (int i = dieCount; i <= simpleDieResult.maxRoll; i++) {
        simpleDieResult.resultProbabilities[i] = 0;
    }
    if (simpleDieResult.denominator == 0) {
        simpleDieResult.artificialDieResult = false;
        simpleDieResult.calculateResults(dieCount, sideSize);
        if (dieCount == 0) {
            simpleDieResult.resultProbabilities[0] = 1;
            simpleDieResult.denominator = 1;
            return simpleDieResult;
        }
    }
    else {
        simpleDieResult.artificialDieResult = true;
    }
    return simpleDieResult;
}

void SimpleDieResult::calculateResults(int dieCount, int sideSize) {
    if (artificialDieResult) {
        cout << "Cannot calculate results for an artificialDieResult" << endl;
        exit(0);
    }
    vector<int> variations = {};
    for (int j = 0; j < sideSize; j++) {
        variations.push_back(0);
    }
    calc(dieCount, 0, variations, dieCount);
    denominator = pow(sideSize, dieCount);
};


void SimpleDieResult::processVariation(int initialSum, vector<int>& variations) {
    int variationSum = dieCount;
    for (size_t k = 0; k < variations.size(); k++) {
        variationSum += variations[k] * k;
    }
    long long topOfMultinomialCoefficient = factorial(initialSum);
    long long bottomOfMultinomialCoefficient = 1;
    for (size_t j = 0; j < variations.size(); j++) {
        bottomOfMultinomialCoefficient *= factorial(variations[j]);
    }
    resultProbabilities[variationSum] += (topOfMultinomialCoefficient / bottomOfMultinomialCoefficient);
    return;
}
void ArtificialDieResult::calculateResults(int a, int b) {
    cout << "Cannot calculate artificialDieResults" << endl;
}
void ArtificialDieResult::calculateResults(DieResult a, int b) {
    cout << "Cannot calculate artificialDieResults" << endl;
}
void ArtificialDieResult::calculateResults(int a, DieResult b) {
    cout << "Cannot calculate artificialDieResults" << endl;
}
void ArtificialDieResult::calculateResults(DieResult a, DieResult b) {
    cout << "Cannot calculate artificialDieResults" << endl;
}
Token::Token(Type type,
    const string& s,
    int precedence,
    bool unary
)
    : type{ type }
    , str(s)
    , precedence{ precedence }
    , unary{ unary }
{}


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
            case '(':   token = Token::Type::LeftParen; break;
            case ')':   token = Token::Type::RightParen; break;
            case '*':   token = Token::Type::Operator; precedence = 3; break;
            case '+':   token = Token::Type::Operator; precedence = 2; break;
            case 'd':   token = Token::Type::Operator; precedence = 7; break;
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

long long GCD(long long a, long long b) {
    if (b == 0)
        return a;
    return GCD(b, a % b);
}

long long LCM(vector<long long>& input)
{
    long long result = input[0];
    for (size_t i = 1; i < input.size(); i++) {
        result = (((input[i] * result)) / (GCD(input[i], result)));
    }
    return result;
}

//Converts denominators of two results to equal each other, then adds the probabilities
DieResult combineResult(DieResult&& a, DieResult&& b) {
    vector<long long> denominators = { a.denominator, b.denominator };
    if (a.denominator == 0 || b.denominator == 0) {
        printf("The resulting die could have negative sides (If you're sure it can't, something has overflown into the negatives. Yay!), and you cant roll a negative sided die. So... don't do that :) \n");
        system("pause");
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

DieResult addResult(DieResult& a, DieResult& b) {
    int newDenominator = a.denominator * b.denominator;
    DieResult newResult = ArtificialDieResult();
    newResult.denominator = newDenominator;
    for (tuple<int, int> resultA : a.resultProbabilities) {
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
    DieResult newResult = ArtificialDieResult();
    newResult.denominator = newDenominator;
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
    DieResult newResult = ArtificialDieResult();
    newResult.denominator = newDenominator;
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
    DieResult newResult = ArtificialDieResult();
    newResult.denominator = 1;
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
            if (!operatorStack.empty()) {
                operatorStack.pop_back();
            }
            else {
                return { Token{Token::Type::Err, "Invalid Input"} };
            }
        }
        break;
        default:
            return { Token{Token::Type::Err, "Invalid Input"} };
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
                    DieResult dieResult = DieResult::Create(lhs, rhs);
                    stack.push_back(dieResult);
                    break;
                }
            }
        }
        break;

        default:
            cout << (token.str) << endl;
            return SimpleDieResult::Create(1, 1);
        }
    }
    if (stack.size() > 0) {
        return stack.back();
    }
    cout << "Invalid input" << endl;
    return SimpleDieResult::Create(1, 1);
}


int main() {
    DieResult testDie = DieResult::Create(1, SimpleDieResult::Create(1, 2));
    testDie.logResult();
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