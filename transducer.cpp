//
// Created by vlad_ on 03.03.2021.
//

#include "transducer.hpp"
#include "utility.hpp"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <ranges>

finiteStateTransducer::finiteStateTransducer() {
    isNondeterministic = false;
    hasLambdaTransitions = false;

    size_t nrStates;

    std::cout << "Number of states: ";
    std::cin >> nrStates;
    std::cout << "States: ";

    for (size_t i = 0; i < nrStates; ++i) {
        std::string state;
        std::cin >> state;
        q.insert(state);
    }

    bool validInitState;

    do {
        std::cout << "Initial state: ";
        validInitState = true;
        std::cin >> q0;

        if (!q.contains(q0)) {
            validInitState = false;
            std::cout << "Invalid initial state!\n";
        }
    } while (!validInitState);

    size_t finStateCount;

    std::cout << "Number of final states: ";
    std::cin >> finStateCount;
    std::cout << "Final states: ";

    for (size_t i = 0; i < finStateCount; ++i) {
        std::string finState;
        bool validFinState;

        do {
            validFinState = true;
            std::cin >> finState;

            if (!q.contains(finState)) {
                validFinState = false;
                std::cout << "Invalid final state! <" << finState << ">\n";
            }
        } while (!validFinState);

        f.insert(finState);
    }

    size_t inputAlphaDim;

    std::cout << "Input alphabet dimension: ";
    std::cin >> inputAlphaDim;
    std::cout << "Letters: ";

    for (size_t i = 0; i < inputAlphaDim; ++i) {
        char letter;
        std::cin >> letter;
        vIn.insert(letter);
    }

    size_t outputAlphaDim;

    std::cout << "Output alphabet dimension: ";
    std::cin >> outputAlphaDim;
    std::cout << "Letters: ";

    for (size_t i = 0; i < outputAlphaDim; ++i) {
        char letter;
        std::cin >> letter;
        vOut.insert(letter);
    }

    size_t transCount;

    std::cout << "Transition count: ";
    std::cin >> transCount;
    std::cout << "Transitions:  (format: currentState letter nextState translation; For lambda use '~'!)\n";

    for (size_t i = 0; i < transCount; ++i) {
        std::string currentState, nextState, translation;
        char letter;
        bool validTransition;

        do {
            validTransition = true;
            std::cin >> currentState >> letter >> nextState >> translation;

            if (
                    !q.contains(currentState)
                    || !q.contains(nextState)
                    || (letter != '~' && !vIn.contains(letter))
                    || (translation != "~" && std::any_of(translation.begin(),
                                                          translation.end(),
                                                          [this](char letter) { return !vOut.contains(letter); })
                    )) {
                validTransition = false;
                std::cout << "Invalid transition!\n";
            }
        } while (!validTransition);

        if (letter == '~') {
            hasLambdaTransitions = true;
        }

        if (!d.contains({currentState, letter})) {
            d[{currentState, letter}] = {{nextState, {translation}}};
        } else {
            isNondeterministic = true;

            if (!d[{currentState, letter}].contains(nextState)) {
                d[{currentState, letter}][nextState] = {translation};
            } else {
                d[{currentState, letter}][nextState].insert(translation);
            }
        }
    }
    std::cout << "The machine was generated!\n";
}

finiteStateTransducer::finiteStateTransducer(std::ifstream &buffer) {
    isNondeterministic = false;
    hasLambdaTransitions = false;

    size_t nrStates;

    buffer >> nrStates;

    for (size_t i = 0; i < nrStates; ++i) {
        std::string state;
        buffer >> state;
        q.insert(state);
    }

    buffer >> q0;

    size_t finStateCount;

    buffer >> finStateCount;

    for (size_t i = 0; i < finStateCount; ++i) {
        std::string finState;

        buffer >> finState;

        f.insert(finState);
    }

    size_t inputAlphaDim;

    buffer >> inputAlphaDim;

    for (size_t i = 0; i < inputAlphaDim; ++i) {
        char letter;
        buffer >> letter;
        vIn.insert(letter);
    }

    size_t outputAlphaDim;

    buffer >> outputAlphaDim;

    for (size_t i = 0; i < outputAlphaDim; ++i) {
        char letter;
        buffer >> letter;
        vOut.insert(letter);
    }

    size_t transCount;

    buffer >> transCount;

    for (size_t i = 0; i < transCount; ++i) {
        std::string currentState, nextState, translation;
        char letter;

        buffer >> currentState >> letter >> nextState >> translation;

        if (letter == '~') {
            hasLambdaTransitions = true;
        }

        if (!d.contains({currentState, letter})) {
            d[{currentState, letter}] = {{nextState, {translation}}};
        } else {
            isNondeterministic = true;

            if (!d[{currentState, letter}].contains(nextState)) {
                d[{currentState, letter}][nextState] = {translation};
            } else {
                d[{currentState, letter}][nextState].insert(translation);
            }
        }
    }
}

