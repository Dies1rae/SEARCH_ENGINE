#include "document.h"

Document::Document():id(0), relevance(0.0), rating(0) {}

Document::Document(int id, double relevance, int rating): id(id), relevance(relevance), rating(rating) {}

std::ostream& operator<<(std::ostream& out, const Document& document) {
    out << "{ "s
        << "document_id = "s << document.id << ", "s
        << "relevance = "s << document.relevance << ", "s
        << "rating = "s << document.rating << " }"s;
    return out;
}