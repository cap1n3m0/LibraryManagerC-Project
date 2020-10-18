#include <iostream>
#include <cwchar>
#include <windows.h>
#include <Wincon.h>

#define CLRSCR system("cls");  // Definitions
#define CONSOLE_HANDLE GetStdHandle(STD_OUTPUT_HANDLE);

using namespace std;

class User { // Creating class "User"
public:
	string user, pass;
	int books_checked_out; 
	User(string u, string p, int b, int i) {
		user = u;
		pass = p;
		books_checked_out = b; 
	}
	User() {}
};

User user_list[1000]; // List that holds objects of type User
																												
class Book {  // Creating class "Book"
public:
	string title;
	string author;
	int pages;
	int number;
	int holder; 
	bool isCheckedOut;

	Book(string t, string a, int p, int n, int h, bool i) {
		title = t;
		author = a;
		pages = p;
		number = n;
		holder = h; 
		isCheckedOut = i;
	}
};

Book book1("A title", "A Author", 110, 1, 0, false);  // Instantiation of Objects of type Book
Book book2("B title", "B Author", 120, 2, 0, false);
Book book3("C title", "C Author", 130, 3, 0, false);
Book book4("D title", "D Author", 140, 4, 0, false);
Book book5("E title", "E Author", 150, 5, 0, false);
Book book6("F title", "F Author", 160, 6, 0, false);
Book book7("G title", "G Author", 170, 7, 0, false);

Book book[7] = { book1, book2, book3, book4, book5, book6, book7 };  // List that holds objects of type Book

// variable definitions

string password[10] = {}; 
string username[10] = {};

bool entered_account = false;

int selected_choice, console_choice, ID_number = 1, books_checked_out = 0;

const int book_size = sizeof(book) / sizeof(Book);
int password_size = sizeof(password)/sizeof(string);
int username_size = sizeof(username) / sizeof(string); 

string _title[book_size] = {};
string _pass1, _pass2, _user;
string _username, _password;

char check_out_choice;

HANDLE console = CONSOLE_HANDLE; 

CONSOLE_FONT_INFOEX font;

User USER = user_list[ID_number - 1];

// Method definitions

void main_page();
void account_screen(int i);
void sign_in();
void sign_up();
void create_account(); 
void check_out_page();
void show_book_details(int num);
void check_out(int num);
void check_in_page();
void return_book(); 
void console_options();
void set_font_color(int c);
void set_font_size(int f);

// Main method

int main() { 
	main_page();
	return 0;
}

void main_page() { // Main Menu Screen 
	CLRSCR;
	cout << "Welcome to the library" << endl;
	cout << "" << endl;
	cout << "" << endl;
	cout << "[1]Sign in" << endl;
	cout << "" << endl;
	cout << "[2]Sign up" << endl;
	cout << "" << endl;
	cout << "[3]Console options" << endl;
	cout << "" << endl;
	cout << "" << endl;
	cin >> selected_choice;
	switch (selected_choice) {
	case 1:
		sign_in();
		break;
	case 2:
		sign_up();
		break;
	case 3: 
		console_options(); 
		break; 
	default:
		cout << "Invalid choice, you will be forwarded back to the main page" << endl;
	}
}

void sign_up() { // Sign up page
	CLRSCR;
	bool _clear = true;
	int i; 
	cout << "Enter username" << endl;
	cin >> _user;
	cout << "Enter password" << endl;
	cin >> _pass2;
	cout << "Enter password again" << endl;
	cin >> _pass1;
	for (i = 0; i <= password_size; i++) {
		if (_user == username[i]) {
			if (_pass1 == password[i]) {
				_clear = false;
				break;
			}
		}
	}
	if (_clear) {
		if (_pass1 == _pass2) {
			create_account(); 
		}
		else {
			cout << "Passwords do not match" << endl;
		}
	}
}