fstConfigurationMap finiteStateTransducer::lambdaClosure(const fstConfiguration &initConfiguration) const {
    fstConfigurationMap closureConfigurations = {initConfiguration};
    size_t cardinal = 0;

    while (cardinal != closureConfigurations.size()) {
        cardinal = closureConfigurations.size();

        auto nextClosureConfigurations = closureConfigurations;

        for (const auto &[currentState, currentTranslations] : closureConfigurations) {
            if (d.contains({currentState, '~'})) {
                auto transitions = d.at({currentState, '~'});

                for (const auto&[nextState, nextTranslations] : transitions) {
                    if (nextClosureConfigurations.contains(nextState)) {
                        continue;
                    }

                    nextClosureConfigurations[nextState] = {};

                    std::set<std::string> joinedTranslations;

                    for (const auto &currentTranslation : currentTranslations) {
                        for (const auto &nextTranslation:nextTranslations) {
                            if (nextTranslation == "~") {
                                joinedTranslations.insert(currentTranslation);
                            } else {
                                joinedTranslations.insert(currentTranslation + nextTranslation);
                            }
                        }
                    }

                    nextClosureConfigurations[nextState].insert(joinedTranslations.begin(), joinedTranslations.end());
                }
            }
        }
        closureConfigurations = nextClosureConfigurations;
    }
    return closureConfigurations;
}

std::set<std::string> finiteStateTransducer::translate(const std::string &word) const {
    std::set<std::string> translations;

    if (word.size() == 1 && word.front() == '~' && f.contains(q0)) {
        translations.insert("");
    }

    fstConfigurationMap currentConfigurations;

    if (hasLambdaTransitions) {
        currentConfigurations = lambdaClosure({q0, {""}});
    } else {
        currentConfigurations.insert({q0, {""}});
    }

    for (const auto letter : word) {
        std::map<std::string, std::set<std::string>> nextConfigurations;

        for (const auto &[currentState, currentTranslations] : currentConfigurations) {
            if (d.contains({currentState, letter})) {
                auto transitions = d.at({currentState, letter});

                for (const auto&[nextState, nextTranslations]: transitions) {
                    std::set<std::string> joinedTranslations;

                    for (const auto &currentTranslation : currentTranslations) {
                        for (const auto &nextTranslation:nextTranslations) {
                            if (nextTranslation == "~") {
                                joinedTranslations.insert(currentTranslation);
                            } else {
                                joinedTranslations.insert(currentTranslation + nextTranslation);
                            }
                        }
                    }

                    nextConfigurations[nextState].insert(joinedTranslations.begin(), joinedTranslations.end());
                }
            }
        }
        currentConfigurations = nextConfigurations;

        if (hasLambdaTransitions) {
            auto currentClosure = currentConfigurations;

            for (const auto &configuration : currentClosure) {
                auto closure = lambdaClosure(configuration);

                currentConfigurations.insert(closure.begin(), closure.end());
            }
        }
    }

    for (const auto &[state, resultedTranslations] : currentConfigurations) {
        if (f.contains(state)) {
            translations.insert(resultedTranslations.begin(), resultedTranslations.end());
        }

    }
    return translations;
}

void finiteStateTransducer::printConfiguration() const {
    std::cout << "-----\nMachine configuration:\n\n";

    if (isNondeterministic) {
        std::cout << "The machine is nondeterministic!\n";
    }

    if (hasLambdaTransitions) {
        std::cout << "The machine contains lambda-transitions!\n";
    }

    std::cout << "Number of states: " << q.size() << "\n";
    std::cout << "States: ";

    for (const auto &state : q)
        std::cout << state << " ";

    std::cout << "Initial state: " << q0 << "\n";

    std::cout << "Number of final states: " << f.size() << "\n";
    std::cout << "Final states: ";

    for (const auto &state : f)
        std::cout << state << " ";

    std::cout << "\nInput alphabet dimension: " << vIn.size() << "\n";
    std::cout << "Letters: ";

    for (const auto letter : vIn)
        std::cout << letter << " ";

    std::cout << "\nOutput alphabet dimension: " << vOut.size() << "\n";
    std::cout << "Letters: ";

    for (const auto letter : vOut)
        std::cout << letter << " ";

    std::cout << "Transition count: " << d.size() << "\n";
    std::cout << "Transitions:\n";

    for (const auto &transition : d) {
        const auto&[initState, letter] = transition.first;
        const auto &configurations = transition.second;

        if (!isNondeterministic) {
            for (const auto &[finState, translations] : configurations) {
                for (const auto &translation : translations) {
                    std::cout << "d(" << initState << ',' << letter << ")=(" << finState << ',' << translation << ")\n";
                }
            }
        } else {
            std::cout << "d(" << initState << ',' << letter << ")={";

            for (const auto &[finState, translations] : configurations) {
                for (const auto &translation : translations) {
                    std::cout << "(" << finState << ',' << translation << ")";

                    if (++translations.find(translation) != translations.end()) {
                        std::cout << ',';
                    }
                }

                if (++configurations.find(finState) != configurations.end())
                    std::cout << ',';
            }
            std::cout << "}\n";
        }
    }
}

