#include "test_example_functions.h"
void TestExcludeStopWordsFromAddedDocumentContent() {
    //basictest from YA
    const int doc_id = 42;
    const std::string content = "cat in the city";
    const std::vector<int> ratings = { 1, 2, 3 };
    {
        SearchServer server;
        server.AddDocument(doc_id, content, DocumentStatus::ACTUAL, ratings);
        const auto found_docs = server.FindTopDocuments("in");
        ASSERT_EQUAL(found_docs.size(), 1u);
        const Document& doc0 = found_docs[0];
        ASSERT_EQUAL(doc0.id, doc_id);
    }

    {
        SearchServer server;
        server.SetStopWords("in the");
        server.AddDocument(doc_id, content, DocumentStatus::ACTUAL, ratings);
        ASSERT_HINT(server.FindTopDocuments("in").empty(), "top words must be excluded from documents");
    }
}

void TestAddDocuments() {

    //Adding doc and find it by a words, info in HINT
    const std::string content_1 = "cat in the city";
    const std::string content_2 = "cat in the forest";
    const std::string content_3 = "cat cat cat";
    const std::string content_4 = "big city";
    const std::string content_5 = "mall town";
    const std::vector<int> ratings = { 1, 2, 3 };
    // Сначала убеждаемся, что документы добавлены и могут быть найдены
    {
        SearchServer server;
        server.AddDocument(1, content_5, DocumentStatus::ACTUAL, ratings);
        server.AddDocument(2, content_4, DocumentStatus::ACTUAL, ratings);
        server.AddDocument(3, content_3, DocumentStatus::ACTUAL, ratings);
        server.AddDocument(4, content_2, DocumentStatus::ACTUAL, ratings);
        server.AddDocument(5, content_1, DocumentStatus::ACTUAL, ratings);
        ASSERT_EQUAL_HINT(server.GetDocumentCount(), 5, "Get all adding docs");
        const auto found_docs = server.FindTopDocuments("cat");
        ASSERT_EQUAL_HINT(found_docs.size(), 3, "Get number of docs with \"cat\" word");
        for (const Document& doc : found_docs) {
            ASSERT_HINT((doc.id == 5 || doc.id == 4 || doc.id == 3), "Get property ID's in \"found_docs\" by \"cat\" word");
        }
    }
    // Убедимся, что лишние документы найдены не будут
    {
        SearchServer server;
        server.AddDocument(1, content_1, DocumentStatus::ACTUAL, ratings);
        const auto found_docs = server.FindTopDocuments("dog");
        ASSERT_HINT(found_docs.empty(), "No other docs find");
        ASSERT(server.GetDocumentCount() == 1);
    }
    // Убедимся, что если ни один документ не добавлен, то ничего найдено не будет
    {
        SearchServer server;
        const auto found_docs = server.FindTopDocuments("empty");
        ASSERT_HINT(found_docs.empty(), "No empty docs with empty content find");
        ASSERT(server.GetDocumentCount() == 0);
    }
    // Убедимся, что если запрос пустая строка, то ничего найдено не будет
    {
        SearchServer server;
        server.AddDocument(5, content_1, DocumentStatus::ACTUAL, ratings);
        const auto found_docs = server.FindTopDocuments("");
        ASSERT_HINT(found_docs.empty(), "No data by empty search");
    }
}

