//
// Created by vlad_ on 04.03.2021.
//
#include <ostream>
#include <map>
#include <set>
#include <vector>
#include <stack>

#ifndef TRANSDUCER_UTILITY_HPP
#define TRANSDUCER_UTILITY_HPP

extern size_t currentIndent;

template<class keyType, class valueType>
std::ostream &operator<<(std::ostream &, const std::map<keyType, valueType> &);

template<class valueType>
std::ostream &operator<<(std::ostream &, const std::set<valueType> &);

template<class valueType>
std::ostream &operator<<(std::ostream &, const std::vector<valueType> &);

template<class firstType, class secondType>
std::ostream &operator<<(std::ostream &, const std::pair<firstType, secondType> &);

template<class valueType>
std::ostream &operator<<(std::ostream &, const std::stack<valueType> &);

template<class keyType, class valueType>
std::ostream &operator<<(std::ostream &out, const std::map<keyType, valueType> &object) {
    std::string tabs;
    tabs.insert(tabs.cend(), currentIndent + 1, '\t');

    out << "{\n";

    ++currentIndent;

    for (const auto&[key, value]: object) {
        out << tabs << key << ": ";

        out << value;

        if (++object.find(key) != object.end()) {
            out << ',';
        }

        out << '\n';
    }
    --currentIndent;

    tabs.pop_back();

    out << tabs << '}';
    return out;
}

template<class valueType>
std::ostream &operator<<(std::ostream &out, const std::set<valueType> &object) {
    std::string tabs;
    tabs.insert(tabs.cend(), currentIndent + 1, '\t');

    out << "set{\n";

    ++currentIndent;

    for (const auto &value: object) {
        out << tabs << value;

        if (++object.find(value) != object.end()) {
            out << ',';
        }

        out << '\n';
    }
    --currentIndent;

    tabs.pop_back();

    out << tabs << '}';
    return out;
}

template<class valueType>
std::ostream &operator<<(std::ostream &out, const std::vector<valueType> &object) {
    std::string tabs;
    tabs.insert(tabs.cend(), currentIndent + 1, '\t');

    out << "vector[\n";

    ++currentIndent;

    for (const auto &value: object) {
        out << tabs << value;

        if (value != object.back()) {
            out << ',';
        }

        out << '\n';
    }
    --currentIndent;

    tabs.pop_back();

    out << tabs << ']';
    return out;
}

template<class firstType, class secondType>
std::ostream &operator<<(std::ostream &out, const std::pair<firstType, secondType> &object) {
    std::string tabs;
    tabs.insert(tabs.cend(), currentIndent + 1, '\t');

    out << "pair(\n";

    ++currentIndent;

    out << tabs << object.first << ",\n";
    out << tabs << object.second << '\n';

    --currentIndent;

    tabs.pop_back();

    out << tabs << ')';
    return out;
}

template<class valueType>
std::ostream &operator<<(std::ostream &out, const std::stack<valueType> &object) {
    std::string tabs;
    tabs.insert(tabs.cend(), currentIndent + 1, '\t');

    out << "stack[\n";

    ++currentIndent;

    auto aux_stack = object;

    while (!aux_stack.empty()) {
        out << tabs << aux_stack.top();

        if (aux_stack.size() > 1) {
            out << ',';
        }

        out << '\n';

        aux_stack.pop();
    }
    --currentIndent;

    tabs.pop_back();

    out << tabs << ']';
    return out;
}

#endif //TRANSDUCER_UTILITY_HPP
