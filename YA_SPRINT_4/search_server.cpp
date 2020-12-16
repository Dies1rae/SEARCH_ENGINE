#include "search_server.h"
using namespace std;

SearchServer::SearchServer() = default;

SearchServer::SearchServer(const std::string& stop_words_text): SearchServer(SplitIntoWords(stop_words_text)){}

SearchServer::SearchServer(const std::string_view& stop_words_text): SearchServer(SplitIntoWords(stop_words_text)) {}

//void SearchServer::AddDocument(int document_id, const std::string& document, DocumentStatus status, const std::vector<int>& ratings) {
//    if ((document_id < 0) || (documents_.count(document_id) > 0)) {
//        throw std::invalid_argument("Invalid document_id"s);
//    }
//    const auto words = SplitIntoWordsNoStop(document);
//
//    const double inv_word_count = 1.0 / words.size();
//    for (const std::string& word : words) {
//        this->word_to_document_freqs_[word][document_id] += inv_word_count;
//        this->Word_Frequencies_[document_id][word] += inv_word_count;
//    }
//    this->documents_.emplace(document_id, DocumentData{ ComputeAverageRating(ratings), status });
//    this->document_ids_.push_back(document_id);
//}

void SearchServer::AddDocument(int document_id, const std::string_view& document, DocumentStatus status, const std::vector<int>& ratings) {
    if ((document_id < 0) || (documents_.count(document_id) > 0)) {
        throw std::invalid_argument("Invalid document_id"s);
    }
    const auto words = SplitIntoWordsNoStop(document);

    const double inv_word_count = 1.0 / words.size();
    for (const std::string& word : words) {
        this->word_to_document_freqs_[word][document_id] += inv_word_count;
        this->Word_Frequencies_[document_id][word] += inv_word_count;
    }
    this->documents_.emplace(document_id, DocumentData{ ComputeAverageRating(ratings), status });
    this->document_ids_.push_back(document_id);
}

//std::vector<Document> SearchServer::FindTopDocuments(const std::string& raw_query, DocumentStatus status) const {
//    return FindTopDocuments(raw_query, [status](int document_id [[maybe_unused]], DocumentStatus document_status, int rating [[maybe_unused]] ) {
//        return document_status == status;
//        });
//}
//
//std::vector<Document> SearchServer::FindTopDocuments(const std::string& raw_query) const {
//    return FindTopDocuments(raw_query, DocumentStatus::ACTUAL);
//}

std::vector<Document> SearchServer::FindTopDocuments(const std::string_view& raw_query, DocumentStatus status) const {
    return FindTopDocuments(raw_query, [status](int document_id [[maybe_unused]], DocumentStatus document_status, int rating [[maybe_unused]] ) {
        return document_status == status;
        });
}

std::vector<Document> SearchServer::FindTopDocuments(const std::string_view& raw_query) const {
    return FindTopDocuments(raw_query, DocumentStatus::ACTUAL);
}

int SearchServer::GetDocumentCount() const {
    return this->documents_.size();
}

int SearchServer::GetDocumentId(int index) const {
    return *std::find(this->document_ids_.begin(), this->document_ids_.end(), index);
}

//std::tuple<std::vector<std::string_view>, DocumentStatus> SearchServer::MatchDocument(const std::string& raw_query, int document_id) const {
//    const auto query = ParseQuery(raw_query);
//
//    std::vector<std::string_view> matched_words;
//    for (const std::string& word : query.plus_words) {
//        if (this->word_to_document_freqs_.count(word) == 0) {
//            continue;
//        }
//        if (this->word_to_document_freqs_.at(word).count(document_id)) {
//            matched_words.push_back(word);
//        }
//    }
//    for (const std::string& word : query.minus_words) {
//        if (this->word_to_document_freqs_.count(word) == 0) {
//            continue;
//        }
//        if (this->word_to_document_freqs_.at(word).count(document_id)) {
//            matched_words.clear();
//            break;
//        }
//    }
//    return { matched_words, documents_.at(document_id).status };
//}

