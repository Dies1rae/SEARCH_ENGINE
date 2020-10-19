#include "request_queue.h"

RequestQueue::RequestQueue(const SearchServer& search_server) :search_server_(search_server) {};

std::vector<Document> RequestQueue::AddFindRequest(const std::string& raw_query, DocumentStatus status) {
    if (this->sec_now_ < this->sec_in_day_) {
        this->sec_now_++;
    } else {
        this->sec_now_ = 0;
    }
    QueryResult query_res;
    query_res.Searched_ = this->search_server_.FindTopDocuments(raw_query, status);
    query_res.sec_ = this->sec_now_;

    if (this->requests_.size() == this->sec_in_day_) {
        this->requests_.pop_back();
        this->requests_.push_front(query_res);
    } else {
        this->requests_.push_back(query_res);
    }
    this->zero_query = std::count_if(this->requests_.begin(), this->requests_.end(), [](QueryResult& A) { return A.Searched_.size() == 0; });
    return query_res.Searched_;
}

int RequestQueue::GetNoResultRequests() const {
    return this->zero_query - 1;
}