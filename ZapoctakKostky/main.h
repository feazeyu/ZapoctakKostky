#pragma once
#pragma once
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
class DieResult {
public:

    static DieResult Create(int dieCount, DieResult sideSize);

    static DieResult Create(DieResult dieCount, int sideSize);

    static DieResult Create(DieResult dieCount, DieResult sideSize);

    map<long long, long long> resultProbabilities;
    int maxRoll;
    long long denominator;
    bool artificialDieResult;
    bool simpleDieResult;


    DieResult calculateResults(int dieCount, DieResult sideSize);

    DieResult calculateResults(DieResult dieCount, int sideSize);

    DieResult calculateResults(DieResult dieCount, DieResult sideSize);

    void addConstant(int constant);

    void multiplyByConstant(int constant);

    void multiplyProbabilities(int constant);

    void logResult();

    virtual void processVariation(int initialSum, vector<int>& variations);

    void calc(int targetSum, int i, vector<int>& variations, int initialSum);

};
class SimpleDieResult : public DieResult {
public:
    int dieCount;
    int sideSize;

    static SimpleDieResult Create(int dieCount, int sideSize)
        ;

private:
    void calculateResults(int dieCount, int sideSize);


    void processVariation(int initialSum, vector<int>& variations) override;
};
class ArtificialDieResult : public DieResult {
private:
    void calculateResults(int a, int b);
    void calculateResults(DieResult a, int b);
    void calculateResults(int a, DieResult b);
    void calculateResults(DieResult a, DieResult b);
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
    );

    const Type type;
    const string str;
    const int precedence;
    const bool unary;
};
DieResult combineResult(DieResult&& a, DieResult&& b);

DieResult addResult(DieResult& a, DieResult& b);

DieResult subtractResult(DieResult& a, DieResult& b);

DieResult multiplyResult(DieResult& a, DieResult& b);

DieResult intToDieResult(int input);