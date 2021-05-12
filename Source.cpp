#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
#include <memory>
#include <fstream> 
#include "sqlite3.h"

// forward declaration
class Book; 
class Account; 
class DB; 

// constants
const char upfile[] = "up.txt"; 
const char abfile[] = "ab.txt"; 
const char database[] = "info3.db"; 
std::string TITLE = "title"; 
std::string AUTHOR = "author"; 
std::string PAGES = "pages"; 
std::string OWNER_USERNAME = "owner_username"; 
std::string ID = "id";

const int T_NOV = 3, T_TEX = 3, T_BIO = 3;
const int NOV_REM = 1, TEX_REM = 2, BIO_REM = 3; 
const int FIELD_MATCH = 1, ACCOUNT_E = 2, ACCOUNT_DNE = 3; 
const int USERNAME_LINE = 1, PASSWORD_LINE = 0; 
const int FIELD_LIMIT = 20; 
const int total = 9; 

// function prototypes
std::string get_data(const char file[]); 
void write_data(const char* data, char file[], int new_lines); 
void checkInBook(Book* book, Account* this_account); 
void editMyBooks(Account* this_account); 
int getPlace(Book* book, Account* this_account); 
int getPlace(const std::vector<std::string>& vec, const std::string& item); 
void mainMenu(); 
void bookName(char o); 
bool account_exists(std::string username, std::string password); 
bool check_combo(std::string username, std::string password); 
void signIn(Account* this_account); 
void signUp(Account* this_account); 
void add_books_from_database(); 
void accountPage(Account* this_account); 
void bookDetails(Book* book, Account* this_account); 
void update(DB* db, std::string column, std::string new_value, std::string condition); 
void retrieve_books(std::string titles[], std::string authors[], std::string pages[], std::string owner_username[], std::string ids[]); 
std::string get_title(Book* book); 

DB* currentDB = NULL; 

// class definitions
class Account {
private: 
	int id; 
	static int num_accounts; 
	static std::map<std::string, std::map<std::string, Account*>> combos;
	std::string username; 
	std::string password; 
	std::vector<Book*> books; 
	std::vector<std::string> bookIds; 
public: 
	static std::vector<Account*> accounts; 
	Account(std::string username, std::string password) {
		this->username = username; 
		this->password = password; 
		this->id = num_accounts; 
	}
	int get_books_out() {
		return books.size();
	}
	void add_book(Book* book, int id, Account* this_account) {
		const int total = 9; 
		books.push_back(book); 
		std::string title = "'" +  get_title(book) + "'"; 
		bookIds.push_back(std::to_string(id)); 
		update(currentDB, OWNER_USERNAME, std::to_string(this_account->id), " WHERE title = " + title); 
		std::string titles[total]; 
		std::string authors[total]; 
		std::string pages[total]; 
		std::string owner_username[total]; 
		std::string ids[total]; 
		retrieve_books(titles, authors, pages, owner_username, ids); 
		for (int i = 0; i < total; ++i) {
			std::cout << ids[i] << ", " << titles[i] << ", " << authors[i] << ", " << pages[i] << ", " << owner_username[i] << std::endl; 
		}
	}
	int get_books_size() {
		return books.size(); 
	}
	int get_id() {
		return id; 
	}
	std::string get_username() {
		return this->username; 
	}
	std::string get_password() {
		return this->password; 
	}
	auto erase_book(Book* item, int id) {
		books.erase(books.begin() + getPlace(item, this));
		bookIds.erase(bookIds.begin() + getPlace(bookIds, std::to_string(id)));
	}
	Book* get_book(int index) {
		return books[index]; 
	}
	auto books_start() {
		return books.begin(); 
	}
	auto books_end() {
		return books.end(); 
	}
	
	static void add_new_account(Account* account); 
	static void add_existing_account(Account* account); 
	friend void match_account(std::string username, std::string password, Account* this_account); 
	friend void get_books(std::vector<Book*> allBooks, Account* this_account); 
};

std::vector<Account*> Account::accounts; 
std::map<std::string, std::map<std::string, Account*>> Account::combos;
int Account::num_accounts = 0; 