std::tuple<std::vector<std::string_view>, DocumentStatus> SearchServer::MatchDocument(const std::string_view& raw_query, int document_id) const {
    std::string query_tmp = static_cast<string>(raw_query);
    const auto query = ParseQuery(query_tmp);

    std::vector<std::string_view> matched_words;
    for (const std::string& word : query.plus_words) {
        if (this->word_to_document_freqs_.count(word) == 0) {
            continue;
        }
        if (this->word_to_document_freqs_.at(word).count(document_id)) {
            matched_words.push_back(word);
        }
    }
    for (const std::string& word : query.minus_words) {
        if (this->word_to_document_freqs_.count(word) == 0) {
            continue;
        }
        if (this->word_to_document_freqs_.at(word).count(document_id)) {
            matched_words.clear();
            break;
        }
    }
    return { matched_words, documents_.at(document_id).status };
}

const std::map<std::string_view, double>& SearchServer::GetWordFrequencies(int document_id) const {
    const static std::map<std::string_view, double> WF;
    for (const auto& [id, word_freq] : this->Word_Frequencies_) {
        if (id == document_id) {
            return word_freq;
        }
    }

    return WF;
}

void SearchServer::RemoveDocument(int document_id) {
    this->document_ids_.erase(std::remove(this->document_ids_.begin(), this->document_ids_.end(), document_id), this->document_ids_.end());
    this->documents_.erase(document_id);
    for (auto& [word, freq] : this->GetWordFrequencies(document_id)) {
        this->word_to_document_freqs_.erase(static_cast<std::string>(word));
    }

    this->Word_Frequencies_.erase(document_id);
}

void PrintDocument(const Document& document) {
    std::cout << "{ "s
        << "document_id = "s << document.id << ", "s
        << "relevance = "s << document.relevance << ", "s
        << "rating = "s << document.rating << " }"s << std::endl;
}

void PrintMatchDocumentResult(int document_id, const std::vector<std::string>& words, DocumentStatus status) {
    std::cout << "{ "s
        << "document_id = "s << document_id << ", "s
        << "status = "s << static_cast<int>(status) << ", "s
        << "words ="s;
    for (const std::string& word : words) {
        std::cout << ' ' << word;
    }
    std::cout << "}"s << std::endl;
}

void PrintMatchDocumentResult(int document_id, const std::vector<std::string_view>& words, DocumentStatus status) {
    std::cout << "{ "s
        << "document_id = "s << document_id << ", "s
        << "status = "s << static_cast<int>(status) << ", "s
        << "words ="s;
    for (const std::string_view& word : words) {
        std::cout << ' ' << word;
    }
    std::cout << "}"s << std::endl;
}

void AddDocument(SearchServer& search_server, int document_id, const std::string& document, DocumentStatus status,
    const std::vector<int>& ratings) {
    try {
        search_server.AddDocument(document_id, document, status, ratings);
    } catch (const std::invalid_argument& e) {
        std::cout << "Ошибка добавления документа "s << document_id << ": "s << e.what() << std::endl;
    }
}

void FindTopDocuments(const SearchServer& search_server, const std::string& raw_query) {
    std::cout << "Результаты поиска по запросу: "s << raw_query << std::endl;
    try {
        for (const Document& document : search_server.FindTopDocuments(raw_query)) {
            PrintDocument(document);
        }
    } catch (const std::invalid_argument& e) {
        std::cout << "Ошибка поиска: "s << e.what() << std::endl;
    }
}

void MatchDocuments(const SearchServer& search_server, const std::string& query) {
    try {
        std::cout << "Матчинг документов по запросу: "s << query << std::endl;
        const int document_count = search_server.GetDocumentCount();
        for (int index = 0; index < document_count; ++index) {
            const int document_id = search_server.GetDocumentId(index);
            const auto [words, status] = search_server.MatchDocument(query, document_id);
            PrintMatchDocumentResult(document_id, words, status);
        }
    } catch (const std::invalid_argument& e) {
        std::cout << "Ошибка матчинга документов на запрос "s << query << ": "s << e.what() << std::endl;
    }
}
