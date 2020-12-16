
#include "test_example_functions.h"
using namespace std::string_literals;

void TestExcludeStopWordsFromAddedDocumentContent() {
    //basictest from YA
    const int doc_id = 42;
    const std::string content = "cat in the city"s;
    const std::vector<int> ratings = { 1, 2, 3 };
    {
        SearchServer server(""s);
        server.AddDocument(doc_id, content, DocumentStatus::ACTUAL, ratings);
        const auto found_docs = server.FindTopDocuments("in"s);
        ASSERT_EQUAL(found_docs.size(), 1u);
        const Document& doc0 = found_docs[0];
        ASSERT_EQUAL(doc0.id, doc_id);
    }

    {
        
        SearchServer server("in the"s);
        server.AddDocument(doc_id, content, DocumentStatus::ACTUAL, ratings);
        ASSERT_HINT(server.FindTopDocuments("in"s).empty(), "top words must be excluded from documents"s);
    }
}

void TestAddDocuments() {

    //Adding doc and find it by a words, info in HINT
    const std::string content_1 = "cat in the city"s;
    const std::string content_2 = "cat in the forest"s;
    const std::string content_3 = "cat cat cat"s;
    const std::string content_4 = "big city"s;
    const std::string content_5 = "mall town"s;
    const std::vector<int> ratings = { 1, 2, 3 };
    {
        SearchServer server(""s);
        server.AddDocument(1, content_5, DocumentStatus::ACTUAL, ratings);
        server.AddDocument(2, content_4, DocumentStatus::ACTUAL, ratings);
        server.AddDocument(3, content_3, DocumentStatus::ACTUAL, ratings);
        server.AddDocument(4, content_2, DocumentStatus::ACTUAL, ratings);
        server.AddDocument(5, content_1, DocumentStatus::ACTUAL, ratings);
        ASSERT_EQUAL_HINT(server.GetDocumentCount(), 5, "Get all adding docs"s);
        const auto found_docs = server.FindTopDocuments("cat"s);
        ASSERT_EQUAL_HINT(found_docs.size(), 3, "Get number of docs with \"cat\" word"s);
        for (const Document& doc : found_docs) {
            ASSERT_HINT((doc.id == 5 || doc.id == 4 || doc.id == 3), "Get property ID's in \"found_docs\" by \"cat\" word"s);
        }
    }
    {
        SearchServer server(""s);
        server.AddDocument(1, content_1, DocumentStatus::ACTUAL, ratings);
        const auto found_docs = server.FindTopDocuments("dog"s);
        ASSERT_HINT(found_docs.empty(), "No other docs find"s);
        ASSERT(server.GetDocumentCount() == 1);
    }
    {
        SearchServer server(""s);
        const auto found_docs = server.FindTopDocuments("empty"s);
        ASSERT_HINT(found_docs.empty(), "No empty docs with empty content find"s);
        ASSERT(server.GetDocumentCount() == 0);
    }
    {
        SearchServer server(""s);
        server.AddDocument(5, content_1, DocumentStatus::ACTUAL, ratings);
        const auto found_docs = server.FindTopDocuments(""s);
        ASSERT_HINT(found_docs.empty(), "No data by empty search"s);
    }
}

void Stop_words_doc() {
    const string content1 = "cat in the city"s;
    const string content2 = "the bats whant to blood"s;
    const string content3 = "the pritty pig in mars eat all"s;
    const string content4 = "TEST_ONE_WORD two"s;
    {
        //test without stop words
        SearchServer server(""s);
        server.AddDocument(0, content1, DocumentStatus::ACTUAL, { 1,2,3 });
        server.AddDocument(21, content2, DocumentStatus::ACTUAL, { 2,3,4 });
        server.AddDocument(33, content3, DocumentStatus::ACTUAL, { 3,4,5 });
        const auto found_docs = server.FindTopDocuments("the whant pig"s);
        assert(3 == found_docs.size());
        assert(21 == found_docs[0].id);
        assert(33 == found_docs[1].id);
        assert(0 == found_docs[2].id);
    }
    {
        //test with stop words
        SearchServer server("the"s);
        server.AddDocument(0, content1, DocumentStatus::ACTUAL, { 1,2,3 });
        server.AddDocument(21, content2, DocumentStatus::ACTUAL, { 2,3,4 });
        server.AddDocument(33, content3, DocumentStatus::ACTUAL, { 3,4,5 });
        const auto found_docs = server.FindTopDocuments("the whant pig"s);
        assert(2 == found_docs.size());
        assert(21 == found_docs[0].id);
        assert(33 == found_docs[1].id);
    }
    {
        //test one word in stop & querty & content
        SearchServer server("TEST_ONE_WORD"s);
        server.AddDocument(0, content4, DocumentStatus::ACTUAL, { 1,2,3 });
        const auto found_docs = server.FindTopDocuments("TEST_ONE_WORD"s);
        assert(0 == found_docs.size());
    }
    {
        //empty content add
        SearchServer server(""s);
        server.AddDocument(0, ""s, DocumentStatus::ACTUAL, { 1,2,3 });
        const auto found_docs = server.FindTopDocuments(""s);
        assert(0 == found_docs.size());
    }
}

