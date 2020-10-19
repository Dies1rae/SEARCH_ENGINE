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
   
    return 0;
}
//-----------------------------------------------------