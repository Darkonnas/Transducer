//
// Created by vlad_ on 03.03.2021.
//

#ifndef TRANSLATOR_TRANSDUCER_HPP
#define TRANSLATOR_TRANSDUCER_HPP

#include <string>
#include <map>
#include <set>
#include <vector>
#include <tuple>
#include <stack>

typedef std::pair<std::string, char> fstTransitionArgument;
typedef std::map<std::string, std::set<std::string>> fstTransitionResult, fstConfigurationMap;
typedef std::map<fstTransitionArgument, fstTransitionResult> fstTransitionFunction;
typedef std::pair<std::string, std::set<std::string>> fstConfiguration;

class finiteStateTransducer {
    std::set<std::string> q, f;
    std::set<char> vIn, vOut;
    fstTransitionFunction d;
    std::string q0;
    bool isNondeterministic;
    bool hasLambdaTransitions;

public:
    finiteStateTransducer();

    explicit finiteStateTransducer(std::ifstream &);

    fstConfigurationMap lambdaClosure(const fstConfiguration &) const;

    std::set<std::string> translate(const std::string &) const;

    void printConfiguration() const;
};

typedef std::tuple<std::string, char, char> sttTransitionArgument;
typedef std::map<std::string, std::set<std::pair<std::string, std::string>>> sttTransitionResult;
typedef std::map<sttTransitionArgument, sttTransitionResult> sttTransitionFunction;
typedef std::map<std::string, std::set<std::pair<std::stack<char>, std::string>>> sttConfigurationMap;
typedef std::pair<std::string, std::set<std::pair<std::stack<char>, std::string>>> sttConfiguration;

class stackTransducer {
    std::set<std::string> q, f;
    std::set<char> vIn, vOut, g;
    sttTransitionFunction d;
    std::string q0;
    char z0;
    bool isNondeterministic;
    bool hasLambdaTransitions;
public:
    stackTransducer();

    stackTransducer(std::ifstream &);

    sttConfigurationMap lambdaClosure(const sttConfiguration &) const;

    std::set<std::string> translate(const std::string &) const;

    void printConfiguration() const;
};


#endif //TRANSLATOR_TRANSDUCER_HPP
