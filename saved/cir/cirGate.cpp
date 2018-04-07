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
#include <bitset>
#include "cirGate.h"
#include "cirMgr.h"
#include "util.h"

using namespace std;


// TODO: Keep "CirGate::reportGate()", "CirGate::reportFanin()" and
//       "CirGate::reportFanout()" for cir cmds. Feel free to define
//       your own variables and functions.

extern CirMgr *cirMgr;
unsigned CirGate::_globalRef = 0;

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
	if(_gateType == CONST_GATE) cout << ", line " << (_lineNo);
	else 						cout << ", line " << (_lineNo+1);

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
	// FEC
	cout << "= FECs:";
	unsigned blk = 0;
	blk = cirMgr->printFEC(_bukSl.first, this);
	for(int i = 0, n = 38 - blk; i < n; i++)
    	cout << ' ';
   	cout << "  =" << endl;
   	// Value
	bitset<32> bitset1 (_valueSet);
	cout << "= Value: ";
	for(int i = 0; i < 32; i++){
		cout << bitset1[i];
		if(i%4 == 3 && i != 31) cout << '_';
	}
	cout << " =" << endl;
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

void
CirGate::eraseFanOut(int& i){
	if(i != _fanOutList.size() - 1){
		_fanOutList[i] = _fanOutList[_fanOutList.size() - 1];
		i --;
	}
	_fanOutList.pop_back();
}

void
CirGate::eraseFanIn(int& i){
	if(i == 0)
		_fanInList[0] = _fanInList[1];
	_fanInList.pop_back();
}

void
CirGate::setValSet(bool b){
	if(b) _valueSet = (_valueSet << 1) + 1;
	else _valueSet = _valueSet << 1;
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
	this->_sw = false;
}




