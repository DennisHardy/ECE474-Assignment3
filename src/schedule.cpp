#include "schedule.h"

void setALAPS(vector<operation *> ops, int lat) {
   int earliestSuc;
   ops.at(1)->setALAPTime(lat + 1);
   ops.at(1)->setALAPDone(true);

   while (!ops.at(0)->isALAPDone()) {
      for (int i = 0; i < ops.size(); i++) {
         operation *op = ops.at(i);
         if (!op->isALAPDone()) {
            bool sucsALAPed = true;
            earliestSuc = lat + 1;

            for (int j = 0; j < op->getSucSize(); j++) {
               operation *suc = op->getSucAt(j);
               if (!(suc == op)) {
                  sucsALAPed &= suc->isALAPDone();
                  if (suc->getALAPTime() < earliestSuc) {
                     earliestSuc = suc->getALAPTime();
                  }
               }
            }
            if (sucsALAPed) {
               if (op->getType() == MUL) {
                  op->setALAPTime(earliestSuc - 2);
               } else if (op->getType() == DIV || op->getType() == MOD) {
                  op->setALAPTime(earliestSuc - 3);
               } else {
                  op->setALAPTime(earliestSuc - 1);
               }
               op->setALAPDone(true);
               if (op->getALAPTime() < 0) {
                  cout << "ERROR Could Not ALAP schedule" << endl;
               }
            }
         }
      }
   }
   return;
}
void List_R(vector<operation *> ops, int lat) {
   int needed[ERROR_RES] = {1, 1, 1, 1};
   int used[ERROR_RES];
   //"Schedule In NOP"
   ops.at(0)->scheduleAt(0);
   vector<operation *> unscheduled = ops;
   for (int time = 0; time < lat; time++) {
      used[ADDER] = 0;
      used[MULTR] = 0;
      used[LOGIC] = 0;
      used[DIVDR] = 0;
      for (operation *op :
           unscheduled) { // schedule all operations with zero slack
         if (op->getALAPTime() == time) {
            op->scheduleAt(time);
            used[op->getResource()]++;
         }
      }
      for (int resource = ADDER; resource < ERROR_RES;
           resource++) { // For each resource type
         if (used[resource] > needed[resource]) {
            needed[resource]++;
         } else if (used[resource] < needed[resource]) {
            for (int i = 0; i < needed[resource] - used[resource]; i++) {
            }
         }
      }
   }
}
