//
// Created by revol-xut on 17.09.20.
//

#include <algorithm>
#include <map>
#include <unordered_set>
#include <iostream>

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
            std::cout << "Konnte kein passendes Wort für das Lückenwort: " << pattern << " finden" << std::endl;
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

    // Find-Min-Size
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
        result_copy.reserve(words_.size());
        result_copy[index] = word;

        recursive(possible_words_copy, result_copy);
    }
}

auto MatcherClass::pattern_match(const std::string &pattern, const std::string &word) -> bool {
    auto count = []( const char* str) -> unsigned int{
        unsigned int count = 0;
        while (*str != 0)
        {
            if ((*str & 0xc0) != 0x80){
                ++count;
            }

            ++str;
        }
        return count;
    };

    auto unicode_finder = [count](const std::string& str){
        return str.size() - count(str.c_str());
    };

    auto is_unicode = [](const char data) -> bool{
        return (data & 0xc0) == 0xc0;
    };

    unsigned int off_set_word = 0, off_set_pattern = 0;
    if (word.size() + unicode_finder(pattern) == pattern.size() + unicode_finder(word)) {
        for (std::string::size_type i = 0; i < pattern.size(); i++) {
            if (pattern.at(i + off_set_pattern) != word.at(i + off_set_word) and pattern.at(i + off_set_pattern) != *"_") {
                return false;
            }

            if(is_unicode(pattern.at(i + off_set_pattern))){
                off_set_pattern++;
            }
            if(is_unicode(word.at(i + off_set_word))){
                off_set_word++;
            }

        }
        return true;
    }
    return false;
}

void MatcherClass::add_special_symbols(const std::string &text, std::vector<std::string> &result_container) {
    std::unordered_set<std::string> unwanted_characters = {".", "!", ",", "?"};

    auto is_unicode = [](const char data) -> bool{
        return (data & 0xc0) == 0xc0;
    };

    for (const auto &possible_result: corrected_text_) {
        std::string result_string;
        std::map<unsigned int, char> positions;

        for (const auto &str: possible_result) {
            result_string += str + " ";
        }

        unsigned int inserter_index = 0;
        for (std::string::size_type i = 0; i < text.size(); i++) {
            if(result_string.size() > i and is_unicode(result_string.at(i)) ) {
                inserter_index++;
            }
            if ( unwanted_characters.contains(std::string(&text.at(i), 1))) {
                result_string.insert(std::min(inserter_index, static_cast<unsigned int>(result_string.size())), std::string(&text.at(i), 1));
            }
            inserter_index++;
        }

        result_container.push_back(result_string);
    }
}

auto MatcherClass::contains_solution(const std::vector<std::string> &other_vector) -> bool {
    auto lambda = [other_vector](const std::vector<std::string> &sol) { return sol == other_vector; };
    return std::ranges::any_of(corrected_text_.cbegin(), corrected_text_.cend(), lambda);
}