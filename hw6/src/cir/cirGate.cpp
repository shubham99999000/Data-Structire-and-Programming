/****************************************************************************
  FileName     [ cirGate.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define class CirAigGate member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <stdarg.h>
#include <cassert>
#include "cirGate.h"
#include "cirMgr.h"
#include "util.h"

using namespace std;

extern CirMgr *cirMgr;
unsigned CirGate::_globalRef = 0;

// TODO: Implement memeber functions for class(es) in cirGate.h

/**************************************/
/*   class CirGate member functions   */
/**************************************/
string CirGate::getTypeStr() const{
	string str;
	if(_gateType == PI_GATE) 	      str = "PI";
	else if(_gateType == PO_GATE)    str = "PO";
	else if(_gateType == AIG_GATE)   str = "AIG";
	else if(_gateType == CONST_GATE) str = "CONST";
	else										str = "UNDEF";	
	return str;
}

void 
CirGate::printGate() const{
	int i;
	cout << " " << getTypeStr();
	if(_gateType == AIG_GATE) cout << " ";										// CONST0 (no space)
	else if(_gateType == PI_GATE || _gateType == PO_GATE) cout << "  ";
	cout << getGateId() << " ";

	// Undef & Inv
	for(i = 0; i < _fanInList.size(); i++){
		if(_fanInList[i].gate()->getGatetype() == UNDEF_GATE)  cout << '*'; 		
		if(_fanInList[i].isInv())                              cout << '!';
		cout << _fanInList[i].gate()->getGateId() << ' ';
	}
	// Symbol
	if(_gateType == 1){
		if(hasSymbol()) cout << "(" << getGateId() << "GAT)";
	}

	else if(_gateType == 2){
		if(hasSymbol()) cout << "(" << _fanInList[0].gate()->getGateId() << "GAT$PO)";
	}
	cout << endl;
}

void
CirGate::reportGate() const
{
	int carry = 1, track = 0, carryL = 1;
	cout << "==================================================" << endl;
	cout << "= " << getTypeStr() << '(' << _gateId << ')';
	if(hasSymbol()){
		if(_gateType == PI_GATE)
			cout << "\"" << getGateId() << "GAT\"";
		else if(_gateType == PO_GATE)
			cout << "\"" << _fanInList[0].gate()->getGateId() << "GAT$PO\"";
		track = _symbol.size() + 2;
	}
	cout << ", line " << (_lineNo+1);

	// deal space
	float j = _gateId;
	while(j/10 > 1){
		j = j/10;
		carry++;
	}
	float k = (_lineNo+1);
	while(k/10 > 1){
		k = k/10;
		carryL++;
	}
	track += getTypeStr().size() + carry + 2 + 2 + 2 + 4 + 1 + carryL;
	for(int i = 0; i < (50-2-track); i++)
		cout << ' ';
	cout  << " =" << endl;
	cout << "==================================================" << endl;
}

void
CirGate::reportFanin(int level) const
{
   assert (level >= 0);
   vector<const CirGate*> _track;
   faninPrint(_track, this, level, false, 0);
}

void
CirGate::reportFanout(int level) const
{
   assert (level >= 0);
   vector<const CirGate*> _track;
   fanoutPrint(_track, this, level, 0, 0);
}

void CirGate::faninPrint(vector<const CirGate*>& _track, const CirGate* g, int level, bool inV, int depth) const{
	int i;
	for(i = 0; i < depth; i++)
		cout << "  ";
	if(inV) cout << '!';
	cout << g->getTypeStr() << ' ' << g->getGateId();

	if(depth == level){																							// not sure
		cout << endl;
		return;	
	}	
	// detect exist
	for(i = 0; i < _track.size(); i++){
		if(_track[i] == g){
			if(g->getFanInSize() != 0) cout << " (*)" << endl;
			else cout << endl;
			return;
		}
	}
	cout << endl;
	_track.push_back(g);
	

	for(i = 0; i < g->getFanInSize(); i++){
		faninPrint(_track, g->getFanIn(i), level, g->_fanInList[i].isInv(), depth+1);
	}
}

void CirGate::fanoutPrint(vector<const CirGate*>& _track, const CirGate* g, int level, const CirGate* fanIn, int depth)const{
	int i;
	for(i = 0; i < depth; i++)
		cout << "  ";
	if(fanIn){
		for(i = 0; i < g->getFanInSize(); i++){
			if(g->getFanIn(i) == fanIn){
				if(g->_fanInList[i].isInv()) cout << '!';
				break;
			}
		}
	}
	cout << g->getTypeStr() << ' ' << g->getGateId();

	if(depth == level){																							// not sure
		cout << endl;
		return;	
	}	
	// detect exist
	for(i = 0; i < _track.size(); i++){
		if(_track[i] == g){
			if(g->getFanOutSize() != 0) cout << " (*)" << endl;
			else cout << endl;
			return;
		}
	}
	cout << endl;
	_track.push_back(g);
	

	for(i = 0; i < g->getFanOutSize(); i++){
		fanoutPrint(_track, g->getFanOut(i), level, g, depth+1);
	}
	
}


void CirGate::dfs(GateList& d){
	for(int i = 0; i < _fanInList.size(); i++){
		CirGate* c = _fanInList[i].gate();
		if((c->eqaulGloRef() == false) && (c->getGatetype() != UNDEF_GATE)){
			c->setToGlo();
		 	c->dfs(d);
		}		 
	}
	d.push_back(this);
}