stackTransducer::stackTransducer() {
    isNondeterministic = false;
    hasLambdaTransitions = false;

    size_t nrStates;

    std::cout << "Number of states: ";
    std::cin >> nrStates;
    std::cout << "States: ";

    for (size_t i = 0; i < nrStates; ++i) {
        std::string state;
        std::cin >> state;
        q.insert(state);
    }


    bool validInitState;

    do {
        std::cout << "Initial state: ";
        validInitState = true;
        std::cin >> q0;

        if (!q.contains(q0)) {
            validInitState = false;
            std::cout << "Invalid initial state!\n";
        }
    } while (!validInitState);

    size_t finStateCount;

    std::cout << "Number of final states: ";
    std::cin >> finStateCount;
    std::cout << "Final states: ";

    for (size_t i = 0; i < finStateCount; ++i) {
        std::string finState;
        bool validFinState;

        do {
            validFinState = true;
            std::cin >> finState;

            if (!q.contains(finState)) {
                validFinState = false;
                std::cout << "Invalid final state! <" << finState << ">\n";
            }
        } while (!validFinState);

        f.insert(finState);
    }

    size_t inputAlphaDim;

    std::cout << "Input alphabet dimension: ";
    std::cin >> inputAlphaDim;
    std::cout << "Letters: ";

    for (size_t i = 0; i < inputAlphaDim; ++i) {
        char letter;
        std::cin >> letter;
        vIn.insert(letter);
    }

    size_t outputAlphaDim;

    std::cout << "Output alphabet dimension: ";
    std::cin >> outputAlphaDim;
    std::cout << "Letters: ";

    for (size_t i = 0; i < outputAlphaDim; ++i) {
        char letter;
        std::cin >> letter;
        vOut.insert(letter);
    }

    size_t stackAlphaDim;

    std::cout << "Stack alphabet dimension: ";
    std::cin >> stackAlphaDim;
    std::cout << "Symbols: ";

    for (size_t i = 0; i < stackAlphaDim; ++i) {
        char letter;
        std::cin >> letter;
        vOut.insert(letter);
    }

    bool validInitStackSymbol;
    do {
        std::cout << "Initial stack symbol: ";
        validInitStackSymbol = true;
        std::cin >> z0;

        if (!g.contains(z0)) {
            validInitStackSymbol = false;
            std::cout << "Invalid initial stack symbol!\n";
        }
    } while (!validInitStackSymbol);

    size_t transCount;

    std::cout << "Transition count: ";
    std::cin >> transCount;
    std::cout << "Transitions:  (format: currentState letter stackTop nextState stackPush translation; For lambda use '~'!)\n";

    for (size_t i = 0; i < transCount; ++i) {
        std::string currentState, nextState, stackPush, translation;
        char letter, stackTop;
        bool validTransition;

        do {
            validTransition = true;
            std::cin >> currentState >> letter >> stackTop >> nextState >> stackPush >> translation;

            if (
                    !q.contains(currentState)
                    || !q.contains(nextState)
                    || (letter != '~' && !vIn.contains(letter))
                    || !g.contains(stackTop)
                    || (stackPush != "~" && std::any_of(stackPush.begin(),
                                                        stackPush.end(),
                                                        [this](char symbol) { return !g.contains(symbol); }))
                    || (translation != "~" && std::any_of(translation.begin(),
                                                          translation.end(),
                                                          [this](char letter) { return !vOut.contains(letter); })
                    )) {
                validTransition = false;
                std::cout << "Invalid transition!\n";
            }
        } while (!validTransition);

        if (letter == '~') {
            hasLambdaTransitions = true;
        }

        if (!d.contains({currentState, letter, stackTop})) {
            d[{currentState, letter, stackTop}] = {{nextState, {{stackPush, translation}}}};
        } else {
            isNondeterministic = true;

            if (!d[{currentState, letter, stackTop}].contains(nextState)) {
                d[{currentState, letter, stackTop}][nextState] = {{stackPush, translation}};
            } else {
                d[{currentState, letter, stackTop}][nextState].insert({stackPush, translation});
            }
        }
    }
    std::cout << "The machine was generated!\n";
}

