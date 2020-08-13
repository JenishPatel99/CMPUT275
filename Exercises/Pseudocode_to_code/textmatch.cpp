//~~~~~~~~~~~~~~~~~~~~~~~~~
// Name = Jenish Patel
// CMPUT 275, Winter 2020
// Pseudocode to code
//~~~~~~~~~~~~~~~~~~~~~~~~~
#include <iostream>
// Header used for getting length of char array
#include <cstring>

using namespace std;

/*
	Description: The following program takes in two char array and
	compares array 1 (s) to array 2 (t). It prints all indices i such
	that array 1 appears as a substring of array 2 starting at index i
	in array 2.

	Arguments: const char *s, const char *t
*/
void textMatch(const char *s, const char *t) {
	int step[strlen(s)];
	int k;

 	step[0] = -1;
	k = -1;

	// Computes an array step[] where step[i] is equal to the largest k
	// such that the first and last k+1 characters in s[0..i] form the same
	// string
	for (unsigned int i = 1; i < strlen(s) + 1; i++) {
		while ((k >= 0) && (s[k + 1] != s[i])) {
			k = step[k];
		}
		if (s[k + 1] == s[i]) {
			++k;
		}
		step[i] = k;
	}

	int m;

	m = -1;

	// WIth the usage of step[] array the following block of code
	// finds all occurances of s as a substring of t.
	for (unsigned int i = 0; i < strlen(t) + 1; i++) {
		while ((m >= 0) && s[m + 1] != t[i]) {
			m = step[m];
		}
		if (s[m + 1] == t[i]) {
			++m;
		}
		if (m == ((signed int) (strlen(s) - 1))) {
			cout << i + 1 - strlen(s) << " ";
			m = step[m];
		}
	}
	cout << endl;
}
int main() {
	// Inititalize the max number of chars to be 1000000 for both array
	// but to satisfy the null terminator, the addition of 1 extra element
	// is needed.
	char s[1000001];
	char t[1000001];

	// Reads input
	cin >> s;
	cin >> t;

	textMatch(s,t);
	
	return 0;
}