void create_account() { // Create new account method
	username[ID_number - 1] = _user;
	password[ID_number - 1] = _pass1;
	User new_user(_user, _pass1, 0, 1);
	USER = new_user;
	cout << "Account successfully created!" << endl;
	cout << USER.pass << "" << USER.user << endl;
	ID_number++;
	CLRSCR;
	main_page();
}

void sign_in() { // Sign in page
	CLRSCR; 
	cout << "Enter username" << endl;
	cin >> _username;
	cout << "Enter password" << endl;
	cin >> _password;
	for (int i = 0; i <= password_size - 1; i++) {
		if (username[i] == _username) {
			if (password[i] == _password) {
				entered_account = true;
				ID_number = i + 2; 
				CLRSCR;
				account_screen(i);
				break;
			}
		}
	}
	if (!entered_account) {
		cout << "Invalid Username/Password" << endl;
		cout << "Press 3 to be forwarded back to the main screen" << endl;
		cin >> selected_choice;
		if (selected_choice == 3) {
			CLRSCR;
			main_page();
		}
	}
}

void account_screen(int i) { // My Account screen
	CLRSCR;
	string temp_title = "";
	cout << "My Account" << endl;
	cout << "" << endl;
	cout << "ID Number: " << ID_number - 1 << endl;
	cout << "" << endl;	
	cout << "" << endl;
	if (USER.books_checked_out > 0) {
		for (int i = 0; i < book_size; i++) {
				if (book[i].isCheckedOut) {
					if (book[i].holder == ID_number - 1) {
						_title[i] = book[i].title;
						temp_title = temp_title + _title[i] + ", ";
					}
				}
		}
		USER.books_checked_out > 0 ? cout << "Books checked out : " << temp_title << endl : cout << "No books checked out: " << endl;
	} else {
		cout << "No books checked out" << endl;
	}
	cout << "" << endl;
	cout << "" << endl;
	cout << "[1]Check out book" << endl;
	cout << "[2]Check in book" << endl;
	cout << "Press 3 to go back to the main screen" << endl;
	cin >> selected_choice;
	switch (selected_choice) {
	case 1:
		check_out_page();
		break;
	case 2:
		check_in_page();
		break;
	case 3:
		CLRSCR;
		main_page();
		break;
	}
}

void check_out_page() { // Check out books page
	CLRSCR;
	cout << "Check out books" << endl;
	for (int i = 0; i < 7; i++) {
		cout << "["<< i + 1 << "] " << book[i].title << endl;
	}
	cout << "" << endl;
	cout << "" << endl;
	cout << "To select a book, press the number next to it" << endl;
	cout << "To go back, press 8" << endl;
	cin >> selected_choice;
	selected_choice < book_size + 1 ? show_book_details(selected_choice) : account_screen(ID_number - 1);
}


void check_in_page() { // Return books page; 
	CLRSCR; 
	cout << "Return library books" << endl; 
	cout << "" << endl; 
	cout << "" << endl;
	if (USER.books_checked_out > 0) {
		for (int i = 0; i < book_size; i++) {
			if (book[i].isCheckedOut) {
				if (book[i].holder == ID_number - 1) {
					cout << "[" << book[i].number << "] " << book[i].title << endl;
				}
			}
		}
		cout << "To return a book, press the number beside. To go back, press 0" << endl;
		cin >> selected_choice;
		if (selected_choice <= book_size) {
			if (selected_choice > 0) {
				return_book();
			} else {
				account_screen(ID_number - 1); 
			}
		}
	} else {
		cout << "No books checked out" << endl; 
		cout << "Press 3 to return back" << endl;
		cin >> selected_choice; 
		if (selected_choice == 3) {
			account_screen(ID_number - 1); 
		}
	}
}

void return_book() {
	cout << "Return " << book[selected_choice - 1].title << "? (y/n)" << endl;
	cin >> check_out_choice;
	if (check_out_choice == 'y') {
		book[selected_choice - 1].isCheckedOut = false;
		book[selected_choice - 1].holder = 0; 
		cout << book[selected_choice - 1].title << " has been checked out" << endl;
		USER.books_checked_out--; 
	} else {
		check_in_page(); 
	}
	cout << "" << endl;
	cout << "Press b to return back" << endl;
	cin >> check_out_choice;
	if (check_out_choice == 'b') {
		account_screen(ID_number - 1);
	}
}

