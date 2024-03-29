#include "string_processing.h"
using namespace std::string_literals;

//std::vector<std::string> SplitIntoWords(const std::string& text) {
//    std::vector<std::string> words;
//    std::string word;
//    for (const char c : text) {
//        if (c == ' ') {
//            if (!word.empty()) {
//                words.push_back(word);
//                word.clear();
//            }
//        } else {
//            word += c;
//        }
//    }
//    if (!word.empty()) {
//        words.push_back(word);
//    }
//    return words;
//}

std::vector<std::string> SplitIntoWords(const std::string_view& text) {
    std::vector<std::string> words;
    std::string word;
    for (const char c : text) {
        if (c == ' ') {
            if (!word.empty()) {
                words.push_back(word);
                word.clear();
            }
        }
        else {
            word += c;
        }
    }
    if (!word.empty()) {
        words.push_back(word);
    }
    return words;
}