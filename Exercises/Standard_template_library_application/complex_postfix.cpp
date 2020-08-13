// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Name: Jenish Patel (!572027)
// CMPUT 275, WINTER 2020
// Weekly Exercise: STL Conepts (2)
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// Include necessary libraries
#include <iostream>
#include <utility>
#include <stack>

using namespace std;

// type definition for pair structure
typedef pair<long long, long long> complex_number;

/*
	Description: Adds the first and second adjacent elements of two 
				 pairs, Stores the result into new_pair

	Return: pair<long long, long long> structure involving new_pair
*/
complex_number addition(complex_number num1, complex_number num2) {
	complex_number sum;

	sum.first = num1.first + num2.first;
	sum.second = num1.second + num2.second;

	return sum;
}

/*
	Description: Subtracts the first and second adjacent elements of two 
				 pairs, Stores the result into new_pair

	Return: pair<long long, long long> structure involving new_pair
*/
complex_number subtraction(complex_number num1, complex_number num2) {
	complex_number diff;

	diff.first = num1.first - num2.first;
	diff.second = num1.second - num2.second;

	return diff;
}

/*
	Description: Performs complex multipication on two complex numbers,
				 in this case the elements of two pairs.Stores the result 
				 into new_pair

	Return: pair<long long, long long> structure involving new_pair
*/
complex_number multiply(complex_number num1, complex_number num2) {
	complex_number product;

	product.first = (num1.first * num2.first) - (num1.second * num2.second);
	product.second = (num1.first * num2.second) + (num1.second * num2.first);

	return product;
}

/*
	Description: Performs complex negation on one complex numbers,
				 in this case the elements of one pair. Stores the result 
				 into new_pair

	Return: pair<long long, long long> structure involving new_pair
*/
complex_number negation(complex_number num1) {
	complex_number neg;

	neg.first = num1.first * -1;
	neg.second = num1.second * -1;

	return neg;
}

/*
	Description: Performs complex conjugation on one complex numbers,
				 in this case the elements of one pair. Stores the result 
				 into new_pair

	Return: pair<long long, long long> structure involving new_pair
*/
complex_number conjugate(complex_number num1) {
	complex_number con;

	con.first = num1.first;
	con.second = num1.second * -1;

	return con;
}

/* driver function */
int main() {
	stack<complex_number> s;
	complex_number num1;
	complex_number num2;
	while (true) {
		char temp;
		cin >> temp;
		// Adds a complex number (pair)
		if (temp == 'V') {
			cin >> num1.first >> num1.second;
			s.push(num1);
		} else if (temp == 'B') {	// Performs binary operaton	
			char op;
			cin >> op;
			num1 = s.top();
			s.pop();
			num2 = s.top();
			s.pop();
			if (op == '+') {
				s.push(addition(num1, num2));
			} else if (op == '-') {
				s.push(subtraction(num1, num2));
			} else if (op == '*') {
				s.push(multiply(num1, num2));
			}
		} else if (temp == 'U') {	// Performs unary operaton
			char op;
			cin >> op;
			num1 = s.top();
			s.pop();
			if (op == '-') {
				s.push(negation(num1));
			} else if (op == 'c') {
				s.push(conjugate(num1));
			}
		} else if (temp == 'S') {	// Stops program
			break;
		}
	}

	complex_number result = s.top();
	s.pop();

	cout << result.first << " " << result.second << endl;
}