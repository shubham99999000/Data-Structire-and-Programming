#include "p2Table.h"
#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>
#include <iomanip>
#include <cstring>

using namespace std;

// Implement member functions of class Row and Table here

void Row::init(int column){
	_data = new int [column];
	for (int i = 0; i < column; i++)
		_data[i] = INT_MAX;
}

void Row::delete_memory(){
	delete [] _data;
}

bool Table::read(const string& csvFile)
{
	int row = 0, comma = 1;
	saved_col = 0;
	saved_row = 0;
	char ch;
	bool end_row = 0;

	ifstream in(csvFile);
	if(!in)
		return false;

	else{
		while(in.get(ch)){
			if(ch == ','){
				comma++;
				end_row = 0;
			}
			else if(ch == '\r'){						// end of row
				end_row = 1;
				row++;
			}
			else if(ch == '\n'){						// getline end
				if(end_row == 1){
					saved_row = row-1;
					saved_col = ceil(comma/saved_row)+1;
					break;
				}
			}		
		}
		in.close();
        in.clear();
		return true;
	}
     // TODO
}

void Table::init(int row, int column){
	//_rows = new Row [row]; // check assign vector class
	//_rows = vector<Row> init_rows(row);
	_rows.resize(row);
	for(int i = 0; i < row; i++){
		_rows[i].init(column);
	}
}

void Table::delete_memory(){
	for(int i = 0; i < saved_row; i++){
		_rows[i].delete_memory();		
	}

}

bool Table::valueAssign(const string& csvFile){
	//cout << _rows[1][1] << endl;
	ifstream in(csvFile);
	char ch;
	string accumNumber = "";
	int i = 0,j = 0;
	bool end_row = 0;
	if(!in)
		return false;

	else{
		//cout << " Row : " << saved_row << " " << " Column : " << saved_col << endl;
		while(in.get(ch)){
			if(ch == ','){
				if(!accumNumber.empty()){
					_rows[i][j] = stoi(accumNumber);
					//cout << "current char : " << ch << " row : " << i << " column : " << j << endl;
					//cout << _rows[i][j] << " is saved!" << endl;
					accumNumber.clear();

				}
				else{					
				}
				j++;
			}
			else if(ch == '\r'){						// end of row
				if(!accumNumber.empty()){
					_rows[i][j] = stoi(accumNumber);
					//cout << "current char : " << ch << " row : " << i << " column : " << j << endl;
					//cout << _rows[i][j] << " is saved!" << endl;
					accumNumber.clear();				

				}
				else{
				}

				end_row = 1;
				i++;
				j = 0;
			}
			else if(ch == '\n'){						// getline end
				if(end_row == 1){
					break;
				}
				else{
					if(!accumNumber.empty()){
						_rows[i][j] = stoi(accumNumber);
						//cout << "current char : " << ch << " row : " << i << " column : " << j << endl;
						//cout << _rows[i][j] << " is saved!" << endl;
						accumNumber.clear();						
					}
				}

			}
			else{
				accumNumber.push_back(ch);
				end_row = 0;

			}
		}

		in.close();
        in.clear();
        accumNumber.clear();
		return true;
	}

}

void Table::addRow(string cmd, string parameter){
    int columnPush = 1;
    cmd.push_back(' ');
    //cout << "Length : " << cmd.length() << " Original number string : " << cmd << endl;
    Row addRow;
    _rows.push_back(addRow);
    _rows[saved_row].init(saved_col);

    while(cmd.find(" ") != -1 && columnPush <= saved_col){
    	while(cmd.at(0) == ' ')        // to erase redundant ' '
            cmd.erase(0,1);

        parameter = cmd.substr(0, cmd.find(" "));
        cmd.erase(0, cmd.find(" ")+1);

        if(parameter != "-")
       		_rows[saved_row][columnPush-1] = stoi(parameter);
       	else 
       		_rows[saved_row][columnPush-1] = INT_MAX;
        columnPush ++;
    }
    saved_row ++;  // # of row changed


	cmd.clear();
	parameter.clear();
            
}

void Table::print(){
	for(int i = 0; i < saved_row; i++){
		for(int j = 0; j < saved_col; j++){
			if(_rows[i][j] == INT_MAX)
				cout << "    ";
			else
				cout << setw(4) << right << _rows[i][j];
		}
		cout << endl;	
	}
}

void Table::sum(int paramInt){
	int temp = 0;
	if(paramInt > saved_col-1)
		cout << "Exceed # of column!" << endl;
	else{
		for(int i = 0; i < saved_row; i++){
			if(_rows[i][paramInt] != INT_MAX)
				temp += _rows[i][paramInt];
		}
		cout << "The summation of data in column #" << paramInt << " is " << temp << "." << endl;
	}

}

void Table::ave(int paramInt){
	float temp = 0, devider = 0;
	if(paramInt > saved_col-1)
		cout << "Exceed # of column!" << endl;
	else{
		for(int i = 0; i < saved_row; i++){
			if(_rows[i][paramInt] != INT_MAX){
				temp += _rows[i][paramInt];
				devider++;
			}
		}

		if(temp/devider == floor(temp/devider))
			cout << "The average of data in column #" << paramInt << " is " << floor(temp/devider * 10) / 10 << ".0." << endl;

		else{
			if(temp/devider * 100 - (floor(temp/devider * 10) * 10) >= 5)
				cout << "The average of data in column #" << paramInt << " is " << ceil(temp/devider * 10) / 10 << "." << endl;
		
			else 
		 		cout << "The average of data in column #" << paramInt << " is " << floor(temp/devider * 10) / 10 << "." << endl;
		}			 
	}
}

void Table::max(int paramInt){
	int temp = 0;
	if(paramInt > saved_col-1)
		cout << "Exceed # of column!" << endl;
	else{
		for(int i = 0; i < saved_row; i++){
			if(_rows[i][paramInt] > temp && _rows[i][paramInt] != INT_MAX)
				temp = _rows[i][paramInt];
		}
	cout << "The maximum of data in column #" << paramInt << " is " << temp << "." << endl;
	}	
}

void Table::min(int paramInt){
	int temp = INT_MAX;
	if(paramInt > saved_col-1)
		cout << "Exceed # of column!" << endl;
	else{
		for(int i = 0; i < saved_row; i++){
			if(_rows[i][paramInt] < temp && _rows[i][paramInt] != INT_MAX)
				temp = _rows[i][paramInt];
		}
	cout << "The minimum of data in column #" << paramInt << " is " << temp << "." << endl;
	}
}

void Table::count(int paramInt){
	int distCount = 1;
	bool sameNum = false;
	if(paramInt > saved_col-1)
		cout << "Exceed # of column!" << endl;
	else{
		if(_rows[0][paramInt] == INT_MAX)
			distCount = 0;				// don't count first one

		for(int i = 0; i < saved_row; i++){
			for(int j = 0; j < i; j++){
				if((_rows[j][paramInt] == _rows[i][paramInt]) && (_rows[i][paramInt] != INT_MAX) && (_rows[j][paramInt] != INT_MAX))
					sameNum = true;
				if(j == (i-1) && sameNum == false && (_rows[i][paramInt] != INT_MAX)) 
					distCount++;
			}
			sameNum = false;
		}
		cout << "The distinct count of data in column #" << paramInt << " is " << distCount << "." << endl;
			
	}	

}


