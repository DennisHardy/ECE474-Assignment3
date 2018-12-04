#include "io.h"
#include <cctype>
#include <istream>
#include <sstream>

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

vector<string> splitLine(string line) {
   vector<string> out;

   stringstream stream;
   stream.str(line);

   while (stream) {
      string temp;
      stream >> temp;
      if (temp.size() > 0) {
         char last = temp[temp.length() - 1];
         if (last == ',') {
            temp = temp.substr(0, temp.size() - 1);
         }
         out.push_back(temp);
      }
   }

   return out;
}
void cleanPredecessors(vector<operation *> ops) {
   for (operation *op : ops) {
      for (int i = 0; i < op->getSucSize(); i++) {
         operation *suc = op->getSucAt(i);
         if (suc == op) {
            op->removeSucAt(i);
         }
      }
   }
}
