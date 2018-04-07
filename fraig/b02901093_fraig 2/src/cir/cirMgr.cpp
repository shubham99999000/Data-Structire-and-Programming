/****************************************************************************
  FileName     [ cirMgr.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir manager functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cstdio>
#include <ctype.h>
#include <cassert>
#include <cstring>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"

using namespace std;

// TODO: Implement memeber functions for class CirMgr

/*******************************/
/*   Global variable and enum  */
/*******************************/
CirMgr* cirMgr = 0;

enum CirParseError {
   EXTRA_SPACE,
   MISSING_SPACE,
   ILLEGAL_WSPACE,
   ILLEGAL_NUM,
   ILLEGAL_IDENTIFIER,
   ILLEGAL_SYMBOL_TYPE,
   ILLEGAL_SYMBOL_NAME,
   MISSING_NUM,
   MISSING_IDENTIFIER,
   MISSING_NEWLINE,
   MISSING_DEF,
   CANNOT_INVERTED,
   MAX_LIT_ID,
   REDEF_GATE,
   REDEF_SYMBOLIC_NAME,
   REDEF_CONST,
   NUM_TOO_SMALL,
   NUM_TOO_BIG,

   DUMMY_END
};

bool lexOptions(const string& option, vector<string>& tokens, size_t nOpts);

/**************************************/
/*   Static varaibles and functions   */
/**************************************/
static unsigned lineNo = 0;  // in printint, lineNo needs to ++
static unsigned colNo  = 0;  // in printing, colNo needs to ++
//static char buf[1024];
static string errMsg;
static int errInt;
static CirGate *errGate;

