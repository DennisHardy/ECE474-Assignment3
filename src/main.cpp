/******************************************************************************
Assignment 3   ECE 474

*******************************************************************************/
#include "io.h"
#include "operation.h"
#include "schedule.h"
#include "variable.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

int main(int argc, char *argv[]) {
   /******************** Verify IO ********************/
   if (argc != 4) {
      cout << "Input and/or output files not specified" << endl;
      cout << "usage: hlsyn cFile latency verilogFile" << endl;
      return -1;
   }

   ifstream cFile;
   int latency;
   ofstream verilogFile;

   verilogFile.open(argv[3]);
   cFile.open(argv[1]);
   latency = atoi(argv[2]);
   if (!verilogFile.is_open()) {
      cout << "Could not open verilogFile: " << argv[3] << endl;
      return -1;
   }
   if (!cFile.is_open()) {
      cout << "Could not open cFile: " << argv[1] << endl;
      return -1;
   }
   if (latency <= 0) {
      cout << "Invalid latency: " << argv[2] << endl;
      return -1;
   }

   /******************** Read Input File ********************/
   vector<variable *> variables;
   vector<operation *> operations;
   vector<operation *> avaliable;
   operation inop;
   operation onop;
   operations.push_back(&inop);
   operations.push_back(&onop);
   int currentLineIndex = 0;
   string currentLine;
   bool error = false;
   vector<operation *> inIfs;
   int count = 0;
   bool justExited = false;
   operation *lastIf;

   while (!cFile.eof()) {
      if (getline(cFile, currentLine) && !error) {
         currentLineIndex++;
         vector<string> words = splitLine(currentLine);
         if (words.size() < 1) { // Blank Line
            // cout << "blank line: " << currentLine <<endl;
         } else if (words.at(0).compare("}") == 0) {
            // cout << "close if line: " << currentLine <<endl;
            justExited = true;
            lastIf = inIfs.back();
            inIfs.pop_back();
         } else if (words.at(0).compare("else") == 0) { // else line
            // cout << "else line: " << currentLine <<endl;
            count++;
            if (justExited) {
               operation *tempOp = new operation(words, variables, inIfs, count,
                                                 lastIf, operations); // TODO
               inIfs.push_back(tempOp);
               operations.push_back(tempOp);
               justExited = false;
            } else {
               cout << "Error: Else without corresponding If" << endl;
            }
         } else if (words.size() < 3) { // Blank Line
            // cout << "blank line: " << currentLine <<endl;
         } else if (words.at(0).compare(0, 2, "//") == 0) { // Comment Line
            // cout << "comment line: " << currentLine <<endl;
         } else if (words.size() > 2 && (words.at(0).compare("input") == 0 ||
                                         words.at(0).compare("output") == 0 ||
                                         words.at(0).compare("variable") ==
                                             0)) { // Line Defining variable
            // cout << "declaration line: "  << currentLine <<endl;
            for (int i = 2; i < words.size(); i++) {
               if (words.at(i).compare(0, 2, "//") ==
                   0) { // ignore anything after '//'
                  break;
               }
               variable *tempVar = new variable(words, i, &inop, &onop); // TODO
               variables.push_back(tempVar);
               justExited = false;
            }
         } else if (words.at(1).compare("=") == 0) { // operation line
            // cout << "op line: " << currentLine <<endl;
            count++;
            operation *tempOp = new operation(words, variables, inIfs, count,
                                              lastIf, operations); // TODO
            operations.push_back(tempOp);
            justExited = false;
         } else if (words.at(0).compare("if") == 0 &&
                    words.at(1).compare("(") == 0 &&
                    words.at(3).compare(")") == 0 &&
                    words.at(4).compare("{") == 0) { // if line
            // cout << "if line: " << currentLine <<endl;
            count++;
            operation *tempOp = new operation(words, variables, inIfs, count,
                                              lastIf, operations); // TODO
            inIfs.push_back(tempOp);
            operations.push_back(tempOp);
            justExited = false;
         }
      }
   }

   /******************** Schedule Operations ********************/

   cleanPredecessors(operations);

   cout << "\nSuccessors:\n";
   for (int i = 2; i < operations.size(); i++) {
      cout << "v" << operations.at(i)->getId() << ":  ";
      for (int j = 0; j < operations.at(i)->getSucSize(); j++) {
         int sucId = operations.at(i)->getSucAt(j)->getId();
         cout << "v" << sucId << ", ";
      }
      cout << endl;
   }
   setALAPS(operations, latency);

   cout << "\n ALAP Timing:\n";
   for (int i = 1; i <= latency; i++) {
      cout << "t" << i << ": ";
      for (int j = 2; j < operations.size(); j++) {
         if (operations.at(j)->getALAPTime() == i) {
            cout << "v" << j - 1 << ", ";
         }
      }
      cout << "\n";
   }
   /*cout << "\n Producers:\n";
   for(int i=0; i<variables.size();i++){
      cout << variables.at(i)->getName() <<":  ";
      for(int j=0; j<variables.at(i)->getProducers().size();j++){
         cout << "v" << variables.at(i)->getProducers().at(j)->getId()<<", ";
      }
      cout << "\n";
   }*/

   /******************** Write Verilog File********************/

   cout << "DONE:" << argv[1] << endl << endl;
   cFile.close();
   verilogFile.close();
}
