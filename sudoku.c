#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <ctype.h>
#include <getopt.h>
#include <stdbool.h>

void readSudoku(int x[][9], FILE *in);
void printSudoku(int x[][9]);
int validateRows(int x[9][9]);
int validateCols(int x[9][9]);
int validateSubGrids(int x[9][9]); // Broken: Need to fix subgrid locator
bool validSudokuGrid(int x[][9]);

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
    // validateRows(sudoku_grid);
    // validateCols(sudoku_grid);
    if(validSudokuGrid(sudoku_grid)) {
        printf("Valid Grid! \n");
    } else {
        printf("Not Valid Grid! \n");
    }
    validateSubGrids(sudoku_grid);

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

// Used to validate rows per 3x3 grid
int validateRows(int x[9][9]) {
    
    // Traversing Rows
    for(int i = 0; i < 9; i++) {

        // Storing a zero at the last index of the array
        // Used for validation later on
        int rowValidate[9] = {0};  

        // Traversing Columns
        for(int j = 0; j < 9; j++) {

            // Holds current integer value wherever our array is at
            int currVal = x[i][j];

            // Checking to see if our index is used or not
            // Hence, using the compliment to check for dupes
            // Example, currVal = x[0][1] ; row zero column 1
            // currVal evaluates to 7
            if(rowValidate[currVal - 1] == 0) {
                rowValidate[currVal - 1] = 1;
            } else {    // Checks if dupllicate or out of bounds
                printf("Row: %d does not have the required values\n", i + 1);
                return 0;
            }
        }
    }
}

// Validating columns in the 3x3 grid *BACK UP*
// Fixing column tracking
// int validateCols(int x[9][9]) {
//     int col = 0;
//     // Traversing Rows
//     for(int i = 0; i < 9; i++) {
//         // Initialzing array to detect for duplicate values
//         int colValidate[9] = {0};
//         // Traversing columns
//         for(int j = 0; j < 9; j++) {
//             // Holds current value depending on row / col
//             int currVal = x[i][j];

//             // If the index is filled with a zero
//             // that means the index is not taken
//             if(colValidate[currVal - 1] == 0) {
//                 colValidate[currVal - 1] = 1;   // fill index with 1 (true)
//             } else {    // Checks if dupllicate or out of bounds
//                 printf("Column: %d does not have the required values\n", j + 1);
//                 return 0;
//             }
//         }   col ++;
//     }
// }

// Function to check 3x3 Sub-Grids
int validateCols(int x[9][9]) {
    // Traversing Rows
    for(int i = 0; i < 9; i += 3) {
        // Trvaversing Columns
        for(int j = 0; j < 9; j+=3) {

            // Init array to check if value already exists
            int subgridValidate[9] = {0};

            // Limiting to check only 3x3 subgrids instead of whole 9x9
            for(int k = i; k < i + 3; k++) {
                // Limiting to check only 3x3 subgrids instead of whole 9x9
                for(int m = j; m < j + 3; m++) {
                    int currVal = x[k][m];  // Store whatever value you are on into currVal

                    // If the index does not hold that value set to 1
                    if(subgridValidate[currVal - 1] == 0) {
                        subgridValidate[currVal - 1] = 1;
                    } else {    // If index is already taken then there is a duplicate
                        printf("Column: %d does not have required values \n",m + 1);
                    }
                }
            }
        }
    }
}

