/****************************************************************************
  FileName     [ memCmd.cpp ]
  PackageName  [ mem ]
  Synopsis     [ Define memory test commands ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2007-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#include <iostream>
#include <iomanip>
#include "memCmd.h"
#include "memTest.h"
#include "cmdParser.h"
#include "util.h"

using namespace std;

extern MemTest mtest;  // defined in memTest.cpp

bool
initMemCmd()
{
   if (!(cmdMgr->regCmd("MTReset", 3, new MTResetCmd) &&
         cmdMgr->regCmd("MTNew", 3, new MTNewCmd) &&
         cmdMgr->regCmd("MTDelete", 3, new MTDeleteCmd) &&
         cmdMgr->regCmd("MTPrint", 3, new MTPrintCmd)
      )) {
      cerr << "Registering \"mem\" commands fails... exiting" << endl;
      return false;
   }
   return true;
}


//----------------------------------------------------------------------
//    MTReset [(size_t blockSize)]
//----------------------------------------------------------------------
CmdExecStatus
MTResetCmd::exec(const string& option)
{
   // check option
   string token;
   if (!CmdExec::lexSingleOption(option, token))
      return CMD_EXEC_ERROR;
   if (token.size()) {
      int b;
      if (!myStr2Int(token, b) || b < int(toSizeT(sizeof(MemTestObj)))) {
         cout << toSizeT(b) << endl;
         cerr << "Illegal block size (" << token << ")!!" << endl;
         return CmdExec::errorOption(CMD_OPT_ILLEGAL, token);
      }
      #ifdef MEM_MGR_H
      mtest.reset(toSizeT(b));
      #else
      mtest.reset();
      #endif // MEM_MGR_H
   }
   else
      mtest.reset();
   return CMD_EXEC_DONE;
}

void
MTResetCmd::usage(ostream& os) const
{  
   os << "Usage: MTReset [(size_t blockSize)]" << endl;
}

void
MTResetCmd::help() const
{  
    cout << setw(15) << left << "MTReset: " 
        << "(memory test) reset memory manager" << endl;
}


//----------------------------------------------------------------------
//    MTNew <(size_t numObjects)> [-Array (size_t arraySize)]
//----------------------------------------------------------------------
CmdExecStatus
MTNewCmd::exec(const string& option)
{
   // TODO
    if (option.empty())
        return CmdExec::errorOption(CMD_OPT_MISSING, "");

    vector<string> options;
    int numObj, arrSize;
    if (!CmdExec::lexOptions(option, options))
        return CMD_EXEC_ERROR;

    if(myStr2Int(options[0],numObj)){
        if(numObj <= 0)
            return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[0]);
        else{
            // distinguish array or object
            if(options.size() == 1){
                try{
                    mtest.newObjs(numObj); 
                }
                catch(bad_alloc){
                    return CMD_EXEC_ERROR;
                }        
            }
            // array
            else if(myStrNCmp("-Array",options[1],2) == 0){
                if(options.size() == 2)
                    return CmdExec::errorOption(CMD_OPT_MISSING, options[1]);
                
                if(options.size() >3)
                    return CmdExec::errorOption(CMD_OPT_EXTRA, options[3]);

                if(myStr2Int(options[2],arrSize)){
                    if(arrSize <= 0)
                        return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[2]);
                    else{
                        try{
                           mtest.newArrs(numObj,arrSize); 
                        }
                        catch(bad_alloc){
                            return CMD_EXEC_ERROR;
                        }                       
                    }
                }               
                else 
                    return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[2]);
            }

            else 
                return CmdExec::errorOption(CMD_OPT_EXTRA, options[1]);
        }   
    }
    else if(myStrNCmp("-Array",options[0],2) == 0){      
        if(options.size() == 3){
            if(!myStr2Int(options[1],arrSize))
                return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[1]);
            if(!myStr2Int(options[2],numObj))
                return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[2]);
            if(arrSize <= 0)
                return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[1]);
            if(numObj <= 0)
                return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[2]);

            try{
               mtest.newArrs(numObj,arrSize); 
            }
            catch(bad_alloc){
                return CMD_EXEC_ERROR;
            }        
        }
        else if(options.size() < 3)
            return CmdExec::errorOption(CMD_OPT_MISSING, options[1]);
        else
            return CmdExec::errorOption(CMD_OPT_EXTRA, options[3]);
    }


    else
        return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[0]);

   return CMD_EXEC_DONE;
}

void
MTNewCmd::usage(ostream& os) const
{  
   os << "Usage: MTNew <(size_t numObjects)> [-Array (size_t arraySize)]\n";
}

void
MTNewCmd::help() const
{  
   cout << setw(15) << left << "MTNew: " 
        << "(memory test) new objects" << endl;
}


//----------------------------------------------------------------------
//    MTDelete <-Index (size_t objId) | -Random (size_t numRandId)> [-Array]
//----------------------------------------------------------------------
CmdExecStatus
MTDeleteCmd::exec(const string& option)
{
   // TODO
    if (option.empty())
        return CmdExec::errorOption(CMD_OPT_MISSING, "");
    vector<string> options;
    int idx;
    if (!CmdExec::lexOptions(option, options))
        return CMD_EXEC_ERROR;

    if(myStrNCmp("-Index",options[0],2) == 0){
        if(options.size() == 1)
            return CmdExec::errorOption(CMD_OPT_MISSING, options[0]);
        if(!myStr2Int(options[1],idx))
            return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[1]);
        if(idx < 0)
            return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[1]);

        size_t sz;
        if(myStrNCmp("-Array",options[2],2) == 0){
            sz = mtest.getArrListSize();
            if(idx >= sz){
                cout << "Size of array list (" << sz << ") is <= " << idx << "!!" << endl;
                return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[1]);
            }
            else 
                mtest.deleteArr(idx);
        }   
        else{
            if(options.size() == 2){
                sz = mtest.getObjListSize();
                if(idx >= sz){
                    cout << "Size of object list (" << sz << ") is <= " << idx << "!!" << endl;
                    return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[1]);
                }
                else 
                    mtest.deleteObj(idx);
            }
            else
                return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[2]);
        } 
    }

    else if(myStrNCmp("-Random",options[0],2) == 0){
        if(options.size() == 1)
            return CmdExec::errorOption(CMD_OPT_MISSING, options[0]);
        if(!myStr2Int(options[1],idx))
            return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[1]);
        if(idx <= 0)
            return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[1]);

        size_t sz;
        if(myStrNCmp("-Array",options[2],2) == 0){
            sz = mtest.getArrListSize();
            if(sz == 0)
                cout << "Size of array list is 0!!" << endl;
            else{
                for(int i = 0; i < idx; i++)
                    mtest.deleteArr(rnGen(sz));
            } 
        }   
        else{
            if(options.size() == 2){
                sz = mtest.getObjListSize();
                if(sz == 0)
                    cout << "Size of object list is 0!!" << endl;
                else{                
                    for(int i = 0; i < idx; i++)
                        mtest.deleteObj(rnGen(sz));
                }
            }
            else
                return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[2]);
        } 
    }

    else if(myStrNCmp("-Array",options[0],2) == 0){
        if(options.size() == 1)
            return CmdExec::errorOption(CMD_OPT_MISSING, options[0]);

        size_t sz;
        if(myStrNCmp("-Random",options[1],2) == 0){
            if(options.size() == 3){
                if(!myStr2Int(options[2],idx))
                    return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[2]);
                if(idx <= 0)
                    return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[2]);

                sz = mtest.getArrListSize();
                if(sz == 0)
                    cout << "Size of array list is 0!!" << endl;
                else{
                    for(int i = 0; i < idx; i++)
                        mtest.deleteArr(rnGen(sz));
                } 
                
            }
            else if(options.size() < 3)
                return CmdExec::errorOption(CMD_OPT_MISSING, options[1]);
            else
                return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[3]);
        }

        else if(myStrNCmp("-Index",options[1],2) == 0){
            if(options.size() == 3){
                if(!myStr2Int(options[2],idx))
                    return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[2]);
                if(idx < 0)
                    return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[2]);

                sz = mtest.getArrListSize();
                if(idx >= sz){
                    cout << "Size of array list (" << sz << ") is <= " << idx << "!!" << endl;
                    return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[2]);
                }
                else 
                    mtest.deleteArr(idx);
            
            }
            else if(options.size() < 3)
                return CmdExec::errorOption(CMD_OPT_MISSING, options[1]);
            else
                return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[3]);
        }

        else
            return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[1]);
    }
    else 
        return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[0]);

   return CMD_EXEC_DONE;
}

void
MTDeleteCmd::usage(ostream& os) const
{  
   os << "Usage: MTDelete <-Index (size_t objId) | "
      << "-Random (size_t numRandId)> [-Array]" << endl;
}

void
MTDeleteCmd::help() const
{  
   cout << setw(15) << left << "MTDelete: " 
        << "(memory test) delete objects" << endl;
}


//----------------------------------------------------------------------
//    MTPrint
//----------------------------------------------------------------------
CmdExecStatus
MTPrintCmd::exec(const string& option)
{
   // check option
   if (option.size())
      return CmdExec::errorOption(CMD_OPT_EXTRA, option);
   mtest.print();

   return CMD_EXEC_DONE;
}

void
MTPrintCmd::usage(ostream& os) const
{  
   os << "Usage: MTPrint" << endl;
}

void
MTPrintCmd::help() const
{  
   cout << setw(15) << left << "MTPrint: " 
        << "(memory test) print memory manager info" << endl;
}


