#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <ctype.h>
#include <getopt.h>

void readSudoku(int x[][9], FILE *in);
void printSudoku(int x[][9]);

/* These are the only two global variables allowed in your program */
static int verbose = 0;
static int use_fork = 0;

// This is a simple function to parse the --fork argument.
// It also supports --verbose, -v
void parse_args(int argc, char *argv[])
{
    int c;
    while (1)
    {
        static struct option long_options[] =
            {
                {"verbose", no_argument, 0, 'v'},
                {"fork", no_argument, 0, 'f'},
                {0, 0, 0, 0}};
        int option_index = 0;
        c = getopt_long(argc, argv, "vf", long_options, &option_index);
        if (c == -1)
            break;

        switch (c)
        {
        case 'f':
            use_fork = 1;
            break;
        case 'v':
            verbose = 1;
            break;
        default:
            exit(1);
        }
    }
}

// Part A: Function to check whether or not input is a valid Sudoku
void input_check() {
    
}

int main(int argc, char *argv[])
{

    // Commented this out because this will be used later
    // parse_args(argc, argv);

    // if (verbose && use_fork) {
    //     printf("We are forking child processes as workers.\n");
    // } else if (verbose) {
    //     printf("We are using worker threads.\n");
    // }

    // printf("Test");
    
    // Initializing sudoku grid to parse file grid
    int sudoku_grid[9][9];

    if(argc == 1) {
        printf("File successfully opened!\n");
    }

    readSudoku(sudoku_grid, stdin);
    printSudoku(sudoku_grid);

    return 0;
}

void readSudoku(int x[][9], FILE *in) {
    fseek(in, -1, SEEK_CUR); // Seek to start off the current position of the file ptr

    char entry;
    int i, j, totalVals = 0;

	while((fread(&entry, 1, 1, in)) > 0 && totalVals < 81){ // Read 81 digits from stdin file
		if(entry != '\n'){ // Ignore newline
			if(isdigit(entry)){
				++totalVals;
				x[i][j] = entry - '0'; // Store integer representation
				++j;
				if(j == 9){
					j = 0;
					++i;
				}
			}
		}
	}
}

void printSudoku(int x[][9]){
    int i=0, j=0; // i = rows, j = cols
    for (i=0; i<9; i++) {
        for (j=0; j<9; j++) {
            // if we are on the third or fifth number of sub-grid 
            // we make a space between nums
            if (2 == j || 5 == j) {
                printf("%d   ", x[i][j]);
            } 
            // if we are on the last num of row we make a space
            else if (8 == j) {
                printf("%d\n", x[i][j]);
            } 
            // anything else we make a space
            else {
                printf("%d ", x[i][j]);
            }
        }
        // if we are on row 3 or row 5 we make a space
        if (2 == i || 5 == i) {
            printf("\n");
        }
    }
}