#pragma once
#include <vector>

template <typename TIterator>
class IteratorRange {
public:
    TIterator begin_;
    TIterator end_;
    size_t size_;

    IteratorRange(TIterator begin, TIterator end, size_t size) :begin_(begin), end_(end), size_(size) {};

    auto begin() const {
        return this->begin_;
    }

    auto end() const {
        return this->end_;
    }

    size_t size() {
        return this->size_;
    }
};

template <typename TIterator>
class Paginator {
private:
    std::vector<IteratorRange<TIterator>> pages_;
public:
    explicit Paginator(TIterator begin, TIterator end, size_t page_size) {
        int base_container_size = end - begin;
        while (begin != end) {
            if (base_container_size - static_cast<int>(page_size) < 0) {
                while (base_container_size - static_cast<int>(page_size) < 0) {
                    page_size--;
                }
            }
            else {
                base_container_size -= page_size;
            }
            IteratorRange TMP_range(begin, end, page_size);
            this->pages_.push_back(TMP_range);
            begin += page_size;

        }
    }

    auto begin() const {
        return this->pages_.begin();
    }

    auto end() const {
        return this->pages_.end();
    }

};

template <typename TContainer>
auto Paginate(const TContainer& c, size_t page_size) {
    return Paginator(begin(c), end(c), page_size);
}