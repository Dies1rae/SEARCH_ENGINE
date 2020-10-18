#pragma once
#include "document.h"

#include <string>
#include <vector>
#include <algorithm>
#include <cmath>
#include <map>
#include <set>
#include <utility>
#include <vector>
#include <sstream>
#include <tuple>
#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <optional>
#include <stack>
#include <deque>


class SearchServer {
public:
    inline static constexpr int INVALID_DOCUMENT_ID = -1;
    inline static constexpr int MAX_RESULT_DOCUMENT_COUNT = 5;
    inline static constexpr double EPSILON = 1e-6;

    explicit SearchServer();
    template <typename TStringContainer>
    explicit SearchServer(const TStringContainer& stop_words) : stop_words_(UniqueWordsForStop(stop_words)) {
        for (const auto& word : this->UniqueWordsForStop(stop_words)) {
            if (!IsValidWord(word)) {
                throw std::invalid_argument("Œ¯Ë·Í‡");
            }
        }
    }
    explicit SearchServer(const std::string& stop_words_text);
    template<typename TStrContainer>
    std::set<std::string> UniqueWordsForStop(const TStrContainer& texts) const {
        std::set<std::string> non_empty_strings;
        for (const std::string& str : texts) {
            if (!str.empty()) {
                non_empty_strings.insert(str);
            }
        }
        return non_empty_strings;
    }
    std::vector<std::string> SplitIntoWords(const std::string& text) const;
    void SetStopWords(const std::string& text);
    void AddDocument(int document_id, const std::string& document, DocumentStatus status, const std::vector<int>& rait);
    template <typename TDocumentPredicate>
    std::vector<Document> FindTopDocuments(const std::string& raw_query, const TDocumentPredicate key_mapper) const {
        try {
            const Query query = ParseQuery(raw_query);
            std::vector<Document> result;
            std::vector<Document> matched_documents = FindAllDocuments(query);
            for (const auto& doc_ : matched_documents) {
                if (key_mapper(doc_.id, static_cast<DocumentStatus>(this->INFO_.Status[doc_.id]), doc_.rating)) {
                    result.push_back(doc_);
                }
            }
            std::sort(result.begin(), result.end(), [](const Document& lhs, const Document& rhs) {
                return lhs.relevance > rhs.relevance || (abs(lhs.relevance - rhs.relevance) < EPSILON && lhs.rating > rhs.rating);
                });
            if (result.size() > MAX_RESULT_DOCUMENT_COUNT) {
                result.resize(MAX_RESULT_DOCUMENT_COUNT);
            }
            return result;
        }
        catch (const std::exception& e) {
            throw std::invalid_argument(e.what());
        }
    }
    std::vector<Document> FindTopDocuments(const std::string& raw_query, DocumentStatus status = DocumentStatus::ACTUAL) const;
    int GetDocumentCount()const;
    int GetMAXIDCount()const;
    int GetDocumentId(int index) const;
    std::tuple<std::vector<std::string>, DocumentStatus> MatchDocument(const std::string& raw_query, int document_id) const;

private:
    struct Info_Data {
        std::vector<DocumentStatus>Status;
        std::vector<int> av_raits;
    };

    struct QueryWord {
        std::string data;
        bool is_minus;
        bool is_stop;
    };

    struct Query {
        std::set<std::string> plus_words;
        std::set<std::string> minus_words;
    };

    int hightestID_ = 0;
    int document_count_ = 0;
    std::vector<int> document_ids_;
    std::set<std::string> stop_words_;
    std::map<std::string, std::map<int, double>> word_to_document_freqs_;
    Info_Data INFO_;

    bool IsStopWord(const std::string& word) const {
        return stop_words_.count(word) > 0;
    }

    std::vector<std::string> SplitIntoWordsNoStop(const std::string& text) const {
        std::vector<std::string> words;
        for (const std::string& word : SearchServer::SplitIntoWords(text)) {
            if (!IsStopWord(word)) {
                words.push_back(word);
            }
        }
        return words;
    }

    QueryWord ParseQueryWord(std::string text) const {
        bool is_minus = false;
        if (text[0] == '-') {
            is_minus = true;
            text = text.substr(1);
        }
        return{ text, is_minus, IsStopWord(text) };
    }

    Query ParseQuery(const std::string& text) const {
        Query query;
        for (const std::string& word : SplitIntoWords(text)) {
            try {
                const QueryWord query_word = ParseQueryWord(word);
                if (!query_word.is_stop) {
                    if (query_word.is_minus) {
                        if (query_word.data[0] == '-' || query_word.data.size() == 0) {
                            throw std::invalid_argument("Œ¯Ë·Í‡!");
                        }
                        query.minus_words.insert(query_word.data);
                    }
                    else {
                        query.plus_words.insert(query_word.data);
                    }
                }
            }
            catch (const std::exception& e) {
                throw std::invalid_argument(e.what());
            }
        }
        return query;
    }

    double ComputeWordInverseDocumentFreq(const std::string& word) const {
        return log(static_cast<double>(document_count_) / static_cast<double>(word_to_document_freqs_.at(word).size()));
    }

    std::vector<Document> FindAllDocuments(const Query& query) const {
        std::map<int, double> document_to_relevance;
        for (const std::string& word : query.plus_words) {
            if (word_to_document_freqs_.count(word) == 0) {
                continue;
            }
            const double inverse_document_freq = ComputeWordInverseDocumentFreq(word);
            for (const auto [document_id, term_freq] : word_to_document_freqs_.at(word)) {
                document_to_relevance[document_id] += term_freq * inverse_document_freq;
            }
        }
        for (const std::string& word : query.minus_words) {
            if (word_to_document_freqs_.count(word) == 0) {
                continue;
            }
            for (const auto [document_id, _] : word_to_document_freqs_.at(word)) {
                document_to_relevance.erase(document_id);
            }
        }
        std::vector<Document> matched_documents;
        for (const auto [document_id, relevance] : document_to_relevance) {
            matched_documents.push_back({ document_id, relevance, this->INFO_.av_raits[document_id] });
        }
        return matched_documents;
    }

    static int ComputeAverageRating(const std::vector<int>& ratings) {
        if (ratings.empty()) {
            return 0;
        }
        int rait_size = ratings.size();
        int rait_summ = 0;
        for (const int& hits : ratings) {
            rait_summ += hits;
        }
        return rait_summ / rait_size;
    }

    static bool IsValidWord(const std::string& word) {
        return none_of(word.begin(), word.end(), [](char c) {
            return c >= '\0' && c < ' ';
            });
    }
};