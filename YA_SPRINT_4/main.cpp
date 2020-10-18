#include "paginator.h"
#include "read_input_functions.h"
#include "request_queue.h"
#include "document.h"
#include "search_server.h"
#include "string_processing.h"
#include "test_example_functions.h"
//---------------------------------------------------
#ifdef _WIN32
#include <Windows.h>
#endif
using namespace std;

//---------------------------------------------------

int main() {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    TestSearchServer();
    SearchServer search_server("и в на"s);
    RequestQueue request_queue(search_server);

    search_server.AddDocument(1, "пушистый кот пушистый хвост"s, DocumentStatus::ACTUAL, { 7, 2, 7 });
    search_server.AddDocument(2, "пушистый пЄс и модный ошейник"s, DocumentStatus::ACTUAL, { 1, 2, 3 });
    search_server.AddDocument(3, "большой кот модный ошейник "s, DocumentStatus::ACTUAL, { 1, 2, 8 });
    search_server.AddDocument(4, "большой пЄс скворец евгений"s, DocumentStatus::ACTUAL, { 1, 3, 2 });
    search_server.AddDocument(5, "большой пЄс скворец василий"s, DocumentStatus::ACTUAL, { 1, 1, 1 });

    // 1439 запросов с нулевым результатом
    for (int i = 0; i < 1439; ++i) {
        request_queue.AddFindRequest("пустой запрос"s);
    }
    // все еще 1439 запросов с нулевым результатом
    request_queue.AddFindRequest("пушистый пЄс"s);
    // новые сутки, первый запрос удален, 1438 запросов с нулевым результатом
    request_queue.AddFindRequest("большой ошейник"s);
    // первый запрос удален, 1437 запросов с нулевым результатом
    request_queue.AddFindRequest("скворец"s);
    cout << "«апросов, по которым ничего не нашлось "s << request_queue.GetNoResultRequests() << endl;
    cout << "«апросов, по которым что-то нашлось "s << request_queue.GetResultRequests() << endl;
    return 0;
}
//-----------------------------------------------------