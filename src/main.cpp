/******************************************************************************
Assignment 3   ECE 474

*******************************************************************************/
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include "operation.h"
#include "variable.h"
#include "io.h"
#include "schedule.h"

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
    vector<variable*> variables;
    vector<operation*> operations;
    operation inop;
    operation onop;
    operations.push_back(&inop);
    operations.push_back(&onop);
    int currentLineIndex = 0;
    string currentLine;
    bool error = false;
    vector<operation*> inIfs;
    
    while (!cFile.eof()) {
        if(getline(cFile, currentLine) && !error){
            currentLineIndex++;
            vector<string> words = splitLine(currentLine);
            if (words.size() < 1) { //Blank Line
                cout << "blank line: " << currentLine <<endl;
            }
            else if(words.at(0).compare("}") == 0){
                cout << "close if line: " << currentLine <<endl;
                inIfs.pop_back();
            }
            else if (words.size() < 3) { //Blank Line
                cout << "blank line: " << currentLine <<endl;
            }
            else if (words.at(0).compare(0, 2, "//") == 0) { //Comment Line
                cout << "comment line: " << currentLine <<endl;
            }
            else if (words.size()>2 && (words.at(0).compare("input") == 0 || words.at(0).compare("output") == 0 ||
                                        words.at(0).compare("variable") == 0)) { //Line Defining variable
                cout << "declaration line: "  << currentLine <<endl;
                for(int i = 2; i<words.size(); i++){
                    if(words.at(i).compare(0,2,"//")==0){//ignore anything after '//'
                        break;
                    }
                    variable* tempVar = new variable(words, i, &inop, &onop);//TODO
                    variables.push_back(tempVar);
                }
            }
            else if (words.at(1).compare("=") == 0) { //operation line
                cout << "op line: " << currentLine <<endl;
                operation* tempOp = new operation(words, variables, inIfs);//TODO
                operations.push_back(tempOp);
            }
            else if(words.at(0).compare("if") == 0 && words.at(1).compare("(") == 0 && words.at(3).compare(")") == 0 && words.at(4).compare("{") == 0){ //if line
                cout << "if line: " << currentLine <<endl;
                operation* tempOp = new operation(words, variables, inIfs);//TODO
                inIfs.push_back(tempOp);
                operations.push_back(tempOp);
            }
        }
    }
    
    /******************** Schedule Operations ********************/
    //find all operation predecessors and successors ()
    for (int i=0; i<operations.size(); i++) {
        operation* curOp = operations.at(i);
        for (int j = 0; j < curOp->getOutputs().size(); j++) {
            variable* curOutput =curOp->getOutputs().at(j);
            for(int k=0; k < curOutput->getConsumers().size(); k++){
                operation* sucOp = curOutput->getConsumers().at(k);
                if(curOutput->getName().compare("zrin")==0){
                    
                }
                curOp->addSucessor(sucOp);
                sucOp->addPredecessor(curOp);
            }
        }
        /*for (int j = 0; j<curOp->getInputs().size(); j++) {
            variable* curInput = curOp->getInputs().at(j);
            for (int k=0; k<curInput->getProducers().size(); k++) {
                operation* predOp = curInput->getProducers().at(k);
                curOp->addPredecessor(predOp);
                predOp->addSucessor(curOp);
            }
        }*/
    }
    
    setALAPS(operations, latency);
    
    for (int i=2; i<operations.size(); i++) {
        cout << i-1<< " " << operations.at(i)->getALAPTime() <<endl;
    }
    
    /******************** Write Verilog File********************/
    
    cout << "DONE:" << argv[1]<<endl;
    cFile.close();
    verilogFile.close();
}
