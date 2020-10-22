#pragma once
#include <string>
#include <sstream>
#include <iostream>
using namespace std::string_literals;

struct Document {
public:
    Document();
    Document(int id_, double relevance_, int rating_);
    int id;
    double relevance;
    int rating;
};

enum class DocumentStatus {
    ACTUAL,
    IRRELEVANT,
    BANNED,
    REMOVED,
};

std::ostream& operator<<(std::ostream& out, const Document& document);
