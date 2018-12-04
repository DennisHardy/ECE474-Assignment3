#ifndef SCHED_H
#define SCHED_H

#include "operation.h"
#include <iostream>
#include <vector>
using namespace std;
void setALAPS(vector<operation *> ops, int lat);
void List_R(vector<operation *> ops, int lat);
#endif
