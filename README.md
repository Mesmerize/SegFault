# sudoku-project

Starter code for the sudoku project.

    $ make sudoku.x
    gcc -o sudoku.x -g -lpthread sudoku.c
    $ ./sudoku.x < someInputFile

    Steps to accomplish the project:
    - Learn to parse sudoko grid file
    - Check to see if the grids are valid sudoko grids due to certain criterias
    - Keeps track of processes/threads
    - Check each of the sudoko properties with a different thread for pt1
    - Check each of the sudoko properties with a different process for pt2


How to Run the Program:

    - Compile with gcc -v sudoku.c
    - then run ./a.out tests/[filename of grid]

Tasks (After Meeting):

    - Phi: Printing of grid appropriately
    - Erika: Validation checking of rows and columns 
    - Brandon: Initialize Parsing of stdin grid + threads
