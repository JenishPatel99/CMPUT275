//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Names: Jenish Patel, Mattheas Jameison
// ID: 1601689, 1572027
// CMPUT 275, Winter 2020
// Assignment 2, part 2
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include <iostream>
#include <unordered_map>
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>
#include <utility>
#include <sstream>
#include "dijkstra.cpp"
#include "digraph.cpp"
#include "serialport.cpp"
#include "wdigraph.h"

// included for part 2
#include <cassert>
#include "serialport.h"


struct Point {
	long long lat;	// latitude of the point
	long long lon;	// longitude of the point
};

enum ConnectionState {
  	WAITINGFORACK, LISTEN, END, CALCULATE, COMMUNICATE
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
	Reads in query requests from the serial port (from arduino). In this query the
	function is given two points, each having their own latitude and longtitude. This
	function then decides the two closest known vertices to these points in the weighted
	directed graph, uses dijkstra to find the shortest path between them (if possible),
	then using the serial port the function communicates the number of way points, 
	between these two points, and then communicates the latitudes and longtitudes of 
	these intermediate points so the arduino can draw a path between the points. 

  Arguments:
    graph (WDigraph): Instance of the weighted directed Wdigraph class
    points (unordered_map): Hash table with vertex as key and Point struct as value continaing all vertexs in graph
    
  Returns:
    Nothing
*/
void runQuery(WDigraph& graph, unordered_map<int, Point>& points) {

	SerialPort Serial("/dev/ttyACM0");
	int arr[2000];
	int i = 0;
	int size;

	// sets initial state
	ConnectionState state = LISTEN;

	string line;
	

	while(state!=END){
		
		if(state==LISTEN){

			line ="";
			do{
				line = Serial.readline(10);

			}while(line.substr(0,1) != "R");

			// change state of FSM
			state = CALCULATE;

		}else if(state == CALCULATE){

			// READ IN LATITUDE FOR POINT 1

			// this should return where the space is after lat1
			size_t found = line.find(" ",2);
			size_t prev_found = found;
			
			// new char array to hold lat for point 1
			char lat1[found-2]; // maybe -3

			// copy sub string of string line to char array
			line.copy(lat1,found-2,2);

			// convert substring char array to int value
			long long latitude1 = atoi(lat1);

			

			// READ IN LONGTITUDE FOR POINT 1

			// finds where the next space is after lon1
			found = line.find(" ", prev_found+1);

			// new char array to hold lon1  
			char lon1[found-prev_found];

			// copy sub string of string line to char array
			line.copy(lon1,found-prev_found,prev_found+1);

			// convert substring char array to int value
			long long longitude1 = atoi(lon1);

			prev_found=found;

			

			// READ IN LATITUDE FOR POINT 2

			// finds where the next comma is for second vertex number
			found = line.find(" ", prev_found+1);

			// new char array to hold lat2 number 
			char lat2[found-prev_found];

			// copy sub string of string line to char array
			line.copy(lat2,found-prev_found,prev_found+1);

			// convert substring char array to int value
			long long latitude2 = atoi(lat2);

			prev_found=found;


			
			// READ IN LONGTITUDE FOR POINT 2

			// new char array to hold lon2 number
			char lon2[line.length()-prev_found];

			// copy sub string of string line to char array
			line.copy(lon2,line.length()-prev_found,prev_found+1);

			// convert substring char array to int value
			long long longitude2 = atoi(lon2);


			// hash table tree that will be our path from start to end
			unordered_map<int, PIL> tree;

			// Convert two points to two known vertices
			int startVertex = findClosestNode(latitude1, longitude1, points);
			int endVertex = findClosestNode(latitude2, longitude2, points);

			// Find least cost paths from point1 to all other points and store in "tree"
			dijkstra(graph, startVertex, tree);

			if(tree.begin() == tree.end()){
				cout << "N = 0" << endl;
				assert(Serial.writeline("N 0\n"));
				state = END;

			}else{
				int temp_id = endVertex;

				// finds all necessary vertices needed to be traversed to go from start to end vertex
				while(arr[i] != startVertex){
					if(i > 1999){ 
						i = 0;
						state = END;
						break;
					}
					
					arr[i+1] = temp_id;
					temp_id = tree[temp_id].second;
					i++;
				}
				// tells terminal how many "intermediate" vertices it should expect
				size = i;
				
				// no path wait for next query
				if (size <= 0){
					state = END;

				}else{
					// tell client the num of waypoints then change to communication state
					assert(Serial.writeline("N "+to_string(size)+"\n"));
					state = COMMUNICATE; 
				}
				
			}// end of if statement
				
		}else if(state==COMMUNICATE){ 

			// reading in the ACK from arduino acknwloding the N(num of waypoints)
			string temp1;
			temp1 = Serial.readline(1500); // change back to 2000
			
			// Communicate way points if server recieves A from client
			if(temp1.substr(0,1)=="A"){ // temp1
				
				int loop_iterations = size;
				for(int j=0; j<loop_iterations; j++){
					int temp3 = arr[size]; 
					assert(Serial.writeline("W "+to_string(points[temp3].lat)+" "+to_string(points[temp3].lon)+"\n"));//put <> around \n??????????
					size--;

					// read in expected A from client acknowledging waypoint
					temp1 = Serial.readline(1000);
					
					// break from loop and exit function if waypoint not ackowledged
					if(temp1.substr(0,1)!="A"){ // temp1
						state=END;
						break;
					}
				}

				// after we have communicated all waypoints succesfully
				assert(Serial.writeline("E\n")); 
				state = END;
			}else{
				state=END;
			}

		}else if(state=END){ 
			return;
		}

	}// end of while ConnectionState!=END

}// end of void runQuery()



// Driver function
int main() {
	WDigraph graph;
	unordered_map<int, Point> points;
	string filename = "edmonton-roads-2.0.1.txt";
	readGraph(filename, graph, points);
	
	while(true){
		runQuery(graph, points);
	}
	
}// end of int main()