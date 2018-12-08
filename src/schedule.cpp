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
	}
	for (int step = 2; step < ops.size(); step++)
	{
		for (int sucid = 0; sucid < ops.at(step)->getSucSize(); sucid++)
		{
			preid = ops.at(step)->getSucAt(sucid)->getId();
			ops.at(preid + 1)->addEdge();
		}
	}
	int needed[ERROR_RES] = { 1, 1, 1, 1 };
	int used[ERROR_RES];
	//"Schedule In NOP"
	vector<operation *> listing = ops;
	listing.erase(listing.begin(), listing.begin() + 2);
	/*	for (int dis = 0; dis < listing.size(); dis++)
		{
			cout << "v" << dis + 1 << " dege is " << listing.at(dis)->getEdge() << "\n";
		}*/
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
	int delnum;
	int waitnum;
	used[ADDER] = 1;
	used[MULTR] = 1;
	used[LOGIC] = 1;
	used[DIVDR] = 1;

	for (int I = 1; I <= lat; I++) {
		scheduledcomadd = false;
		scheduledcommul = false;
		scheduledcomdid = false;
		scheduledcomlog = false;
		cout << "I = " << I << " ";

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




		for (int check1 = 0; check1 < schedulingadd.size(); check1++)
		{
			if (schedulingadd.at(check1)->getschetime() <= 0)
			{
				int edgeid;
				for (int del = 0; del < schedulingadd.at(check1)->getSucSize(); del++)
				{
					edgeid = schedulingadd.at(check1)->getSucAt(del)->getId() - 1;
					listing.at(edgeid)->minsedge();
				}
				delnum = schedulingadd.at(check1)->getId();
				for (int count4 = 0; count4 < unschedulingadd.size(); count4++)
				{
					if (unschedulingadd.at(count4)->getId() == delnum)
					{
						unschedulingadd.erase(unschedulingadd.begin() + count4);

					}
				}
				schedulingadd.erase(schedulingadd.begin() + check1);
			}
		}
		for (int check1 = 0; check1 < schedulingmul.size(); check1++)
		{
			if (schedulingmul.at(check1)->getschetime() <= 0)
			{
				int edgeid;
				for (int del = 0; del < schedulingmul.at(check1)->getSucSize(); del++)
				{
					edgeid = schedulingmul.at(check1)->getSucAt(del)->getId() - 1;
					listing.at(edgeid)->minsedge();
				}
				delnum = schedulingmul.at(check1)->getId();
				for (int count4 = 0; count4 < unschedulingmul.size(); count4++)
				{
					if (unschedulingmul.at(count4)->getId() == delnum)
					{
						unschedulingmul.erase(unschedulingmul.begin() + count4);

					}
				}
				schedulingmul.erase(schedulingmul.begin() + check1);
			}
		}
		for (int check1 = 0; check1 < schedulingdid.size(); check1++)
		{
			if (schedulingdid.at(check1)->getschetime() <= 0)
			{
				int edgeid;
				for (int del = 0; del < schedulingdid.at(check1)->getSucSize(); del++)
				{
					edgeid = schedulingdid.at(check1)->getSucAt(del)->getId() - 1;
					listing.at(edgeid)->minsedge();
				}
				delnum = schedulingdid.at(check1)->getId();
				for (int count4 = 0; count4 < unschedulingdid.size(); count4++)
				{
					if (unschedulingdid.at(count4)->getId() == delnum)
					{
						unschedulingdid.erase(unschedulingdid.begin() + count4);

					}
				}
				schedulingdid.erase(schedulingdid.begin() + check1);
			}
		}
		for (int check1 = 0; check1 < schedulinglog.size(); check1++)
		{
			if (schedulinglog.at(check1)->getschetime() <= 0)
			{
				int edgeid;
				for (int del = 0; del < schedulinglog.at(check1)->getSucSize(); del++)
				{
					edgeid = schedulinglog.at(check1)->getSucAt(del)->getId() - 1;
					listing.at(edgeid)->minsedge();
				}
				delnum = schedulingadd.at(check1)->getId();
				for (int count4 = 0; count4 < unschedulinglog.size(); count4++)
				{
					if (unschedulinglog.at(count4)->getId() == delnum)
					{
						unschedulinglog.erase(unschedulinglog.begin() + count4);

					}
				}
				schedulinglog.erase(schedulinglog.begin() + check1);
			}
		}



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
				else
				{
					unschedulingdid.push_back(listing.at(count1));
				}
				listing.at(count1)->setEdge(-1);
			}
		}

		//////////////////////////////////////////////////////////////////////////////////
		for (int count2 = 0; count2 < unschedulingadd.size(); count2++)
		{
			waitnum = unschedulingadd.at(count2)->getALAPTime()-I;
			if (waitnum == 0)
			{
				unschedulingadd.at(count2)->setschetime(1);
				schedulingadd.push_back(unschedulingadd.at(count2));
				used[ADDER] = schedulingadd.size();
				scheduledcomadd = true;
			}
		}

		for (int count2 = 0; count2 < unschedulingmul.size(); count2++)
		{
			waitnum = unschedulingmul.at(count2)->getALAPTime()-I;
			if (waitnum == 0)
			{
				unschedulingmul.at(count2)->setschetime(2);
				schedulingmul.push_back(unschedulingmul.at(count2));
				used[MULTR] = schedulingmul.size();
				scheduledcommul = true;

			}
		}

		for (int count2 = 0; count2 < unschedulingdid.size(); count2++)
		{
			waitnum = unschedulingdid.at(count2)->getALAPTime()-I;
			if (waitnum == 0)
			{
				unschedulingdid.at(count2)->setschetime(3);
				schedulingdid.push_back(unschedulingdid.at(count2));
				used[DIVDR] = schedulingdid.size();
				scheduledcomdid = true;

			}
		}
		for (int count2 = 0; count2 < unschedulinglog.size(); count2++)
		{
			waitnum = unschedulinglog.at(count2)->getALAPTime()-I;
			if (waitnum == 0)
			{
				unschedulinglog.at(count2)->setschetime(1);
				schedulinglog.push_back(unschedulinglog.at(count2));
				used[LOGIC] = schedulinglog.size();
				scheduledcomlog = true;
			}
		}

		if (!scheduledcomadd && (unschedulingadd.size() > 0))
		{
			if (used[ADDER] < schedulingadd.size())
			{
				int waitnum2 = lat;
				int pointer = 0;
				for (int count3 = 0; count3 < unschedulingadd.size(); count3++)
				{
					waitnum = unschedulingadd.at(count3)->getALAPTime()-I;
					if (waitnum < waitnum2)
					{
						pointer = count3;
						waitnum2 = waitnum;
					}
				}
				unschedulingadd.at(pointer)->setschetime(1);
				schedulingadd.push_back(unschedulingadd.at(pointer));
				unschedulingadd.erase(unschedulingadd.begin() + pointer);

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
					waitnum = unschedulingmul.at(count3)->getALAPTime()-I;
					if (waitnum < waitnum2)
					{
						pointer = count3;
						waitnum2 = waitnum;
					}
				}
				unschedulingmul.at(pointer)->setschetime(2);
				schedulingmul.push_back(unschedulingmul.at(pointer));
				unschedulingmul.erase(unschedulingmul.begin() + pointer);
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
					waitnum = unschedulingdid.at(count3)->getALAPTime()-I;
					if (waitnum < waitnum2)
					{
						pointer = count3;
						waitnum2 = waitnum;
					}
				}
				unschedulingdid.at(pointer)->setschetime(3);

				schedulingdid.push_back(unschedulingdid.at(pointer));
				unschedulingdid.erase(unschedulingdid.begin() + pointer);

			}
		}

		if (!scheduledcomlog && (unschedulinglog.size() > 0))
		{
			cout << "" << "\n";

			if (used[LOGIC] > schedulinglog.size())
			{
				int waitnum2 = lat;
				int pointer = 0;
				for (int count3 = 0; count3 < unschedulinglog.size(); count3++)
				{
					waitnum = unschedulinglog.at(count3)->getALAPTime()-I;
					if (waitnum < waitnum2)
					{
						pointer = count3;
						waitnum2 = waitnum;
					}
				}
				unschedulinglog.at(pointer)->setschetime(1);

				schedulinglog.push_back(unschedulinglog.at(pointer));
				unschedulinglog.erase(unschedulinglog.begin() + pointer);

			}
		}
		cout << "schedulemul = " << unschedulingmul.size() << "\n";

		/////////////////////////////////
	}
	cout << "system need " << used[ADDER] << "adder, " << used[MULTR] << "mul, " << used[LOGIC] << " logics, " << used[DIVDR] << " dids" << "\n";

}
