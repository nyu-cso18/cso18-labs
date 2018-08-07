## Write a program to average columns in a CSV file

Write a C program to parse a `*.csv` file where fields are separated by the ';' character and print out the average of each column (which is an integer), separated by the ';' character.

Your executable file must be named avgcsv. It should take as argument the name of a csv file and outputs the average of each column, separated by ';'.
You should print out the average to the precision of 2 decimal points.

For example, suppose the contents of the example.csv are as follows:
```
$ cat example.csv   
10;25.5;56
22;10;100.4
```

Then, the expected output of the command `./avgcsv example.csv` should be:

```
$ ./avgcsv example.csv
16.00;17.75;78.20
```

We expect you to write a Makefile to generate the `avgcsv` executable file from your source code. Recitation 1 teaches you how to write a Makefile.

We do not provide you with any tests and will test your program under a few csv files of our own choosing during grading time. We highly encourage you to write unit tests for your program.

Once finished, add relevant source files and Makefile to your git repo by typing `git add *.c *.h Makefile`. Commit and push `git commit -am "finish avgcsv"; git push origin`


