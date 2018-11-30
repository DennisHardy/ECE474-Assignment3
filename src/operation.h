#ifndef OP_H
#define OP_H

#include <vector>
#include "variable.h"

using namespace std;

enum operationType {REG, ADD, SUB, MUL, COMP, MUX, SHR, SHL, IF, FOR, NOP, ERROR_OP=-1};
enum resource {ADDER, MULTR, LOGIC, DIVDR, ERROR_RES=-1};

class operation{
private:
    operationType type;
    vector<variable*> inputs;
    vector<variable*> outputs;
    vector<operation*> successors;
    vector<operation*> predecessors;
    //bool sign;
    //int width;
    //int id;
    int scheduledTime;
    bool scheduled;
    int ALAPTime;
    bool ALAPDone;
public:
    operation();
    operation(vector<string> words, vector<variable*> avaliable, vector<operation*> inIfs);
    void addOutput(variable* addMe);
    void addInput(variable* addMe);
    vector<variable*> getInputs();
    vector<variable*> getOutputs();
    void addSucessor(operation* successor);
    void addPredecessor(operation* predecessor);
    void setALAPTime(int ALAPTime);
    void setALAPDone(bool done);
    int getALAPTime();
    bool isALAPDone();
    int getSucSize();
    operation* getSucAt(int i);
    operationType getType();
};
operationType parseOp(string in);
#endif