void Account::add_new_account(Account* account) {
	Account::num_accounts++; 
	Account::accounts.emplace_back(account); 
	char field[FIELD_LIMIT]; 
	strcpy(field, upfile); 
	write_data(account->username.c_str(), field, 1); 
	write_data(account->password.c_str(), field, 1); 
}

void Account::add_existing_account(Account* account) {
	for (auto acc : Account::accounts) {
		if (acc->get_password() == account->get_password() || acc->get_username() == account->get_username()) {
			return; 
		}
	}
	Account::num_accounts++; 
	Account::accounts.emplace_back(account); 
}

void match_account(std::string username, std::string password, Account* this_account) {
	for (Account* account : Account::accounts) {
		if (account->get_username() == username) {
			if (account->get_password() == password) {
				this_account = account; 
			}
		}
	}
}	

class Book {
private: 
	int id; 
	int pages; 
	std::string author, title;
	std::string str_owner; 
	Account* owner;
	bool is_free; 
	bool is_mine; 
public:
	static std::vector<Book*> allBooks;
	static int total_books; 
	Book(std::string t, std::string a, int p) {
		pages = p;
		title = t;
		author = a;
		owner = NULL;
		is_free = true; 
		is_mine = true; 
		++Book::total_books; 
		id = Book::total_books; 
	}
	void set_owner(Account& account) {
		owner = &account; 
		str_owner = account.get_username(); 
	}
	int get_pages() {
		return pages; 
	}
	std::string get_author() {
		return author; 
	}
	std::string get_title() {
		return title; 
	}
	friend std::string get_title(Book* book); 
	Account* get_account() {
		return owner; 
	}
	bool get_status() {
		return is_free;
	}
	int get_id() {
		return id; 
	}
	void book() {
		is_free = !is_free;
		is_mine = !is_mine; 
	}
	bool is_owner(Account* account) {
		return account->get_id() == owner->get_id(); 
	}
	std::string full(Account* this_account) {
		std::string str = std::to_string(this_account->get_id()) + " : " + std::to_string(id); 
		return str; 
	}
	friend void return_book(Book*, Account*); 
	friend void get_books(std::vector<Book*> allBooks, Account* this_account); 
};	

void get_books(std::vector<Book*> allBooks, Account* this_account) {
	std::cout << "\n"; 
	std::cout << "Books size: "  <<  this_account->get_books_out() << std::endl; 
	std::cout << "\n"; 
	for (auto book : allBooks) {
		
	}
	std::cout << "\n"; 
}

std::string get_title(Book* book) {
	return book->title; 
}

std::vector<Book*> Book::allBooks; 
int Book::total_books = 0; 

void return_book(Book* book, Account* account) {
	book->owner = NULL; 
	std::string title = "'" + book->title + "'";  
	update(currentDB, OWNER_USERNAME, "'NONE'", "WHERE title = " + title); 
	std::string titles[total]; 
	std::string authors[total]; 
	std::string pages[total]; 
	std::string owner_username[total]; 
	std::string ids[total]; 
	retrieve_books(titles, authors, pages, owner_username, ids); 
	for (int i = 0; i < total; ++i) {
		std::cout << ids[i] << ", " << titles[i] << ", " 
		<< authors[i] << ", " << pages[i] << ", " << owner_username[i] << std::endl; 
	}
}

class Novels : public Book {
public: 
    Novels(std::string title, std::string author, int pages) : Book(title, author, pages) {}; 
};	

class Biographies : public Book {
public: 
    Biographies(std::string title, std::string author, int pages) : Book(title, author, pages) {};
};

class Textbooks : public Book {
public: 
    Textbooks(std::string title, std::string author, int pages) : Book(title, author, pages) {};
};