static bool
parseError(CirParseError err)
{
   switch (err) {
      case EXTRA_SPACE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Extra space character is detected!!" << endl;
         break;
      case MISSING_SPACE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Missing space character!!" << endl;
         break;
      case ILLEGAL_WSPACE: // for non-space white space character
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Illegal white space char(" << errInt
              << ") is detected!!" << endl;
         break;
      case ILLEGAL_NUM:
         cerr << "[ERROR] Line " << lineNo+1 << ": Illegal "
              << errMsg << "!!" << endl;
         break;
      case ILLEGAL_IDENTIFIER:
         cerr << "[ERROR] Line " << lineNo+1 << ": Illegal identifier \""
              << errMsg << "\"!!" << endl;
         break;
      case ILLEGAL_SYMBOL_TYPE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Illegal symbol type (" << errMsg << ")!!" << endl;
         break;
      case ILLEGAL_SYMBOL_NAME:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Symbolic name contains un-printable char(" << errInt
              << ")!!" << endl;
         break;
      case MISSING_NUM:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Missing " << errMsg << "!!" << endl;
         break;
      case MISSING_IDENTIFIER:
         cerr << "[ERROR] Line " << lineNo+1 << ": Missing \""
              << errMsg << "\"!!" << endl;
         break;
      case MISSING_NEWLINE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": A new line is expected here!!" << endl;
         break;
      case MISSING_DEF:
         cerr << "[ERROR] Line " << lineNo+1 << ": Missing " << errMsg
              << " definition!!" << endl;
         break;
      case CANNOT_INVERTED:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": " << errMsg << " " << errInt << "(" << errInt/2
              << ") cannot be inverted!!" << endl;
         break;
      case MAX_LIT_ID:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Literal \"" << errInt << "\" exceeds maximum valid ID!!"
              << endl;
         break;
      case REDEF_GATE:
         cerr << "[ERROR] Line " << lineNo+1 << ": Literal \"" << errInt
              << "\" is redefined, previously defined as "
              << errGate->getTypeStr() << " in line " << errGate->getLineNo()
              << "!!" << endl;
         break;
      case REDEF_SYMBOLIC_NAME:
         cerr << "[ERROR] Line " << lineNo+1 << ": Symbolic name for \""
              << errMsg << errInt << "\" is redefined!!" << endl;
         break;
      case REDEF_CONST:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Cannot redefine const (" << errInt << ")!!" << endl;
         break;
      case NUM_TOO_SMALL:
         cerr << "[ERROR] Line " << lineNo+1 << ": " << errMsg
              << " is too small (" << errInt << ")!!" << endl;
         break;
      case NUM_TOO_BIG:
         cerr << "[ERROR] Line " << lineNo+1 << ": " << errMsg
              << " is too big (" << errInt << ")!!" << endl;
         break;
      default: break;
   }
   return false;
}

/**************************************************************/
/*   class CirMgr member functions for circuit construction   */
/**************************************************************/
CirGate* CirMgr::getGate(unsigned t) const{
   for(int i = 1; i <= _totalList.size(); i ++){
      if(_totalList[i-1]->getGateId() == t) return _totalList[i-1];
   }
   return 0;
}

CirGate* CirMgr::getUndef(unsigned t)const{
   for(int i = 1; i <= _undefList.size(); i ++){
      if(_undefList[i-1]->getGateId() == t) return _undefList[i-1];
   }
   return 0;
}

bool
CirMgr::readCircuit(const string& fileName)
{
char ch;
   vector<string> fileStr;
   string str;

   ifstream in(fileName);
   if(!in){
      cout << "Cannot open design " << fileName << "!!" << endl;
      return false;
   }

  
   while(in.get(ch)){
      if(ch == '\n'){            // getline end
         fileStr.push_back(str);
         str = "";
      }  
      else 
         str.push_back(ch);
   }
   in.close();
   in.clear();

   int i;
   // line 1
   vector<string> tokens;
   if(lexOptions(fileStr[0], tokens, 6) == false) return false;
   if(tokens[0] != "aag"){
      errMsg = tokens[0];
      return parseError(ILLEGAL_IDENTIFIER);
   }
   for(i = 1; i <= 5; i++){
      if(myStr2Int(tokens[i], synt[i-1]) == false){
         errMsg = tokens[i];
         return parseError(ILLEGAL_NUM);
      }
      if(synt[i-1] < 0){
         errMsg = "Number";
         errInt = synt[i-1];
         return parseError(NUM_TOO_SMALL);
      }
   }
   if(synt[0] < synt[1] + synt[2] + synt[4]){
      errMsg = "Number of variables";
      errInt = synt[0];
      return parseError(NUM_TOO_SMALL);
   }

   // pi
   lineNo = 1;// line 2
   for(i = 1; i < 1 + synt[1]; i++){
      int Num;
      if(myStr2Int(fileStr[i], Num) == false){
         errMsg = fileStr[i];
         return parseError(ILLEGAL_NUM);
      }
      if(Num < 0){
         errMsg = "literal ID";
         errInt = Num;
         return parseError(NUM_TOO_SMALL);
      }
      CirGate* temp;
      if(Num != 0)  temp = new CirGate(unsigned(Num/2), PI_GATE, fileStr[i], i, false);    // gateId, getType, lineNum, lineNo(start from zero)
      else          temp = new CirGate(unsigned(Num/2), CONST_GATE, fileStr[i], 0, false);
      _totalList.push_back(temp);
      _piList.push_back(temp);

   }
   // po
   lineNo = 1 + synt[1];
   for(i = 0; i < synt[3]; i++){
      int Num;
      if(myStr2Int(fileStr[i + lineNo], Num) == false){
         errMsg = fileStr[i + lineNo];
         return parseError(ILLEGAL_NUM);
      }
      if(Num < 0){
         errMsg = "literal ID";
         errInt = Num;
         return parseError(NUM_TOO_SMALL);
      }
      CirGate* temp = new CirGate(unsigned(1 + i + synt[0]), PO_GATE, fileStr[i+lineNo], unsigned(i + lineNo), false);
      _totalList.push_back(temp);
      _poList.push_back(temp);
      //cout << unsigned(i + synt[0]) << " " << unsigned(Num) << endl;
   }

   //aig 
   lineNo = 1 + synt[1] + synt[3];
   int j = lineNo;
   for(; lineNo < (j+synt[4]); lineNo++){
      tokens.clear();
      int Num;
      if(lexOptions(fileStr[lineNo], tokens, 3) == false) return false;

      if(myStr2Int(tokens[0], Num) == false){
         errMsg = tokens[0];
         return parseError(ILLEGAL_NUM);
      }
      if(Num < 0){
         errMsg = "literal ID";
         errInt = Num;
         return parseError(NUM_TOO_SMALL);
      }

      int tempNum = Num;

      if(myStr2Int(tokens[1], Num) == false){
         errMsg = tokens[1];
         return parseError(ILLEGAL_NUM);
      }
      if(Num < 0){
         errMsg = "literal ID";
         errInt = Num;
         return parseError(NUM_TOO_SMALL);
      }
      if(myStr2Int(tokens[2], Num) == false){
         errMsg = tokens[2];
         return parseError(ILLEGAL_NUM);
      }
      if(Num < 0){
         errMsg = "literal ID";
         errInt = Num;
         return parseError(NUM_TOO_SMALL);
      }

      CirGate* temp = new CirGate(unsigned(tempNum/2), AIG_GATE, fileStr[lineNo], unsigned(lineNo), false);
      _totalList.push_back(temp);
   }
   while(fileStr.size() != lineNo){
      // symbol
      if(myStrNCmp("c", fileStr[lineNo], 1) != 0 ){
         string tok;
         size_t n = myStrGetTok(fileStr[lineNo], tok);
         if(n < 0){
            errMsg = "Symbol";
            return parseError(MISSING_IDENTIFIER);
         }
         char ch = tok[0];
         tok = tok.substr(1);
         int Num;
         if(myStr2Int(tok, Num) == false){
            errMsg = tok;
            colNo = 1;
            return parseError(ILLEGAL_SYMBOL_TYPE);
         }
         if(Num < 0){
            errMsg = "literal ID";
            errInt = Num;
            return parseError(NUM_TOO_SMALL);
         }
         tok = fileStr[lineNo].substr(n+1);

         if(ch == 'i')         _piList[Num]->setSymbol(tok);
         else if(ch == 'o')    _poList[Num]->setSymbol(tok);
         else{
            errMsg = ch;
            colNo = 0;
            return parseError(ILLEGAL_SYMBOL_TYPE);
         }
      }
      else break;
      lineNo++;
   }

   //set connection
   for(i = 0; i < _totalList.size(); i++){
      if(_totalList[i]->getGatetype() == PO_GATE){
         int Num;
         if(myStr2Int(_totalList[i]->getLineNum(), Num) == false){
            errMsg = _totalList[i]->getLineNum();
            return parseError(ILLEGAL_NUM);
         }
         CirGate* g = getGate(Num/2);           // fanin
         if(g == 0){
            if(Num/2 == 0){
               g = new CirGate(0, CONST_GATE, "CONST", 0, false);      // gateId, getType, lineNum, lineNo(start from zero)
               _totalList.push_back(g);
            }
            // undef
            else{
               g = getUndef(Num/2);
               if(g == 0){
                  g = new CirGate(unsigned(Num/2), UNDEF_GATE, "UNDEF", 0, false); 
                  _undefList.push_back(g);
               }
            }
         }
         _totalList[i]->addFanIn(g,Num%2);
         g->addFanOut(_totalList[i]);
      }
      else if(_totalList[i]->getGatetype() == AIG_GATE){
         int Num;
         tokens.clear();
         if(lexOptions(_totalList[i]->getLineNum(), tokens, 3) == false) return false;
         for(j = 1; j < 3; j++){
            myStr2Int(tokens[j], Num);
            CirGate* g = getGate(Num/2);
            if(g == 0){
               if(Num/2 == 0){
                  g = new CirGate(0, CONST_GATE, "CONST", 0, false);      // gateId, getType, lineNum, lineNo(start from zero)
                  _totalList.push_back(g);
               }
               // undef
               else{
                  g = getUndef(Num/2);
                  if(g == 0){
                     g = new CirGate(unsigned(Num/2), UNDEF_GATE, "UNDEF", 0, false); 
                     _undefList.push_back(g);
                  }
               }
            }
            _totalList[i]->addFanIn(g,Num%2);
            g->addFanOut(_totalList[i]);
         }
      }
   }

   // default to set all gate true
   for(int i = 0; i < _totalList.size(); i ++)
      _totalList[i]->setSw(true);
   for(int i = 0; i < _undefList.size(); i ++)
      _undefList[i]->setSw(true);

   // if no CONST0, add it
   CirGate* g0 = getGate(0);
   if(g0 == 0){
      g0 = new CirGate(0, CONST_GATE, "CONST", 0, false);
      _totalList.push_back(g0);
      g0->setSw(false);
   }
   // setRepSim = false 
   setRepSim(false);

   
   

   // dfs
   CirGate::setGlobalRef();
   for(i = 0; i < _poList.size(); i++)
      _poList[i]->dfs(_dfsList);


   return true;
}


/**********************************************************/
/*   class CirMgr member functions for circuit printing   */
/**********************************************************/
/*********************
Circuit Statistics
==================
  PI          20
  PO          12
  AIG        130
------------------
  Total      162
*********************/
void
CirMgr::printSummary() const
{
   cout << "Circuit Statistics" << endl;
   cout << "==================" << endl; 
   cout << "  PI   " << setw(9) << right << synt[1] << "  " << endl;
   cout << "  PO   " << setw(9) << right << synt[3] << "  " << endl;
   int consT = 0;
   if(getGate(0)) consT = 1;
   cout << "  AIG  " << setw(9) << right << (_totalList.size() - synt[1] - synt[3] - consT) << "  " << endl;
   cout << "------------------" << endl; 
   cout << "  Total" << setw(9) << right << _totalList.size() - consT << "  " << endl;
}

void
CirMgr::printNetlist() const
{
   for(int i = 0; i < _dfsList.size(); i++){
      cout << "[" << i << "]";
      _dfsList[i]->printGate();
   }
}

void
CirMgr::printPIs() const
{
   cout << "PIs of the circuit:";
   for(int i = 0; i < _piList.size(); i++)
      cout << ' ' << _piList[i]->getGateId();
   cout << endl;
}

void
CirMgr::printPOs() const
{
   cout << "POs of the circuit:";
   for(int i = 0; i < _poList.size(); i++)
      cout << ' ' << _poList[i]->getGateId();
   cout << endl;
}

// for testing
void
CirMgr::printDfs() const
{
   cout << "DFS of the circuit:";
   for(int i = 0; i < _dfsList.size(); i++){
      cout << "ID is : " << _dfsList[i]->getGateId() << ' ';      
      cout << "   FanIn is : ";
      for(int j = 0; j < _dfsList[i]->getFanInSize(); j++){         
         cout << _dfsList[i]->getFanIn(j)->getGateId() << ' ';
      }
      cout << "   FanOut is : ";
      for(int j = 0; j < _dfsList[i]->getFanOutSize(); j++){         
         cout << _dfsList[i]->getFanOut(j)->getGateId() << ' ';
      }
      cout << endl;
   }
   cout << endl;
}

void
CirMgr::printFloatGates() const
{
   int i, j, k;
   bool add;
   IdList c1, c2;

   for(i = 1; i <= _undefList.size(); i++){
      for(j = 1; j <= (_undefList[i-1]->getFanOutSize()); j++){
         add = true;
         unsigned n = _undefList[i-1]->getFanOut(j-1)->getGateId();
         for(k = 0; k < c1.size(); k++){
            if(n == c1[k]) 
               add = false;
         }
         if(add) 
            c1.push_back(n);
      }
   }
   sort(c1.begin(), c1.end());

   for(i = 1; i <= _totalList.size(); i++){
      GateType g =  _totalList[i-1]->getGatetype();
      if(g!= PO_GATE && g != UNDEF_GATE && g != CONST_GATE && _totalList[i-1]->getFanOutSize() == 0){
         add = true;
         unsigned n = _totalList[i-1]->getGateId();
         for(k = 0; k < c2.size(); k++){
            if(n == c2[k]) 
               add = false;
         }
         if(add) 
            c2.push_back(n);
      }
   }
   sort(c2.begin(), c2.end());

   if(c1.size() > 0){
      cout << "Gates with floating fanin(s) : ";
      for(i = 1; i <= c1.size(); i++) cout << c1[i-1] << ' ';
      cout << endl;
   }

   if(c2.size() > 0){
      cout << "Gates defined but not used : ";
      for(i = 1; i <= c2.size(); i++) cout << c2[i-1] << ' ';
      cout << endl;
   }
}

unsigned
CirMgr::printFEC(const unsigned& hch, const CirGate* gt ) const
{
   unsigned blk = 0;
   if(gt->getGatetype() != AIG_GATE) return blk;
   if(_simHach.size() > 0){ 
      for(int i = 0, n = _simHach[hch]->size(); i < n; i++){
         CirGate* g = _simHach[hch]->getGate(i);
         if(g != gt){
            ++blk;
            bool iFEC = ((gt->getValue() == g->getValue())? false : true);
            cout << ' ';
            if(iFEC){
               cout << '!';
               ++blk;
            }
            ++blk;
            unsigned id = g->getGateId();
            cout << id;
            while(id/10 > 1){
               id = id/10;
               ++blk;
            }
         }
      }
   }
   else cout << "  ";
   return blk;
}  

void
CirMgr::printFECPairs() const
{
   if(_simHach.size() > 0){
      int it = 0;
      for(int i = 0, n = _simHach.size(); i < n; i++){
         int m = _simHach[i]->size();
         if(m >= 2){
            CirGate* gT = _simHach[i]->getGate(0);
            cout << '[' << it << "]"; 
            for(int j = 0; j < m; j++){
               CirGate* g = _simHach[i]->getGate(j);
               cout << ' ';
               bool iFEC = ((gT->getValue() == g->getValue())? false : true);
               if(iFEC) cout << '!';
               cout << g->getGateId();   
            }
            cout << endl;
            ++it;
         }
      }
   }
}

void
CirMgr::writeAag(ostream& outfile) const
{
   int i;
   // Header
   outfile << "aag ";
   for (i = 1; i < 5; i++)
      outfile << synt[i-1] << ' ';

   GateList Aiggate;
   for(i = 1; i <= _dfsList.size(); i++){
      if(_dfsList[i-1]->getGatetype() == AIG_GATE)
         Aiggate.push_back(_dfsList[i-1]);
   }
   outfile << Aiggate.size() << endl;
   // Pi, Po
   for(i = 1; i <= _piList.size(); i++)
      outfile << _piList[i-1]->getLineNum() << endl;
   for(i = 1; i <= _poList.size(); i++)
      outfile << _poList[i-1]->getLineNum() << endl;
   // AIG
   for(i = 1; i <= Aiggate.size(); i++)
      outfile << Aiggate[i-1]->getLineNum() << endl;
   // Sym
   for(i = 1; i <= _piList.size(); i++){
      if(_piList[i-1]->hasSymbol())      
         outfile << 'i'  << i << ' ' << _piList[i-1]->getSymbol() << endl;
   }
   for(i = 1; i <= _poList.size(); i++){
      if(_poList[i-1]->hasSymbol())      
         outfile << 'o'  << i << ' ' << _poList[i-1]->getSymbol() << endl;
   }

   outfile << "c" << endl;
   outfile << "AAG output by b02901093" << endl;
}

void
CirMgr::writeGate(ostream& outfile, CirGate *g) const
{
}

bool 
lexOptions(const string& option, vector<string>& tokens, size_t nOpts)
{
   string token;
   size_t n = myStrGetTok(option, token);
   while (token.size()) {
      tokens.push_back(token);
      n = myStrGetTok(option, token, n);
   }
   if (nOpts != 0) {
      if (tokens.size() < nOpts) {
         cerr << "Error: Missing option" << " after ()" << "!!" << endl;
         return false;
      }
      if (tokens.size() > nOpts) {
         cerr << "Error: Extra option!! (" << tokens[nOpts] << ")" << endl;
         return false;
      }
   }
   return true;
}

