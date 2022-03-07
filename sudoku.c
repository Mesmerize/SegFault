/*
 *Authors: Brandon Luna, Erika Mendoza, and Phi Do
 *File/Project: sudoku.c / First CS450 Project
 *Date: 3/6/2022
*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <ctype.h>
#include <getopt.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/wait.h>

typedef struct {
    int rows;
    int cols;
    int res;
    int (*sudokuGrid)[9];
} Sudoku;

void readSudoku(int (*sudokuGrid)[9], FILE *in);
void printSudoku(int x[][9]);
int columnValidation(Sudoku* sudoku);
void *columnValidationRoutine(void *sudoku);
int rowValidation(Sudoku* sudoku);
void *rowValidationRoutine(void *sudoku);
int subgrid3x3Validation(Sudoku* sudoku);
void  *subgrid3x3ValidationRoutine(void* sudoku);

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

    // Initializing sudoku grid to parse file grid
    Sudoku sudoku;
    int sudoku_grid[9][9];
    sudoku.sudokuGrid = sudoku_grid;

    // Initializing 27 Threads
    int numThreads = 27;
    pthread_t tid[numThreads];
    int results[numThreads];
    int thread_count = 0;
    bool validSudokuGrid = true;

    int p;
    int value;

    Sudoku** thread_boxes = malloc(numThreads * sizeof(Sudoku*));

    if (argc == 1)
    {
        printf("File successfully opened!\n");
    }

    // Commented this out because this will be used later
    parse_args(argc, argv);

    if(use_fork) {

            printf("We are forking child processes as workers.\n");
            readSudoku(sudoku.sudokuGrid, stdin);
            printSudoku(sudoku_grid);
            
            for(int i = 0; i < 9; i++) {
                for(int j = 0; j < 9; j++) {
                    if(i % 3 == 0 && j % 3 == 0) {
                            p = fork();
                            if(p == 0) {
                                // printf("subgrids!\n");
                                Sudoku* multThreads = (Sudoku*) malloc(sizeof(Sudoku));
                                multThreads->rows = i;
                                multThreads->cols = j;
                                multThreads->sudokuGrid = sudoku.sudokuGrid;
                                thread_boxes[i] = multThreads;
                                thread_count++;
                                // subgrid3x3ValidationRoutine((void *) thread_boxes[i]);
                                exit(subgrid3x3Validation(thread_boxes[i]));
                            }
                    }
                    if(i == 0) {
                            p = fork();
                            if(p == 0) {
                                // printf("col: %d\n", j);
                                Sudoku* multThreads = (Sudoku*) malloc(sizeof(Sudoku));
                                multThreads->cols = j;
                                multThreads->sudokuGrid = sudoku.sudokuGrid;
                                thread_boxes[i] = multThreads;
                                thread_count++;
                                // columnValidationRoutine((void *) thread_boxes[i]);
                                exit(columnValidation(thread_boxes[i]));
                            }
                    }
                    if(j == 0) {
                            p = fork();
                            if(p == 0) {
                                // printf("row: %d\n", i);
                                Sudoku* multThreads = (Sudoku*) malloc(sizeof(Sudoku));
                                multThreads->rows = i;
                                multThreads->sudokuGrid = sudoku.sudokuGrid;
                                thread_boxes[i] = multThreads;
                                thread_count++;
                                // rowValidationRoutine((void *) thread_boxes[i]);
                                exit(rowValidation(thread_boxes[i]));
                            }
                    }
                }
            }

            // While loop that reaps children (27 children)
            while((p = waitpid(-1, &value, 0)) != -1){
                // printf( "%d: My child %d did exit(%d)\n", (int)getpid(), p, WEXITSTATUS(value));

                if(WEXITSTATUS(value) != 1) {
                    printf("The input is not a valid Sudoku. \n");
                    validSudokuGrid = false;
                    break;
                } 

            }

            if(validSudokuGrid) {
                printf("The input is a valid Sudoku. \n");
                validSudokuGrid = true;
            }

        }
    else {
        printf("We are using worker threads.\n");
        readSudoku(sudoku.sudokuGrid, stdin);
        printSudoku(sudoku_grid);

        for(int i = 0; i < 9; i++) {
            for(int j = 0; j < 9; j++) {
                if(i % 3 == 0 && j % 3 == 0) {
                    Sudoku* multThreads = (Sudoku*) malloc(sizeof(Sudoku));
                    multThreads->rows = i;
                    multThreads->cols = j;
                    multThreads->sudokuGrid = sudoku.sudokuGrid;
                    thread_boxes[thread_count] = multThreads;
                    pthread_create( &tid[thread_count], NULL, subgrid3x3ValidationRoutine, (void*) thread_boxes[thread_count]);
                    thread_count++;
                }
                if(i == 0) {
                    Sudoku* multThreads = (Sudoku*) malloc(sizeof(Sudoku));
                    multThreads->cols = j;
                    multThreads->sudokuGrid = sudoku.sudokuGrid;
                    thread_boxes[thread_count] = multThreads;
                    pthread_create( &tid[thread_count], NULL, columnValidationRoutine, (void*) thread_boxes[thread_count]);
                    thread_count++;
                }
                if(j == 0) {
                    Sudoku* multThreads = (Sudoku*) malloc(sizeof(Sudoku));
                    multThreads->rows = i;
                    multThreads->sudokuGrid = sudoku.sudokuGrid;
                    thread_boxes[thread_count] = multThreads;
                    pthread_create( &tid[thread_count], NULL, rowValidationRoutine, (void*) thread_boxes[thread_count]);
                    thread_count++;
                }
            }
        }

        // Joining and Terminating 27 threads
        for(int i = 0; i< 27; i++) {
            pthread_join(tid[i], NULL);
            // printf("Thread %10x joined\n", tid[i]);
            fflush(stdout);
        }
        for(int i = 0; i < numThreads; i++) {
            // printf("%d \n", thread_boxes[i]->res);
            if(thread_boxes[i]->res != 1) {
                printf("The input is not a valid Sudoku. \n");
                validSudokuGrid = false;
                break;
            } 
        }
        
        if(validSudokuGrid) {
            printf("The input is a valid Sudoku. \n");
        }

    }


    return 0;
}

void readSudoku(int (*sudokuGrid)[9], FILE *in)
{
    fseek(in, -1, SEEK_CUR); // Seek to start off the current position of the file ptr

    char character;
    int i, j, totalVals = 0;

    while ((fread(&character, 1, 1, in)) > 0 && totalVals < 81)
    { // Read 81 digits from stdin file
        if (character != '\n')
        { // Ignore newline
            if (isdigit(character))
            {
                ++totalVals;
                sudokuGrid[i][j] = character - '0'; // Store integer representation
                ++j;
                if (j == 9)
                {
                    j = 0;
                    ++i;
                }
            }
        }
    }
}

void printSudoku(int (*sudokuGrid)[9])
{
    int i = 0, j = 0; // i = rows, j = cols
    for (i = 0; i < 9; i++)
    {
        for (j = 0; j < 9; j++)
        {
            // if we are on the third or fifth number of sub-grid
            // we make a space between nums
            if (2 == j || 5 == j)
            {
                printf("%d   ", sudokuGrid[i][j]);
            }
            // if we are on the last num of row we make a space
            else if (8 == j)
            {
                printf("%d\n", sudokuGrid[i][j]);
            }
            // anything else we make a space
            else
            {
                printf("%d ", sudokuGrid[i][j]);
            }
        }
        // if we are on row 3 or row 5 we make a space
        if (2 == i || 5 == i)
        {
            printf("\n");
        }
    }
}

int columnValidation(Sudoku* sudoku) {
    // Confirming that params indicate a valid divison of the grid
    // parameters *params = (parameters*) param;  // params pointer to use member variables

    // Initializing rows and columns with our struct member variables
    int rows = sudoku->rows;
    int cols = sudoku->cols;

    // Base Case: if row is not zero to start out or column is greater than zero in the beginning output error
    if(rows != 0 || cols > 8) {
        fprintf(stderr, "Invalid row or column for the column subdivision: happened at row: %d, col: %d\n", rows + 1,cols + 1);
        return 0;
        pthread_exit(NULL);
    }

    // Check to see if there are any duplicates in each column
    int validationArr[9] = {0};

    // Traversing rows and checking if 1 - 9 appears once or value greater 9 or less than 1
    for(int i = 0; i < 9; i++) {
        // Stores integer representation at curr col
        int currVal = sudoku->sudokuGrid[i][cols];
        // printf("col: %d\n", i);
        // printf("currVal: %d\n", currVal);
        // printf("%d\n", currVal);
        // if the integer is less than zero or greater than or if the number has already appeared 
        // the worker thread will exit (terminate)
        if(currVal < 1 || currVal > 9 || validationArr[currVal - 1] == 1) {
            // printf("entered here!\n");
            fprintf(stderr, "Column: %d does not have required values (either a duplicate or value not in range of 1 - 9) \n", cols + 1);
            return 0;
            pthread_exit(NULL);
        }
        else {
            // printf("entered here hehe!\n");
            validationArr[currVal - 1] = 1;
        }
    }

    // If broken out of for loop then the col subdivision is valid.
    // validation[18 + cols] = 1;
    return 1;
    pthread_exit(NULL);
}

int rowValidation(Sudoku* sudoku) {
    // Confirming that params indicate a valid divison of the grid
    // parameters *params = (parameters*) param;  // params pointer to use member variables

    // Initializing rows and columns with our struct member variables
    int rows = sudoku->rows;
    int cols = sudoku->cols;

    // If column does not start at 1 we have an error of out of bounds or if row is greater than 9 
    if(cols != 0 || rows > 8) {
        fprintf(stderr, "Invalid row or column for the column subdivision: happened at row: %d, col: %d\n", rows + 1,cols + 1);
        return 0;
        pthread_exit(NULL);
    }

    // Check to see if there are any duplicates in each column
    int validationArr[9] = {0};

    // Traversing cols and checking if 1 - 9 appears once or value greater 9 or less than 1
    for(int i = 0; i < 9; i++) {
        // Stores integer representation at curr row
        int currVal = sudoku->sudokuGrid[rows][i];
        // printf("row %d\n", currVal);
        // if the integer is less than zero or greater than or if the number has already appeared 
        // the worker thread will exit (terminate)
        if(currVal < 1 || currVal > 9 || validationArr[currVal - 1] == 1) {
            fprintf(stderr, "Row: %d does not have required values (either a duplicate or value not in range of 1 - 9) \n", rows + 1);
            return 0;
            pthread_exit(NULL);
        }
        else {
            // printf("entered here hehe!\n");
            validationArr[currVal - 1] = 1;
        }
    }

    // If broken out of for loop then the col subdivision is valid.
    return 1;
    pthread_exit(NULL);
}

int subgrid3x3Validation(Sudoku* sudoku) {
    // Initializing rows and columns with our struct member variables
    int rows = sudoku->rows;
    int cols = sudoku->cols;

    if(rows > 6 || rows % 3 != 0 || cols > 6 || cols % 3 != 0) {
        fprintf(stderr, "Invalid row or column for subdivision: happened at row: %d, col: %d\n", rows + 1,cols + 1);
        return 0;
        pthread_exit(NULL);
    }
    // Check to see if there are any duplicates in each column
    int validationArr[9] = {0};

    for(int i = rows; i < rows + 3; i++) {
        for(int j = cols; j < cols + 3; j++) {
            // Stores integer representation at curr row
            int currVal = sudoku->sudokuGrid[i][j];
            // if the integer is less than zero or greater than or if the number has already appeared 
            // the worker thread will exit (terminate)
            if(validationArr[currVal - 1] == 0) {
                validationArr[currVal - 1] = 1;
            }
            else if((validationArr[currVal - 1] == 1) && (i >= 0 && i <=2)) {    // checks for dupes, segfaults if num > 9
                if( j >= 0 && j <= 2 ){
                    printf("The top left subgrid does not have the required values (either a duplicate or value not in range of 1 - 9)\n");
                    pthread_exit(NULL);
                    return 0;
                }
                else if ( j >= 3 && j <= 5 ){
                    printf("The top mid subgrid does not have the required values (either a duplicate or value not in range of 1 - 9)\n");
                    pthread_exit(NULL);
                    return 0;
                }
                else if ( j >= 6 && j <= 8 ){
                    printf("The top right subgrid does not have the required values (either a duplicate or value not in range of 1 - 9)\n");
                    pthread_exit(NULL);
                    return 0;
                }
            }
            else if((validationArr[currVal - 1] == 1) && (i >= 3 && i <=5) ) {    // checks for dupes, segfaults if num > 9
                if( j >= 0 && j <= 2 ){
                    printf("The left subgrid does not have the required values (either a duplicate or value not in range of 1 - 9)\n");
                    pthread_exit(NULL);
                    return 0;
                }
                else if ( j >= 3 && j <= 5 ){
                    printf("The mid subgrid does not have the required values (either a duplicate or value not in range of 1 - 9)\n");
                    pthread_exit(NULL);
                    return 0;
                }
                else if ( j >= 6 && j <= 8 ){
                    printf("The left right subgrid does not have the required values (either a duplicate or value not in range of 1 - 9)\n");
                    pthread_exit(NULL);
                    return 0;
                }
            }
            else if((validationArr[currVal - 1] == 1) && (i >= 6 && i <=8)) {    // checks for dupes, segfaults if num > 9
                if( j >= 0 && j <= 2 ){
                    printf("The bottom left subgrid does not have the required values (either a duplicate or value not in range of 1 - 9)\n");
                    pthread_exit(NULL);
                    return 0;
                }
                else if ( j >= 3 && j <= 5 ){
                    printf("The bottom mid subgrid does not have the required values (either a duplicate or value not in range of 1 - 9)\n");
                    pthread_exit(NULL);
                    return 0;
                }
                else if ( j >= 6 && j <= 8 ){
                    printf("The bottom right subgrid does not have the required values (either a duplicate or value not in range of 1 - 9)\n");
                    pthread_exit(NULL);
                    return 0;
                }
            }
        }
    }

    // If broken out of for loop then the subdivision is valid.
    return 1;
    pthread_exit(NULL);
}

void *columnValidationRoutine(void *param) {
    Sudoku* sudoku = (Sudoku*) param;
    sudoku->res = columnValidation(sudoku);
    return NULL;
}

void *rowValidationRoutine(void *param) {
    Sudoku* sudoku = (Sudoku*) param;
    sudoku->res = rowValidation(sudoku);
    return NULL;
}

void *subgrid3x3ValidationRoutine(void *param) {
    Sudoku* sudoku = (Sudoku*) param;
    sudoku->res = subgrid3x3Validation(sudoku);
    return NULL;
}