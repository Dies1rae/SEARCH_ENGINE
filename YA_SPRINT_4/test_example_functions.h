
#pragma once
#include "search_server.h"
#include "paginator.h"
#include "request_queue.h"
#include "string_processing.h"

#include <assert.h>
#include <algorithm>
#include <cmath>
#include <map>
#include <set>
#include <vector>
#include <tuple>
#include <cassert>
#include <cstdlib>
#include <optional>
#include <stack>
#include <deque>
#include <iostream>

using namespace std::string_literals;

template <typename TType, typename UType>
void AssertEqualImpl(const TType& t, const UType& u, const std::string& t_str, const std::string& u_str, const std::string& file,
    const std::string& func, uint32_t line, const std::string& hint) {
    if (t != u) {
        std::cout << std::boolalpha;
        std::cout << file << "(" << line << "): " << func << ": ";
        std::cout << "ASSERT_EQUAL(" << t_str << ", " << u_str << ") failed: ";
        std::cout << t << " != " << u << ".";
        if (!hint.empty()) {
            std::cout << " Hint: " << hint;
        }
        std::cout << std::endl;
        std::abort();
    }
}

template <typename TType, typename TContainer>
void AssertEqualImpl(const TContainer& t, const TContainer& u, const std::string& t_str, const std::string& u_str, const std::string& file,
    const std::string& func, uint32_t line, const std::string& hint) {
    if (t.size() == u.size()) {
        for (size_t ptr = 0; ptr < t.size(); ptr++) {
            if (t[ptr != u[ptr]]) {
                std::cout << std::boolalpha;
                std::cout << file << "(" << line << "): " << func << ": ";
                std::cout << "ASSERT_EQUAL(" << t_str << ", " << u_str << ") failed: ";
                std::cout << t << " != " << u << ".";
                if (!hint.empty()) {
                    std::cout << " Hint: " << hint;
                }
                std::cout << std::endl;
                std::abort();
            }
        }
    } else {
        std::cout << std::boolalpha;
        std::cout << file << "(" << line << "): " << func << ": ";
        std::cout << "ASSERT_EQUAL(" << t_str << ", " << u_str << ") failed: ";
        std::cout << t << " != " << u << ".";
        if (!hint.empty()) {
            std::cout << " Hint: " << hint;
        }
        std::cout << std::endl;
        std::abort();
    }
}

#define ASSERT_EQUAL(a, b) AssertEqualImpl((a), (b), #a, #b, __FILE__, __FUNCTION__, __LINE__, "")
#define ASSERT_EQUAL_HINT(a, b, hint) AssertEqualImpl((a), (b), #a, #b, __FILE__, __FUNCTION__, __LINE__, (hint))

void AssertImpl(bool value, const std::string& expr_str, const std::string& file, const std::string& func, uint32_t line, const std::string& hint);

#define ASSERT(expr) AssertImpl(!!(expr), #expr, __FILE__, __FUNCTION__, __LINE__, "")
#define ASSERT_HINT(expr, hint) AssertImpl(!!(expr), #expr, __FILE__, __FUNCTION__, __LINE__, (hint))




void TestExcludeStopWordsFromAddedDocumentContent();
void TestAddDocuments();
void Stop_words_doc();
void Minus_doc();
void Match_doc();
void Raiting_doc();
void Status_doc();
void Relev_sort_doc();
void Predicat_doc();
void Relev_calculate_doc();
void Request_queue_test();
void Paginator_test();
//------------

void TestSearchServer();
