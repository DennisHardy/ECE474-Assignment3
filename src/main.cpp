/******************************************************************************
Assignment 3   ECE 474
*******************************************************************************/
#include "io.h"
#include "operation.h"
#include "schedule.h"
#include "variable.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <math.h>
//#include <tgmath.h>

using namespace std;

int main(int argc, char *argv[]) {
	/******************** Verify IO ********************/
	if (argc != 4) {
		cout << "Input and/or output files not specified" << endl;
		cout << "usage: hlsyn cFile latency verilogFile" << endl;
		return -1;
	}

	ifstream cFile;
	int latency;
	ofstream verilogFile;

	verilogFile.open(argv[3]);
	cFile.open(argv[1]);
	latency = atoi(argv[2]);
//	verilogFile.open("C:\\Users\\Zachary\\Desktop\\Test.txt");
//	cFile.open("C:\\Users\\Zachary\\Desktop\\test1.txt");
//	latency = 11;

	if (!verilogFile.is_open()) {
		cout << "Could not open verilogFile: " << argv[3] << endl;
		return -1;
	}
	if (!cFile.is_open()) {
		cout << "Could not open cFile: " << argv[1] << endl;
		return -1;
	}
	if (latency <= 0) {
		cout << "Invalid latency: " << argv[2] << endl;
		return -1;
	}

	/******************** Read Input File ********************/
	vector<variable *> variables;
	vector<operation *> operations;
	vector<operation *> avaliable;
	operation inop;
	operation onop;
	operations.push_back(&inop);
	operations.push_back(&onop);
	int currentLineIndex = 0;
	string currentLine;
	bool error = false;
	vector<operation *> inIfs;
	int count = 0;
	bool justExited = false;
	operation *lastIf;
	lastIf = NULL;

	while (!cFile.eof()) {
		if (getline(cFile, currentLine) && !error) {
			currentLineIndex++;
			vector<string> words = splitLine(currentLine);
			if (words.size() < 1) { // Blank Line
			   // cout << "blank line: " << currentLine <<endl;
			}
			else if (words.at(0).compare("}") == 0) {
				// cout << "close if line: " << currentLine <<endl;
				justExited = true;
				lastIf = inIfs.back();
				inIfs.pop_back();
			}
			else if (words.at(0).compare("else") == 0) { // else line
			// cout << "else line: " << currentLine <<endl;
				count++;
				if (justExited) {
					operation *tempOp = new operation(words, variables, inIfs, count,
						lastIf, operations); // TODO
					inIfs.push_back(tempOp);
					operations.push_back(tempOp);
					justExited = false;
				}
				else {
					cout << "Error: Else without corresponding If" << endl;
				}
			}
			else if (words.size() < 3) { // Blank Line
			// cout << "blank line: " << currentLine <<endl;
			}
			else if (words.at(0).compare(0, 2, "//") == 0) { // Comment Line
			// cout << "comment line: " << currentLine <<endl;
			}
			else if (words.size() > 2 && (words.at(0).compare("input") == 0 ||
				words.at(0).compare("output") == 0 ||
				words.at(0).compare("variable") ==
				0)) { // Line Defining variable
// cout << "declaration line: "  << currentLine <<endl;
				for (int i = 2; i < words.size(); i++) {
					if (words.at(i).compare(0, 2, "//") ==
						0) { // ignore anything after '//'
						break;
					}
					variable *tempVar = new variable(words, i, &inop, &onop); // TODO
					variables.push_back(tempVar);
					justExited = false;
				}
			}
			else if (words.at(1).compare("=") == 0) { // operation line
			// cout << "op line: " << currentLine <<endl;
				count++;
				operation *tempOp = new operation(words, variables, inIfs, count,
					lastIf, operations); // TODO
				operations.push_back(tempOp);
				justExited = false;
			}
			else if (words.at(0).compare("if") == 0 &&
				words.at(1).compare("(") == 0 &&
				words.at(3).compare(")") == 0 &&
				words.at(4).compare("{") == 0) { // if line
		// cout << "if line: " << currentLine <<endl;
				count++;
				operation *tempOp = new operation(words, variables, inIfs, count,
					lastIf, operations); // TODO
				inIfs.push_back(tempOp);
				operations.push_back(tempOp);
				justExited = false;
			}
		}
	}

	/******************** Schedule Operations ********************/

	cleanPredecessors(operations);

	cout << "\nSuccessors:\n";
	for (int i = 2; i < operations.size(); i++) {
		cout << "v" << operations.at(i)->getId() << ":  ";
		for (int j = 0; j < operations.at(i)->getSucSize(); j++) {
			int sucId = operations.at(i)->getSucAt(j)->getId();
			cout << "v" << sucId << ", ";
		}
		cout << endl;
	}
	setALAPS(operations, latency);
	List_R(operations, latency);
	cout << "\n ALAP Timing:\n";
	for (int i = 1; i <= latency; i++) {
		cout << "t" << i << ": ";
		for (int j = 2; j < operations.size(); j++) {
			if (operations.at(j)->getALAPTime() == i) {
				cout << "v" << j - 1 << ", ";
			}
		}
		cout << "\n";
	}
	cout << "\n Scheduled Timing:\n";
	for (int i = 1; i <= latency; i++) {
		cout << "t" << i << ": ";
		for (int j = 2; j < operations.size(); j++) {
			if (operations.at(j)->getscheduledstate() == i) {
				cout << "v" << j - 1 << ", ";
			}
		}
		cout << "\n";
	}
	/*cout << "\n Producers:\n";
	for(int i=0; i<variables.size();i++){
	   cout << variables.at(i)->getName() <<":  ";
	   for(int j=0; j<variables.at(i)->getProducers().size();j++){
		  cout << "v" << variables.at(i)->getProducers().at(j)->getId()<<", ";
	   }
	   cout << "\n";
	}*/

	/******************** Write Verilog File********************/
	cout << "GENERATING OUTPUT FILE" << endl;
	verilogFile << "module HLSM (Clk, Rst, Start, Done";//MODULE START
	for (int i = 0; i < variables.size(); i++) {
		if (variables.at(i)->getType() == 0 || variables.at(i)->getType() == 1) {
			verilogFile << ", " << variables.at(i)->getName();
		}
	}
	verilogFile << ");" << endl;
	verilogFile << "input Clk, Rst, Start;" << endl;//INPUT START
	for (int i = 0; i < variables.size(); i++) {
		if (variables.at(i)->getType() == 0) {
			verilogFile << "input ";
			if (variables.at(i)->isSigned()) {
				verilogFile << "signed ";
			}
			if (variables.at(i)->getWidth() > 1) {
				verilogFile << "[" << variables.at(i)->getWidth() - 1 << ":0] ";
			}
			verilogFile << variables.at(i)->getName() << "; " << endl;
		}
	}
	verilogFile << "output reg Done;" << endl;//OUTPUT START
	for (int i = 0; i < variables.size(); i++) {
		if (variables.at(i)->getType() == 1) {
			verilogFile << "output reg ";
			if (variables.at(i)->isSigned()) {
				verilogFile << "signed ";
			}
			if (variables.at(i)->getWidth() > 1) {
				verilogFile << "[" << variables.at(i)->getWidth() - 1 << ":0] ";
			}
			verilogFile << variables.at(i)->getName() << "; " << endl;
		}
	}
	for (int i = 0; i < variables.size(); i++) {
		if (variables.at(i)->getType() == 2) {
			verilogFile << "reg ";
			if (variables.at(i)->isSigned()) {
				verilogFile << "signed ";
			}
			if (variables.at(i)->getWidth() > 1) {
				verilogFile << "[" << variables.at(i)->getWidth() - 1 << ":0] ";
			}
			verilogFile << variables.at(i)->getName() << "; " << endl;
		}
	}
	verilogFile << "reg [" << ceil(log(latency) / log(2)) - 1 << ":0] State;" << endl;
	verilogFile << "parameter Wait = 0, Final = 1, ";
	for (int i = 0; i < (latency); i++) {
		verilogFile << "S" << i << " = " << i+2;
		if (i != latency-1) { verilogFile << ", "; }
		else { verilogFile << ";" << endl; }
	}
	verilogFile << "always @(posedge Clk) begin" << endl;
	verilogFile << "if (Rst == 1) begin" << endl << "Done <= 0;" << endl << "State <= Wait;" << endl << "end" << endl;
	verilogFile << "else begin" << endl;
	verilogFile << "case(State)" << endl;
	verilogFile << "Wait: begin" << endl;
	verilogFile << "Done <= 0;" << endl;
	verilogFile << "if (Start == 1) begin" << endl;
	verilogFile << "State <= S0;" << endl;
	verilogFile << "end" << endl;
	verilogFile << "else begin" << endl;
	verilogFile << "State <= Wait;" << endl;
	verilogFile << "end" << endl << "end" << endl;
	for (int i = 1; i < latency + 1; i++) {
		//output case
		verilogFile << "S" << i - 1 << ": begin" << endl;
		for (int j = 0; j < operations.size(); j++) {
			if ((operations.at(j)->getscheduledstate() == i) && (operations.at(j)->getInIfs().size() == 0)) {
				switch (operations.at(j)->getType()) {
				case 0://REG
					verilogFile << "//REG";
					break;
				case 1://ADD
					verilogFile << operations.at(j)->getOutputs().at(0)->getName() << " <= ";
					verilogFile << operations.at(j)->getInputs().at(0)->getName() << " + ";
					verilogFile << operations.at(j)->getInputs().at(1)->getName() << ";";
					break;
				case 2://SUB
					verilogFile << operations.at(j)->getOutputs().at(0)->getName() << " <= ";
					verilogFile << operations.at(j)->getInputs().at(0)->getName() << " - ";
					verilogFile << operations.at(j)->getInputs().at(1)->getName() << ";";
					break;
				case 3://MUL
					verilogFile << operations.at(j)->getOutputs().at(0)->getName() << " <= ";
					verilogFile << operations.at(j)->getInputs().at(0)->getName() << " * ";
					verilogFile << operations.at(j)->getInputs().at(1)->getName() << ";";
					break;
				case 4://COMP
					verilogFile << "if (" << operations.at(j)->getInputs().at(0)->getName();
					if (operations.at(j)->getCompType() == 0) { verilogFile << " < "; }
					if (operations.at(j)->getCompType() == 1) { verilogFile << " > "; }
					if (operations.at(j)->getCompType() == 2) { verilogFile << " == "; }
					verilogFile << operations.at(j)->getInputs().at(1)->getName() << ") begin" << endl;
					verilogFile << operations.at(j)->getOutputs().at(0)->getName() << " <= 1;" << endl << "end" << endl;
					verilogFile << "else begin" << endl << operations.at(j)->getOutputs().at(0)->getName() << " <= 0;" << endl << "end";
					break;
				case 5://MUX
					verilogFile << operations.at(j)->getOutputs().at(0)->getName() << " <= (";
					verilogFile << operations.at(j)->getInputs().at(0)->getName() << ")?";
					verilogFile << operations.at(j)->getInputs().at(1)->getName() << ":";
					verilogFile << operations.at(j)->getInputs().at(2)->getName() << ";";
					break;
				case 6://SHR
					verilogFile << operations.at(j)->getOutputs().at(0)->getName() << " <= ";
					verilogFile << operations.at(j)->getInputs().at(0)->getName() << " >> ";
					verilogFile << operations.at(j)->getInputs().at(1)->getName() << ";";
					break;
				case 7://SHL
					verilogFile << operations.at(j)->getOutputs().at(0)->getName() << " <= ";
					verilogFile << operations.at(j)->getInputs().at(0)->getName() << " << ";
					verilogFile << operations.at(j)->getInputs().at(1)->getName() << ";";
					break;
				case 8://IF x1
					verilogFile << "if (" << operations.at(j)->getInputs().at(0)->getName() << ") begin" << endl;
					for (int k = 0; k < operations.size(); k++) {
						for (int l = 0; l < operations.at(k)->getInIfs().size(); l++) {
							if (operations.at(k)->getInIfs().at(l) == operations.at(j) && (operations.at(k)->getInIfs().size() - (l + 1)) == 0) {
								if (operations.at(k)->getType() != 8 && operations.at(k)->getType() != 9 && operations.at(k)->getType() != 4) {
									verilogFile << operations.at(k)->getOutputs().at(0)->getName() << " <= ";
									if (operations.at(k)->getType() == 5) { verilogFile << "("; }
									verilogFile << operations.at(k)->getInputs().at(0)->getName();
									if (operations.at(k)->getType() == 5) { verilogFile << ") "; }
									switch (operations.at(k)->getType()) {
									case 1: verilogFile << " + "; break;	case 2: verilogFile << " - "; break;	case 3: verilogFile << " * "; break;
									case 5: verilogFile << ":"; break;		case 6: verilogFile << " >> "; break;	case 7: verilogFile << " << "; break;
									default: verilogFile << "0 //"; break;
									}
									verilogFile << operations.at(k)->getInputs().at(1)->getName() << ";" << endl;
								}
								else if (operations.at(k)->getType() == 4) {	//COMP
									verilogFile << "if (" << operations.at(k)->getInputs().at(0)->getName();
									if (operations.at(k)->getCompType() == 0) { verilogFile << " < "; }
									if (operations.at(k)->getCompType() == 1) { verilogFile << " > "; }
									if (operations.at(k)->getCompType() == 2) { verilogFile << " == "; }
									verilogFile << operations.at(k)->getInputs().at(1)->getName() << ") begin" << endl;
									verilogFile << operations.at(k)->getOutputs().at(0)->getName() << " <= 1;" << endl << "end" << endl;
									verilogFile << "else begin" << endl << operations.at(k)->getOutputs().at(0)->getName() << " <= 0;" << endl << "end";
								}
								else if (operations.at(k)->getType() == 8) {	//IF x2
									verilogFile << "if (" << operations.at(k)->getInputs().at(0)->getName() << ") begin" << endl;
									for (int l = 0; l < operations.size(); l++) {
										for (int m = 0; m < operations.at(l)->getInIfs().size(); m++) {
											if (operations.at(l)->getInIfs().at(m) == operations.at(k)) {
												if (operations.at(l)->getType() != 8 && operations.at(l)->getType() != 9 && operations.at(l)->getType() != 4) {
													verilogFile << operations.at(l)->getOutputs().at(0)->getName() << " <= ";
													if (operations.at(l)->getType() == 5) { verilogFile << "("; }
													verilogFile << operations.at(l)->getInputs().at(0)->getName();
													if (operations.at(l)->getType() == 5) { verilogFile << ") "; }
													switch (operations.at(l)->getType()) {
													case 1: verilogFile << " + "; break;	case 2: verilogFile << " - "; break;	case 3: verilogFile << " * "; break;
													case 5: verilogFile << ":"; break;		case 6: verilogFile << " >> "; break;	case 7: verilogFile << " << "; break;
													default: verilogFile << "0 //"; break;
													}
													verilogFile << operations.at(l)->getInputs().at(1)->getName() << ";" << endl;
												}
												else if (operations.at(l)->getType() == 4) {	//COMP
													verilogFile << "if (" << operations.at(l)->getInputs().at(0)->getName();
													if (operations.at(l)->getCompType() == 0) { verilogFile << " < "; }
													if (operations.at(l)->getCompType() == 1) { verilogFile << " > "; }
													if (operations.at(l)->getCompType() == 2) { verilogFile << " == "; }
													verilogFile << operations.at(l)->getInputs().at(1)->getName() << ") begin" << endl;
													verilogFile << operations.at(l)->getOutputs().at(0)->getName() << " <= 1;" << endl << "end" << endl;
													verilogFile << "else begin" << endl << operations.at(l)->getOutputs().at(0)->getName() << " <= 0 " << endl << "end";
												}
												else if (operations.at(l)->getType() == 8) {	//IF x3
													//FIXME
													verilogFile << "if (" << operations.at(l)->getInputs().at(0)->getName() << ") begin" << endl;
													for (int m = 0; m < operations.size(); m++) {
														for (int n = 0; n < operations.at(m)->getInIfs().size(); n++) {
															if (operations.at(m)->getInIfs().at(n) == operations.at(l)) {
																if (operations.at(m)->getType() != 8 && operations.at(m)->getType() != 9 && operations.at(m)->getType() != 4) {
																	verilogFile << operations.at(m)->getOutputs().at(0)->getName() << " <= ";
																	if (operations.at(m)->getType() == 5) { verilogFile << "("; }
																	verilogFile << operations.at(m)->getInputs().at(0)->getName();
																	if (operations.at(m)->getType() == 5) { verilogFile << ") "; }
																	switch (operations.at(m)->getType()) {
																	case 1: verilogFile << " + "; break;	case 2: verilogFile << " - "; break;	case 3: verilogFile << " * "; break;
																	case 5: verilogFile << ":"; break;		case 6: verilogFile << " >> "; break;	case 7: verilogFile << " << "; break;
																	default: verilogFile << "0 //"; break;
																	}
																	verilogFile << operations.at(m)->getInputs().at(1)->getName() << ";" << endl;
																}
																else if (operations.at(m)->getType() == 4) {	//COMP
																	verilogFile << "if (" << operations.at(m)->getInputs().at(0)->getName();
																	if (operations.at(m)->getCompType() == 0) { verilogFile << " < "; }
																	if (operations.at(m)->getCompType() == 1) { verilogFile << " > "; }
																	if (operations.at(m)->getCompType() == 2) { verilogFile << " == "; }
																	verilogFile << operations.at(m)->getInputs().at(1)->getName() << ") begin" << endl;
																	verilogFile << operations.at(m)->getOutputs().at(0)->getName() << " <= 1;" << endl << "end" << endl;
																	verilogFile << "else begin" << endl << operations.at(m)->getOutputs().at(0)->getName() << " <= 0 " << endl << "end";
																}
																else if (operations.at(m)->getType() == 8) {	//IF x4
																	verilogFile << "if (1) begin" << endl;
																	//FIXME
																	//FIXME	
																	verilogFile << "end" << endl;
																}
																else if (operations.at(m)->getType() == 9) {	//ELSE x4
																	verilogFile << "else begin" << endl;
																	//FIXME
																	//FIXME
																	verilogFile << "end" << endl;
																}
															}
														}
													}
													verilogFile << "end" << endl;
													//FIXME	
												}
												else if (operations.at(l)->getType() == 9) {	//ELSE x3
													verilogFile << "else begin" << endl;
													//FIXME
													//FIXME
													verilogFile << "end" << endl;
												}
											}
										}
									}
									verilogFile << "end" << endl;
								}
								else if (operations.at(k)->getType() == 9) {	//ELSE x2
									verilogFile << "else begin" << endl;
									for (int l = 0; l < operations.size(); l++) {
										for (int m = 0; m < operations.at(l)->getInIfs().size(); m++) {
											if (operations.at(l)->getInIfs().at(m) == operations.at(k) && (operations.at(l)->getInIfs().size() - (m + 1)) == 0) {
												if (operations.at(l)->getType() != 8 && operations.at(l)->getType() != 9 && operations.at(l)->getType() != 4) {
													verilogFile << operations.at(l)->getOutputs().at(0)->getName() << " <= ";
													if (operations.at(l)->getType() == 5) { verilogFile << "("; }
													verilogFile << operations.at(l)->getInputs().at(0)->getName();
													if (operations.at(l)->getType() == 5) { verilogFile << ") "; }
													switch (operations.at(l)->getType()) {
													case 1: verilogFile << " + "; break;	case 2: verilogFile << " - "; break;	case 3: verilogFile << " * "; break;
													case 5: verilogFile << ":"; break;		case 6: verilogFile << " >> "; break;	case 7: verilogFile << " << "; break;
													default: verilogFile << "0 //"; break;
													}
													verilogFile << operations.at(l)->getInputs().at(1)->getName() << ";" << endl;
												}
												else if (operations.at(l)->getType() == 4) {	//COMP
													verilogFile << "if (" << operations.at(l)->getInputs().at(0)->getName();
													if (operations.at(l)->getCompType() == 0) { verilogFile << " < "; }
													if (operations.at(l)->getCompType() == 1) { verilogFile << " > "; }
													if (operations.at(l)->getCompType() == 2) { verilogFile << " == "; }
													verilogFile << operations.at(l)->getInputs().at(1)->getName() << ") begin" << endl;
													verilogFile << operations.at(l)->getOutputs().at(0)->getName() << " <= 1;" << endl << "end" << endl;
													verilogFile << "else begin" << endl << operations.at(l)->getOutputs().at(0)->getName() << " <= 0;" << endl << "end";
												}
												else if (operations.at(l)->getType() == 8) {	//IF x3
													//FIXME
													verilogFile << "if (" << operations.at(l)->getInputs().at(0)->getName() << ") begin" << endl;
													for (int m = 0; m < operations.size(); m++) {
														for (int n = 0; n < operations.at(m)->getInIfs().size(); n++) {
															if (operations.at(m)->getInIfs().at(n) == operations.at(l)) {
																if (operations.at(m)->getType() != 8 && operations.at(m)->getType() != 9 && operations.at(m)->getType() != 4) {
																	verilogFile << operations.at(m)->getOutputs().at(0)->getName() << " <= ";
																	if (operations.at(m)->getType() == 5) { verilogFile << "("; }
																	verilogFile << operations.at(m)->getInputs().at(0)->getName();
																	if (operations.at(m)->getType() == 5) { verilogFile << ") "; }
																	switch (operations.at(m)->getType()) {
																	case 1: verilogFile << " + "; break;	case 2: verilogFile << " - "; break;	case 3: verilogFile << " * "; break;
																	case 5: verilogFile << ":"; break;		case 6: verilogFile << " >> "; break;	case 7: verilogFile << " << "; break;
																	default: verilogFile << "0 //"; break;
																	}
																	verilogFile << operations.at(m)->getInputs().at(1)->getName() << ";" << endl;
																}
																else if (operations.at(m)->getType() == 4) {	//COMP
																	verilogFile << "if (" << operations.at(m)->getInputs().at(0)->getName();
																	if (operations.at(m)->getCompType() == 0) { verilogFile << " < "; }
																	if (operations.at(m)->getCompType() == 1) { verilogFile << " > "; }
																	if (operations.at(m)->getCompType() == 2) { verilogFile << " == "; }
																	verilogFile << operations.at(m)->getInputs().at(1)->getName() << ") begin" << endl;
																	verilogFile << operations.at(m)->getOutputs().at(0)->getName() << " <= 1;" << endl << "end" << endl;
																	verilogFile << "else begin" << endl << operations.at(m)->getOutputs().at(0)->getName() << " <= 0 " << endl << "end";
																}
																else if (operations.at(m)->getType() == 8) {	//IF x4
																	verilogFile << "if (1) begin" << endl;
																	//FIXME
																	//FIXME	
																	verilogFile << "end" << endl;
																}
																else if (operations.at(m)->getType() == 9) {	//ELSE x4
																	verilogFile << "else begin" << endl;
																	//FIXME
																	//FIXME
																	verilogFile << "end" << endl;
																}
															}
														}
													}
													verilogFile << "end" << endl;
													//FIXME	
												}
												else if (operations.at(l)->getType() == 9) {	//ELSE x3
													verilogFile << "else begin" << endl;
													//FIXME
													//FIXME
													verilogFile << "end" << endl;
												}
											}
										}
									}
									//FIXME
									verilogFile << "end" << endl;
								}
							}
						}
					}
					verilogFile << "end";
					break;
				case 9://ELSE x1
					verilogFile << "else begin" << endl;
					for (int k = 0; k < operations.size(); k++) {
						for (int l = 0; l < operations.at(k)->getInIfs().size(); l++) {
							if (operations.at(k)->getInIfs().at(l) == operations.at(j) && (operations.at(k)->getInIfs().size() - (l + 1)) == 0) {
								if (operations.at(k)->getType() != 8 && operations.at(k)->getType() != 9 && operations.at(k)->getType() != 4) {
									verilogFile << operations.at(k)->getOutputs().at(0)->getName() << " <= ";
									if (operations.at(k)->getType() == 5) { verilogFile << "("; }
									verilogFile << operations.at(k)->getInputs().at(0)->getName();
									if (operations.at(k)->getType() == 5) { verilogFile << ") "; }
									switch (operations.at(k)->getType()) {
									case 1: verilogFile << " + "; break;	case 2: verilogFile << " - "; break;	case 3: verilogFile << " * "; break;
									case 5: verilogFile << ":"; break;		case 6: verilogFile << " >> "; break;	case 7: verilogFile << " << "; break;
									default: verilogFile << "0 //"; break;
									}
									verilogFile << operations.at(k)->getInputs().at(1)->getName() << ";" << endl;
								}
								else if (operations.at(k)->getType() == 4) {	//COMP
									verilogFile << "if (" << operations.at(k)->getInputs().at(0)->getName();
									if (operations.at(k)->getCompType() == 0) { verilogFile << " < "; }
									if (operations.at(k)->getCompType() == 1) { verilogFile << " > "; }
									if (operations.at(k)->getCompType() == 2) { verilogFile << " == "; }
									verilogFile << operations.at(k)->getInputs().at(1)->getName() << ") begin" << endl;
									verilogFile << operations.at(k)->getOutputs().at(0)->getName() << " <= 1;" << endl << "end" << endl;
									verilogFile << "else begin" << endl << operations.at(k)->getOutputs().at(0)->getName() << " <= 0;" << endl << "end";
								}
								else if (operations.at(k)->getType() == 8) {	//IF x2
									verilogFile << "if (" << operations.at(k)->getInputs().at(0)->getName() << ") begin" << endl;
									for (int l = 0; l < operations.size(); l++) {
										for (int m = 0; m < operations.at(l)->getInIfs().size(); m++) {
											if (operations.at(l)->getInIfs().at(m) == operations.at(k)) {
												if (operations.at(l)->getType() != 8 && operations.at(l)->getType() != 9 && operations.at(l)->getType() != 4) {
													verilogFile << operations.at(l)->getOutputs().at(0)->getName() << " <= ";
													if (operations.at(l)->getType() == 5) { verilogFile << "("; }
													verilogFile << operations.at(l)->getInputs().at(0)->getName();
													if (operations.at(l)->getType() == 5) { verilogFile << ") "; }
													switch (operations.at(l)->getType()) {
													case 1: verilogFile << " + "; break;	case 2: verilogFile << " - "; break;	case 3: verilogFile << " * "; break;
													case 5: verilogFile << ":"; break;		case 6: verilogFile << " >> "; break;	case 7: verilogFile << " << "; break;
													default: verilogFile << "0 //"; break;
													}
													verilogFile << operations.at(l)->getInputs().at(1)->getName() << ";" << endl;
												}
												else if (operations.at(l)->getType() == 4) {	//COMP
													verilogFile << "if (" << operations.at(l)->getInputs().at(0)->getName();
													if (operations.at(l)->getCompType() == 0) { verilogFile << " < "; }
													if (operations.at(l)->getCompType() == 1) { verilogFile << " > "; }
													if (operations.at(l)->getCompType() == 2) { verilogFile << " == "; }
													verilogFile << operations.at(l)->getInputs().at(1)->getName() << ") begin" << endl;
													verilogFile << operations.at(l)->getOutputs().at(0)->getName() << " <= 1;" << endl << "end" << endl;
													verilogFile << "else begin" << endl << operations.at(l)->getOutputs().at(0)->getName() << " <= 0;" << endl << "end";
												}
												else if (operations.at(l)->getType() == 8) {	//IF x3
													verilogFile << "if (1) begin" << endl;
													//FIXME
													//FIXME	
													verilogFile << "end" << endl;
												}
												else if (operations.at(l)->getType() == 9) {	//ELSE x3
													verilogFile << "else begin" << endl;
													//FIXME
													//FIXME
													verilogFile << "end" << endl;
												}
											}
										}
									}
									verilogFile << "end" << endl;
								}
								else if (operations.at(k)->getType() == 9) {	//ELSE x2
									verilogFile << "else begin" << endl;
									for (int l = 0; l < operations.size(); l++) {
										for (int m = 0; m < operations.at(l)->getInIfs().size(); m++) {
											if (operations.at(l)->getInIfs().at(m) == operations.at(k)) {
												if (operations.at(l)->getType() != 8 && operations.at(l)->getType() != 9 && operations.at(l)->getType() != 4) {
													verilogFile << operations.at(l)->getOutputs().at(0)->getName() << " <= ";
													if (operations.at(l)->getType() == 5) { verilogFile << "("; }
													verilogFile << operations.at(l)->getInputs().at(0)->getName();
													if (operations.at(l)->getType() == 5) { verilogFile << ") "; }
													switch (operations.at(l)->getType()) {
													case 1: verilogFile << " + "; break;	case 2: verilogFile << " - "; break;	case 3: verilogFile << " * "; break;
													case 5: verilogFile << ":"; break;		case 6: verilogFile << " >> "; break;	case 7: verilogFile << " << "; break;
													default: verilogFile << "0 //"; break;
													}
													verilogFile << operations.at(l)->getInputs().at(1)->getName() << ";" << endl;
												}
												else if (operations.at(l)->getType() == 4) {	//COMP
													verilogFile << "if (" << operations.at(l)->getInputs().at(0)->getName();
													if (operations.at(l)->getCompType() == 0) { verilogFile << " < "; }
													if (operations.at(l)->getCompType() == 1) { verilogFile << " > "; }
													if (operations.at(l)->getCompType() == 2) { verilogFile << " == "; }
													verilogFile << operations.at(l)->getInputs().at(1)->getName() << ") begin" << endl;
													verilogFile << operations.at(l)->getOutputs().at(0)->getName() << " <= 1;" << endl << "end" << endl;
													verilogFile << "else begin" << endl << operations.at(l)->getOutputs().at(0)->getName() << " <= 0;" << endl << "end";
												}
												else if (operations.at(l)->getType() == 8) {	//IF x3
													verilogFile << "if (1) begin" << endl;
													//FIXME
													//FIXME	
													verilogFile << "end" << endl;
												}
												else if (operations.at(l)->getType() == 9) {	//ELSE x3
													verilogFile << "else begin" << endl;
													//FIXME
													//FIXME
													verilogFile << "end" << endl;
												}
											}
										}
									}
									//FIXME
									verilogFile << "end" << endl;
								}
							}
						}
					}
					verilogFile << "end";
					break;
				case 10: verilogFile << "//FOR ";
					break;
				case 11: verilogFile << "//DIV ";
					break;
				case 12: verilogFile << "//MOD ";
					break;
				case 13: verilogFile << "//NOP ";
					break;
				}
				verilogFile << endl;
			}
		}
		if (i != latency) { verilogFile << "State <= S" << i << ";" << endl << "end" << endl; }
		else { verilogFile << "State <= Final;" << endl << "end" << endl; }
	}
	verilogFile << "Final: begin" << endl;
	verilogFile << "Done <= 1;" << endl << "State <= Wait;" << endl << "end" << endl << "endcase" << endl << "end" << endl << "end" << endl << "endmodule" << endl;

	cout << "DONE:" << argv[1] << endl << endl;
	cFile.close();
	verilogFile.close();
}
