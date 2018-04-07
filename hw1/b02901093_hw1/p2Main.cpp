#include <iostream>
#include <string>
#include "p2Table.h"
#include <cstring>

using namespace std;

int main(int argc, char *argv[ ])
{
   Table table;
   // TODO: read in the csv file
   string csvFile;
   cout << "Please enter the file name: ";
   cin >> csvFile;
   if (table.read(csvFile)){
      //cout << table.saved_col;
      table.init(table.saved_row, table.saved_col);         // initiate int_max
      table.valueAssign(csvFile);

      cout << "File \"" << csvFile << "\" was read in successfully." << endl;

   }
   else {
      cout << "csvFile does not exist." << endl;
      exit(-1); // csvFile does not exist.

   }
   // TODO read and execute commands

   string cmd="", segment="", parameter="";

   cout << "Please type command: PRINT, ADD, MAX, MIN, SUM, COUNT, AVE" << endl;  

   while(getline(cin, cmd)){
      if(cmd.find(" ") != -1){     // deal with 2 more string situation
         //cout << cmd << endl;
         segment = cmd.substr(0, cmd.find(" "));
         cmd.erase(0, cmd.find(" ")+1);

         if(segment != "ADD"){

            while(cmd.find(' ') != -1){         // to erase redundant ' '
               cmd.erase(0,cmd.find(' ')+1);
            }

            parameter = cmd; 
            int paramInt = stoi(parameter);

            if(segment == "MAX")
               table.max(paramInt);

            else if(segment == "MIN")
               table.min(paramInt);

            else if(segment == "COUNT")
               table.count(paramInt);
            
            else if(segment == "AVE")
               table.ave(paramInt);              

            else if(segment == "SUM")
               table.sum(paramInt);                             

            else 
               cout << "Wrong Command! " << endl;
                        
         }

         else{
            //ADD :: special deal
            table.addRow(cmd, parameter);               
         }

      }

      else if(cmd == ""){
      }

      else{      
         if(cmd == "PRINT"){
            //PRINT :: special deal
            table.print(); 
         }

         else if (cmd == "EXIT"){
            break;
         }

         else{
            cout << "Wrong Command! " << endl;
         }         
      }

      cmd.clear();
      segment.clear();
      parameter.clear();
   }     
   table.delete_memory();      
   return 0;
}
