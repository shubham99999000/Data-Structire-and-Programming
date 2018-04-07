/****************************************************************************
  FileName     [ cirSim.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir simulation functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <fstream>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cassert>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"

using namespace std;


// TODO: Keep "CirMgr::randimSim()" and "CirMgr::fileSim()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/

/**************************************/
/*   Static varaibles and functions   */
/**************************************/

/************************************************/
/*   Public member functions about Simulation   */
/************************************************/
void
CirMgr::randomSim()
{

	string str;
	int failCount = 0;
	int hachSize = 0;
	int j, numPi, i = 0;
	cout << "MAX_FAILS = " << synt[1]*2 << endl;

	while(failCount < synt[1]*2){
		for(j = 0; j < synt[1]; j++){
			char binary[] = "01";
			str.push_back(binary[(rand() % 2)]);
		}
		j = 0;
		numPi = 0;
		while(j < _piList.size()){
			if(_piList[j]->getGatetype() == PI_GATE){
				piValSet(_piList[j], str[numPi]);
				numPi ++;
			}
			j ++;
		}  	 
		
		for(int j = 0, hashCount = 0; j < _dfsList.size(); j++){  		
	   		if(_dfsList[j]->getFanInSize() != 0) defValue(_dfsList[j], i);															// non PI, const (AIG, UNF, PO)
	   		if(_dfsList[j]->getGatetype() != PI_GATE && _dfsList[j]->getGatetype() != PO_GATE) {							// non PI, PO    (AIG, UNF, Const)
	   			setHash(_dfsList[j], i, hashCount);
	   			hashCount ++;
	   		}	
   		}
	   	resetSep();
		resetPi();

		str = "";
		i++;

		if(hachSize == _simHach.size()) failCount ++;
	   	else failCount = 0;
	   	hachSize = _simHach.size(); 
	   	cout << " Total #FEC Group = " << hachSize << "\033[A" << endl;
	}
	cout << i << " patterns simulated." << endl;
}

void
CirMgr::fileSim(ifstream& patternFile)
{
	// read file
	char ch;
	vector<string> fileStr;
   string str;
   bool patternErr = false;
   char errCh;

	while(patternFile.get(ch)){
		if(ch != '0' && ch != '1' && ch != '\n'){
			patternErr = true;
			errCh = ch;
		}
      if(ch == '\n'){            // getline end
      	if(patternErr) {
      		cout << "Error: Pattern(" << str << ") contains a non-0/1 character(‘" <<  errCh << "’)." << endl;
      		return;
      	}
      	if(str.length() != synt[1] && str.length() != 0){
      		cout << "Error: Pattern(" << str << ") length(" << str.length() << ") does not match the number of inputs(" << synt[1] << ") in a circuit!!" << endl;
      		return;
      	}
         if(str.length() != 0) fileStr.push_back(str);
         //cout << str << endl;
         str = "";
      }  
      else 
         str.push_back(ch);
   }
   patternFile.close();
   patternFile.clear();
   //printDfs();

   for(int i = 0; i < fileStr.size(); i ++){
   	int j = 0, numPi = 0;
   	while(j < _piList.size()){
   		if(_piList[j]->getGatetype() == PI_GATE){
   			piValSet(_piList[j], fileStr[i][numPi]);
   			numPi ++;
   		}
   		j ++;
   	}  	 

   	for(int j = 0, hashCount = 0; j < _dfsList.size(); j++){  		
   		if(_dfsList[j]->getFanInSize() != 0) defValue(_dfsList[j], i);																            // non PI, const (AIG, UNF, PO)
   		if(_dfsList[j]->getGatetype() != PI_GATE && _dfsList[j]->getGatetype() != PO_GATE) {													// non PI, PO    (AIG, UNF, Const)
   			setHash(_dfsList[j], i, hashCount);
   			hashCount ++;
   		}	
   	}

   	const char* fChr = fileStr[i].c_str();
   	*_simLog << fChr << ' ';
   	for(j = 0; j < _poList.size(); j++)
   		*_simLog << _poList[j]->getValue();
   	*_simLog << '\n';

   	resetSep();
   	resetPi();
   	cout << " Total #FEC Group = " << _simHach.size() << "\033[A" << endl;
   }
   cout << fileStr.size() << " patterns simulated." << endl;

   // to prove that I am right
   /*
   int countPair = 0;
   for(int i = 0; i < _simHach.size(); i++){
   	if(_simHach[i]->size() > 1) ++countPair;
   }
   cout << countPair;
   */
  
}

void
CirMgr::piValSet(CirGate* g, char& pi){
	if(pi == '1'){
		g->setValue(true);
		g->setValSet(true);
	}
	else g->setValSet(false);
}

void 
CirMgr::defValue(CirGate* g, int& it){
	g->setValToPre();
	if(g->getFanInSize() == 2){
		bool gV0 = g->getFanIn(0)->getValue();
		if(g->faninIsInv(0)) gV0 = !gV0;
		bool gV1 = g->getFanIn(1)->getValue();
		if(g->faninIsInv(1)) gV1 = !gV1;
		g->setValue(gV0 && gV1);
		g->setValSet(gV0 && gV1);

	}
	else {
		bool gV = g->getFanIn(0)->getValue();
		if(g->faninIsInv(0)) gV = !gV;
		g->setValue(gV);
		g->setValSet(gV);

	}
}

void
CirMgr::setHash(CirGate* g, int& it, int& col){
	if(it == 0 && _simHach.size() <= 1){
		if(_simHach.size() == 0) {
			FECGroup* f = new FECGroup(0, 0);
			_simHach.push_back(f);
		}
		_simHach[0]->insert(g);
		g->setBukSl(0, col);
	}
	else{
		CirGate* _topHashG = _simHach[g->getBuk()]->getGate(0);
		if(((g->getValue() == _topHashG->getValue()) && (g->getPreVal() != _topHashG->getPreVal())) || ((g->getValue() != _topHashG->getValue()) && (g->getPreVal() == _topHashG->getPreVal()))){
			size_t origBuk = g->getBuk();
			size_t blankSlot = g->getSl();

			if(!_simHach[origBuk]->isSep()){		
				// set Seperate
				_simHach[origBuk]->setSep(true);
				// new FECGroup and insert first gate 	
				size_t _hashSize = _simHach.size();
				FECGroup* newFEC = new FECGroup(_hashSize, 0);
				_simHach.push_back(newFEC);
				_simHach[_hashSize]->insert(g);

				// change gate's bucket and slot and original FECGroup's seperative bucket
				_simHach[origBuk]->setSepBuk(_hashSize);	
				g->setBukSl(_hashSize, 0);						
			}
			// isSep & move gate to new buk;
			else{
				size_t sepBuk = _simHach[origBuk]->getSepBuk();
				size_t sl = _simHach[sepBuk]->size();
				_simHach[sepBuk]->insert(g);
				g->setBukSl(sepBuk, sl);
			}
			// move last to full the blank
			CirGate* _lastGate = _simHach[origBuk]->getLastPop();
			if(g != _lastGate){
				_simHach[origBuk]->setGate(_lastGate, blankSlot);
				_lastGate->setBukSl(origBuk, blankSlot);
			}
		}
	} 
}

void CirMgr::resetSep(){
	for(int i = 0; i < _simHach.size(); i++)
		_simHach[i]->setSep(false);
}

void 
CirMgr::resetPi(){
	for(int i = 0; i < _piList.size(); i ++)
		_piList[i]->setValue(false);
}



/*************************************************/
/*   Private member functions about Simulation   */
/*************************************************/