void Stop_words_doc() {
    //stop words, info in HINT
    const std::string content1 = "cat in the city";
    const std::string content2 = "the bats whant to blood";
    const std::string content3 = "the pritty pig in mars eat all";
    const std::string content4 = "TEST_ONE_WORD two";

    {
        //test without stop words
        SearchServer server;
        server.SetStopWords("");

        server.AddDocument(0, content1, DocumentStatus::ACTUAL, { 1,2,3 });
        server.AddDocument(21, content2, DocumentStatus::ACTUAL, { 2,3,4 });
        server.AddDocument(33, content3, DocumentStatus::ACTUAL, { 3,4,5 });
        const auto found_docs = server.FindTopDocuments("the whant pig");

        ASSERT_EQUAL_HINT(3, found_docs.size(), "Testing size of found container without \"top\" words");
        ASSERT_EQUAL(21, found_docs[0].id);
        ASSERT_EQUAL(33, found_docs[1].id);
        ASSERT_EQUAL(0, found_docs[2].id);
    }

    {
        //test with stop words
        SearchServer server;
        server.SetStopWords("the");

        server.AddDocument(0, content1, DocumentStatus::ACTUAL, { 1,2,3 });
        server.AddDocument(21, content2, DocumentStatus::ACTUAL, { 2,3,4 });
        server.AddDocument(33, content3, DocumentStatus::ACTUAL, { 3,4,5 });
        const auto found_docs = server.FindTopDocuments("the whant pig");

        ASSERT_EQUAL_HINT(2, found_docs.size(), "ize of container with one \"top\" word");
        ASSERT_EQUAL(21, found_docs[0].id);
        ASSERT_EQUAL(33, found_docs[1].id);
    }

    {
        //test one word in stop & querty & content
        SearchServer server;
        server.SetStopWords("TEST_ONE_WORD");

        server.AddDocument(0, content4, DocumentStatus::ACTUAL, { 1,2,3 });
        const auto found_docs = server.FindTopDocuments("TEST_ONE_WORD");

        ASSERT_EQUAL_HINT(0, found_docs.size(), "Test size of container when whole content is \"top\" word");
    }

    {
        //empty content add
        SearchServer server;
        server.SetStopWords("");

        server.AddDocument(0, "", DocumentStatus::ACTUAL, { 1,2,3 });
        const auto found_docs = server.FindTopDocuments("");

        ASSERT_EQUAL_HINT(0, found_docs.size(), "When all - \"top\", \"content\", \"query\" are empty");
    }

}

void Minus_doc() {
    //minus words, info in HINT
    SearchServer server;
    std::vector<Document> docs;
    server.AddDocument(42, "aaa aaa1 aaa2 aaa3 abcde", DocumentStatus::ACTUAL, { 1, 2, 3 });
    server.AddDocument(45, "bbb bbb1 bbb2 bbb3 abcde", DocumentStatus::ACTUAL, { 1, 2, 3 });
    {//test1
        docs = server.FindTopDocuments("abcde -aaa bbb");
        ASSERT_EQUAL_HINT(docs.size(), 1, "Test with \"minus\" word in \"query\"");
        ASSERT_EQUAL(docs.at(0).id, 45);
    }
    {//test2
        docs = server.FindTopDocuments("-abcde aaa bbb");
        ASSERT_EQUAL_HINT(docs.size(), 0, "Test when \"minus\" word is in \"content\" std::strings");
    }
    {//test3
        docs = server.FindTopDocuments("-no_words aaa bbb");
        ASSERT_EQUAL_HINT(docs.size(), 2, "Test when \"minus\" word is not in \"content\" std::strings");

        for (const auto& doc : docs)
        {
            ASSERT(doc.id == 45 || doc.id == 42);
        }
    }
}

