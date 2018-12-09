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
					}
					else if (op->getType() == DIV || op->getType() == MOD) {
						op->setALAPTime(earliestSuc - 3);
					}
					else {
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
	int preid;
	
	for (int count = 0; count < ops.size(); count++)
	{
		ops.at(count)->setEdge(0);
		ops.at(count)->setscheduledstate(0);
		ops.at(count)->setschetime(-1);
	}
	for (int step = 2; step < ops.size(); step++)
	{
		for (int sucid = 0; sucid < ops.at(step)->getSucSize(); sucid++)
		{
			preid = ops.at(step)->getSucAt(sucid)->getId();
			ops.at(preid + 1)->addEdge();
		}
	}
	/*for (int test1 = 2; test1 < ops.size(); test1++)
	{
		cout << "v" << ops.at(test1)->getId() << " edge is " << ops.at(test1)->getEdge() << ".\n";
	}*/
	int needed[ERROR_RES] = { 1, 1, 1, 1 };
	int used[ERROR_RES];
	//"Schedule In NOP"
	vector<operation *> listing = ops;
	listing.erase(listing.begin(), listing.begin() + 2);

	vector<operation *> unschedulingadd;
	vector<operation *> unschedulingmul;
	vector<operation *> unschedulingdid;
	vector<operation *> unschedulinglog;

	vector<operation *> schedulingadd;
	vector<operation *> schedulingmul;
	vector<operation *> schedulingdid;
	vector<operation *> schedulinglog;

	bool scheduledcomadd = false;
	bool scheduledcommul = false;
	bool scheduledcomdid = false;
	bool scheduledcomlog = false;
	bool scheduledcomif = false;

	int delnum;
	int waitnum;
	int deladd=0, delmul=0, deldid=0, dellog=0,delif=0;

	used[ADDER] = 1;
	used[MULTR] = 1;
	used[LOGIC] = 1;
	used[DIVDR] = 1;

	for (int I = 1; I <= lat; I++) {
		/*cout << "I =" << I << ":" << "\n";
		for (int test1 = 0; test1 < listing.size(); test1++)
		{
			cout << "v" << listing.at(test1)->getId() << " edge is " << listing.at(test1)->getEdge() << " schetime is " << listing.at(test1)->getschetime() <<  ".\n";
		}*/
		scheduledcomadd = false;
		scheduledcommul = false;
		scheduledcomdid = false;
		scheduledcomlog = false;
		/////////////////////////////check schetime,remove edge
		deladd = 0;
		delmul = 0;
		deldid = 0;
		dellog = 0;

		for (int check1 = 0; check1 < schedulingadd.size(); check1++)
		{
			if (schedulingadd.at(check1)->getschetime() == 0)
			{
				deladd++;
				int edgeid;
				for (int del = 0; del < schedulingadd.at(check1)->getSucSize(); del++)
				{
					edgeid = schedulingadd.at(check1)->getSucAt(del)->getId() - 1;
					listing.at(edgeid)->minsedge();
				}
			}
		}
		if (deladd != 0)
		{
			for (int remove = 0; remove < deladd; remove++)
			{
				schedulingadd.erase(schedulingadd.begin());

			}
		}
		for (int check1 = 0; check1 < schedulingmul.size(); check1++)
		{
			if (schedulingmul.at(check1)->getschetime() == 0)
			{
				deladd++;
				int edgeid;
				for (int del = 0; del < schedulingmul.at(check1)->getSucSize(); del++)
				{
					edgeid = schedulingmul.at(check1)->getSucAt(del)->getId() - 1;
					listing.at(edgeid)->minsedge();
				}
			}
		}
		if (delmul != 0)
		{
			for (int remove = 0; remove < delmul; remove++)
			{
				schedulingmul.erase(schedulingmul.begin());

			}
		}

		
		for (int check1 = 0; check1 < schedulingdid.size(); check1++)
		{
			if (schedulingdid.at(check1)->getschetime() == 0)
			{
				deldid++;
				int edgeid;
				for (int del = 0; del < schedulingdid.at(check1)->getSucSize(); del++)
				{
					edgeid = schedulingdid.at(check1)->getSucAt(del)->getId() - 1;
					listing.at(edgeid)->minsedge();
				}
			}
		}
		if (deldid != 0)
		{
			for (int remove = 0; remove < deldid; remove++)
			{
				schedulingdid.erase(schedulingdid.begin());

			}
		}
		for (int check1 = 0; check1 < schedulinglog.size(); check1++)
		{
			if (schedulinglog.at(check1)->getschetime() <= 0)
			{
				dellog++;
				int edgeid;
				for (int del = 0; del < schedulinglog.at(check1)->getSucSize(); del++)
				{
					edgeid = schedulinglog.at(check1)->getSucAt(del)->getId() - 1;
					listing.at(edgeid)->minsedge();
				}
			}
		}
		if (dellog != 0)
		{
			for (int remove = 0; remove < dellog; remove++)
			{
				schedulinglog.erase(schedulinglog.begin());

			}
		}

		/////////check edge if is 0;
		for (int count1 = 0; count1 < listing.size(); count1++)
		{
			if (listing.at(count1)->getEdge() == 0)
			{

				if (listing.at(count1)->getType() == MUL)
				{
					unschedulingmul.push_back(listing.at(count1));
				}
				else if (listing.at(count1)->getType() == ADD
					|| listing.at(count1)->getType() == SUB)
				{
					unschedulingadd.push_back(listing.at(count1));
				}
				else if (listing.at(count1)->getType() == COMP
					|| listing.at(count1)->getType() == MUX
					|| listing.at(count1)->getType() == SHR
					|| listing.at(count1)->getType() == SHL)
				{
					unschedulinglog.push_back(listing.at(count1));
				}
				else if (listing.at(count1)->getType() == IF)
				{
					if (count1 == 0)
					{
						ops.at(count1 + 2)->setscheduledstate(I);
						int edgeid;
						for (int del = 0; del < listing.at(count1)->getSucSize(); del++)
						{
							edgeid = listing.at(count1)->getSucAt(del)->getId() - 1;
							listing.at(edgeid)->minsedge();
						}
						listing.at(count1)->setEdge(-1);
					}
					else if (listing.at(count1 - 1)->getschetime() == 0)
					{
						ops.at(count1 + 2)->setscheduledstate(I);
						int edgeid;
						for (int del = 0; del < listing.at(count1)->getSucSize(); del++)
						{
							edgeid = listing.at(count1)->getSucAt(del)->getId() - 1;
							listing.at(edgeid)->minsedge();
						}
						listing.at(count1)->setEdge(-1);
					}
					
				}
				else
				{
					unschedulingdid.push_back(listing.at(count1));
				}
				if (listing.at(count1)->getType() != IF)
				{
					listing.at(count1)->setEdge(-1);
				}
			}
		}

/////////////////////////////////set scheduling ops
		//adder
		for (int count2 = 0; count2 < unschedulingadd.size(); count2++)
		{
			waitnum = unschedulingadd.at(count2)->getALAPTime() - I;
			if (waitnum == 0)
			{
				unschedulingadd.at(count2)->setschetime(1);
				schedulingadd.push_back(unschedulingadd.at(count2));
				used[ADDER] = schedulingadd.size();
				scheduledcomadd = true;
				for (int count5 = 0; count5 < ops.size(); count5++)
				{
					if (unschedulingadd.at(count2)->getId() == ops.at(count5)->getId())
					{
						ops.at(count5)->setscheduledstate(I);
					}
				}
			}
		}
		if (scheduledcomadd)
		{
			for (int check2 = 2; check2 < ops.size(); check2++)
			{
				if (ops.at(check2)->getscheduledstate() != 0)
				{
					for (int check3 = 0; check3 < unschedulingadd.size(); check3++)
					{
						if (unschedulingadd.at(check3)->getId() == ops.at(check2)->getId())
						{
							unschedulingadd.erase(unschedulingadd.begin() + check3);
						}
					}
				}
			}
		}
		if (!scheduledcomadd && (unschedulingadd.size() > 0))
		{
			if (used[ADDER] >= schedulingadd.size())
			{
				int waitnum2 = lat;
				int pointer = 0;
				for (int count3 = 0; count3 < unschedulingadd.size(); count3++)
				{
					waitnum = unschedulingadd.at(count3)->getALAPTime() - I;
					if (waitnum < waitnum2)
					{
						pointer = count3;
						waitnum2 = waitnum;
					}
				}
				unschedulingadd.at(pointer)->setschetime(1);
				for (int count5 = 0; count5 < ops.size(); count5++)
				{
					if (unschedulingadd.at(pointer)->getId() == ops.at(count5)->getId())
					{
						ops.at(count5)->setscheduledstate(I);
					}
				}
				schedulingadd.push_back(unschedulingadd.at(pointer));
				unschedulingadd.erase(unschedulingadd.begin() + pointer);

			}
		}
		//muler
		for (int count2 = 0; count2 < unschedulingmul.size(); count2++)
		{
			waitnum = unschedulingmul.at(count2)->getALAPTime() - I;
			if (waitnum == 0)
			{
				unschedulingmul.at(count2)->setschetime(2);
				schedulingmul.push_back(unschedulingmul.at(count2));
				used[MULTR] = schedulingmul.size();
				scheduledcommul = true;
				for (int count5 = 0; count5 < ops.size(); count5++)
				{
					if (unschedulingmul.at(count2)->getId() == ops.at(count5)->getId())
					{
						ops.at(count5)->setscheduledstate(I);
					}
				}
			}
		}
		if (scheduledcommul)
		{
			for (int check2 = 2; check2 < ops.size(); check2++)
			{
				if (ops.at(check2)->getscheduledstate() != 0)
				{
					for (int check3 = 0; check3 < unschedulingmul.size(); check3++)
					{
						if (unschedulingmul.at(check3)->getId() == ops.at(check2)->getId())
						{
							unschedulingmul.erase(unschedulingmul.begin() + check3);
						}
					}
				}
			}
		}
		if (!scheduledcommul && (unschedulingmul.size() > 0))
		{
			if (used[MULTR] > schedulingmul.size())
			{
				int waitnum2 = lat;
				int pointer = 0;
				for (int count3 = 0; count3 < unschedulingmul.size(); count3++)
				{
					waitnum = unschedulingmul.at(count3)->getALAPTime() - I;
					if (waitnum < waitnum2)
					{
						pointer = count3;
						waitnum2 = waitnum;
					}
				}
				unschedulingmul.at(pointer)->setschetime(2);
				for (int count5 = 0; count5 < ops.size(); count5++)
				{
					if (unschedulingmul.at(pointer)->getId() == ops.at(count5)->getId())
					{
						ops.at(count5)->setscheduledstate(I);
					}
				}
				schedulingmul.push_back(unschedulingmul.at(pointer));
				unschedulingmul.erase(unschedulingmul.begin() + pointer);
			}
		}
		
		//dider
		for (int count2 = 0; count2 < unschedulingdid.size(); count2++)
		{
			waitnum = unschedulingdid.at(count2)->getALAPTime() - I;
			if (waitnum == 0)
			{
				unschedulingdid.at(count2)->setschetime(3);
				schedulingdid.push_back(unschedulingdid.at(count2));
				used[DIVDR] = schedulingdid.size();
				scheduledcomdid = true;
				for (int count5 = 0; count5 < ops.size(); count5++)
				{
					if (unschedulingdid.at(count2)->getId() == ops.at(count5)->getId())
					{
						ops.at(count5)->setscheduledstate(I);
					}
				}
			}
		}
		if (scheduledcomdid)
		{
			for (int check2 = 2; check2 < ops.size(); check2++)
			{
				if (ops.at(check2)->getscheduledstate() != 0)
				{
					for (int check3 = 0; check3 < unschedulingdid.size(); check3++)
					{
						if (unschedulingdid.at(check3)->getId() == ops.at(check2)->getId())
						{
							unschedulingdid.erase(unschedulingdid.begin() + check3);
						}
					}
				}
			}
		}
		if (!scheduledcomdid && (unschedulingdid.size() > 0))
		{
			if (used[DIVDR] > schedulingdid.size())
			{
				int waitnum2 = lat;
				int pointer = 0;
				for (int count3 = 0; count3 < unschedulingdid.size(); count3++)
				{
					waitnum = unschedulingdid.at(count3)->getALAPTime() - I;
					if (waitnum < waitnum2)
					{
						pointer = count3;
						waitnum2 = waitnum;
					}
				}
				unschedulingdid.at(pointer)->setschetime(3);
				for (int count5 = 0; count5 < ops.size(); count5++)
				{
					if (unschedulingdid.at(pointer)->getId() == ops.at(count5)->getId())
					{
						ops.at(count5)->setscheduledstate(I);
					}
				}
				schedulingdid.push_back(unschedulingdid.at(pointer));
				unschedulingdid.erase(unschedulingdid.begin() + pointer);

			}
		}

		//loger
		for (int count2 = 0; count2 < unschedulinglog.size(); count2++)
		{
			waitnum = unschedulinglog.at(count2)->getALAPTime() - I;
			if (waitnum == 0)
			{
				unschedulinglog.at(count2)->setschetime(1);
				schedulinglog.push_back(unschedulinglog.at(count2));
				used[LOGIC] = schedulinglog.size();
				scheduledcomlog = true;
				for (int count5 = 0; count5 < ops.size(); count5++)
				{
					if (unschedulinglog.at(count2)->getId() == ops.at(count5)->getId())
					{
						ops.at(count5)->setscheduledstate(I);
					}
				}
			}
		}
		if (scheduledcomlog)
		{
			for (int check2 = 2; check2 < ops.size(); check2++)
			{
				if (ops.at(check2)->getscheduledstate() != 0)
				{
					for (int check3 = 0; check3 < unschedulinglog.size(); check3++)
					{
						if (unschedulinglog.at(check3)->getId() == ops.at(check2)->getId())
						{
							unschedulinglog.erase(unschedulinglog.begin() + check3);
						}
					}
				}
			}
		}
		if (!scheduledcomlog && (unschedulinglog.size() > 0))
		{

			if (used[LOGIC] > schedulinglog.size())
			{
				int waitnum2 = lat;
				int pointer = 0;
				for (int count3 = 0; count3 < unschedulinglog.size(); count3++)
				{
					waitnum = unschedulinglog.at(count3)->getALAPTime() - I;
					if (waitnum < waitnum2)
					{
						pointer = count3;
						waitnum2 = waitnum;
					}
				}
				unschedulinglog.at(pointer)->setschetime(1);
				for (int count5 = 0; count5 < ops.size(); count5++)
				{
					if (unschedulinglog.at(pointer)->getId() == ops.at(count5)->getId())
					{
						ops.at(count5)->setscheduledstate(I);
					}
				}
				schedulinglog.push_back(unschedulinglog.at(pointer));
				unschedulinglog.erase(unschedulinglog.begin() + pointer);

			}
		}
//////////////////////////////////


		



		

		//////////////////////////////////////////////////////////////////////////////////
		
		///////////
		
		
		for (int check1 = 0; check1 < schedulingadd.size(); check1++)
		{
			schedulingadd.at(check1)->minschetime();
		}
		for (int check1 = 0; check1 < schedulingmul.size(); check1++)
		{
			schedulingmul.at(check1)->minschetime();
		}
		for (int check1 = 0; check1 < schedulingdid.size(); check1++)
		{
			schedulingdid.at(check1)->minschetime();
		}
		for (int check1 = 0; check1 < schedulinglog.size(); check1++)
		{
			schedulinglog.at(check1)->minschetime();
		}
		/////////////////////////////////
	}
	cout << "system need " << used[ADDER] << "adder, " << used[MULTR] << "mul, " << used[LOGIC] << " logics, " << used[DIVDR] << " dids" << "\n";
	for (int test1 = 2; test1 < ops.size(); test1++)
	{
		cout << "v" << ops.at(test1)->getId() << " lantancy clcye is " << ops.at(test1)->getscheduledstate() << ".\n";
	}
}