// Needs intense fixing
int validateSubGrids(int x[9][9]) {
    for(int i = 0; i < 9; i += 3) {
        for(int j = 0; j < 9; j+=3) {
            int subgridValidate[9] = {0};

            for(int k = i; k < i + 3; k++) {
                for(int m = j; m < j + 3; m++) {
                    int currVal = x[k][m];
                    if(subgridValidate[currVal - 1] == 0) {
                        subgridValidate[currVal - 1] = 1;
                    }  else {printf("row: %d, col: %d", k, m);}
                    // else if((subgridValidate[currVal - 1] == 1) && (k >= 0 && k <=2)) {    // checks for dupes, segfaults if num > 9
                    //     if( m >= 0 && m <= 2 ){
                    //         printf("The top left subgrid does not have the required values\n");
                    //         return 0;
                    //     }
                    //     else if ( m >= 3 && m <= 5 ){
                    //         printf("The top mid subgrid does not have the required values\n");
                    //                                     printf("%d %d", i,j);

                    //         return 0;
                    //     }
                    //     else if ( m >= 6 && m <= 8 ){
                    //         printf("The top right subgrid does not have the required values\n");
                    //                                                                 printf("%d %d", i,j);

                    //         return 0;
                    //     }
                    // } 
                    // else if((subgridValidate[currVal - 1] == 1) && (k >= 3 && k <=5) ) {    // checks for dupes, segfaults if num > 9
                    //     if( m >= 0 && m <= 2 ){
                    //         printf("The left subgrid does not have the required values\n");
                    //         printf("%d %d", i,j);
                    //         return 0;
                    //     }
                    //     else if( m >= 3 && m <= 5 ){
                    //         printf("The left mid subgrid does not have the required values\n");
                    //         printf("%d %d", i,j);
                    //         return 0;
                    //     }   
                    //     else if( m >= 6 && m <= 8 ){
                    //         printf("The left right subgrid does not have the required values\n");
                    //         return 0;
                    //     }
                    // }
                    // else if((subgridValidate[currVal - 1] == 1) && (k >= 6 && k <=8)) {    // checks for dupes, segfaults if num > 9
                    //     if( m >= 0 && m <= 2 ){
                    //         printf("The bottom left subgrid does not have the required values\n");
                    //         printf("%d %d", i,j);
                    //         return 0;
                    //     }
                    //     else if( m >= 3 && m <= 5 ){
                    //         printf("The bottom mid subgrid does not have the required values\n");
                    //         return 0;
                    //     }   
                    //     else if( m >= 6 && m <= 8 ){
                    //         printf("The bottom right subgrid does not have the required values\n");
                    //         return 0;
                    //     }
                    // }
                    // else //all subgrid have correct values.
                    // {
                    //     return 1; // true
                    // }

                    // else if((subgridValidate[currVal - 1] == 1) && (k >= 0 && k <=2) && (m >= 0 && m <= 2)) {    // checks for dupes, segfaults if num > 9     
                    //     printf("The top left subgrid does not have the required values\n");
                    //     return 0;
                    // } 
                    // else if((subgridValidate[currVal - 1] == 1) && (k >= 0 && k <=2) && (m >= 3 && m <= 5)) {    // checks for dupes, segfaults if num > 9
                    //     printf("The top mid subgrid does not have the required values\n");
                    //     return 0;
                    // } 
                    // else if((subgridValidate[currVal - 1] == 1) && (k >= 0 && k <=2) && (m >= 6 && m <= 8)) {    // checks for dupes, segfaults if num > 9
                    //     printf("The top right subgrid does not have the required values\n");
                    //     return 0;
                    // } 
                    // if((subgridValidate[currVal - 1] == 1) && (k >= 3 && k <=5) && (m >= 0 && m <= 2)) {    // checks for dupes, segfaults if num > 9
                    //     printf("The left subgrid does not have the required values\n");
                    //     printf("%d %d", i,j);
                    //     return 0;
                    // } 
                    // else if((subgridValidate[currVal - 1] == 1) && (k >= 3 && k <=5) && (m >= 3 && m <= 5)) {    // checks for dupes, segfaults if num > 9
                    //     printf("The left mid subgrid does not have the required values\n");
                    //     return 0;
                    // } 
                    // else if((subgridValidate[currVal - 1] == 1) && (k >= 3 && k <=5) && (m >= 6 && m <= 8)) {    // checks for dupes, segfaults if num > 9
                    //     printf("The left right subgrid does not have the required values\n");
                    //     return 0;
                    // } 
                    // else if((subgridValidate[currVal - 1] == 1) && (k >= 6 && k <=8) && (m >= 6 && m <= 8)) {    // checks for dupes, segfaults if num > 9
                    //     printf("The bottom left subgrid does not have the required values\n");
                    // }
                }
            }
        }
    }

}

bool validSudokuGrid(int x[][9]) {
    return validateCols(x) && validateRows(x) && validateSubGrids(x); // if all true = valid 9x9, else = not valid 9x9
}