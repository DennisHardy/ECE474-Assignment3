#include "operation.h"
#include <iostream>
#include <limits.h>

using namespace std;

operation::operation() {
	type = NOP;
	// sign = false;
	// width = 0;;
	scheduledTime = 0;
	scheduled = true;
	ALAPTime = INT_MAX; //INT_MAX = 21474639647
	ALAPDone = false;
	id = 0;
}
operation::operation(vector<string> words, vector<variable *> avaliableVars,
	vector<operation *> inIfs, int count, operation *lastIf,
	vector<operation *> avalavaliableOps) {
	id = count;
	scheduledTime = 0;
	scheduled = false;
	ALAPTime = INT_MAX;
	ALAPDone = false;
	this->inIfs = inIfs;
	if (words.size() <= 3) {
		type = ELSE;
	}
	else {
		type = parseOp(words.at(3));
	}
	if (type == ERROR_OP) {
		cout << "Error: Invalid Operation Type" << endl;
		return;
	}
	for (int i = 0; i < inIfs.size(); i++) {
		inIfs.at(i)->addSucessor(this);
		this->addPredecessor(inIfs.at(i));
		// cout << "Made link**: "<<
		// inIfs.at(i)->getId()<<"-->"<<this->getId()<<endl;
	}
	vector<string> inputsS;
	vector<string> outputsS;
	if (type == IF) {
		inputsS.push_back(words.at(2));
	}
	else if (type == ELSE) {
		for (int i = 0; i < lastIf->getInputs().size(); i++) {
			inputsS.push_back(lastIf->getInputs().at(i)->getName());
		}
	}
	else {
		inputsS.push_back(words.at(2));
		inputsS.push_back(words.at(4));
		outputsS.push_back(words.at(0));
	}

	if (type == MUX) {
		if (words.at(5).compare(":") != 0) {
			cout << "Error: Invalid Operation" << endl;
			return;
		}
		inputsS.push_back(words.at(6));
	}

	bool found;
	// check that inputs provided exist
	for (int i = 0; i < inputsS.size(); i++) {
		found = false;
		for (int j = 0; j < avaliableVars.size(); j++) {
			if (inputsS.at(i).compare(avaliableVars.at(j)->getName()) == 0) {
				found = true;
				if (avaliableVars.at(j)->getType() ==
					OUTPUT) { // can't use output as input
					cout << "Output: " << avaliableVars.at(j)->getName()
						<< " used as input." << endl;
					return;
				}
				this->inputs.push_back(avaliableVars.at(j));
				avaliableVars.at(j)->addConsumer(this);
			}
		}
		if (!found) {
			cout << "Input: " << inputsS.at(i) << " not declared" << endl;
			return;
		}
	}
	// check that outputs provided exist
	for (int i = 0; i < outputsS.size(); i++) {
		found = false;
		for (int j = 0; j < avaliableVars.size(); j++) {
			if (outputsS.at(i).compare(avaliableVars.at(j)->getName()) == 0) {
				found = true;
				if (avaliableVars.at(j)->getType() ==
					INPUT) { // can't use input as output
					cout << "Input: " << avaliableVars.at(j)->getName()
						<< " used as output." << endl;
					return;
				}
				this->outputs.push_back(avaliableVars.at(j));
				this->inputs.push_back(avaliableVars.at(j)); // testing her
				avaliableVars.at(j)->addProducer(this);
				avaliableVars.at(j)->addConsumer(this);
				// this->sign |= (avaliableVars.at(j)->isSigned());
			}
		}
		if (!found) {
			cout << "Output: " << outputsS.at(i) << " not declared" << endl;
			return;
		}
	}
	for (int j = 0; j < this->getInputs().size(); j++) {
		variable *curInput = this->getInputs().at(j);
		for (int k = 0; k < curInput->getProducers().size(); k++) {
			operation *potentialPred = curInput->getProducers().at(k);
			bool inScope = true;
			for (int l = 0; l < potentialPred->getInIfs().size(); l++) {
				inScope &= this->inIf(potentialPred->getInIfs().at(l));
			}
			if (inScope) {
				potentialPred->addSucessor(this);
				this->addPredecessor(potentialPred);
				// cout << "Made link: "<<
				// this->getId()<<"<--"<<potentialPred->getId()<<endl;
			}
			/*operation* predOp =
			curInput->getProducers().at(curInput->getProducers().size()-1);
			this->addPredecessor(predOp);
			predOp->addSucessor(this);
			*/
		}
	}
	this->setResource();
}
void operation::addOutput(variable *addMe) {
	return this->outputs.push_back(addMe);
}
void operation::addInput(variable *addMe) {
	return this->inputs.push_back(addMe);
}
vector<variable *> operation::getInputs() { return this->inputs; }
vector<variable *> operation::getOutputs() { return this->outputs; }
void operation::addSucessor(operation *successor) {
	this->successors.push_back(successor);
}
void operation::addPredecessor(operation *predecessor) {
	this->predecessors.push_back(predecessor);
}
int operation::getpreedge() { return this->predecessors.size(); }
void operation::setALAPTime(int time) { this->ALAPTime = time; }
void operation::setALAPDone(bool done) { this->ALAPDone = done; }
int operation::getEdge() { return this->edge; }
void operation::addEdge() { this->edge++; }
void operation::setEdge(int numedge) { this->edge = numedge; }
int operation::getALAPTime() { return this->ALAPTime; }
bool operation::isALAPDone() { return this->ALAPDone; }
int operation::getSucSize() { return this->successors.size(); }
operationType operation::getType() { return this->type; }
operation *operation::getSucAt(int i) { return this->successors.at(i); }
void operation::setschetime(int timenum1) { this->schetime = timenum1; }
int operation::getschetime() { return this->schetime; }
void operation::minsedge() { this->edge--; }
void operation::minschetime() { this->schetime--; }
void operation::removeSucAt(int i) {
	this->successors.erase(this->successors.begin() + i);
}
int operation::getId() { return id; }
void operation::scheduleAt(int time) {
	this->scheduledTime = time;
	this->scheduled = true;
}
resource operation::getResource() { return this->res; }
vector<operation *> operation::getInIfs() { return this->inIfs; }
bool operation::inIf(operation *anIf) {
	for (operation *IF : this->inIfs) {
		if (IF == anIf) {
			return true;
		}
	}
	return false;
}
void operation::setResource() {
	switch (type) {
	case ADD:
	case SUB:
		this->res = ADDER;
		break;
	case MUL:
		this->res = MULTR;
		break;
	case COMP:
	case MUX:
	case SHR:
	case SHL:
	case IF:
	case ELSE:
	case FOR:
		this->res = LOGIC;
		break;
	case DIV:
	case MOD:
		this->res = DIVDR;
		break;
	default:
		this->res = ERROR_RES;
		break;
	}
}
operationType parseOp(string in) {
	if (in.compare("+") == 0) {
		return ADD;
	}
	else if (in.compare("-") == 0) {
		return SUB;
	}
	else if (in.compare("*") == 0) {
		return MUL;
	}
	else if (in.compare(">") == 0) {
		return COMP;
	}
	else if (in.compare("<") == 0) {
		return COMP;
	}
	else if (in.compare("==") == 0) {
		return COMP;
	}
	else if (in.compare("?") == 0) {
		return MUX;
	}
	else if (in.compare(">>") == 0) {
		return SHR;
	}
	else if (in.compare("<<") == 0) {
		return SHL;
	}
	else if (in.compare(")") == 0) {
		return IF;
	}
	else if (in.compare("") == 0) {
		return ELSE;
	}
	else if (in.compare("/") == 0) {
		return DIV;
	}
	else if (in.compare("%") == 0) {
		return DIV;
	}
	else {
		return ERROR_OP;
	}
}
