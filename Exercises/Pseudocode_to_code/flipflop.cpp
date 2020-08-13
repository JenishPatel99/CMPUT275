//~~~~~~~~~~~~~~~~~~~~~~~~~
// Name = Jenish Patel
// CMPUT 275, Winter 2020
// FlipFlopSort
//~~~~~~~~~~~~~~~~~~~~~~~~~
#include <iostream>

using namespace std;

/*
	Description: Sorts an array a[], with given size n, into ascending
	order. It is an in-pace sorting algorithm. Using recursion the function
	is able to sort array with a running time of O(n^2.7)

	Arguments: uint32_t a[]; input array, uint32_t n; array size
*/
void flipFlopSort(uint32_t a[], uint32_t n) {
	if (n == 2) {
		if (a[1-1] > a[2-1]) {
			swap(a[1-1], a[2-1]);
		}
	} else {
		// Due to a unsolved complication with the ceiling function I had to manually
		// set the ceiling of following arthimetic.
		flipFlopSort(a, (2*n + (3 - 1))/3);
		flipFlopSort(&a[n - (2*n + (3 - 1))/3],(2*n + (3 - 1))/3);
		flipFlopSort(a, (2*n + (3 - 1))/3);
	}
}

// Driver function
int main() {
	uint32_t n;
	// Reads in length of array
	cin >> n;
	uint32_t a[n];

	// Reads in numbers into array
	for (uint32_t i = 0; i < n; i++) {
		cin >> a[i];
	}

	// If there are only 1 element occurs no sorting is required
	if (n != 1) {
		flipFlopSort(a, n);
		// Displayed sorted array
		for (uint32_t i = 0; i < n; i++) {
			if (i < n-2) {
				cout << a[i] << " ";
			} else {
				cout << a[i];
			}
		}
		cout << endl;
	} else {
		cout << a[0] << endl;
	}

	return 0;
}