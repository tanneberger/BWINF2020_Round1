//
// Created by revol-xut on 31.08.20.
//
#include<algorithm>
#include <iostream>
#include <sstream>
#include <unordered_set>

#include "matcher_class.hpp"

/*!
 * @brief Divides text on whitespaces
 * @param input_text user input
 * @return splited text
 */
auto split_text(const std::string &input_text) -> std::vector<std::string> {
    std::stringstream ss(input_text);
    std::istream_iterator<std::string> begin(ss);
    std::istream_iterator<std::string> end;
    std::vector<std::string> divided_text(begin, end);
    return divided_text;
}


/*!
 * @brief Removes language symbols for sentence structure like commas, dots, question marks etc.
 * @param text
 */
void preprocessing_text(std::vector<std::string> &text) {
    std::vector<unsigned int> delete_vector;
    std::unordered_set<std::string> unwanted_characters = {".", "!", ",", "?"};

    for (unsigned int i = 0; i < text.size(); i++) {
        if (unwanted_characters.contains(text.at(i))) {
            // Entire String is not wanted => Delete String
            delete_vector.push_back(i);
        } else {
            // Detecting and deleting unwanted chars from string
            for (const auto &unwanted_character : unwanted_characters)
                text.at(i).erase(std::remove(text.at(i).begin(), text.at(i).end(), *(unwanted_character.c_str())),
                                 text.at(i).end());
        }
    }

    for (auto index: delete_vector) {
        text.erase(text.begin() + index);
    }
}


int main() {
    std::cout << "Aufgabe 1 des Bundeswettbewerbs Informatik 2020" << std::endl;
    std::cout << "===============================================" << std::endl << std::endl;
    std::cout << "Bitte geb nun den Lückentext ein. Buchstaben die weggelassen wurden werden durch einen _ Markiert."
              << std::endl;
    std::string gap_text;
    getline(std::cin, gap_text);
   // gap_text = "_h __, _a_ __r ___e __b___!";

    std::cout << "Dein Eingegebener Lückentext: \"" << gap_text << "\"" << std::endl;

    std::vector<std::string> divided_text = split_text(gap_text);
    preprocessing_text(divided_text);

    std::cout << "Text nach der Vorverarbeitung: \"";
    for (const auto &str: divided_text) {
        std::cout << str << " ";
    }
    std::cout << "\" \n\n";
    std::vector<std::string> words;

    std::cout << "Bitte gebe nun alle Wörter die in diesem Text vorkommen ein Wörter durch Whitespaces voneinander trennen" << std::endl;

    std::string word_text;
    getline(std::cin, word_text);
    words = split_text(word_text);

    std::cout << "Dein Lückentext erwartet: " << divided_text.size() << " Wörter angegebene Wörter: " << words.size() << std::endl;
    std::cout << "Deine Angegebenen Wörter:" << std::endl;
    std::cout << "[";
    for (const auto &str: words) {
        std::cout << str << ",";
    }
    std::cout << "]\n" << std::endl;
    std::cout << "Starting Matching ... \n";

    MatcherClass matching_algo(divided_text, words);
    auto state = matching_algo.match();

    if (state == result::failure) {
        std::cout
                << "Bitte überprüfe deinen Input auf korrekt heit oder UTF-8 Zeichen (nur ASCII Zeichen sind zulässig)"
                << std::endl;
        return failure;
    }

    std::vector<std::string> result;
    matching_algo.add_special_symbols(gap_text, result);

    unsigned int i = 0;
    for (const auto &parsed_str : result) {
        std::cout << i << " : " << parsed_str << "\n";
        i++;
    }

    return 0;
}