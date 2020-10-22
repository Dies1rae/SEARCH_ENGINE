#pragma once
#include <vector>
#include "search_server.h"
#include "document.h"
#include <algorithm>
#include <string>
#include <deque>

class RequestQueue {
private:
    struct QueryResult {
    public:
        QueryResult();
        std::vector<Document> Searched_;
        int sec_;
    };
    const SearchServer& search_server_;
    std::deque<QueryResult> requests_;
    inline static constexpr int sec_in_day_ = 1440;
    int sec_now_;
    int zero_query;
    int fill_query;

public:
    explicit RequestQueue(const SearchServer& search_server);

    template <typename TDocumentPredicate>
    std::vector<Document> AddFindRequest(const std::string& raw_query, TDocumentPredicate document_predicate) {
        if (this->sec_now_ < this->sec_in_day_) {
            this->sec_now_++;
        } else {
            this->sec_now_ = 0;
        }
        QueryResult query_res;
        query_res.Searched_ = this->search_server_.FindTopDocuments(raw_query, document_predicate);
        query_res.sec_ = this->sec_now_;

        if (this->requests_.size() == this->sec_in_day_) {
            this->requests_.pop_back();
            this->requests_.push_front(query_res);
        } else {
            this->requests_.push_back(query_res);
        }
        this->zero_query = count_if(this->requests_.begin(), this->requests_.end(), [](QueryResult& A) { return A.Searched_.size() == 0; });
        this->fill_query = count_if(this->requests_.begin(), this->requests_.end(), [](QueryResult& A) { return A.Searched_.size() > 0; });
        return query_res.Searched_;
    }

    std::vector<Document> AddFindRequest(const std::string& raw_query, DocumentStatus status = DocumentStatus::ACTUAL);

    int GetNoResultRequests() const;
    int GetResultRequests() const;
};
