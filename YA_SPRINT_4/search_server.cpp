#include "search_server.h"

SearchServer::SearchServer() = default;

SearchServer::SearchServer(const std::string& stop_words_text) : SearchServer(SplitIntoWords(stop_words_text)) {
    for (const auto& word : this->SplitIntoWords(stop_words_text)) {
        if (!IsValidWord(word)) {
            throw std::invalid_argument("Ошибка");
        }
    }
}

std::vector<std::string> SearchServer::SplitIntoWords(const std::string& text) const {
    std::vector<std::string> words;
    std::string word;
    for (const char c : text) {
        if (c == ' ') {
            words.push_back(word);
            word = "";
        }
        else {
            word += c;
        }
    }
    if (!IsValidWord(word)) {
        throw std::invalid_argument("Ошибка");
    }
    words.push_back(word);
    return words;
}

void SearchServer::SetStopWords(const std::string& text) {
    for (const std::string& word : SplitIntoWords(text)) {
        stop_words_.insert(word);
    }
}

void SearchServer::AddDocument(int document_id, const std::string& document, DocumentStatus status, const std::vector<int>& rait) {
    if (document_id < 0) {
        throw std::invalid_argument("Bad ID");
    }
    if (find(this->document_ids_.begin(), this->document_ids_.end(), document_id) != this->document_ids_.end()) {
        throw std::invalid_argument("Bad ID");
    }
    if (!IsValidWord(document)) {
        throw std::invalid_argument("Bad symbols in document");
    }
    ++this->document_count_;
    if (this->hightestID_ <= document_id) {
        this->hightestID_ = document_id + 1;
        this->INFO_.av_raits.resize(this->hightestID_);
        this->INFO_.Status.resize(this->hightestID_);
        this->INFO_.Status[document_id] = status;
        this->INFO_.av_raits[document_id] = ComputeAverageRating(rait);
    }
    else {
        this->INFO_.Status.at(document_id) = status;
        this->INFO_.av_raits.at(document_id) = ComputeAverageRating(rait);
    }
    const std::vector<std::string> words = SplitIntoWordsNoStop(document);
    const double inv_word_count = 1.0 / words.size();
    for (const std::string& word : words) {
        this->word_to_document_freqs_[word][document_id] += inv_word_count;
    }
    this->document_ids_.push_back(document_id);
    std::sort(this->document_ids_.begin(), this->document_ids_.end());
}

std::vector<Document> SearchServer::FindTopDocuments(const std::string& raw_query, DocumentStatus status) const {
    try {
        const Query query = ParseQuery(raw_query);
        std::vector<Document> result;
        std::vector<Document> matched_documents = FindAllDocuments(query);
        for (const auto& ptr : matched_documents) {
            if (this->INFO_.Status[ptr.id] == status) {
                result.push_back(ptr);
            }
        }
        sort(result.begin(), result.end(), [](const Document& lhs, const Document& rhs) {
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

int SearchServer::GetDocumentCount()const {
    return this->document_count_;
}

int SearchServer::GetMAXIDCount()const {
    return this->hightestID_;
}

int SearchServer::GetDocumentId(int index) const {
    if (index < 0 && index > GetDocumentCount()) {
        throw std::out_of_range("ID Ошибка");
    }
    try {
        this->document_ids_.at(index);
    }
    catch (const std::exception& e) {
        throw std::out_of_range("ID Ошибка");
        throw e.what();
    }
    return document_ids_[index];
}

std::tuple<std::vector<std::string>, DocumentStatus> SearchServer::MatchDocument(const std::string& raw_query, int document_id) const {
    if (document_id > this->hightestID_) {
        throw std::invalid_argument("Ошибка  ID");
    }
    //------------------
    try {
        const Query query = ParseQuery(raw_query);
        std::vector<std::string> words_plus;
        for (const std::string& ptr : query.minus_words) {
            if (word_to_document_freqs_.count(ptr) == 0) {
                continue;
            }
            else {
                for (const auto [document__id, term_freq] : this->word_to_document_freqs_.at(ptr)) {
                    if (document__id == document_id) {
                        words_plus.clear();
                        std::tuple<std::vector<std::string>, DocumentStatus> res(words_plus, this->INFO_.Status[document_id]);
                        return res;
                    }
                }
            }
        }
        for (const auto& ptr : query.plus_words) {
            if (word_to_document_freqs_.count(ptr) == 0) {
                continue;
            }
            else {
                for (const auto [document__id, term_freq] : this->word_to_document_freqs_.at(ptr)) {
                    if (document__id == document_id) {
                        words_plus.push_back(ptr);
                    }
                }
            }
        }
        std::sort(words_plus.begin(), words_plus.end(), [](const std::string& A, const std::string& B) {
            return A < B;
            });
        std::tuple<std::vector<std::string>, DocumentStatus> res(words_plus, this->INFO_.Status[document_id]);
        return res;
    }
    catch (const std::exception& e) {
        throw std::invalid_argument(e.what());
    }
}