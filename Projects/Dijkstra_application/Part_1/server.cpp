//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Names: Jenish Patel, Mattheas Jameison
// ID: 1601689, 1572027
// CMPUT 275, Winter 2020
// Assignment 2, part 1
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include <iostream>
#include <unordered_map>
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>
#include <utility>
#include <sstream>
#include "wdigraph.h"
#include "heap.h"
#include "dijkstra.h"

struct Point {
	long long lat;	// latitude of the point
	long long lon;	// longitude of the point
};


/*
  Description:
    Calculates Manhattan distance between two points, passed in by two 
    Point structs that continas a lat and lon inside them

  Arguments:
    pt1 (Point): Instance of Point struct with lat/lon
    pt2 (Point): Instance of Point struct with lat/lon
    
  Returns:
    Manhattan distance
*/
long long manhattan(const Point& pt1, const Point& pt2) {
	return (abs(pt1.lat - pt2.lat) + abs(pt1.lon - pt2.lon));

}// end of long long manhattan()


/*
  Description:
    Opens and reads from a text file to construct a directed graph. It reads
    line by line with the first character either being a V or E denoting you to 
    add a vertex or edge respectively. It deals with each case indivdually adding 
    both the vertex and edges to an instance of the WDigrpah object and also adds
    the vertices along with their lat/ lon's as key/ value pairs to a hash table
    points. 

  Arguments:
    filename (string): Name of file to be opened
    pt2 (WDigraph): Instance of Wdigraph that we build in this function
    points (unordered_map): Hash table with vertex as key and Point struct as value
    
  Returns:
    Nothing
*/
void readGraph(string filename, WDigraph& graph, unordered_map<int, Point>& points) {
	ifstream file;
	file.open(filename);
	string line;

	if (file.is_open()) {
		// checks if file reaches end of file
		while (!file.eof()) {
			getline(file, line);
			if (line[0] == 'V') {
				// temporary point to store lat/ lon of current vertex
				Point temp_point;

				// reads in the vertex ID number and stores it
				size_t index = line.find(',',2);
				size_t old = index;
				char first_arr[index - 2];
				line.copy(first_arr, index-2, 2);
				int vertex = atoi(first_arr); 

				// reads in the latitude and stores it
				index = line.find(',', old+1);
				char second_arr[index-old];
				line.copy(second_arr, index-old, old+1);
				old = index;
				long long temp_lat = static_cast<long long>(atof(second_arr) * 100000);

				// reads in the longitude and stores it
				index = line.find(',', old+1);
				char third_arr[index-old];
				line.copy(third_arr, index-old, old+1);
				long temp_lon = static_cast<long long>(atof(third_arr) * 100000);

				// Fills the temporary point with lat/lon values
				temp_point.lat = temp_lat;
				temp_point.lon = temp_lon;

				// ads vertex to undirected graph and inserts the vertex and (lat,lon) to hash table
				graph.addVertex(vertex);
				points.insert(make_pair(vertex, temp_point));

			} else if (line[0] == 'E') {
				// reads in and stores the start vertex
				size_t index = line.find(',',2);
				size_t old = index;
				char start_arr[index - 2];
				line.copy(start_arr, index-2, 2);
				uint32_t start = atoi(start_arr);

				// reads in and stores stores the end vertex
				index = line.find(',',old+1);
				char end_arr[index-old];
				line.copy(end_arr, index-old, old+1);
				uint32_t end = atoi(end_arr);

				// two temp Point structs to store the lat/ lon of the read in vertices
				Point start_point = {0,0};
				Point end_point = {0,0};

				// iterates over hash table to find both vertices and the lat/lon associated with them
				unordered_map<int, Point>::const_iterator start_it = points.find(start);
				if (start_it != points.end()) {
					start_point = start_it->second;
				}
				
				unordered_map<int, Point>::const_iterator end_it = points.find(end);
				if (end_it != points.end()) {
					end_point = end_it->second;
				}

				long long cost = manhattan(start_point, end_point);

				// Adds the edge to the undirected graph	
				graph.addEdge(start, end, cost);

			}

		}

	}
	file.close();

}// end of void readGraph()