class DB {
public: 
	sqlite3* db; 
	char* messages; 
	std::vector<std::string> data; 
	std::string sql; 
	int result;
	void print_result(const char* data) {
	if(result != SQLITE_OK ){
		fprintf(stderr, "SQL error: %s\n", messages);
		sqlite3_free(messages);
		} else {
			fprintf(stdout, data);
		}
	} 
	void connect() {
		result = sqlite3_open(database, &db); 
		if (result == 0) {
			std::cout << "opened database" << std::endl;
		}
	}
	static int callback(void *NotUsed, int argc, char **argv, char **azColName){
		return 0;
	}
	static int print_database(void *NotUsed, int argc, char **argv, char **azColName) {
		for(int i = 0; i < argc; ++i) {
			currentDB->data.push_back(argv[i]);  
		}
		return 0; 
	}
	void init_table_books() {
		sql = 
		"CREATE TABLE BOOKS ("
		"id INTEGER PRIMARY KEY,"
		"title TEXT NOT NULL,"
		"author TEXT NOT NULL,"
		"pages INTEGER NOT NULL,"
		"owner_username TEXT NOT NULL"
		");"; 
		result = sqlite3_exec(db, sql.c_str(), callback, 0, &messages);
		print_result("Created table\n"); 
	}
	void insert(std::string data_str) {
		sql = 
		"INSERT INTO BOOKS (id, title, author, pages, owner_username)"  
		"VALUES " + data_str + ";"; 
		result = sqlite3_exec(db, sql.c_str(), callback, 0, &messages); 
		print_result("Records created successfully\n"); 
	}
	void delete_data(std::string condition) {
		sql = "DELETE FROM BOOKS WHERE " + condition; 
		result = sqlite3_exec(db, sql.c_str(), callback, 0, &messages); 
		print_result("Record deleted successfully\n"); 
	}
	void clear() {
		sql = "DELETE FROM BOOKS"; 
		result = sqlite3_exec(db, sql.c_str(), callback, 0, &messages); 
		print_result("Records cleared successfully\n"); 
	}
	void retrieve(std::string type) {
		sql = "SELECT " + type + " from BOOKS";
		result = sqlite3_exec(db, sql.c_str(), print_database, 0, &messages); 
		print_result("Data retrieved successfully\n"); 
	}
	friend void update(DB* db, std::string column, std::string new_value, std::string condition); 
	void close() {sqlite3_close(db);}
	std::string* get_data(std::string* arr) {
		for (int i = 0; i < currentDB->data.size(); ++i) {
			arr[i] = currentDB->data[i]; 
		}
		currentDB->data.clear(); 
		return arr;
	}
}; 

void update(DB* db, std::string column, std::string new_value, std::string condition) {
	db->sql = "UPDATE BOOKS SET " + column + " = " + new_value + condition; 
	std::cout << db->sql << std::endl; 
	db->result = sqlite3_exec(db->db, db->sql.c_str(), db->print_database, 0, &db->messages); 
	db->print_result("Data updated successfully\n"); 
}

DB* books_db = new DB(); 

// Function definitions

void invalidChoice(int time=3) {
	std::cout << "Invalid choice" << std::endl;
}

void clear_data(const char file[]) {
	std::ofstream f_obj; 
	f_obj.open(file); 
	if (f_obj.is_open()) {
		f_obj << "";
	}
	f_obj.close(); 
}

std::string get_data(const char file[], int new_lines=0) {
	std::ifstream f_obj; 
	f_obj.open(file); 
	if (f_obj.is_open()) {
		std::string line, _line;
		while (!f_obj.eof()) {
			getline(f_obj, line); 
			_line += line;
			while (new_lines > 0) {
				_line += "\n"; 
				--new_lines; 
			} 
		} 
		f_obj.close(); 
		return _line; 
	}
	return ""; 
}

void write_data(const char* data, char file[], int new_lines) {
	std::ofstream f_obj;
	char* buffer = NULL; 
	f_obj.open(file, std::ios::app);  
	if (f_obj.is_open()) {
		f_obj << data;
		while (new_lines > 0) {
			f_obj << "\n"; 
			--new_lines; 
		}
	}
	f_obj.close(); 
}

void checkInBook(Book* book, Account* this_account) {
	char choice; 
	int option;
	std::cout << "Are you sure you want to return " << get_title(book) << "(y/n)?";
	std::cin >> choice;
	if (choice == 'y') {
		return_book(book, this_account); 
		std::cout << "Press 1 to return back to account page" << std::endl;
		std::cin >> option;
		if (option == 1) {
			accountPage(this_account);
		}
	}
	else if (choice == 'n') {
		accountPage(this_account);
	}
	else {
		invalidChoice();
	}
}

