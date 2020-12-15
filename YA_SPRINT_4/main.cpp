// search_server_s4_t2_solution.cpp
#include "search_server.h"
#include "paginator.h"
#include "request_queue.h"
#include "test_example_functions.h"
#include "log_duration.h"
#include "process_queries.h"

#include <iostream>
#include <random>
using namespace std;


int main() {
    TestSearchServer();
    std::cout << "All test's are OK"s << std::endl;

    SearchServer search_server("and with"s);

    for (
        int id = 0;
        const string & text : {
        "funny pet and nasty rat"s,
            "funny pet with curly hair"s,
            "funny pet and not very nasty rat"s,
            "pet with rat and rat and rat"s,
            "nasty rat with curly hair"s,
    }
    ) {
        search_server.AddDocument(++id, text, DocumentStatus::ACTUAL, { 1, 2 });
    }

    const vector<string> queries = {
        "nasty rat -not"s,
        "not very funny nasty pet"s,
        "curly hair"s
    };
    for (
        int id = 0;
        const auto & documents : ProcessQueries(search_server, queries)
        ) {
        LOG_DURATION("TEST"s, std::cerr);
        LOG_DURATION("TEST"s, std::cout);
        LOG_DURATION("TEST"s);
        cout << documents.size() << " documents for query ["s << queries[id++] << "]"s << endl;
    }

    return 0;
}