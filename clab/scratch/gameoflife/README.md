## Game of Life
Write a C program to simulate the [Conway's game of life](https://en.wikipedia.org/wiki/Conway%27s_Game_of_Life).

In a game of life of size n by n, the universe is two dimensional and consists of n by n cells. Each cell is in one of two possible states, "live"/"dead" (or "populated"/"unpopulated"). Every cell interacts with its eight neighbours, which are the cells that are horizontally, vertically, or diagonally adjacent. At each step in time, the following transitions occur:

Any live cell with fewer than two live neighbours dies, as if caused by under-population.
Any live cell with two or three live neighbours lives on to the next generation.
Any live cell with more than three live neighbours dies, as if by over-population.
Any dead cell with exactly three live neighbours becomes a live cell, as if by reproduction.
The initial pattern constitutes the seed of the system. The first generation is created by applying the above rules simultaneously to every cell in the seed; births and deaths occur simultaneously, and the discrete moment at which this happens is sometimes called a tick. The rules continue to be applied repeatedly to create further generations.

What about boundary conditions? We treat the borders of the 2D world as if they wrap around. In a universe of n x n cells, let's refer to the top-left cell as in position (0,0) and the bottom right cell as in position (n-1,n-1). If borders wrap around, then the 8 neighbors of (i,j) are:

* ((i+1)%n,j) right neighbor
* ((i-1)%n,j) left neighbor
* (i,(j+1)%n) top neighbor
* (i,(j-1)%n) bottom neighbor
* ((i+1)%n,(j+1)%n) topright neighbor
* ((i-1)%n,(j+1)%n) topleft neighbor
* ((i+1)%n,(j-1)%n) bottomright neighbor
* ((i-1)%n,(j-1)%n) bottomleft neighbor
Your executable file must be named gameoflife. It should take two arguments. The first argument is the name of a seed pattern file. The second argument is the number of ticks to run for the simulation. The seed pattern file contains one line per row of the universe. If the cell is "dead", its position is marked with the '.' character. If the cell is "live", the position is marked with the 'x' character. The seed file also effectively specifies the size of the universe to simulate.

For example, the contents of an example seed file `example_seed` are as follows:
```
$ cat example_seed
.....
.....
.xxx.
.....
.....
```
Then, the expected output of the command `./gameoflife example_seed 1` should be:

```
$ ./gameoflife example_seed 1
.....
..x..
..x..
..x..
.....
```

The expected output of the command `./gameoflife example_seed 2` should be:
```
$ ./gameoflife example_seed 2
.....
.....
.xxx.
.....
.....
```

Like in exercise 1 and 2, you should write a Makefile to compile your program and write your own unit tests to check the correctness. During grading, we'll test your program using seed files of our own choosing.

Once finished, add relevant source files and Makefile to your git repo by typing `git add *.c *.h Makefile`. Commit and push `git commit -am "gameoflife"; git push origin`