void Match_doc() {
    //all var of matching, info in HINT
    const int doc_id = 0;
    const std::string content = "cat in the city";
    const std::vector<int> ratings = { 1, 2, 3 };

    {
        SearchServer server;
        server.AddDocument(doc_id, content, DocumentStatus::ACTUAL, ratings);
        const auto result = server.MatchDocument("cat -in", 0);
        ASSERT_HINT(std::get<0>(result).empty(), "Test \"MatchDocument\" with minus word in \"query\"");
        ASSERT(std::get<1>(result) == DocumentStatus::ACTUAL);

    }

    {
        SearchServer server;
        server.AddDocument(doc_id, content, DocumentStatus::ACTUAL, ratings);
        const auto result = server.MatchDocument("cat", 0);
        ASSERT_EQUAL_HINT(std::get<0>(result).size(), 1, "Test \"MatchDocument\" without minus word in \"query\"");
        ASSERT(std::get<1>(result) == DocumentStatus::ACTUAL);
    }

    {
        SearchServer server;
        server.AddDocument(doc_id, content, DocumentStatus::BANNED, ratings);
        const auto result = server.MatchDocument("cat city privet", 0);
        ASSERT_EQUAL_HINT(std::get<0>(result).size(), 2, "Matching with BANNED \"tatus\"");
        ASSERT(std::get<1>(result) == DocumentStatus::BANNED);
        for (const auto& word : std::get<0>(result))
        {
            ASSERT(word == "cat" || word == "city");
        }
    }

    {
        SearchServer server;
        server.AddDocument(doc_id, content, DocumentStatus::IRRELEVANT, ratings);
        const auto result = server.MatchDocument("cat city privet", 0);
        ASSERT_EQUAL_HINT(std::get<0>(result).size(), 2, "Matching with IRRELEVANT \"tatus\"");
        ASSERT(std::get<1>(result) == DocumentStatus::IRRELEVANT);
        for (const auto& word : std::get<0>(result))
        {
            ASSERT(word == "cat" || word == "city");
        }
    }

    {
        SearchServer server;
        server.AddDocument(doc_id, content, DocumentStatus::REMOVED, ratings);
        const auto result = server.MatchDocument("cat city privet", 0);
        ASSERT_EQUAL_HINT(std::get<0>(result).size(), 2, "Matching with REMOVED \"tatus\"");
        ASSERT(std::get<1>(result) == DocumentStatus::REMOVED);
        for (const auto& word : std::get<0>(result))
        {
            ASSERT(word == "cat" || word == "city");
        }
    }
}

void Raiting_doc() {
    //Sort algo\positioning, info in HINT
    {
        // pos
        SearchServer server;
        server.SetStopWords("");
        server.AddDocument(45, "average deal of", DocumentStatus::ACTUAL, { 1, 2, 3 });
        server.AddDocument(23, "average deal of", DocumentStatus::ACTUAL, { 0 });
        server.AddDocument(0, "average deal of", DocumentStatus::ACTUAL, { 4, 5, 6 });
        const auto found_docs = server.FindTopDocuments("deal");
        for (int ptr = 1; ptr < found_docs.size(); ptr++)
        {
            ASSERT_HINT(found_docs[ptr - 1].rating >= found_docs[ptr].rating, "Check right position with raitings sort from H --> L");
        }
        ASSERT_EQUAL_HINT(found_docs[0].rating, int((4 + 5 + 6) / 3), "Matching math of raitings");
        ASSERT_EQUAL_HINT(found_docs[1].rating, int((1 + 2 + 3) / 3), "Matching math of raitings");
        ASSERT_EQUAL_HINT(found_docs[2].rating, 0, "Matching math of raitings");
    }

    {
        SearchServer server;
        const std::string content = "tralala";
        server.SetStopWords("");
        server.AddDocument(45, content, DocumentStatus::ACTUAL, { 1, 2, 3 });
        server.AddDocument(23, content, DocumentStatus::ACTUAL, { 0 });
        server.AddDocument(0, content, DocumentStatus::ACTUAL, { 4, 5, 6 });
        server.AddDocument(1, content, DocumentStatus::ACTUAL, { -1, -4, -10 });
        const auto found_docs = server.FindTopDocuments(content);
        ASSERT_EQUAL_HINT(found_docs[0].rating, int((4 + 5 + 6) / 3), "Matching math of raitings");
        ASSERT_EQUAL_HINT(found_docs[1].rating, int((1 + 2 + 3) / 3), "Matching math of raitings");
        ASSERT_EQUAL_HINT(found_docs[2].rating, 0, "Matching math of raitings - ZERO case");
        ASSERT_EQUAL_HINT(found_docs[3].rating, int((-1 - 4 - 10) / 3), "Matching math of raitings - minus case");
    }
}

