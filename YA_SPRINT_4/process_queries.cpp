#include "process_queries.h"


std::vector<std::vector<Document>> ProcessQueries(const SearchServer& search_server,
	const std::vector<std::string>& queries) {
	std::vector<std::vector<Document>> res(queries.size());

	std::transform(std::execution::par, queries.begin(), queries.end(), res.begin(), [&search_server]
	(const std::string& query) {
			return search_server.FindTopDocuments(query);
		});


	return res;
}

std::vector<Document> ProcessQueriesJoined(const SearchServer& search_server,
	const std::vector<std::string>& queries) {
	std::vector<Document> res;

	for (size_t ptr = 0; ptr < queries.size(); ptr++) {
		for (const Document& doc : search_server.FindTopDocuments(queries[ptr])) {
			res.push_back(doc);
		}
	}
	
	return res;
}