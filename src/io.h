#ifndef IO_H
#define IO_H

#include <string>
#include <vector>
#include "operation.h"
using namespace std;
vector<string> splitLine(string line);
void cleanPredecessors(vector<operation*> ops);
#endif