void Status_doc() {
    //raiting and status, info in HINT
    {
        const std::string query{ "kind cat with long tail" };
        SearchServer server;
        server.SetStopWords("with");
        server.AddDocument(1, "nasty cat beautiful tail", DocumentStatus::ACTUAL, { 0, 5, 10 });
        server.AddDocument(2, "old angry fat dog with short tail", DocumentStatus::ACTUAL, { -5, 0, 35 });
        server.AddDocument(3, "huge fat parrot", DocumentStatus::IRRELEVANT, { -2, -1, 0 });
        server.AddDocument(4, "not beautiful cat", DocumentStatus::ACTUAL, { -7, -3, -5 });
        server.AddDocument(5, "human tail", DocumentStatus::BANNED, { 0 });
        server.AddDocument(6, "mall elephant with cat face", DocumentStatus::REMOVED, { 100, 100, 100 });
        server.AddDocument(7, "cat cat cat", DocumentStatus::IRRELEVANT, { 1, -1, 1 });
        ASSERT_EQUAL_HINT(server.GetDocumentCount(), 7, "Check all doc get in");
        const auto default_docs = server.FindTopDocuments(query);
        ASSERT_EQUAL_HINT(default_docs.size(), 3, "Check all with status by default(actual)");
        const auto actural_docs = server.FindTopDocuments(query, DocumentStatus::ACTUAL);
        ASSERT_EQUAL_HINT(actural_docs.size(), 3, "Check all with status by manual input(actual)");
        for (const Document& doc : actural_docs)
        {
            ASSERT(doc.id == 1 || doc.id == 2 || doc.id == 4);
        }
        const auto banned_docs = server.FindTopDocuments(query, DocumentStatus::BANNED);
        ASSERT_EQUAL_HINT(banned_docs.size(), 1, "Check all with status by manual input(BANNED)");
        ASSERT_EQUAL(banned_docs.at(0).id, 5);
        const auto irrelevant_docs = server.FindTopDocuments(query, DocumentStatus::IRRELEVANT);
        ASSERT_EQUAL_HINT(irrelevant_docs.size(), 1, "Check all with status by manual input(IRRELEVANT)");
        ASSERT_EQUAL(irrelevant_docs.at(0).id, 7);
        const auto removed_docs = server.FindTopDocuments(query, DocumentStatus::REMOVED);
        ASSERT_EQUAL_HINT(removed_docs.size(), 1, "Check all with status by manual input(REMOVED)");
        ASSERT_EQUAL(removed_docs.at(0).id, 6);
    }

    {
        const std::string query{ "kind cat with long tail" };
        SearchServer server;
        server.SetStopWords("with");
        server.AddDocument(1, "nasty cat beautiful tail", DocumentStatus::ACTUAL, { 0, 5, 10 });
        server.AddDocument(2, "old angry fat dog with short tail", DocumentStatus::ACTUAL, { -5, 0, 35 });
        const auto removed_docs = server.FindTopDocuments(query, DocumentStatus::REMOVED);
        ASSERT_EQUAL_HINT(removed_docs.size(), 0, "Check case if NO content by status");
    }
}

void Relev_sort_doc() {
    //and one more position by rait
    {
        //relev from formula
        std::vector<int> rait{ 1,2,3 };

        SearchServer server;
        server.SetStopWords("");

        server.AddDocument(0, "calculate average raiting not a big deal", DocumentStatus::ACTUAL, rait);
        server.AddDocument(1, "calculate raiting not a big deal", DocumentStatus::ACTUAL, rait);
        server.AddDocument(2, "calculate deal not", DocumentStatus::ACTUAL, rait);
        server.AddDocument(3, "calculate deal big like duck", DocumentStatus::ACTUAL, rait);
        server.AddDocument(4, "deal not mick brrrr", DocumentStatus::ACTUAL, rait);
        server.AddDocument(5, "deal a are not mick brrrr", DocumentStatus::ACTUAL, rait);
        server.AddDocument(6, "go to mars are not big deal", DocumentStatus::ACTUAL, rait);
        server.AddDocument(7, "ell the crack is a deal", DocumentStatus::ACTUAL, rait);

        const auto found_docs = server.FindTopDocuments("not");
        for (size_t i = 1; i < found_docs.size(); ++i)
        {
            ASSERT_HINT(found_docs[i - 1].relevance - found_docs[i].relevance > 1e-6 || (abs(found_docs[i - 1].relevance - found_docs[i].relevance) < 1e-6 && (found_docs[i - 1].rating >= found_docs[i].rating)), "Test cheks position by relev in container");
        }
    }
}

