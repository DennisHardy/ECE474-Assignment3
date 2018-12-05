#ifndef VAR_H
#define VAR_H

#include <string>
#include <vector>
class operation;
using namespace std;

enum variableType { INPUT, OUTPUT, VARIABLE, ERROR_VAR = -1 };

class variable {
 private:
   string name;
   int width;
   bool sign;
   variableType type;
   vector<operation *> producers;
   vector<operation *> consumers;

 public:
   string getName();
   int getWidth();
   bool isSigned();
   variableType getType();
   variable();
   variable(vector<string> words, int index, operation *inop, operation *onop);
   void addProducer(operation *producer);
   vector<operation *> getProducers();
   void addConsumer(operation *consumer);
   vector<operation *> getConsumers();
};

variableType varTypeParser(string dataType);
int typeToWidth(string dataType);
bool typeToSign(string dataType);
#endif