/*
  Description:
    Finds the closest existing vertex which is defined in the points hash 
    table from a latitude and longtitude parameter given as parameters. It does
    this by iterating through all vertices in the points hash table and finding
    the vertex with the smallest manhattan distance from the given lat/ lon. 

  Arguments:
    lat (const long long): Latitude of some point
    lon (const long long): Longtitude of some point
    points (unordered_map): Hash table with vertex as key and Point struct as value continaing all vertexs in graph
    
  Returns:
    The vertex that is closest to the point defined by lat and lon
*/
int findClosestNode(const long long lat, const long long lon, unordered_map<int, Point>& points) {
	// Trying to find a vertex closest to this point
	Point point;
	point.lat = lat;
	point.lon = lon;

	// reads in a vertex from hash table and finds distance to that vertex
	int vertex = points.begin()->first;
	long long dist = manhattan(points[vertex], point);
	long long temp_dist;

	// iterates through all vertices updating the closest one
	// based on manhattan distance
	for (pair<int, Point> it : points) {
		temp_dist = manhattan(it.second, point);
		if (temp_dist < dist) {
			vertex = it.first;
			dist = temp_dist;
		}

	}
	return vertex;

}// end of intfindClosestNode()


/*
  Description:
    Reads in from the terminal (or if compiled a certain way reads in from file) and
    is given two latitudes and two longtitudes which correspond to a start and end vertex.
    This function, if possible, returns the "path" you have to follow to get from the
    start to end vertex, i.e the vertices you need to traverse in order to get from
    one vertex to the other. It prints the lat/lon of these vertices to the terminal
    or again into a file if compiled prooperly. 

  Arguments:
    graph (WDigraph): Instance of the weighted directed Wdigraph class
    points (unordered_map): Hash table with vertex as key and Point struct as value continaing all vertexs in graph
    
  Returns:
    Nothing
*/
void runQuery(WDigraph& graph, unordered_map<int, Point>& points) {
	// read in string to be deciphered
	string start;
	cin >> start;

	if (start == "R") {
		string A;

		unordered_map<int, PIL> tree;

		// lat and lon of two points
		string slat1;
		string slon1;
		string slat2;
		string slon2;

		cin >> slat1 >> slon1 >> slat2 >> slon2;

		// convert string to lon long
		long long lat1 = stoll(slat1);
		long long lon1 = stoll(slon1);
		long long lat2 = stoll(slat2);
		long long lon2 = stoll(slon2);

		// Convert two points to two known vertices
		int startVertex = findClosestNode(lat1, lon1, points);
		int endVertex = findClosestNode(lat2, lon2, points);

		// Find least cost paths from startVertex to all other vertices and store in "tree"
		dijkstra(graph, startVertex, tree);


		if (tree.begin() != tree.end()) {
			int arr[2000];
			int temp_id = endVertex;
			int i = 0;
			arr[0] = 0;

			// finds all necessary vertices needed to be traversed to go from start to end vertex
			while (arr[i] != startVertex) {
				arr[i+1] = temp_id;
				temp_id = tree[temp_id].second;
				i++;
			}

			// tells terminal how many "intermediate" vertices it should expect
			int size = i;
			cout << "N " << size << endl;
			cin >> A;

			int counter = 0;
			// output the lat/lon of intermediate vertices
			// Waits for acknowledgement "A" before proceeding along
			// as we reach the end a flag is there to tell the program
			// that no more waypoints will be release
			while ((A == "A") && (counter < i)) {
				A = "B";
				int temp = arr[size];
				cout << "W ";
				cout << points[temp].lat << " " << points[temp].lon << endl;
				cin >> A;
				--size;
				counter++;
			}
			// signifies the session has ended
			cout << "E" << endl;

		} else {
			// no path between start and end vertex
			cout << "N " << 0 << endl;
		}

	}

}// end of void runQuery()


// Driver function
int main() {
	WDigraph graph;
	unordered_map<int, Point> points;
	string filename = "edmonton-roads-2.0.1.txt";
	
	readGraph(filename, graph, points);

	// Part 1 only takes in one query
	runQuery(graph, points);

}// end of int main()