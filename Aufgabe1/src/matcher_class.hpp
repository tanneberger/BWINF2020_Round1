//
// Created by revol-xut on 17.09.20.
//

#ifndef BWINF2020_MATCHER_CLASS_HPP
#define BWINF2020_MATCHER_CLASS_HPP

#include <vector>
#include <string>

enum result{
    success,
    failure
};

class MatcherClass {
public:
    MatcherClass(const std::vector<std::string>& text, const std::vector<std::string>& words);
    ~MatcherClass();

    auto match() -> result;
    void recursive( std::vector<std::vector<std::string>> possible_words, std::vector<std::string> result);

    void add_special_symbols(const std::string& text, std::vector<std::string>& result_container);
private:

    auto contains_solution(const std::vector<std::string>& other_vector) -> bool;
    static auto pattern_match(const std::string& pattern, const std::string& word) -> bool;

    std::vector<std::string> text_;
    std::vector<std::string> words_;
    std::vector<std::vector<std::string>> corrected_text_;
};


#endif //BWINF2020_MATCHER_CLASS_HPP
