#include <time.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>
#include <assert.h>
#include <stdbool.h>


#include "ai.h"
#include "utils.h"
#include "hashtable.h"
#include "chessformer.h"
#include "node.h"
#include "queue.h"

node_t *create_init_node(chessformer_t* chessformer) {
	node_t *n = (node_t *) malloc(sizeof(node_t));
	assert(n);

	n->depth = 0;
	n->num_childs = 0;
	n->move_delta_x = 0;
	n->move_delta_y = 0;
	n->state.map = (char **) malloc(sizeof(char *) * chessformer->lines);
	assert(n->state.map);
	int mapLength = chessformer->num_chars_map / chessformer->lines;
	for(int i = 0; i < chessformer->lines; i++){
		n->state.map[i] = (char *) malloc(sizeof(char) * (mapLength + 1));
		assert(n->state.map[i]);
		for(int j = 0; j < mapLength; j++){
			n->state.map[i][j] = chessformer->map[i][j];
		}
		n->state.map[i][mapLength] = '\0';
	}

	n->state.player_x = chessformer->player_x;
	n->state.player_y = chessformer->player_y;

	n->parent = NULL;

	return n;
}

/**
 * Copy a src into a dst state
*/
void copy_state(chessformer_t* init_data, state_t* dst, state_t* src){
	dst->map = malloc(sizeof(char *) * init_data->lines);
	assert(dst->map);
	for (int i = 0; i < init_data->lines; i++){
		int width = strlen(src->map[i]) + 1;
		dst->map[i] = malloc(width);
		assert(dst->map[i]);
		memcpy(dst->map[i], src->map[i], width);
	}
	dst->player_x = src->player_x;
	dst->player_y = src->player_y;
}

node_t* create_node(chessformer_t* init_data, node_t* parent){
	node_t *new_n = (node_t *) malloc(sizeof(node_t));
	new_n->parent = parent;
	new_n->depth = parent->depth + 1;
	copy_state(init_data, &(new_n->state), &(parent->state));
	return new_n;
}

/**
 * Apply an action to node n, create a new node resulting from 
 * executing the action, and return if the player moved
*/
bool applyAction(chessformer_t *init_data, node_t *n, node_t **new_node, int move_delta_x, int move_delta_y){
	bool player_moved = false;

    *new_node = create_node(init_data, n);
    (*new_node)->move_delta_x = move_delta_x;
	(*new_node)->move_delta_y = move_delta_y;

    player_moved = execute_move(init_data, &((*new_node)->state), move_delta_x, move_delta_y);

	return player_moved;
}

void free_state(chessformer_t *init_data, state_t *state) {
    // Free each row of the map
    for (int i = 0; i < init_data->lines; i++) {
        free(state->map[i]);  // Free each row
    }
    // Free the map array itself
    free(state->map);  // Free the pointer to the rows
}

/**
 * Given a 2D map, returns a 1D map
*/
void flatten_map(chessformer_t* init_data, char **dst_map, char **src_map){

	int current_i = 0;
	for (int i = 0; i < init_data->lines; ++i)
	{
		int width = strlen(src_map[i]);
		for (int j = 0; j < width; j++){
			(*dst_map)[current_i] = src_map[i][j];
			current_i++;
		}
	}
}

bool winning_state(chessformer_t chessformer){
	for (int i = 0; i < chessformer.lines; i++) {
		for (int j = 0; j < chessformer.num_chars_map/chessformer.lines; j++) {
			if (chessformer.map[i][j] == '$') {
				return false;
			}
		}
	}
	return true;
}

char *saveSolution(node_t *finalNode) {
	int hierarchyCount = 0;
	node_t *current = finalNode;
	while(current){
		hierarchyCount++;
		current = current->parent;
	}
	char *soln = (char *) malloc(sizeof(char) * hierarchyCount * 2 + 1);
	assert(soln);
	
	current = finalNode;
	int left = hierarchyCount - 1;
	while(current){
		if(! current->parent){
			current = current->parent;
			continue;
		}
		left--;
		char x = current->parent->state.player_x + current->move_delta_x + '`';
		char y = current->parent->state.player_y + current->move_delta_y + '0';
		soln[2 * left] = x;
		soln[2 * left + 1] = y;
		current = current->parent;
	}

	soln[2 * (hierarchyCount - 1)] = '\0';

	return soln;
}