void Minus_doc() {
    //minus words, info in HINT
    SearchServer server(""s);
    std::vector<Document> docs;
    server.AddDocument(42, "aaa aaa1 aaa2 aaa3 abcde"s, DocumentStatus::ACTUAL, { 1, 2, 3 });
    server.AddDocument(45, "bbb bbb1 bbb2 bbb3 abcde"s, DocumentStatus::ACTUAL, { 1, 2, 3 });
    {//test1
        docs = server.FindTopDocuments("abcde -aaa bbb"s);
        ASSERT_EQUAL_HINT(docs.size(), 1, "Test with \"minus\" word in \"query\""s);
        ASSERT_EQUAL(docs.at(0).id, 45);
    }
    {//test2
        docs = server.FindTopDocuments("-abcde aaa bbb"s);
        ASSERT_EQUAL_HINT(docs.size(), 0, "Test when \"minus\" word is in \"content\" std::strings"s);
    }
    {//test3
        docs = server.FindTopDocuments("-no_words aaa bbb"s);
        ASSERT_EQUAL_HINT(docs.size(), 2, "Test when \"minus\" word is not in \"content\" std::strings"s);
        for (const auto& doc : docs) {
            ASSERT(doc.id == 45 || doc.id == 42);
        }
    }
}

void Match_doc() {
    //all var of matching, info in HINT
    const int doc_id = 0;
    const std::string content = "cat in the city"s;
    const std::vector<int> ratings = { 1, 2, 3 };
    {
        SearchServer server(""s);
        server.AddDocument(doc_id, content, DocumentStatus::ACTUAL, ratings);
        const auto result = server.MatchDocument("cat -in"s, 0);
        ASSERT_HINT(std::get<0>(result).empty(), "Test \"MatchDocument\" with minus word in \"query\""s);
        ASSERT(std::get<1>(result) == DocumentStatus::ACTUAL);

    }

    {
        SearchServer server(""s);
        server.AddDocument(doc_id, content, DocumentStatus::ACTUAL, ratings);
        const auto result = server.MatchDocument("cat"s, 0);
        ASSERT_EQUAL_HINT(std::get<0>(result).size(), 1, "Test \"MatchDocument\" without minus word in \"query\""s);
        ASSERT(std::get<1>(result) == DocumentStatus::ACTUAL);
    }

    {
        SearchServer server(""s);
        server.AddDocument(doc_id, content, DocumentStatus::BANNED, ratings);
        const auto result = server.MatchDocument("cat city privet"s, 0);
        ASSERT_EQUAL_HINT(std::get<0>(result).size(), 2, "Matching with BANNED \"tatus\""s);
        ASSERT(std::get<1>(result) == DocumentStatus::BANNED);
        for (const auto& word : std::get<0>(result)) {
            ASSERT(word == "cat"s || word == "city"s);
        }
    }

    {
        SearchServer server(""s);
        server.AddDocument(doc_id, content, DocumentStatus::IRRELEVANT, ratings);
        const auto result = server.MatchDocument("cat city privet"s, 0);
        ASSERT_EQUAL_HINT(std::get<0>(result).size(), 2, "Matching with IRRELEVANT \"tatus\""s);
        ASSERT(std::get<1>(result) == DocumentStatus::IRRELEVANT);
        for (const auto& word : std::get<0>(result)) {
            ASSERT(word == "cat" || word == "city"s);
        }
    }

    {
        SearchServer server(""s);
        server.AddDocument(doc_id, content, DocumentStatus::REMOVED, ratings);
        const auto result = server.MatchDocument("cat city privet"s, 0);
        ASSERT_EQUAL_HINT(std::get<0>(result).size(), 2, "Matching with REMOVED \"tatus\""s);
        ASSERT(std::get<1>(result) == DocumentStatus::REMOVED);
        for (const auto& word : std::get<0>(result)) {
            ASSERT(word == "cat"s || word == "city"s);
        }
    }
}