stackTransducer::stackTransducer(std::ifstream &buffer) {
    isNondeterministic = false;
    hasLambdaTransitions = false;

    size_t nrStates;

    buffer >> nrStates;

    for (size_t i = 0; i < nrStates; ++i) {
        std::string state;
        buffer >> state;
        q.insert(state);
    }

    buffer >> q0;

    size_t finStateCount;

    buffer >> finStateCount;

    for (size_t i = 0; i < finStateCount; ++i) {
        std::string finState;
        buffer >> finState;
        f.insert(finState);
    }

    size_t inputAlphaDim;

    buffer >> inputAlphaDim;

    for (size_t i = 0; i < inputAlphaDim; ++i) {
        char letter;
        buffer >> letter;
        vIn.insert(letter);
    }

    size_t outputAlphaDim;

    buffer >> outputAlphaDim;

    for (size_t i = 0; i < outputAlphaDim; ++i) {
        char letter;
        buffer >> letter;
        vOut.insert(letter);
    }

    size_t stackAlphaDim;

    buffer >> stackAlphaDim;

    for (size_t i = 0; i < stackAlphaDim; ++i) {
        char symbol;
        buffer >> symbol;
        vOut.insert(symbol);
    }

    buffer >> z0;

    size_t transCount;

    buffer >> transCount;

    for (size_t i = 0; i < transCount; ++i) {
        std::string currentState, nextState, stackPush, translation;
        char letter, stackTop;

        buffer >> currentState >> letter >> stackTop >> nextState >> stackPush >> translation;

        if (letter == '~') {
            hasLambdaTransitions = true;
        }

        if (!d.contains({currentState, letter, stackTop})) {
            d[{currentState, letter, stackTop}] = {{nextState, {{stackPush, translation}}}};
        } else {
            isNondeterministic = true;

            if (!d[{currentState, letter, stackTop}].contains(nextState)) {
                d[{currentState, letter, stackTop}][nextState] = {{stackPush, translation}};
            } else {
                d[{currentState, letter, stackTop}][nextState].insert({stackPush, translation});
            }
        }
    }
}

sttConfigurationMap stackTransducer::lambdaClosure(const sttConfiguration &initConfiguration) const {
    sttConfigurationMap closureConfigurations = {initConfiguration};

    bool foundNext = true;

    while (foundNext) {
        auto nextClosureConfigurations = closureConfigurations;

        for (const auto &[currentState, outputs] : closureConfigurations) {
            for (const auto &[currentStack, currentTranslation] : outputs) {
                if (currentStack.empty()) {
                    continue;
                }

                if (d.contains({currentState, '~', currentStack.top()})) {
                    auto transitions = d.at({currentState, '~', currentStack.top()});

                    for (const auto&[nextState, modifications] : transitions) {
                        std::set<std::pair<std::stack<char>, std::string>> joinedOutputs;

                        for (const auto &[stackPush, nextTranslation]:modifications) {
                            std::stack<char> nextStack = currentStack;

                            if (stackPush == std::string(1, nextStack.top()) && nextClosureConfigurations.contains(nextState) &&
                                !f.contains(nextState)) {
                                continue;
                            }

                            nextStack.pop();

                            if (stackPush != "~") {
                                for (const auto &symbol : stackPush | std::views::reverse) {
                                    nextStack.push(symbol);
                                }
                            }

                            if (nextTranslation == "~") {
                                joinedOutputs.insert({nextStack, currentTranslation});
                            } else {
                                joinedOutputs.insert({nextStack, currentTranslation + nextTranslation});
                            }
                        }
                        if (!joinedOutputs.empty()) {
                            nextClosureConfigurations[nextState].insert(joinedOutputs.begin(), joinedOutputs.end());
                        }
                    }
                }
            }
        }
        foundNext = (nextClosureConfigurations != closureConfigurations);
        closureConfigurations = nextClosureConfigurations;
    };
    return closureConfigurations;
}