void editMyBooks(Account* this_account) {
	for (auto i = Book::allBooks.begin(); i != Book::allBooks.end(); i++) {
		auto item = *i;
		if (find(this_account->books_start(), 
		this_account->books_end(), item) != this_account->books_end()) {
			if (this_account->get_books_size() > 0) {
				if (item->get_account() != this_account) {
					this_account->erase_book(item, item->get_id()); 
				}
			}
		}
		else if (item->get_account() == this_account) {
			this_account->add_book(item, item->get_id(), this_account); 
		}
	}
}

int getPlace(Book* book, Account* this_account) {
	int temp = 0;
	for (auto i = this_account->books_start(); i != this_account->books_end(); i++) {
		if (*i == book) {
			return temp;
		}
		temp++;
	}
	return -1; 
}

int getPlace(const std::vector<std::string>& vec, const std::string& item) {
	int temp = 0;
	for (auto i = vec.begin(); i != vec.end(); i++) {
		if (*i == item) {
			return temp;
		}
		temp++;
	}
	return -1; 
}

void mainMenu() {
	int option; 
	std::cout << "Welcome to the Library!" << "\n\n";
	std::cout << "Sign in[1]" << std::endl;
	std::cout << "Sign up[2]" << std::endl;
	std::cin >> option;
	Account* this_account = NULL; 
	option == 1 ? signIn(this_account) : option == 2 ? signUp(this_account) : invalidChoice();
}

void bookName(char o) {
	int start = 0;
	int end = T_NOV;
	switch (o) {
		case 'b': 
			start = T_NOV;
			end = start + T_BIO;
			break;
		case 'c':
			start = T_BIO + T_NOV;
			end = start + T_TEX;
			break;
		default: 
			invalidChoice();
	}
	for (int i = start; i < end; i++) {
		std::cout << get_title(Book::allBooks[i]) << "[" << i - start << "]" << std::endl;;
	}
}

void viewDetails(char choice, Account* this_account) {
	Book* book = NULL; 
	int num;
	bookName(choice);
	std::cout << "Press the number beside the book to view details: " << std::endl;
	std::cin >> num;
	switch(choice) {
		case 'b': 
			num += T_NOV; 
			break; 
		case 'c': 
			num += T_NOV + T_BIO; 
			break; 
	}
	book = Book::allBooks[num];
	bookDetails(book, this_account);
}


void fill(Account* this_account) {
	std::string data = get_data(abfile, 1);
	int inc = 1; 
	bool flag = false;
	std::string line = ""; 
	for (int i = 0; i < data.size(); i += inc) {
		std::string _line = ""; 
		_line.push_back(data[i]); 
		if (!flag && data[i] != '\n') {
			int _id; 
			inc = 2; 
			try { 
				_id = std::stoi(_line);
			} catch(std::exception) {}
			if (_id == this_account->get_id()) {
				flag = true; 
				continue; 
			}
		}
		if (flag) {
			inc = 1; 
			if (data[i] != '\n') {
				line.push_back(data[i]); 
				continue; 
			} 
			flag = false; 
			line = "";  
		}
	}
}

