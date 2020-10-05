//
// Created by revol-xut on 17.09.20.
//

#include <algorithm>
#include <map>
#include <unordered_set>

#include "matcher_class.hpp"

MatcherClass::MatcherClass(const std::vector<std::string> &text, const std::vector<std::string> &words) {
    text_ = text;
    words_ = words;
}

MatcherClass::~MatcherClass() = default;

auto MatcherClass::match() -> result {
    /*!
     * The idea here is that we generate list of words that would fit that position and than start with the smallest
     * list filling the string and removing the word from all list that contain the word
     */
    std::vector<std::string> result(words_.size());
    std::vector<std::vector<std::string>> possible_words;

    unsigned int iterator = 0;
    for (const auto &pattern: text_) {
        possible_words.emplace_back();
        for (const auto &word: words_) {
            if (MatcherClass::pattern_match(pattern, word)) {
                possible_words.at(iterator).push_back(word);
            }
        }
        if (possible_words.at(iterator).empty()) {
            return failure;
        }

        iterator++;
    }
    recursive(possible_words, result);

    return (corrected_text_.empty()) ? failure : success;
}

void MatcherClass::recursive(std::vector<std::vector<std::string>> possible_words, std::vector<std::string> result) {
    unsigned int index = 0;
    unsigned int min_value;

    min_value = std::numeric_limits<unsigned int>::max();
    for (unsigned int i = 0; i < possible_words.size(); i++) {
        if (min_value > possible_words.at(i).size() and not possible_words.at(i).empty()) {
            min_value = possible_words.at(i).size();
            index = i;
        }
    }
    if (possible_words.at(index).empty()) {
        if (result.size() == words_.size() and
            not contains_solution(result)) { // All Words are matching we found a result
            corrected_text_.push_back(result);
        }
        return;
    }

    for (const auto &word : possible_words.at(index)) {
        std::vector<std::vector<std::string>> possible_words_copy = possible_words;

        for (std::vector<std::string> &vec : possible_words_copy) {
            auto iter_index = std::find(vec.begin(), vec.end(), word);
            if (iter_index != vec.end()) {
                vec.erase(iter_index);
            }
        }

        std::vector<std::string> result_copy = result; // making a deep copy
        result.reserve(words_.size());
        result_copy[index] = word;

        recursive(possible_words_copy, result_copy);
    }
}

auto MatcherClass::pattern_match(const std::string &pattern, const std::string &word) -> bool {
    if (word.size() == pattern.size()) {
        for (std::string::size_type i = 0; i < pattern.size(); i++) {
            if (pattern.at(i) != word.at(i) and pattern.at(i) != *"_") {
                return false;
            }
        }
        return true;
    }
    return false;
}

void MatcherClass::add_special_symbols(const std::string &text, std::vector<std::string> &result_container) {
    std::unordered_set<std::string> unwanted_characters = {".", "!", ",", "?"};

    for (const auto &possible_result: corrected_text_) {
        std::string result_string;
        std::map<unsigned int, char> positions;

        auto iterator = 0;
        for (const auto &c : text) {
            if (unwanted_characters.contains(&c)) {
                positions[iterator] = c;
            }
            iterator++;
        }
        for (const auto &str: possible_result) {
            result_string += str + " ";
        }
        for (const auto &pair : positions) {
            result_string.insert(pair.first, &pair.second);
        }
        result_container.push_back(result_string);
    }
}

auto MatcherClass::contains_solution(const std::vector<std::string> &other_vector) -> bool {
    auto lambda = [other_vector](const std::vector<std::string> &sol) { return sol == other_vector; };
    return std::ranges::any_of(corrected_text_.cbegin(), corrected_text_.cend(), lambda);
}