std::set<std::string> stackTransducer::translate(const std::string &word) const {
    std::set<std::string> translations;

    if (word.size() == 1 && word.front() == '~' && f.contains(q0)) {
        translations.insert("");
    }

    sttConfigurationMap currentConfigurations;

    std::stack<char> auxStack;
    auxStack.push(z0);

    if (hasLambdaTransitions) {
        currentConfigurations = lambdaClosure({q0, {{auxStack, ""}}});
    } else {
        currentConfigurations.insert({q0, {{auxStack, ""}}});
    }

    for (const auto letter : word) {
        sttConfigurationMap nextConfigurations;

        for (const auto &[currentState, outputs] : currentConfigurations) {
            for (const auto &[currentStack, currentTranslation] : outputs) {
                if (currentStack.empty()) {
                    continue;
                }

                if (d.contains({currentState, letter, currentStack.top()})) {
                    auto transition = d.at({currentState, letter, currentStack.top()});

                    for (const auto &[nextState, modifications] : transition) {
                        std::set<std::pair<std::stack<char>, std::string>> joinedOutputs;

                        for (const auto &[stackPush, nextTranslation]:modifications) {
                            std::stack<char> nextStack = currentStack;
                            nextStack.pop();

                            if (stackPush != "~") {
                                for (const auto &symbol : stackPush | std::views::reverse) {
                                    nextStack.push(symbol);
                                }
                            }

                            if (nextTranslation == "~") {
                                joinedOutputs.insert({nextStack, currentTranslation});
                            } else {
                                joinedOutputs.insert({nextStack, currentTranslation + nextTranslation});
                            }
                        }
                        nextConfigurations[nextState].insert(joinedOutputs.begin(), joinedOutputs.end());
                    }
                }
            }
        }
        currentConfigurations = nextConfigurations;

        if (hasLambdaTransitions) {
            auto currentClosureConfigurations = currentConfigurations;

            for (const auto &closureConfiguration : currentClosureConfigurations) {
                auto nextClosureConfigurations = lambdaClosure(closureConfiguration);

                for (const auto &[nextState, outputs] : nextClosureConfigurations) {
                    if (!currentConfigurations.contains(nextState)) {
                        currentConfigurations[nextState] = {};
                    }
                    currentConfigurations[nextState].insert(outputs.begin(), outputs.end());
                }
            }
        }
    }
    for (const auto &[state, outputs] : currentConfigurations) {
        if (f.contains(state)) {
            for (const auto&[stack, translation] : outputs) {
                translations.insert(translation);
            }
        }
    }
    return translations;
}

void stackTransducer::printConfiguration() const {
    std::cout << "-----\nMachine configuration:\n\n";

    if (isNondeterministic) {
        std::cout << "The machine is nondeterministic!\n";
    }

    if (hasLambdaTransitions) {
        std::cout << "The machine contains lambda-transitions!\n";
    }

    std::cout << "Number of states: " << q.size() << "\n";
    std::cout << "States: ";

    for (const auto &state : q)
        std::cout << state << " ";

    std::cout << "Initial state: " << q0 << "\n";

    std::cout << "Number of final states: " << f.size() << "\n";
    std::cout << "Final states: ";

    for (const auto &state : f)
        std::cout << state << " ";

    std::cout << "\nInput alphabet dimension: " << vIn.size() << "\n";
    std::cout << "Letters: ";

    for (const auto letter : vIn)
        std::cout << letter << " ";

    std::cout << "\nOutput alphabet dimension: " << vOut.size() << "\n";
    std::cout << "Letters: ";

    for (const auto letter : vOut)
        std::cout << letter << " ";

    std::cout << "\nStack alphabet dimension: " << vOut.size() << "\n";
    std::cout << "Symbols: ";

    for (const auto symbol : g)
        std::cout << symbol << " ";

    std::cout << "Initial stack symbol: " << z0 << "\n";

    std::cout << "Transition count: " << d.size() << "\n";
    std::cout << "Transitions:\n";

    for (const auto &transition : d) {
        const auto&[initState, letter, stackTop] = transition.first;
        const auto &configurations = transition.second;

        if (!isNondeterministic) {
            for (const auto &[finState, outputs] : configurations) {
                for (const auto &[translation, stackPush] : outputs) {
                    std::cout << "d(" << initState << ',' << letter << ',' << stackTop << ")=(" << finState << ',' << translation << ',' << stackPush
                              << ")\n";
                }
            }
        } else {
            std::cout << "d(" << initState << ',' << letter << ',' << stackTop << ")={";

            for (const auto &[finState, outputs] : configurations) {
                for (const auto &output : outputs) {
                    const auto&[translation, stackPush] = output;

                    std::cout << "(" << finState << ',' << translation << ',' << stackPush << ")";

                    if (++outputs.find(output) != outputs.end()) {
                        std::cout << ',';
                    }
                }

                if (++configurations.find(finState) != configurations.end())
                    std::cout << ',';
            }
            std::cout << "}\n";
        }
    }
}