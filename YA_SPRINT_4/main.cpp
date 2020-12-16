// search_server_s4_t2_solution.cpp
#include "search_server.h"
#include "paginator.h"
#include "request_queue.h"
#include "test_example_functions.h"
#include "log_duration.h"
#include "process_queries.h"

#include <iostream>
#include <random>
#include <execution>
using namespace std;


int main() {
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

    const string query = "curly and funny"s;

    auto report = [&search_server, &query] {
        cout << search_server.GetDocumentCount() << " documents total, "s
            << search_server.FindTopDocuments(query).size() << " documents for query ["s << query << "]"s << endl;
    };

    report();
    for (auto& Docs : search_server.FindTopDocuments(query)) {
        cout << Docs.id << ' ' << Docs.relevance << ' ' << Docs.rating << endl;
    }
    // однопоточная версия
    search_server.RemoveDocument(5);
    report();
    for (auto& Docs : search_server.FindTopDocuments(query)) {
        cout << Docs.id << ' ' << Docs.relevance << ' ' << Docs.rating << endl;
    }
    // однопоточная версия
    search_server.RemoveDocument(1);
    report();
    for (auto& Docs : search_server.FindTopDocuments(query)) {
        cout << Docs.id << ' ' << Docs.relevance << ' ' << Docs.rating << endl;
    }
    // многопоточная версия
    search_server.RemoveDocument(2);
    report();
    for (auto& Docs : search_server.FindTopDocuments(query)) {
        cout << Docs.id << ' ' << Docs.relevance << ' ' << Docs.rating << endl;
    }


    return 0;
}