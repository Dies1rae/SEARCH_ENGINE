#include "document.h"

Document::Document() :id(0), relevance(0.0), rating(0) {}
Document::Document(int ID, double REL, int RAIT) : id(ID), relevance(REL), rating(RAIT) {}