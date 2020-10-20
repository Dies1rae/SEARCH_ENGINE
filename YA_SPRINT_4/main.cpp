// search_server_s4_t2_solution.cpp
#include "search_server.h"
#include "paginator.h"
#include "request_queue.h"
#include "test_example_functions.h"
#include <iostream>




int main() {
    SearchServer search_server("� � ��"s);
    RequestQueue request_queue(search_server);

    search_server.AddDocument(1, "�������� ��� �������� �����"s, DocumentStatus::ACTUAL, { 7, 2, 7 });
    search_server.AddDocument(2, "�������� �� � ������ �������"s, DocumentStatus::ACTUAL, { 1, 2, 3 });
    search_server.AddDocument(3, "������� ��� ������ ������� "s, DocumentStatus::ACTUAL, { 1, 2, 8 });
    search_server.AddDocument(4, "������� �� ������� �������"s, DocumentStatus::ACTUAL, { 1, 3, 2 });
    search_server.AddDocument(5, "������� �� ������� �������"s, DocumentStatus::ACTUAL, { 1, 1, 1 });

    // 1439 �������� � ������� �����������
    for (int i = 0; i < 1439; ++i) {
        request_queue.AddFindRequest("������ ������"s);
    }
    // ��� ��� 1439 �������� � ������� �����������
    request_queue.AddFindRequest("�������� ��"s);
    // ����� �����, ������ ������ ������, 1438 �������� � ������� �����������
    request_queue.AddFindRequest("������� �������"s);
    // ������ ������ ������, 1437 �������� � ������� �����������
    request_queue.AddFindRequest("�������"s);
    cout << "��������, �� ������� ������ �� ������� "s << request_queue.GetNoResultRequests();
    return 0;
}