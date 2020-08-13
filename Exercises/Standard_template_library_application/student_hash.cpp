// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Name: Jenish Patel (!572027)
// CMPUT 275, WINTER 2020
// Weekly Exercise: Student hash table
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// Include necessary libraries
#include <iostream>
#include <unordered_set>
#include <string>
using namespace std;

bool check = false;
// Struct containing data of student profile
struct StudentRecord {
	string name;
	int id, grade;
};

// Overloading "==" operator to check for same ID
bool operator==(const StudentRecord& i, const StudentRecord& j) {
    if (i.id == j.id) {
    	return true;
    } else {
    	return false;
    }
};

// Code was implement from cpp reference provided in the description.
// Edits were made to match the requirement of the assignment.
namespace std
{
    template<> struct hash<StudentRecord>
    {
        std::size_t operator()(StudentRecord const& s) const noexcept
        {
            return hash<int>()(s.id);
        }
    };
}

// type defintion of unordered_set<struct>
typedef unordered_set<StudentRecord> SR;

/*
	Description: Upon user's query regarding the student's id, the
				 the query is compared with the structs in hashTable.
				 If ID is found then student profile with that ID is
				 displayed on terminal

	Return: Nothing
*/
void ID_query(SR hashTable) {
	StudentRecord student;
	//placeholders
	student.name = "n/a";
	student.grade = 0;

	cin >> student.id;

	if (hashTable.find(student) != hashTable.end()) {
		student = *hashTable.find(student);
		cout << "Name: " << student.name;
		cout << "ID: " << student.id;
		cout << "Grade: " << student.grade;
	} else {
		cout << "Error: no matches found" << endl;
	}	
}

/*
	Description: Upon user's query regarding the students' names, the
				 the query is compared with the students names in hashTable.
				 If name is found then student profile with that name is
				 displayed on terminal. Multiple outputs are possible because
				 of the case where names are repeated.

	Return: Nothing
*/
void name_query(SR hashTable) {
	bool exhaust = false;
	string user_name;
	cin >> user_name;
	for (auto i : hashTable) {
		if (i.name == user_name) {
			cout << "Name: " << i.name;
			cout << " ID: " << i.id;
			cout << " Grade: " << i.grade;
		}
		cout << endl;
	}
	if (exhaust) {
		cout << "Error: no matches found" << endl;
	}	
}

/*
	Description: Upon user's query regarding the students' grades, the
				 the query is compared with the students grades in hashTable.
				 If name is found then student profile with that name is
				 displayed on terminal. Multiple outputs are possible because
				 of the case where grades are repeated.

	Return: Nothing
*/
void grade_query(SR hashTable) {
	bool exhaust = false;
	int user_grade;
	cin >> user_grade;
	for (auto i : hashTable) {
		if (i.grade == user_grade) {
			cout << "Name: " << i.name;
			cout << " ID: " << i.id;
			cout << " Grade: " << i.grade;
		}
		cout << endl;
	}
	if (exhaust) {
		cout << "Error: no matches found" << endl;
	}	
}

/*
	Description: Inserts a student profile into hash table.
				 If the ID of the new student is already there,
				 then a error message will be displayed.

	Return: Hashtable: unordered_set<>
*/
SR insert_item(SR hashTable) {
	StudentRecord student;

	cin >> student.name >> student.id >> student.grade;

	if (hashTable.find(student) == hashTable.end()) {
		hashTable.insert(student);
	} else {
		cout << "Error: Cannot insert Duplicate ID" << endl;
	}

	return hashTable;
}

/*
	Description: Removes a student profile from hash table.
				 If the ID of the new student is not there,
				 then a error message will be displayed.

	Return: Hashtable: unordered_set<>
*/
SR remove_item(SR hashTable) {
	StudentRecord student;
	
	cin >> student.id;

	//placeholders
	student.name = "nothing";
	student.grade = 0;

	if (hashTable.find(student) != hashTable.end()) {
		hashTable.erase(student);
	} else {
		cout << "Error: Cannot remove non-existent ID" << endl;
	}

	return hashTable;
}

/* Driver function */	
int main() {
	SR hashTable;

	while (true) {
		char temp;
		cin >> temp;
		if (temp == 'I') {
			hashTable = insert_item(hashTable);
		} else if (temp == 'R') {
			hashTable = remove_item(hashTable);
		} else if (temp == 'Q') {
			char temp1;
			cin >> temp1;
			if (temp1 == 'i') {
				ID_query(hashTable);
			} else if (temp1 == 'n') {
				name_query(hashTable);
			} else if (temp1 == 'g') {
				grade_query(hashTable);
			}
		} else if (temp == 'S') {
			break;
		}
		cin.clear();
	}
	return 0;
}