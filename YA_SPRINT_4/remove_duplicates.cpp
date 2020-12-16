#include "remove_duplicates.h"

void RemoveDuplicates(SearchServer& search_server) {
    std::map<int, std::vector<std::string_view>> doc_to_del;
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