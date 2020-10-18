#pragma once
#include <sstream>
#include <iostream>
#include <map>
#include <set>
#include <vector>
#include <iterator>
#include <string>
#include <optional>

#include"document.h"
#include "search_server.h"
#include "paginator.h"

void PrintDocument(const Document& document);

void PrintMatchDocumentResult(int document_id, const std::vector<std::string>& words, DocumentStatus status);

void AddDocument(SearchServer& search_server, int document_id, const std::string& document, DocumentStatus status,
    const std::vector<int>& ratings);
void FindTopDocuments(const SearchServer& search_server, const std::string& raw_query);

void MatchDocuments(const SearchServer& search_server, const std::string& query);

std::ostream& operator<<(std::ostream& out, const Document& document);

template<typename TIterator>
std::ostream& operator<<(std::ostream& out, const IteratorRange<TIterator>& iter) {
    for (auto it = iter.begin_; it < iter.begin_ + iter.size_; it++) {
        out << *it;
    }
    return out;
}


template<typename TKey, typename TValue>
std::ostream& operator<<(std::ostream& out, const std::pair<TKey, TValue>& base_) {
    out << base_.first << ": " << base_.second;
    return out;
}

template<typename TContainer>
std::ostream& Print(std::ostream& out, const TContainer& base_) {
    for (auto it = begin(base_); it != end(base_); ++it) {
        if (it == begin(base_)) {
            out << *it;
        }
        else {
            out << ", " << *it;
        }
    }
    return out;
}

template<typename TKey, typename TValue>
std::ostream& operator<<(std::ostream& out, const std::map<TKey, TValue>& base_) {
    out << '{';
    Print(out, base_);
    out << '}';
    return out;
}

template<typename TType>
std::ostream& operator<<(std::ostream& out, const std::set<TType>& base_) {
    out << '{';
    Print(out, base_);
    out << '}';
    return out;
}

template<typename TType>
std::ostream& operator<<(std::ostream& out, const std::vector<TType>& base_) {
    out << '[';
    Print(out, base_);
    out << ']';
    return out;
}