void Predicat_doc() {
    //predicats, info in HINT
    SearchServer server;
    server.SetStopWords("и в на");
    server.AddDocument(0, "белый кот и модный ошейник", DocumentStatus::ACTUAL, { 8, -3 });
    server.AddDocument(1, "пушистый кот пушистый хвост", DocumentStatus::ACTUAL, { 7, 2, 7 });
    server.AddDocument(2, "ухоженный пёс выразительные глаза", DocumentStatus::ACTUAL, { 5, -12, 2, 1 });
    server.AddDocument(3, "ухоженный скворец евгений", DocumentStatus::BANNED, { 9 });
    {
        const auto found_docs = server.FindTopDocuments("пушистый ухоженный кот", [](int document_id, DocumentStatus status [[maybe_unused]], int rating [[maybe_unused]] ) { return document_id % 2 == 0; });
        ASSERT_EQUAL_HINT(found_docs.size(), 2, "Test check sugar of predicat works fine (only id % 2 == 0)");
        ASSERT_EQUAL(found_docs[0].id, 0);
        ASSERT_EQUAL(found_docs[1].id, 2);
    }

    {
        const auto found_docs = server.FindTopDocuments("пушистый ухоженный кот", [](int document_id [[maybe_unused]], DocumentStatus status [[maybe_unused]], int rating [[maybe_unused]] ) { return false; });
        ASSERT_HINT(found_docs.empty(), "Only FALSE");
    }

    {
        const auto found_docs = server.FindTopDocuments("пушистый ухоженный", [](int document_id [[maybe_unused]], DocumentStatus status [[maybe_unused]], int rating [[maybe_unused]] ) { return true; });
        ASSERT_EQUAL_HINT(found_docs.size(), 3, "Only TRUE");
    }
}

void Relev_calculate_doc() {
    //check relev and EPSILON, info in HINT
    const double EPSILON = 1e-6;
    SearchServer search_server;
    search_server.SetStopWords("и в на");
    std::string raw_query = "пушистый ухоженный кот";
    search_server.AddDocument(0, "белый кот и модный ошейник", DocumentStatus::ACTUAL, { 8, -3 });
    search_server.AddDocument(1, "пушистый кот пушистый хвост", DocumentStatus::ACTUAL, { 7, 2, 7 });
    search_server.AddDocument(2, "ухоженный пёс выразительные глаза", DocumentStatus::ACTUAL, { 0 });
    std::vector<double> res = { .650672, .274653, .101366 };
    const auto found_docs = search_server.FindTopDocuments(raw_query, [](int document_id [[maybe_unused]], DocumentStatus status, int rating [[maybe_unused]] ) { return status == DocumentStatus::ACTUAL; });
    for (size_t i = 0; i < found_docs.size(); i++)
    {
        ASSERT_HINT((found_docs[i].relevance - res[i]) < EPSILON, "Check maths of \"Find tops right\"");
    }

}
//------------
//------------
void TestSearchServer()
{
    TestExcludeStopWordsFromAddedDocumentContent();
    TestAddDocuments();
    Stop_words_doc();
    Minus_doc();
    Match_doc();
    Raiting_doc();
    Status_doc();
    Relev_sort_doc();
    Predicat_doc();
    Relev_calculate_doc();
}


void AssertImpl(bool value, const std::string& expr_str, const std::string& file,
    const std::string& func, uint32_t line, const std::string& hint) {
    if (!value) {
        std::cout << file << "(" << line << "): " << func << ": ";
        std::cout << "ASSERT(" << expr_str << ") failed.";
        if (!hint.empty()) {
            std::cout << " Hint: " << hint;
        }
        std::cout << std::endl;
        std::abort();
    }
}