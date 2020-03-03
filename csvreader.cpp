// csv_parser.cpp: ���������� ����� ����� ��� ����������� ����������.
//

#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>
#include <vector>
#include <map>

//#define _DEBUG_USE
#define _CONSOLE_USE

using namespace std;

vector<vector<string>> table;// ������ �������
// �����, ������������ ������� ��������� ��� ����, ����� �� ���������� ��� �������
// �.�. ������� ����� ���� �������, ���� ������ ������� �����
// ����� ������ -> �������
map<int, int> equals_indexes;
map<string, int> columns;// ����� �������� �������
// ��� -> ������ � �������
map<string, int> rows;// ����� �����

// ������ ����������
const char opers_array[] = { '+', '-', '*', '/' };

int operation(int arg1, int arg2, char oper){
	switch (oper){
		case '+':
			return arg1 + arg2;
		case '-':
			return arg1 - arg2;
		case '*':
			return arg1 * arg2;
		case '/':
			return arg1 / arg2;
	}
}

int parse_arg(string* arg){
	string column = "";
	string row = "";
	const char* arg_c = arg->c_str();
	for (char &i : *arg){
		if (!isdigit(i)){
			column += i;
		} else {
			row += i;
		}
	}
	int i_col = columns.find(column)->second;
	int i_row = rows.find(row)->second;

	return stoi(table[i_row][i_col]);
}

// ������ ���������
string parse_equal(string str, const unsigned int row, const unsigned int column){
	str = str.substr(1, str.length());// ������� "="
	int cell_1 = 0, cell_2 = 0;
	char op;
	for (char oper : opers_array){
		int oper_pos = 0;
		if ((oper_pos = str.find_first_of(oper, 0))){
			string arg1 = str.substr(0, oper_pos);// �������� ������ �������� ���������
			cell_1 = parse_arg(&arg1);// �������� �������� �� ������
			string arg2 = str.substr(oper_pos + 1, str.length());// �������� ������ �������� ���������
			cell_2 = parse_arg(&arg2);// �������� �������� �� ������
			op = oper;
		}
		break;
	}
	return to_string(operation(cell_1, cell_2, op));
}

// ������� �����
void watch_table() {
	for (map<int, int>::iterator it = equals_indexes.begin(); it != equals_indexes.end(); it++){
		unsigned int row = it->first;
		unsigned int column = it->second;
		table[row][column] = parse_equal(table[row][column], row, column);
	}
}

// ������ ������� � �������
void print_table() {

	for (map<string, int>::iterator it = columns.begin(); it != columns.end(); it++){
		cout << it->first;
		if (next(it) != columns.end())
			cout << ",";
		else
			cout << endl;
	}
		;
	for (vector<string> vec : table)
	for (int i = 0; i < vec.size(); i++){
			cout << vec[i];
			if (i != vec.size() - 1)
				cout << ",";
			else
				cout << endl;
	}

}

int main(int argc, char* argv[]){

	#ifdef _DEBUG_USE
	string filename = "E:/file.csv";
	#endif
	#ifdef _CONSOLE_USE
	string filename = "";
	#endif
	if (argc > 0 && argv[1]){
		filename = argv[1];
	}
	char input[1024] = "";
	string row = "", cell = "";
	fstream filehandle(filename.c_str(), ios::in);
	if (filehandle.is_open()){
		filehandle.seekg(0, ios::end);
		size_t f_length = filehandle.tellg();
		filehandle.seekg(0, ios::beg);
		filehandle.read(input, f_length);
		stringstream s(input);
		bool column_read = true;
		int row_i = 0;
		// ���������� ���������� ������� �� csv �����
		while (getline(s, row, '\n')){
			stringstream ss_row(row);
			// ��������� �������� �������, �.�. ������ ������
			// ����� ������� � ������������� ������� �������� ������� ����� ��������������� ����������� ������ � ������ ������� 
			if (column_read){
				int column_i = 0; // ��������� ����� ������� (�������) � ����� �������
				while (getline(ss_row, cell, ',')){
					columns.insert(pair<string, int>(cell, column_i++));
				}
				column_read = false;
			}
			// ��� ����������� ���� �������
			else{
				vector<string> row_vec;
				table.push_back(row_vec);
				int i = table.size() - 1;// ����� ������
				bool row_read = true;
				while (getline(ss_row, cell, ',')){
					// ��������� ����� ���� � ����� �����
					if (row_read){
						rows.insert(pair<string, int>(cell, row_i++));
						row_read = false;
					}

					table[i].push_back(cell);
					// ���� ��������� -> ���������� ������ � �������
					if (cell.at(0) == '=')
						equals_indexes.insert(pair<int, int>(i, table[i].size() - 1));
				}
			}
			
			//size_t comma_amount = count(row.begin(), row.end(), ',');
			//int comma_count = -1;
			//while (getline(ss_row, cell, ',')){
			//	comma_count++;
			//	if (comma_count != comma_amount)
			//		cout << cell << ",";
			//	else
			//		cout << cell << endl;
			//}
		}
		watch_table();
		print_table();
		system("pause");
	}
	
	return 0;
}