void accountPage(Account* this_account) { 
	int j = 0;
	int option; 
	// add books from database
	get_books(Book::allBooks, this_account); 
	std::cout << "\nWelcome to your account page" << std::endl;
	std::cout << "username: " << this_account->get_username() << std::endl; 
	std::cout << "password: " << this_account->get_password() << std::endl;
	std::cout << "id: " << this_account->get_id() << std::endl;  
	fill(this_account); 
	if (this_account->get_books_out() > 0) {
		if (this_account->get_books_size() > 0) {
			editMyBooks(this_account);
			if (this_account->get_books_size() > 0) {
				std::cout << "Here are your signed out books: " << std::endl;
				for (auto i = this_account->books_start(); i != this_account->books_end(); i++) {
					auto item = *i;
					std::cout << get_title(item) << "[" << j << "]" << std::endl;
					std::cout << std::endl;
					j++;
				}
			} else {
				std::cout << "You have no books signed out" << std::endl;
			}
		}
		else {
			std::cout << "You have no books signed out" << std::endl;
		}
	}
	else {
		std::cout << "You have no books signed out" << std::endl;
	}
	std::cout << std::endl;
	if (this_account->get_books_size() > 0) {
		std::cout << "Return books[1]" << std::endl;
	}
	std::cout << "View all books[2]" << std::endl;
	std::cout << "Back[3]" << std::endl;
	std::cin >> option;
	if (option == 1) {
		if (this_account->get_books_size() > 0) {
			std::cout << "To return your books, press the number beside it" << std::endl;
			std::cin >> option;
			if (option <= j) {
				checkInBook(this_account->get_book(option), this_account);
			} else {
				std::cout << "No such book. Press 1 to go back" << std::endl;
				std::cin >> option;
				if (option == 1) {
					accountPage(this_account);
				} else {
					invalidChoice();
				}
			}
		} else {
			std::cout << "You do not have any books signed out" << std::endl;
		}
	}
	else if (option == 2) {
		std::cout << "Press 'a' to view Novels, 'b' to view Biographies" 
		"'c' for Textbooks, or 'x' to go back: " << std::endl;
		char choice; 
		std::cin >> choice;
		switch(choice) {
			case 'a': 
			case 'b': 
			case 'c': 
				viewDetails(choice, this_account);
				break; 
			case 'x': 
				mainMenu(); 
			default: 
				invalidChoice();
		}
	} else {
		mainMenu(); 
	}
}

void checkOutBook(Book* book, Account* this_account) {
	int option; 
	std::cout << "To sign " << get_title(book) << " out, press 1. To go back, press 2" << std::endl;
	std::cin >> option;
	if (option == 1) {
		book->book(); 
		book->set_owner(*this_account); 
		this_account->add_book(book, book->get_id(), this_account); 
		write_data(book->full(this_account).c_str(), (char*)abfile, 1); 
		std::cout << "Book successfully signed out. To go back, press 2" << std::endl;
		std::cin >> option;
		if (option == 2) {
			accountPage(this_account);
		}
	}
	else if (option == 2) {
		accountPage(this_account);
	}
	else {
		invalidChoice();
	}
}

void bookDetails(Book* book, Account* this_account) {
	int option; 
	if (book->get_status()) {
		std::cout << "Book details : " << std::endl;
		std::cout << get_title(book) << std::endl;
		std::cout << book->get_author() << std::endl;
		std::cout << book->get_pages() << std::endl;
		checkOutBook(book, this_account);
	} else {
		std::cout << "This book is currently held by another person. Press 1 to go back. " << std::endl;
		std::cin >> option;
		if (option == 1) {
			accountPage(this_account);
		} else {
			invalidChoice();
		}
	}
}

bool account_exists(std::string username, std::string password, Account* this_account) {
	std::fstream file; 
	file.open(upfile, std::ios::in);
	if (file.is_open()) {
		std::string line;
		int count = 0; 
		while (!file.eof()) {
			std::cout << count; 
			++count; 
			getline(file, line); 
			if (count % 2 == USERNAME_LINE && username == line) {
				match_account(username, password, this_account); 
				return true; 
			} 
		}
		file.close(); 
	} 
	return false; 
}

bool check_combo(std::string username, std::string password) {
	std::fstream file; 
	file.open(upfile, std::ios::in);
	bool flag = false; 
	if (file.is_open()) {
		std::string line;
		int count = 0; 
		while (!file.eof()) {
			++count; 
			getline(file, line); 
			if (!flag) {
				if (username == line) {
					flag = true; 
				}
				continue; 
			} 
			if (password == line) {
				return true; 
			}
			flag = false;
		}	
		file.close(); 
	}
	return false; 
}

void book_file(Account* this_account) {
	int count = 0; 
	std::fstream fout; 
	fout.open(abfile); 
	std::string line = "";
	std::string id, title, author, pages;  
	while (!fout.eof()) {
		++count; 
		getline(fout, line); 
		switch (count % 4) {
			case 0: 
				pages = line; 
			case 1: 
				id = line;
			case 2:
				title = line; 
			case 3: 
				author = line;  
		}
	}
}	