void Raiting_doc() {
    //Sort algo\positioning, info in HINT
    {
        // pos
        SearchServer server(""s);
        server.AddDocument(45, "average deal of"s, DocumentStatus::ACTUAL, { 1, 2, 3 });
        server.AddDocument(23, "average deal of"s, DocumentStatus::ACTUAL, { 0 });
        server.AddDocument(0, "average deal of"s, DocumentStatus::ACTUAL, { 4, 5, 6 });
        const auto found_docs = server.FindTopDocuments("deal"s);
        for (int ptr = 1; ptr < found_docs.size(); ptr++) {
            ASSERT_HINT(found_docs[ptr - 1].rating >= found_docs[ptr].rating, "Check right position with raitings sort from H --> L"s);
        }
        ASSERT_EQUAL_HINT(found_docs[0].rating, int((4 + 5 + 6) / 3), "Matching math of raitings"s);
        ASSERT_EQUAL_HINT(found_docs[1].rating, int((1 + 2 + 3) / 3), "Matching math of raitings"s);
        ASSERT_EQUAL_HINT(found_docs[2].rating, 0, "Matching math of raitings"s);
    }

    {
        SearchServer server(""s);
        const std::string content = "tralala"s;
        server.AddDocument(45, content, DocumentStatus::ACTUAL, { 1, 2, 3 });
        server.AddDocument(23, content, DocumentStatus::ACTUAL, { 0 });
        server.AddDocument(0, content, DocumentStatus::ACTUAL, { 4, 5, 6 });
        server.AddDocument(1, content, DocumentStatus::ACTUAL, { -1, -4, -10 });
        const auto found_docs = server.FindTopDocuments(content);
        ASSERT_EQUAL_HINT(found_docs[0].rating, int((4 + 5 + 6) / 3), "Matching math of raitings"s);
        ASSERT_EQUAL_HINT(found_docs[1].rating, int((1 + 2 + 3) / 3), "Matching math of raitings"s);
        ASSERT_EQUAL_HINT(found_docs[2].rating, 0, "Matching math of raitings - ZERO case"s);
        ASSERT_EQUAL_HINT(found_docs[3].rating, int((-1 - 4 - 10) / 3), "Matching math of raitings - minus case"s);
    }
}

void Status_doc() {
    //raiting and status, info in HINT
    {
        const std::string query{ "kind cat with long tail"s };
        SearchServer server("with"s);
        server.AddDocument(1, "nasty cat beautiful tail"s, DocumentStatus::ACTUAL, { 0, 5, 10 });
        server.AddDocument(2, "old angry fat dog with short tail"s, DocumentStatus::ACTUAL, { -5, 0, 35 });
        server.AddDocument(3, "huge fat parrot"s, DocumentStatus::IRRELEVANT, { -2, -1, 0 });
        server.AddDocument(4, "not beautiful cat"s, DocumentStatus::ACTUAL, { -7, -3, -5 });
        server.AddDocument(5, "human tail"s, DocumentStatus::BANNED, { 0 });
        server.AddDocument(6, "mall elephant with cat face"s, DocumentStatus::REMOVED, { 100, 100, 100 });
        server.AddDocument(7, "cat cat cat"s, DocumentStatus::IRRELEVANT, { 1, -1, 1 });
        ASSERT_EQUAL_HINT(server.GetDocumentCount(), 7, "Check all doc get in"s);
        const auto default_docs = server.FindTopDocuments(query);
        ASSERT_EQUAL_HINT(default_docs.size(), 3, "Check all with status by default(actual)"s);
        const auto actural_docs = server.FindTopDocuments(query, DocumentStatus::ACTUAL);
        ASSERT_EQUAL_HINT(actural_docs.size(), 3, "Check all with status by manual input(actual)"s);
        for (const Document& doc : actural_docs) {
            ASSERT(doc.id == 1 || doc.id == 2 || doc.id == 4);
        }
        const auto banned_docs = server.FindTopDocuments(query, DocumentStatus::BANNED);
        ASSERT_EQUAL_HINT(banned_docs.size(), 1, "Check all with status by manual input(BANNED)"s);
        ASSERT_EQUAL(banned_docs.at(0).id, 5);
        const auto irrelevant_docs = server.FindTopDocuments(query, DocumentStatus::IRRELEVANT);
        ASSERT_EQUAL_HINT(irrelevant_docs.size(), 1, "Check all with status by manual input(IRRELEVANT)"s);
        ASSERT_EQUAL(irrelevant_docs.at(0).id, 7);
        const auto removed_docs = server.FindTopDocuments(query, DocumentStatus::REMOVED);
        ASSERT_EQUAL_HINT(removed_docs.size(), 1, "Check all with status by manual input(REMOVED)"s);
        ASSERT_EQUAL(removed_docs.at(0).id, 6);
    }

    {
        const std::string query{ "kind cat with long tail"s };
        SearchServer server("with"s);
        server.AddDocument(1, "nasty cat beautiful tail"s, DocumentStatus::ACTUAL, { 0, 5, 10 });
        server.AddDocument(2, "old angry fat dog with short tail"s, DocumentStatus::ACTUAL, { -5, 0, 35 });
        const auto removed_docs = server.FindTopDocuments(query, DocumentStatus::REMOVED);
        ASSERT_EQUAL_HINT(removed_docs.size(), 0, "Check case if NO content by status"s);
    }
}

