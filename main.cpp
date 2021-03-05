#include "transducer.hpp"
#include "utility.hpp"
#include <fstream>
#include <iostream>
#include <vector>

int main() {
    std::ifstream input("../Configurations/StackTransducer1.txt");
    stackTransducer ST(input);
    input.close();

    ST.printConfiguration();

    size_t count;

    std::cout << "\nHow many words would you like to translate?\ncount =";
    std::cin >> count;

    std::vector<std::string> words(count);

    std::cout << "Please input the words:\n";

    for (size_t i = 0; i < count; ++i) {
        std::cout << i + 1 << ".";
        std::cin >> words[i];
    }

    std::cout << "\nResult:\n";

    for (const auto& word : words) {
        std::cout << word << " can be translated into: ";
        for (const auto& translation : ST.translate(word)) {
            std::cout << "\"" << translation << "\" ";
        }
        std::cout << '\n';
    }
    return 0;
}
