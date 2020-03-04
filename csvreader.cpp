#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>
#include <vector>
#include <map>
#include <exception>

//#define _DEBUG_USE
#define _CONSOLE_USE

using namespace std;

class Parser {
public:
	Parser(){
		opers_array[0] = '+';
		opers_array[1] = '-';
		opers_array[2] = '*';
		opers_array[3] = '/';
		str_already_was = "";
	};
	~Parser(){};
	vector<vector<string>> table;// вектор таблицы
	// карта, запоминающая индексы выражений для того, чтобы не перебирать всю таблицу
	// т.к. таблица может быть большой, этот вектор ускорит поиск
	// номер строки -> столбец
	multimap<int, int> equals_indexes;
	map<string, int> columns;// карта названия колонок
	// ряд -> индекс в массиве
	map<string, int> rows;// карта рядов
	void watch_table();
	void print_table();
private:
	// массив операторов
	char opers_array[4];
	int operation(int arg1, int arg2, char oper);
	int parse_arg(string* arg, const string* str);
	string parse_equal(const string* str, const unsigned int row, const unsigned int column);
	string str_already_was;
};

int Parser::operation(int arg1, int arg2, char oper){
	switch (oper){
		case '+':
			return arg1 + arg2;
		case '-':
			return arg1 - arg2;
		case '*':
			return arg1 * arg2;
		case '/':
				if (arg2 == 0)
					throw "div by zero";
				return arg1 / arg2;		
	}
}

int Parser::parse_arg(string* arg, const string* str){
	string column = "";
	string row = "";
	if (arg->at(0) == '=')
		*arg = arg->substr(1, arg->length());// убираем "="
	for (char &i : *arg){
		if (!isdigit(i)){
			column += i;
		} else {
			row += i;
		}
	}
	if (column != ""){
		try {
			int i_col = columns.at(column);
			int i_row = rows.at(row);
			if (table[i_row][i_col].at(0) == '='){// если в нужной ячейке лежит выражение
				// на случай, если в таблице ячейки вызывают друг друга и, следовательно не могут посчитаться, что приводит к stack overflow
				if (table[i_row][i_col] == str_already_was){
					cerr << "Looped cells: " << *str << " and " << table[i_row][i_col] << endl;
					#ifdef _DEBUG_USE
					system("pause");
					#endif
					exit(1);
				}
				str_already_was = *str;
				table[i_row][i_col] = parse_equal(&table[i_row][i_col], i_row, i_col);
			}

			return stoi(table[i_row][i_col]);
		}
		catch (exception &e) {// если не существует такой ячейки или ряда
			cerr << "Can't find cell: " << *arg << endl;
			#ifdef _DEBUG_USE
			system("pause");
			#endif
			exit(1);
		}
	} else {// если нет букв в column, значит, это просто число
		return stoi(row);
	}
	
}

// парсер выражений
string Parser::parse_equal(const string* str, const unsigned int row, const unsigned int column){
	int cell_1 = 0, cell_2 = 0;
	char op;
	for (char oper : opers_array){
		int oper_pos = 0;
		if ((oper_pos = str->find_first_of(oper, 0)) >= 0){
			string arg1 = str->substr(0, oper_pos);// получаем первый аргумент выражения
			cell_1 = parse_arg(&arg1, str);// получаем значение из ячейки
			string arg2 = str->substr(oper_pos + 1, str->length());// получаем второй аргумент выражения
			cell_2 = parse_arg(&arg2, str);// получаем значение из ячейки
			op = oper;
			break;
		}
	}
	try{
		return to_string(operation(cell_1, cell_2, op));
	} catch (const char* e){
			cerr << "Division by zero: " << *str << endl;
			#ifdef _DEBUG_USE
			system("pause");
			#endif
			exit(1);
		}

}

// перебор ячеек
void Parser::watch_table() {
	for (map<int, int>::iterator it = equals_indexes.begin(); it != equals_indexes.end(); it++){
		unsigned int row = it->first;
		unsigned int column = it->second;
		if (table[row][column].at(0) == '=')// т.к. может быть уже посчитано
			table[row][column] = parse_equal(&table[row][column], row, column);
	}
}

// печать таблицы в консоль
void Parser::print_table() {

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
	Parser p;
	char input[1024] = "";
	string row = "", cell = "";
	fstream filehandle(filename.c_str(), ios::in);
	if (filehandle.is_open()){
		setlocale(LC_ALL, "Russian");
		filehandle.seekg(0, ios::end);
		size_t f_length = filehandle.tellg();
		filehandle.seekg(0, ios::beg);
		filehandle.read(input, f_length);
		stringstream s(input);
		bool column_read = true;
		int row_i = 0;
		// заполнение внутренней таблицы из csv файла
		while (getline(s, row, '\n')){
			stringstream ss_row(row);
			// считываем названия колонок, т.е. первую строку
			// таким образом в ассоциативном массиве название колонки будет соответствовать порядковому номеру в ячейке таблицы 
			if (column_read){
				int column_i = 0; // считываем номер колонки (столбца) в карту колонок
				while (getline(ss_row, cell, ',')){
					if (column_i == 0 && cell != ""){
						cerr << "First column can't be a string" << endl;
						#ifdef _DEBUG_USE
						system("pause");
						#endif
						return 1;
					}
					for (char& c : cell){// проверка на содержание цифр в ряде столбцов
						if (isdigit(c)){
							cerr << "Column can't contain a number" << endl;
							#ifdef _DEBUG_USE
							system("pause");
							#endif
							return 1;
						}
					}

					
					p.columns.insert(pair<string, int>(cell, column_i++));
				}
				column_read = false;
			}
			// тут заполняется сама таблица
			else{
				vector<string> row_vec;
				p.table.push_back(row_vec);
				int i = p.table.size() - 1;// номер строки
				bool row_read = true;
				while (getline(ss_row, cell, ',')){
					// считываем номер ряда в карту рядов
					if (row_read){
						p.rows.insert(pair<string, int>(cell, row_i++));
						row_read = false;
					}

					p.table[i].push_back(cell);
					// если выражение -> запоминаем строку и столбец
					if (cell.at(0) == '=')
						p.equals_indexes.insert(pair<int, int>(i, p.table[i].size() - 1));
				}
			}
		}
		p.watch_table();
		p.print_table();
		#ifdef _DEBUG_USE
		system("pause");
		#endif
	}
	
	return 0;
}

