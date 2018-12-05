#include "variable.h"
#include "operation.h"
#include <iostream>
#include <vector>
variable::variable() {
   name = "";
   width = 0;
   sign = false;
   type = ERROR_VAR;
}

variable::variable(vector<string> words, int index, operation *inop,
                   operation *onop) {
   type = varTypeParser(words.at(0));
   if (type == ERROR_VAR) {
      cout << "Error: Invalid Variable Type" << endl;
      return;
   }

   sign = typeToSign(words.at(1));
   width = typeToWidth(words.at(1));
   if (width == -1) {
      cout << "Error: Invalid Variable Type" << endl;
      return;
   }
   name = words.at(index);
   if (type == INPUT) {
      producers.push_back(inop);
      inop->addOutput(this);
   } else if (type == OUTPUT) {
      consumers.push_back(onop);
      onop->addInput(this);
   }
}

string variable::getName() { return this->name; }
int variable::getWidth() { return this->width; }
bool variable::isSigned() { return this->sign; }
void variable::addProducer(operation *producer) {
   this->producers.push_back(producer);
}
vector<operation *> variable::getProducers() { return this->producers; }
void variable::addConsumer(operation *consumer) {
   this->consumers.push_back(consumer);
}
vector<operation *> variable::getConsumers() { return this->consumers; }
variableType variable::getType() { return this->type; }
variableType varTypeParser(string dataType) {
   if (dataType.compare("input") == 0) {
      return INPUT;
   } else if (dataType.compare("output") == 0) {
      return OUTPUT;
   } else if (dataType.compare("variable") == 0) {
      return VARIABLE;
   } else {
      return ERROR_VAR;
   }
}
int typeToWidth(string dataType) {
   if (dataType.compare("Int1") == 0 || dataType.compare("UInt1") == 0) {
      return 1;
   } else if (dataType.compare("Int2") == 0 || dataType.compare("UInt2") == 0) {
      return 2;
   } else if (dataType.compare("Int8") == 0 || dataType.compare("UInt8") == 0) {
      return 8;
   } else if (dataType.compare("Int16") == 0 ||
              dataType.compare("UInt16") == 0) {
      return 16;
   } else if (dataType.compare("Int32") == 0 ||
              dataType.compare("UInt32") == 0) {
      return 32;
   } else if (dataType.compare("Int64") == 0 ||
              dataType.compare("UInt64") == 0) {
      return 64;
   } else {
      return -1;
   }
}
bool typeToSign(string dataType) {
   if (dataType.compare("Int1") == 0 || dataType.compare("Int2") == 0 ||
       dataType.compare("Int8") == 0 || dataType.compare("Int16") == 0 ||
       dataType.compare("Int32") == 0) {
      return true;
   } else if (dataType.compare("UInt1") == 0 ||
              dataType.compare("UInt2") == 0 ||
              dataType.compare("UInt8") == 0 ||
              dataType.compare("UInt16") == 0 ||
              dataType.compare("UInt32") == 0) {
      return false;
   } else {
      return false;
   }
}
