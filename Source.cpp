#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
#include <memory>

using namespace std;

class Book {
public:
	int pages, owner;
	string author, title;
	~Book() {}
};

class Novels : public Book {
public:
	Novels(string t, string a, int p) {
		pages = p;
		title = t;
		author = a;
		owner = 0;
	}
};

class Biographies : public Book {
public:
	Biographies(string t, string a, int p) {
		pages = p;
		title = t;
		author = a;
		owner = 0;
	}
};


class Textbooks : public Book {
public:
	Textbooks(string t, string a, int p) {
		pages = p;
		title = t;
		author = a;
		owner = 0;
	}
};

void mainMenu();
void signIn();
void signUp();
void accountPage();
void bookName(int o);
void invalidChoice();
void checkOutBook(Book* book);
void checkInBook(Book* book);
void viewDetails(int num);
void bookDetails(Book* book);
void editMyBooks();
int getPlace(Book* book);

Book* book;

typedef pair<string, string> var;
multimap<int, pair<string, string>> login;

char choice;

vector<Novels*> novels;
vector<Biographies*> biographies;
vector<Textbooks*> textbooks;
vector<Book*> myBooks;
vector<Book*> allBooks;

Novels* n1 = new Novels("novelA", "AuthorA", 100);
Novels* n2 = new Novels("novelB", "AuthorB", 110);
Novels* n3 = new Novels("novelC", "AuthorC", 120);

Biographies* b1 = new Biographies("BioA", "AuthorA", 200);
Biographies* b2 = new Biographies("BioB", "AuthorB", 210);
Biographies* b3 = new Biographies("BioC", "AuthorC", 220);

Textbooks* t1 = new Textbooks("textA", "AuthorA", 300);
Textbooks* t2 = new Textbooks("textB", "AuthorB", 310);
Textbooks* t3 = new Textbooks("textC", "AuthorC", 320);

int option;
int num_accounts = 0;
int ID;
int books_signed_out = 0;

const int NOVELS = 3, TEXTBOOKS = 3, BIOGRAPHIES = 3;

string u, p2, p;

int main() {
	myBooks.clear();
	novels.push_back(n1);
	novels.push_back(n2);
	novels.push_back(n3);

	biographies.push_back(b1);
	biographies.push_back(b2);
	biographies.push_back(b3);

	textbooks.push_back(t1);
	textbooks.push_back(t2);
	textbooks.push_back(t3);

	allBooks.push_back(n1);
	allBooks.push_back(n2);
	allBooks.push_back(n3);

	allBooks.push_back(b1);
	allBooks.push_back(b2);
	allBooks.push_back(b3);

	allBooks.push_back(t1);
	allBooks.push_back(t2);
	allBooks.push_back(t3);

	mainMenu();
	return 0;
}

void mainMenu() {
	cout << "Welcome to the Library!" << endl;
	cout << "" << endl;
	cout << "Sign in[1]" << endl;
	cout << "Sign up[2]" << endl;
	cin >> option;
	option == 1 ? signIn() : option == 2 ? signUp() : invalidChoice();
}

void signIn() {
	bool found = false;
	cout << "Enter username: " << endl;
	cin >> u;
	cout << "Enter password: " << endl;
	cin >> p;
	for (auto i = login.begin(); i != login.end(); i++) {
		if (u == i->second.first) {
			if (p == i->second.second) {
				found = true;
				ID = i->first;
				ID++;
				accountPage();
			}
		}
	}
	if (!found) {
		cout << "Error: Account does not exist / Incorrect username / Incorrect 'password!" << endl;
		mainMenu();
	}
}

void signUp() {
	bool exists = false;
	bool valid = false;
	bool diff_pass = false;
	cout << "Enter a username: " << endl;
	cin >> u;
	cout << "Enter a password: " << endl;
	cin >> p;
	cout << "Repeat password: " << endl;
	cin >> p2;
	for (auto i = login.begin(); i != login.end(); i++) {
		exists = u == i->second.first;
	}
	valid = u != p && u != p2;
	if (p == p2) {
		if (!exists && valid && !diff_pass) {
			login.insert(make_pair(num_accounts, make_pair(u, p)));
			num_accounts++;
		}
	}
	else {
		diff_pass = true;
	}
	if (!valid) {
		cout << "Error: Username and password cannot match!" << endl;
	} if (exists) {
		cout << "Error: Account already exists!" << endl;
	} if (diff_pass) {
		cout << "Error: passwords do not match" << endl;
	} if (valid && !exists && !diff_pass) {
		cout << "Account successfully created!" << endl;
	}
	cout << "Press 3 to return to the home page" << endl;
	cin >> option;
	if (option == 3) {
		mainMenu();
	}
}