void signIn(Account* this_account) {
	std::string u, p; 
	std::cout << "Enter username: " << std::endl;
	std::cin >> u;
	std::cout << "Enter password: " << std::endl;
	std::cin >> p;
	if (!check_combo(u, p)) {
		std::cout << "Error: Account does not exist / Incorrect username / Incorrect password!" << std::endl;
		mainMenu();
	}
	for (auto account : Account::accounts) {
		if (account->get_password() == p && account->get_username() == u) {
			this_account = account; 
			book_file(this_account); 
			accountPage(this_account); 
		}
	}
}

void signUp(Account* this_account) {
	int option; 
	int state; 
	std::string u, p, p2; 
	std::cout << "Enter a username: " << std::endl;
	std::cin >> u;
	std::cout << "Enter a password: " << std::endl;
	std::cin >> p;
	std::cout << "Repeat password: " << std::endl;
	std::cin >> p2;
	if (!account_exists(u, p, this_account)) {
		Account* account = new Account(u, p);
		Account::add_new_account(account);
		std::cout << "Account successfully created!" << std::endl;
	} 
	std::cout << "Press 3 to return to the home page" << std::endl;
	std::cin >> option;
	if (option == 3) {
		mainMenu();
	}
}

void acc_file() {
	std::fstream fout; 
	fout.open(upfile); 
	std::string username;
	std::string password;  
	int count = 0;
	if (fout.is_open()) {
		while (!fout.eof()) {
			++count; 
			if (count % 2 == 1) {
				getline(fout, username);
			} else {
				getline(fout, password);
				Account::add_existing_account(new Account(username, password)); 	
			}
		}
		fout.close(); 
	}
}

void insert_books() { 
	books_db->insert("(1, 'novelA', 'authorA', 100, 'NONE')");  
	books_db->insert("(2, 'novelB', 'authorB', 110, 'NONE')");  
	books_db->insert("(3, 'novelC', 'authorC', 120, 'NONE')");  
	books_db->insert("(4, 'bioA', 'authorA', 200, 'NONE')");  
	books_db->insert("(5, 'bioB', 'authorB', 210, 'NONE')");  
	books_db->insert("(6, 'bioC', 'authorC', 220, 'NONE')");  
	books_db->insert("(7, 'textA', 'authorA', 300, 'NONE')");  
	books_db->insert("(8, 'textB', 'authorB', 310, 'NONE')");  
	books_db->insert("(9, 'textC', 'authorC', 320, 'NONE')");  
}

void retrieve_books(std::string titles[], std::string authors[], 
std::string pages[], std::string owner_username[], std::string ids[]) {
	books_db->retrieve(TITLE); 
	books_db->get_data(titles);  	
	books_db->retrieve(AUTHOR);  
	books_db->get_data(authors);  
	books_db->retrieve(PAGES); 
	books_db->get_data(pages);
	books_db->retrieve(OWNER_USERNAME); 
	books_db->get_data(owner_username);
	books_db->retrieve(ID); 
	books_db->get_data(ids);
}

void add_books_from_database() {
	const size_t total = 9; 
	std::string titles[total]; 
	std::string authors[total]; 
	std::string pages[total]; 
	std::string owner_username[total]; 
	std::string ids[total]; 
	books_db->connect();
	retrieve_books(titles, authors, pages, owner_username, ids); 
	for (int i = 0; i < total; ++i) {
		std::cout << ids[i] << ", " << titles[i] << ", " << authors[i] 
		<< ", " << pages[i] << ", " << owner_username[i] << std::endl; 
	}
	for (int i = 0; i < total; ++i) {	
		Book* book = new Book(titles[i], authors[i], std::stoi(pages[i])); 
		Book::allBooks.push_back(book); 
	}  
	for (auto book : Book::allBooks) {
		std::cout << book->get_id() << ", " << get_title(book) << ", " 
		<< book->get_author() << ", " << book->get_pages() << std::endl; 
	} 
}


int main() {
	currentDB = books_db; 
	add_books_from_database(); 
	acc_file(); 
	mainMenu();
	sqlite3_close(books_db->db);
	return 0;
}

