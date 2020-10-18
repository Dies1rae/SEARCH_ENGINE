#pragma once

struct Document {
public:
    Document();
    Document(int ID, double REL, int RAIT);
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
