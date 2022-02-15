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