#define DEBUG 0

#include <assert.h>
/**
 * Find a solution by exploring all possible paths
 */
void find_solution(chessformer_t* init_data, bool show_solution){
	// Statistics variables
	// Keep track of solving time
	clock_t start = clock();
	unsigned int exploredNodes = 0;
	unsigned int generatedNodes = 0;
	unsigned int duplicatedNodes = 0;
	// Maximum depth limit so far - e.g. in Iterative Deepening
	// int max_depth = 0;
	unsigned solution_size = 0;

	// Solution String containing the sequence of moves
	char* solution = NULL;

	// Optimisation (Algorithm 2 setup)
	// HashTable hashTable;
	// Key size
	// size_t htKeySize = sizeof(char) * init_data->num_chars_map;
	// Data size - same as key size as we do not do anything with data pair.
	// size_t htDataSize = sizeof(char) * init_data->num_chars_map;
	// Hash table capacity - limit not too important, but 26 * 9 covers all
	//		single piece locations on one maximum size board.
	// size_t htCapacity = 26 * 9;
	// ht_setup(&hashTable, htKeySize, htDataSize, htCapacity);
	// Data structure to create a 1D representation of the map
	// Temporary variable to store key for hash table. We look at
	// the state of the map to decide whether we have seen the state
	// before - other ways of flattening the map
	// char *flat_map = calloc(init_data->num_chars_map, sizeof(char));
	// assert(flat_map);

	// Buffer for dequeue
	chessformer_t current_state = *init_data; 
	current_state.soln = "";
	
	// FILL IN
	// Algorithm 1 (or 2) from slide

	// Create the initial node from the starting game state
    node_t* initial_node = create_init_node(init_data);

	// Initialize a queue to explore the nodes
    queue_t* queue = create_queue();  // Assuming you have a queue implementation
    enqueue(queue, initial_node);     // Enqueue the initial node

	// Create an empty array for the exploredTable
    node_t** exploredTable = malloc(sizeof(node_t*) * 100); // Initial size of 100
    int exploredTableSize = 0;
    int exploredTableCapacity = 100;

	// Hash table for duplicate detection
    HashTable hashTable;
    size_t htKeySize = sizeof(char) * init_data->num_chars_map;
    size_t htDataSize = sizeof(char) * init_data->num_chars_map;
    size_t htCapacity = 26 * 9;  // Set hash table capacity, e.g., all possible positions
    ht_setup(&hashTable, htKeySize, htDataSize, htCapacity);
    
    // Temporary variable to store the flattened map (1D representation)
    char *flat_map = calloc(init_data->num_chars_map, sizeof(char));
    assert(flat_map);

    // Determine the board dimensions
    int rows = init_data->lines; // Number of rows
    int cols = init_data->num_chars_map / rows; // Number of columns

	// Main exploration loop
    while (!is_queue_empty(queue)) {
        // Dequeue the current node
        node_t* current_node = dequeue(queue);
        exploredNodes++; // Increment explored nodes count

        // Record the current node in the explored table (resize if necessary)
        if (exploredTableSize >= exploredTableCapacity) {
            exploredTableCapacity *= 2;
            exploredTable = realloc(exploredTable, sizeof(node_t*) * exploredTableCapacity);
        }
        exploredTable[exploredTableSize++] = current_node;

	    // Check if the current node is a winning state
        if (winning_condition(init_data, &current_node->state)) {
            solution = saveSolution(current_node); // Save the solution path
			solution_size = current_node->depth;  // Update solution size with the depth of the solution node

            break;  // Exit the loop once we find a solution
        }

		// Explore possible moves for queen and knight
		for (int y = -rows + 1; y < rows; y++) {
    		for (int x = -cols + 1; x < cols; x++) {
        		// Skip invalid moves (non-queen and non-knight) (Optimisation)
        		if (!valid_queen_move(&current_node->state, x, y) && !valid_knight_move(x, y)) {
            		continue; // Skip invalid moves
        		}
        		node_t* new_node = NULL;

        		bool player_moved = applyAction(init_data, current_node, &new_node, x, y);
				generatedNodes++; // Increment generated nodes count

        		if (!player_moved) { // Only enqueue if the player moved
					free_state(init_data, &(new_node->state));  // Free the map inside new_node->state
            		free(new_node); // Free the new node if the action was not valid
					continue;
        		} 

                // Flatten the map of the new node
                flatten_map(init_data, &flat_map, new_node->state.map);

                // Check for duplicates using the hash table
                if (ht_contains(&hashTable, flat_map)) {
                    duplicatedNodes++; 
                    free_state(init_data, &(new_node->state));  
                    free(new_node);  
                    continue; 
                }

                // Insert the flattened map into the hash table
                ht_insert(&hashTable, flat_map, flat_map);  

            	enqueue(queue, new_node);  // Enqueue the new node for exploration
    		}
		}
    }

	// End FILL IN

	// Stop clock
	clock_t end = clock();
	double cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;

	// Clear memory
	// FILL IN
	// Free memory for nodes stored in explored table and generated nodes still 
	// 		in queue. In Algorithm 2 - also free flat_map used as temporary 
	// 		variable for storing map.

	for (int i = 0; i < exploredTableSize; i++) {
    	if (exploredTable[i] != NULL) {
        	free_state(init_data, &(exploredTable[i]->state));  // Free the map (state) of each node
        	free(exploredTable[i]);  // Free the node itself
    	}
	}

	// Free the exploredTable array itself after all nodes have been freed
	free(exploredTable);
	
	// In Algorithm 2, free hash table.
	 ht_clear(&hashTable);
	 ht_destroy(&hashTable);
	 free(flat_map);
	// End FILL IN

	queue_node_t *current = queue->front;
	while (current != NULL) {
    	queue_node_t *next = current->next;
    
    	// Free the state inside the node held by the queue
    	free_state(init_data, &(current->data->state));  // Access 'data' instead of 'node'
    
    	// Free the node itself
    	free(current->data);  // Free the 'node_t' structure
    	free(current);  // Free the queue node
    	current = next;
	}

	free(queue);

	// Report statistics.

	// Show Solution	
	if(show_solution && solution != NULL) {
		play_solution(*init_data, solution);
	}

	// endwin();

	if(solution != NULL){
		printf("\nSOLUTION:                               \n");
		printf( "%s\n\n", solution);
		FILE *fptr = fopen("solution.txt", "w");
		if (fptr == NULL) {
			printf("Could not open file");
			return ;
		}
		fprintf(fptr,"%s\n", solution);
		fclose(fptr);
		
		free(solution);
	}

	printf("STATS: \n");
	printf("\tExpanded nodes: %'d\n\tGenerated nodes: %'d\n\tDuplicated nodes: %'d\n", exploredNodes, generatedNodes, duplicatedNodes);
	printf("\tSolution Length: %d\n", solution_size);
	printf("\tExpanded/seconds: %d\n", (int)(exploredNodes/cpu_time_used));
	printf("\tTime (seconds): %f\n", cpu_time_used);	
}

void solve(char const *path, bool show_solution)
{
	/**
	 * Load Map
	*/
	chessformer_t chessformer = make_map(path, chessformer);
	
	/**
	 * Count number of boxes and Storage locations
	*/
	map_check(chessformer);

	/**
	 * Locate player x,y position
	*/
	chessformer = find_player(chessformer);
	
	chessformer.base_path = path;

	find_solution(&chessformer, show_solution);

	for(int i = 0; i < chessformer.lines; i++){
		free(chessformer.map[i]);
		free(chessformer.map_save[i]);
	}
	free(chessformer.map);
	free(chessformer.map_save);
	free(chessformer.buffer);

}
