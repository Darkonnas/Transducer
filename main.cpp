#include "transducer.hpp"
#include "utility.hpp"
#include <fstream>
#include <iostream>

int main() {

    bool invalidChoice;
    size_t choice;
    do {
        invalidChoice = false;

        std::cout << "Please choose one of the available transducers:\n";
        std::cout << "1. Finite State Transducer\n";
        std::cout << "2. Stack Transducer\n";
        std::cout << "\nYour choice:";
        std::cin >> choice;

        switch (choice) {
            case 1: {
                std::ifstream fstInput("../Configurations/FiniteTransducer2.txt");
                finiteStateTransducer fst(fstInput);
                fstInput.close();

                fst.printConfiguration();

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

                for (const auto &word : words) {
                    std::cout << word << " can be translated into: ";
                    for (const auto &translation : fst.translate(word)) {
                        std::cout << "\"" << translation << "\" ";
                    }
                    std::cout << '\n';
                }
                break;
            }
            case 2: {
                std::ifstream sttInput("../Configurations/StackTransducer2.txt");
                stackTransducer stt(sttInput);
                sttInput.close();

                stt.printConfiguration();

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

                for (const auto &word : words) {
                    std::cout << word << " can be translated into: ";
                    for (const auto &translation : stt.translate(word)) {
                        std::cout << "\"" << translation << "\" ";
                    }
                    std::cout << '\n';
                }
                break;
            }
            default: {
                std::cout << "\nInvalid choice!\n\n";
                invalidChoice = true;
            }
        }
    } while (invalidChoice);
    return 0;
}