void Relev_sort_doc() {
    //and one more position by rait
    {
        //relev from formula
        std::vector<int> rait{ 1,2,3 };

        SearchServer server(""s);

        server.AddDocument(0, "calculate average raiting not a big deal"s, DocumentStatus::ACTUAL, rait);
        server.AddDocument(1, "calculate raiting not a big deal"s, DocumentStatus::ACTUAL, rait);
        server.AddDocument(2, "calculate deal not"s, DocumentStatus::ACTUAL, rait);
        server.AddDocument(3, "calculate deal big like duck"s, DocumentStatus::ACTUAL, rait);
        server.AddDocument(4, "deal not mick brrrr"s, DocumentStatus::ACTUAL, rait);
        server.AddDocument(5, "deal a are not mick brrrr"s, DocumentStatus::ACTUAL, rait);
        server.AddDocument(6, "go to mars are not big deal"s, DocumentStatus::ACTUAL, rait);
        server.AddDocument(7, "ell the crack is a deal"s, DocumentStatus::ACTUAL, rait);

        const auto found_docs = server.FindTopDocuments("not"s);
        for (size_t i = 1; i < found_docs.size(); ++i) {
            ASSERT_HINT(found_docs[i - 1].relevance - found_docs[i].relevance > 1e-6 || (abs(found_docs[i - 1].relevance - found_docs[i].relevance) < 1e-6 && (found_docs[i - 1].rating >= found_docs[i].rating)), "Test cheks position by relev in container");
        }
    }
}

void Predicat_doc() {
    //predicats, info in HINT
    SearchServer server("и в на"s);
    server.AddDocument(0, "белый кот и модный ошейник"s, DocumentStatus::ACTUAL, { 8, -3 });
    server.AddDocument(1, "пушистый кот пушистый хвост"s, DocumentStatus::ACTUAL, { 7, 2, 7 });
    server.AddDocument(2, "ухоженный пёс выразительные глаза"s, DocumentStatus::ACTUAL, { 5, -12, 2, 1 });
    server.AddDocument(3, "ухоженный скворец евгений"s, DocumentStatus::BANNED, { 9 });
    {
        const auto found_docs = server.FindTopDocuments("пушистый ухоженный кот"s, [](int document_id, DocumentStatus status [[maybe_unused]], int rating [[maybe_unused]] ) { return document_id % 2 == 0; });
        ASSERT_EQUAL_HINT(found_docs.size(), 2, "Test check sugar of predicat works fine (only id % 2 == 0)"s);
        ASSERT_EQUAL(found_docs[0].id, 0);
        ASSERT_EQUAL(found_docs[1].id, 2);
    }

    {
        const auto found_docs = server.FindTopDocuments("пушистый ухоженный кот"s, [](int document_id [[maybe_unused]], DocumentStatus status [[maybe_unused]], int rating [[maybe_unused]] ) { return false; });
        ASSERT_HINT(found_docs.empty(), "Only FALSE"s);
    }

    {
        const auto found_docs = server.FindTopDocuments("пушистый ухоженный"s, [](int document_id [[maybe_unused]], DocumentStatus status [[maybe_unused]], int rating [[maybe_unused]] ) { return true; });
        ASSERT_EQUAL_HINT(found_docs.size(), 3, "Only TRUE"s);
    }
}

