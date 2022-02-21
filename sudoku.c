#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <ctype.h>
#include <getopt.h>

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

    // Building file opening and closing logic

    // If user enters less than 2 arguments it will give out an error
    if (argc < 2)
    {
        fprintf(stderr, "CLI Usage: filename [input-file-of-grid]\n");
        exit(EXIT_FAILURE);
    }

    // Initializing sudoku grid to parse file grid
    int sudoku_grid[9][9];
    int numGrids;

    // Opening File
    FILE *openFile = fopen(argv[1], "r");
    if (!openFile)
    {
        fprintf(stderr, "Could not open \"%s\" for reading! \n", argv[1]);
    } else {
        printf("\"%s\" opened!\n", argv[1]);
    }

    return 0;
}