void accountPage() {
	int num;
	int j = 0;
	cout << "Welcome to your account page" << endl;
	if (books_signed_out > 0) {
		if (myBooks.size() > 0) {
			editMyBooks();
			if (myBooks.size() > 0) {
				cout << "Here are your signed out books: " << endl;
				for (auto i = myBooks.begin(); i != myBooks.end(); i++) {
					auto item = *i;
					cout << item->title << "[" << j << "]" << endl;
					cout << endl;
					j++;
				}
			} else {
				cout << "You have no books signed out" << endl;
			}

		}
		else {
			cout << "You have no books signed out" << endl;
		}
	}
	else {
		cout << "You have no books signed out" << endl;
	}
	cout << endl;
	if (myBooks.size() > 0) {
		cout << "Return books[1]" << endl;
	}
	cout << "View all books[2]" << endl;
	cout << "Back[3]" << endl;
	cin >> option;
	if (option == 1) {
		if (myBooks.size() > 0) {
			cout << "To return your books, press the number beside it" << endl;
			cin >> option;
			if (option <= j) {
				checkInBook(myBooks[option]);
			} else {
				cout << "No such book. Press 1 to go back" << endl;
				cin >> option;
				if (option == 1) {
					accountPage();
				} else {
					invalidChoice();
				}
			}
		} else {
			cout << "You do not have any books signed out" << endl;
		}
	}
	else if (option == 2) {
		cout << "Press 'a' to view Novels, 'b' to view Biographies, 'c' for Textbooks" << endl;
		cin >> choice;

		if (choice == 'a') {
			option = 1;
			viewDetails(num);
		}
		else if (choice == 'b') {
			option = 2;
			viewDetails(num);
		}
		else if (choice == 'c') {
			option = 3;
			viewDetails(num);
		}
		else {
			invalidChoice();
		}

	}
	else {
		mainMenu();
	}
}

void viewDetails(int num) {
	bookName(option);
	cout << "Press the number beside the book to view details" << endl;
	cin >> num;
	if (option == 1) {
		book = novels[num];
	}
	else if (option == 2) {
		book = biographies[num];
	}
	else if (option == 3) {
		book = textbooks[num];
	}
	bookDetails(book);
}

void bookName(int o) {
	switch (o) {
	case 1:
		for (int i = 0; i < NOVELS; i++) {
			cout << novels[i]->title << "[" << i << "]" << endl;
		}
		break;
	case 2:
		for (int i = 0; i < BIOGRAPHIES; i++) {
			cout << biographies[i]->title << "[" << i << "]" << endl;;
		}
		break;
	case 3:
		for (int i = 0; i < TEXTBOOKS; i++) {
			cout << textbooks[i]->title << "[" << i << "]" << endl;
		}
		break;
	default:
		invalidChoice();
		break;
	}
}

void bookDetails(Book* book) {
	if (book->owner == 0) {
		cout << "Book details : " << endl;
		cout << book->title << endl;
		cout << book->author << endl;
		cout << book->pages << endl;
		checkOutBook(book);
	} else {
		cout << "Sorry, this book is currently held by another person. Press 1 to go back. " << endl;
		cin >> option;
		if (option == 1) {
			accountPage();
		} else {
			invalidChoice();
		}
	}
}

void checkOutBook(Book* book) {
	cout << "To sign " << book->title << " out, press 1. To go back, press 2" << endl;
	cin >> option;
	if (option == 1) {
		book->owner = ID;
		myBooks.push_back(book);
		books_signed_out++;
		cout << "Book successfully signed out. To go back, press 2" << endl;
		cin >> option;
		if (option == 2) {
			accountPage();
		}
	}
	else if (option == 2) {
		accountPage();
	}
	else {
		invalidChoice();
	}
}

void checkInBook(Book* book) {
	cout << "Are you sure you want to return " << book->title << "(y/n)?";
	cin >> choice;
	if (choice == 'y') {
		book->owner = 0;
		myBooks.erase(myBooks.begin() + getPlace(book));
		books_signed_out--;
		cout << "Press 1 to return back to account page" << endl;
		cin >> option;
		if (option == 1) {
			accountPage();
		}
	}
	else if (choice == 'n') {
		accountPage();
	}
	else {
		invalidChoice();
	}
}

void editMyBooks() {
	for (auto i = allBooks.begin(); i != allBooks.end(); i++) {
		auto item = *i;
		if (find(myBooks.begin(), myBooks.end(), item) != myBooks.end()) {
			if (myBooks.size() > 0) {
				if (item->owner != ID) {
					myBooks.erase(myBooks.begin() + getPlace(item));
				}
			}
		}
		else if (item->owner == ID) {
			myBooks.push_back(item);
		}
	}
}

int getPlace(Book* book) {
	int temp = 0;
	for (auto i = myBooks.begin(); i != myBooks.end(); i++) {
		if (*i == book) {
			return temp;
		}
		temp++;
	}
}

void invalidChoice() {
	cout << "Invalid choice" << endl;
}
