/*
* CS 594 project
* a backtrack algorithm for minimum dominating set
*/
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <sys/time.h>

/*
* Each vertex has a status.
* White: not decided
* Blue: excluded from dominating set
* Red: included in dominating set
*/

using namespace std;

int n;	//number of vertices
int e;	//number of edges
double btime, etime;
int** adj_matrix;
int delta;	//maximum degree of a graph+1


/* partial dominating set */
int size;
int* dom;

/* minimum dominating set */
int min_size;
int* min_dom;

int n_dominated;	//number of dominated vertices.
int* num_dominated;	//number of times a vertex is dominated by a red vertex.

/*
* number of times a vertex could be dominated if all white vertices were red ones.
* If num_choice[v] is 0, back up.
*/
int* num_choice;

// read the data into adjacency matrix and initialize some variables
int read_graph(char* graph) {
	int i, j, deg, row, col;
	FILE *fp;

	if ((fp=fopen(graph,"r"))==NULL) {
		printf ("Error opening input file %s\n",graph);
		exit(1);
	}

	//read the number of vertices and edges
	rewind(fp);
	fscanf(fp,"%d\t%d",&n,&e);

	//allocate the memory for adj_matrix
	if ((adj_matrix=(int**) malloc(n*sizeof(int*)))==NULL) {
		printf ("Error allocating memory for adj_matrix\n");
		exit(1);
	}
	
	for (i=0; i<n; i++) {
		if ((adj_matrix[i]=(int*) calloc(n,sizeof(int)))==NULL) {
			printf ("Error allocating memory for adj_matrix[%d]\n",i);
			exit(1);
		}
	}

	for(i=0; i<e; i++) { 
		fscanf(fp, "%d\t%d", &row, &col);
		adj_matrix[row][col]=1;
		adj_matrix[col][row]=1;
	}

	fclose(fp);

	/*
	* Initialize some variables.
	* all vertices are white.
	*/
	n_dominated = 0;
	num_choice = new int[n];
	num_dominated = new int[n];
	dom = new int[n];
	min_dom = new int[n];

	size = 0;
	min_size = n;
	delta = 0;
	
	for(i=0; i<n; i++) {
		deg = 1;
		for (j=0; j<n; j++) {
			if(adj_matrix[i][j] > 0)
				deg++;
		}
		if(deg > delta)
			delta = deg;
		num_choice[i] = deg;
		num_dominated[i] = 0;
		min_dom[i] = i;
	}
}

// print array
void print_array(int size, int* data) {
	int i;
	for(i=0; i<size; i++) {
		printf("%d ", data[i]);
	}
}

// recursive funciton to find a minimum dominating set
void min_dom_set(int level, int v) {

	int i, token;
	int n_extra;
	
	int un_dominated;	//number of undominated vertices
	int* neighbors;	//store the neighbors of vertex v
	int neighbor_size;

	un_dominated = n - n_dominated - size;
	n_extra = (un_dominated%(delta+1) == 0) ? (un_dominated/(delta+1)) : (un_dominated/(delta+1) + 1);

	if((size + n_extra) >= min_size)
		return;

	if(level == n) {
		if(size < min_size) {
			int k;
			for(k=0; k<size; k++)
				min_dom[k] = dom[k];
			min_size = size;
		}
		return;
	}

	// find the neighbors of vertex v
	neighbors = new int[n];
	neighbor_size = 0;
	for(i=0; i<n; i++) {
		//if there is edge between the two vertices
		if(adj_matrix[v][i] > 0)
			neighbors[neighbor_size++] = i;  //store the neighbor index
	}

	/*
	* token has two values: 0 and 1
	* If num_choice[v] is at least 1, then token = 1.
	*/
	token = 1; 

	//The exhaustive trackback
	//Try vertex as blue(exclued from dominating set)
	num_choice[v]--;
	if(num_choice[v] > 0) {
		for(i=0; i<neighbor_size; i++)
			num_choice[neighbors[i]]--;

		// If for any vertex v, num_choice[v] is 0 then return.
		for(i=0; i<n; i++)
			if(num_choice[i] < 1)
				token = 0;
		
		if(token == 1) {
			min_dom_set(level+1, v+1);
		}

		//retore neighbors of vertex v
		for(i=0; i<neighbor_size; i++)
			num_choice[neighbors[i]]++;
	}
	num_choice[v]++;

	//Try vertex as red(in dominating set)
	for(i=0; i<neighbor_size; i++) {
		if(!num_dominated[neighbors[i]])
			n_dominated++;
		num_dominated[neighbors[i]]++;
	}
	if(!num_dominated[v])
		n_dominated++;
	num_dominated[v]++;

	dom[size++] = v;
	min_dom_set(level+1, v+1);
	size--;

	//retore neighbors of vertex v
	num_dominated[v]--;
	if(!num_dominated[v])
		n_dominated--;

	for(i=0; i<neighbor_size; i++){
		num_dominated[neighbors[i]]--;
		if(!num_dominated[neighbors[i]])
			n_dominated--;
	}

	delete[] neighbors;
	
}

// get current time
double get_cur_time() {
  struct timeval   tv;
  struct timezone  tz;
  double cur_time;
  
  gettimeofday(&tv, &tz);
  cur_time = tv.tv_sec + tv.tv_usec / 1000000.0;
  
  return cur_time;
}

int main(int argc, char* argv[]) {

	// read the graph file
	read_graph(argv[1]);

	// start to find the minimum dominating set, initial level is 0 and vertex index is 0
	btime = get_cur_time();
	min_dom_set(0, 0);
	etime = get_cur_time();
	fprintf(stderr, "Elapsed time: %lf seconds.\n", etime-btime);
	// print the minimum dominating set
	printf("The size of minimum dominating set: %d \n", min_size);
	printf("The minimum dominating set: ");
	print_array(min_size, min_dom);
	printf("\n");

	return 0;
}