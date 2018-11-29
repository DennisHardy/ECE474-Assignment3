#include "operation.h"
#include <iostream>

using namespace std;

operation::operation(){
    type = NOP;
    //sign = false;
    //width = 0;;
    scheduledTime = 0;
    scheduled= true;
    ALAPTime = INT_MAX;
    ALAPDone = false;
}
operation::operation(vector<string> words, vector<variable*> available){
    scheduledTime = 0;
    scheduled= true;
    ALAPTime = INT_MAX;
    ALAPDone = false;
    type = parseOp(words.at(3));
    if(type == ERROR_OP){
        cout << "Error: Invalid Operation Type" << endl;
        return;
    }
    vector<string> inputsS;
    vector<string> outputsS;
    
    inputsS.push_back(words.at(2));
    inputsS.push_back(words.at(4));
    if(type == MUX){
        if(words.at(5).compare(":") !=0){
            cout << "Error: Invalid Operation" << endl;
            return;
        }
        inputsS.push_back(words.at(6));
    }
    outputsS.push_back(words.at(0));
    
    bool found;
    //check that inputs provided exist
    for(int i = 0; i<inputsS.size(); i++){
        found = false;
        for(int j = 0; j<available.size(); j++){
            if (inputsS.at(i).compare(available.at(j)->getName())==0){
                found = true;
                if(available.at(j)->getType() == OUTPUT){ //can't use output as input
                    cout << "Output: " << available.at(j)->getName() << " used as input."<< endl;
                    return;
                }
                this->inputs.push_back( available.at(j));
                available.at(j)->addConsumer(this);
            }
        }
        if(!found){
            cout << "Input: "<< inputsS.at(i) <<" not declared" <<endl;
            return;
        }
    }
    //check that outputs provided exist
    for(int i = 0; i<outputsS.size(); i++){
        found = false;
        for(int j = 0; j<available.size(); j++){
            if (outputsS.at(i).compare(available.at(j)->getName())==0){
                found = true;
                if(available.at(j)->getType() == INPUT ){ //can't use input as output
                    cout << "Input: " << available.at(j)->getName() << " used as output."<< endl;
                    return;
                }
                this->outputs.push_back(available.at(j));
                available.at(j)->setProducer(this);
                //this->sign |= (available.at(j)->isSigned());
            }
        }
        if(!found){
            cout << "Output: "<< outputsS.at(i) <<" not declared" <<endl;
            return;
        }
    }
}
void operation::addOutput(variable* addMe){
    return this->outputs.push_back(addMe);
}
void operation::addInput(variable* addMe){
    return this->inputs.push_back(addMe);
}
vector<variable*> operation::getInputs(){
    return this->inputs;
}
vector<variable*> operation::getOutputs(){
    return this->outputs;
}
void operation::addSucessor(operation* successor){
    this->successors.push_back(successor);
}
void operation::addPredecessor(operation* predecessor){
    this->predecessors.push_back(predecessor);
}
void operation::setALAPTime(int time){
    this->ALAPTime = time;
}
void operation::setALAPDone(bool done){
    this->ALAPDone = done;
}
int operation::getALAPTime(){
    return this->ALAPTime;
}
bool operation::isALAPDone(){
    return this->ALAPDone;
}
int operation::getSucSize(){
    return this->successors.size();
}
operation* operation::getSucAt(int i){
    return this->successors.at(i);
}
operationType parseOp(string in){
    if(in.compare("+") == 0){
        return ADD;
    }
    else if(in.compare("-") == 0){
        return SUB;
    }
    else if(in.compare("*") ==0){
        return MUL;
    }
    else if(in.compare(">") ==0){
        return COMP;
    }
    else if(in.compare("<") ==0){
        return COMP;
    }
    else if(in.compare("==") ==0){
        return COMP;
    }
    else if(in.compare("?") ==0){
        return MUX;
    }
    else if(in.compare(">>") ==0){
        return SHR;
    }
    else if(in.compare("<<") ==0){
        return SHL;
    }
    else{
        return ERROR_OP;
    }
}
