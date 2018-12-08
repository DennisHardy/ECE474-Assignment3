#ifndef OP_H
#define OP_H

#include "variable.h"
#include <vector>

using namespace std;

enum operationType {
	REG,
	ADD,
	SUB,
	MUL,
	COMP,
	MUX,
	SHR,
	SHL,
	IF,
	ELSE,
	FOR,
	DIV,
	MOD,
	NOP,
	ERROR_OP = -1
};
enum resource { ADDER, MULTR, LOGIC, DIVDR, ERROR_RES };

class operation {
private:
	operationType type;
	vector<variable *> inputs;
	vector<variable *> outputs;
	vector<operation *> successors;
	vector<operation *> predecessors;
	vector<operation *> inIfs;
	// bool sign;
	// int width;
	int id;
	int CompType;
	int edge;
	int scheduledTime;
	int scheduledstate;
	bool scheduled;
	int ALAPTime;
	bool ALAPDone;
	resource res;
	int schetime;

public:
	operation();
	operation(vector<string> words, vector<variable *> avaliable,
		vector<operation *> inIfs, int count, operation *lastif,
		vector<operation *> avalavaliableOps);
	void addOutput(variable *addMe);
	void addInput(variable *addMe);
	int getEdge();
	
	int getpreedge();
	void addEdge();
	void setEdge(int numedge);
	vector<variable *> getInputs();
	vector<variable *> getOutputs();
	void addSucessor(operation *successor);
	void addPredecessor(operation *predecessor);
	void setALAPTime(int ALAPTime);
	void setALAPDone(bool done);
	void scheduleAt(int time);
	int getALAPTime();
	void setschetime(int timenum1);
	int getschetime();
	void minschetime();
	int getscheduledstate();
	void setscheduledstate(int statetime);
	void minsedge();
	bool isALAPDone();
	int getSucSize();
	operation *getSucAt(int i);
	operation *getSuc();
	void removeSucAt(int i);
	operationType getType();
	int getId();
	resource getResource();
	void setResource();
	vector<operation *> getInIfs();
	bool inIf(operation *anIf);
	int getCompType();
};
class ifElse {
private:
	vector<operation *> ifOps;
	vector<operation *> elseOps;

public:
	void addIfOp(operation *addMe);
	void addElseOp(operation *addMe);
};
operationType parseOp(string in);
#endif
