#include "search_server.h"
using namespace std;

SearchServer::SearchServer() = default;

SearchServer::SearchServer(const std::string& stop_words_text): SearchServer(SplitIntoWords(stop_words_text)){}

void SearchServer::AddDocument(int document_id, const std::string& document, DocumentStatus status, const std::vector<int>& ratings) {
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

std::vector<Document> SearchServer::FindTopDocuments(const std::string& raw_query, DocumentStatus status) const {
    return FindTopDocuments(raw_query, [status](int document_id [[maybe_unused]], DocumentStatus document_status, int rating [[maybe_unused]] ) {
        return document_status == status;
        });
}

std::vector<Document> SearchServer::FindTopDocuments(const std::string& raw_query) const {
    return FindTopDocuments(raw_query, DocumentStatus::ACTUAL);
}

int SearchServer::GetDocumentCount() const {
    return this->documents_.size();
}

int SearchServer::GetDocumentId(int index) const {
    return this->document_ids_.at(index);
}

std::tuple<std::vector<std::string>, DocumentStatus> SearchServer::MatchDocument(const std::string& raw_query, int document_id) const {
    const auto query = ParseQuery(raw_query);

    std::vector<std::string> matched_words;
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


const std::map<std::string, double>& SearchServer::GetWordFrequencies(int document_id) const {
    const static map<string, double> WF;
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

void AddDocument(SearchServer& search_server, int document_id, const std::string& document, DocumentStatus status,
    const std::vector<int>& ratings) {
    try {
        search_server.AddDocument(document_id, document, status, ratings);
    } catch (const std::invalid_argument& e) {
        std::cout << "������ ���������� ��������� "s << document_id << ": "s << e.what() << std::endl;
    }
}

void FindTopDocuments(const SearchServer& search_server, const std::string& raw_query) {
    std::cout << "���������� ������ �� �������: "s << raw_query << std::endl;
    try {
        for (const Document& document : search_server.FindTopDocuments(raw_query)) {
            PrintDocument(document);
        }
    } catch (const std::invalid_argument& e) {
        std::cout << "������ ������: "s << e.what() << std::endl;
    }
}

void MatchDocuments(const SearchServer& search_server, const std::string& query) {
    try {
        std::cout << "������� ���������� �� �������: "s << query << std::endl;
        const int document_count = search_server.GetDocumentCount();
        for (int index = 0; index < document_count; ++index) {
            const int document_id = search_server.GetDocumentId(index);
            const auto [words, status] = search_server.MatchDocument(query, document_id);
            PrintMatchDocumentResult(document_id, words, status);
        }
    } catch (const std::invalid_argument& e) {
        std::cout << "������ �������� ���������� �� ������ "s << query << ": "s << e.what() << std::endl;
    }
}

void RemoveDuplicates(SearchServer& search_server) {
    std::map<int, std::vector<std::string>> doc_to_del;
    for (const int document_id : search_server) {
        for (const auto& [word, freq] : search_server.GetWordFrequencies(document_id)) {
            doc_to_del[document_id].push_back(word);
        }
    }

    for (auto it = doc_to_del.begin(); it != doc_to_del.end(); it++) {
        for (auto it2 = std::next(it, 1); it2 != doc_to_del.end(); it2++) {
            if (it->second == it2->second) {
                std::cout << "Found duplicate document id "s << it2->first << std::endl;
                search_server.RemoveDocument(it2->first);
                it2 = doc_to_del.erase(it2);
                it2--;
            }
        }
        
    }
}