void show_book_details(int num) { // display information of selected book
	CLRSCR;
	cout << "Book details" << endl;
	cout << "title: " << book[num-1].title << endl;
	cout << "" << endl;
	cout << "author: " << book[num-1].author << endl;
	cout << "" << endl;
	cout << "pages: " << book[num-1].pages << endl;
	cout << "" << endl;
	cout << "currently held by:  " << book[num - 1].holder << endl;
	cout << "" << endl;
	if (book[num - 1].holder == ID_number - 1) {
		cout << "You have this book" << endl; 
	} else {
		cout << "You do not have this book" << endl;
	}
	if (book[num - 1].holder == 0) {
		cout << "check out book? (y/n)" << endl;
		cin >> check_out_choice;
		check_out_choice == 'y' ? check_out(num - 1) : check_out_page();
	} else {
		cout << "Please choose another book. Press 1 to go back" << endl;
		cin >> selected_choice;
		if (selected_choice == 1) {
			check_out_page(); 
		} 
	}

}

void check_out(int n) {
	CLRSCR;
	book[n].isCheckedOut = true;
	USER.books_checked_out++;
	book[n].holder = ID_number - 1;
	cout << "checked out book" << endl;
	account_screen(n);
}


void console_options() { // Console Options Page
	int num; 
	CLRSCR;  
	cout << "[1]Font Color" << endl; 
	cout << "[2]Font Size" << endl;
	cin >> console_choice; 
	CLRSCR;
	if (console_choice == 1) {
		cout << "Choose color" << endl;
		cout << "" << endl;
		cout << "[1]" << "Blue" << endl;
		cout << "[2]" << "Green" << endl;
		cout << "[3]" << "Light Blue" << endl;
		cout << "[4]" << "Red" << endl;
		cout << "[5]" << "Purple" << endl;
		cout << "[6]" << "Yellow" << endl;
		cout << "[7]" << "White (default)" << endl;
		cout << "" << endl;
		cin >> console_choice;
		set_font_color(console_choice);
	} else {
		cout << "Choose size" << endl;
		cout << "" << endl;
		cout << "[0]" << "10" << endl;
		cout << "[1]" << "12" << endl;
		cout << "[2]" << "14" << endl;
		cout << "[3]" << "18" << endl;
		cout << "[4]" << "22" << endl;
		cout << "[5]" << "24" << endl;
		cout << "[6]" << "26" << endl;
		cout << "[7]" << "30" << endl;
		cout << "[8]" << "32" << endl;
		cout << "[9]" << "34" << endl;
		cout << "" << endl;
		cin >> console_choice;
		switch (console_choice) {
		case 0: 
			num = 10; 
			break; 
		case 1:
			num = 12;
			break;
		case 2:
			num = 14;
			break;
		case 3:
			num = 18;
			break;
		case 4:
			num = 22;
			break;
		case 5:
			num = 24;
			break;
		case 6:
			num = 26;
			break;
		case 7:
			num = 30;
			break;
		case 8:
			num = 32;
			break;
		case 9:
			num = 34;
			break;
		}
		set_font_size(num);
	}
	
}

void set_font_color(int c) { // Change font color
	SetConsoleTextAttribute(console, c);
	cout << "Color successfully set" << endl; 
	main_page(); 
}

void set_font_size(int f) { // Change font size
	CONSOLE_FONT_INFOEX console_info = {0};
	console_info.cbSize = sizeof(console_info);
	console_info.dwFontSize.Y = f; 
	wcscpy_s(console_info.FaceName, L"Lucida Console");
	SetCurrentConsoleFontEx(GetStdHandle(STD_OUTPUT_HANDLE), NULL, &console_info);
	main_page(); 
}
