//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Names: Jenish Patel, Mattheas Jameison
// ID: 1601689, 1572027
// CMPUT 275, Winter 2020
// Assignment 2, part 1
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include "heap.h"
#include "dijkstra.h"

/*
  Description:
    Compute least cost paths that start from initial_pair given vertex
	Use initial_pair binary heap to efficeintely retrieve an unexplored
	vert that has the minimum distance from the start vertex
	at every iteration

  Arguments:
    startVertex (int): Vertex ID where the dijkstra algorithim starts
    graph (WDigraph): Instance of the weighted directed Wdigraph class
    tree (unordered_map): initial_pair search tree used to construct the least cost path to some vertex
    
  Returns:
    Nothing
*/
void dijkstra(const WDigraph& graph, int startVertex, unordered_map<int, PIL>& tree){
	// binary heap named events stores vertex, prev vertex, and time to get to prev vertex
	BinaryHeap<PIL, long long> events;
	
	// instance of typedef pair PIL where prev and current vertex are start vertex
	PII initial_pair;
	initial_pair.first = startVertex;
	initial_pair.second = startVertex;

	// add event to binary heap, start vertex burns at time zero
	events.insert(initial_pair, 0); // item, key

	// initalzie tree (hash table) to be empty
	tree = {};

	while(events.size()>0){
		// creates temp storage struct key/ item pair and stores event that happens in the smallest time
		HeapItem<PIL, long long> temp = events.min(); 

		// remove and return the pair with the smallest key, i.e cost
		events.popMin(); 
		
		int u = temp.item.first; 
		int v = temp.item.second; 
		long long cost = temp.key;

		// checks if vertex has already been burned (true if not burned)
		if (tree.count(v) == 0) { 
			// add current vertex and cost to reach vertex to hash table
			tree[v] = make_pair(cost,u);
	
			// iterate through all neigbours and start "burning" edges in events
			for(auto iter = graph.neighbours(v); iter != graph.endIterator(v); iter++){
				PIL temp_pair(v, *iter);
				long long temp_cost = graph.getCost(v, *iter);  
				events.insert(temp_pair, temp_cost + cost);
			}

		}

	}

}// end of void dijkstra() 