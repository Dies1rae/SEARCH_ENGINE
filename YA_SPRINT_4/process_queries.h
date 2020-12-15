#pragma once

#include <algorithm>
#include <execution>
#include <numeric>

#include "search_server.h"

std::vector<std::vector<Document>> ProcessQueries(const SearchServer& search_server,
	const std::vector<std::string>& queries);