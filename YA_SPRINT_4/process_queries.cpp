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