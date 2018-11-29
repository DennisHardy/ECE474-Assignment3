#include "schedule.h"

void setALAPS(vector<operation*> ops, int lat){
    int earliestSuc;
    ops.at(1)->setALAPTime(lat+1);
    ops.at(1)->setALAPDone(true);
    
    while(!ops.at(0)->isALAPDone()){
        for (int i = 0; i<ops.size(); i++) {
            operation* op = ops.at(i);
            if(!op->isALAPDone()){
                bool sucsALAPed = true;
                earliestSuc = lat+1;
                
                for(int j=0; j<op->getSucSize(); j++){
                    operation* suc = op->getSucAt(j);
                    sucsALAPed &= suc->isALAPDone();
                    if(suc->getALAPTime()<earliestSuc){
                        earliestSuc = suc->getALAPTime();
                    }
                }
                if (sucsALAPed) {
                    op->setALAPTime(earliestSuc-1);
                    op->setALAPDone(true);
                    if(op->getALAPTime()<0){
                        cout << "ERROR";
                    }
                }
            }
        }
    }
    return;
}