void Request_queue_test() {
    SearchServer search_server("и в на"s);
    RequestQueue request_queue(search_server);

    search_server.AddDocument(1, "пушистый кот пушистый хвост"s, DocumentStatus::ACTUAL, { 7, 2, 7 });
    search_server.AddDocument(2, "пушистый пёс и модный ошейник"s, DocumentStatus::ACTUAL, { 1, 2, 3 });
    search_server.AddDocument(3, "большой кот модный ошейник "s, DocumentStatus::ACTUAL, { 1, 2, 8 });
    search_server.AddDocument(4, "большой пёс скворец евгений"s, DocumentStatus::ACTUAL, { 1, 3, 2 });
    search_server.AddDocument(5, "большой пёс скворец василий"s, DocumentStatus::ACTUAL, { 1, 1, 1 });

    // 1439 запросов с нулевым результатом
    for (int i = 0; i < 1439; ++i) {
        request_queue.AddFindRequest("пустой запрос"s);
    }
    // все еще 1439 запросов с нулевым результатом
    request_queue.AddFindRequest("пушистый пёс"s);
    // новые сутки, первый запрос удален, 1438 запросов с нулевым результатом
    request_queue.AddFindRequest("большой ошейник"s);
    // первый запрос удален, 1437 запросов с нулевым результатом
    request_queue.AddFindRequest("скворец"s);
    ASSERT(request_queue.GetNoResultRequests() == 1437);
}

void Paginator_test() {
    SearchServer search_server("and with"s);
    search_server.AddDocument(1, "funny pet and nasty rat"s, DocumentStatus::ACTUAL, { 7, 2, 7 });
    search_server.AddDocument(2, "funny pet with curly hair"s, DocumentStatus::ACTUAL, { 1, 2, 3 });
    search_server.AddDocument(3, "big cat nasty hair"s, DocumentStatus::ACTUAL, { 1, 2, 8 });
    search_server.AddDocument(4, "big dog cat Vladislav"s, DocumentStatus::ACTUAL, { 1, 3, 2 });
    search_server.AddDocument(5, "big dog hamster Borya"s, DocumentStatus::ACTUAL, { 1, 1, 1 });

    const auto search_results = search_server.FindTopDocuments("curly dog"s);
    int page_size = 55;
    const auto pages = Paginate(search_results, page_size);
    ASSERT(pages.end() - pages.begin() == 1);
}

//------------
//------------
void TestSearchServer() {
    TestExcludeStopWordsFromAddedDocumentContent();
    std::cout << "Basic test is OK!"s << std::endl;
    TestAddDocuments();
    std::cout << "Add_Doc test is OK!"s << std::endl;
    Stop_words_doc();
    std::cout << "Stop_word test is OK!"s << std::endl;
    Minus_doc();
    std::cout << "Minus_word test is OK!"s << std::endl;
    Match_doc();
    std::cout << "Matching_Calculate test is OK!"s << std::endl;
    Raiting_doc();
    std::cout << "Raiting_calculate test is OK!"s << std::endl;
    Status_doc();
    std::cout << "Status test is OK!"s << std::endl;
    Relev_sort_doc();
    std::cout << "Relevation test is OK!"s << std::endl;
    Predicat_doc();
    std::cout << "Function_predicate test is OK!"s << std::endl;
    Request_queue_test();
    std::cout << "Request_queue test is OK!"s << std::endl;
    Paginator_test();
    std::cout << "Paginator test is OK!"s << std::endl;
}


void AssertImpl(bool value, const std::string& expr_str, const std::string& file,
    const std::string& func, uint32_t line, const std::string& hint) {
    if (!value) {
        std::cout << file << "("s << line << "): "s << func << ": "s;
        std::cout << "ASSERT("s << expr_str << ") failed."s;
        if (!hint.empty()) {
            std::cout << " Hint: "s << hint;
        }
        std::cout << std::endl;
        std::abort();
    }
}
