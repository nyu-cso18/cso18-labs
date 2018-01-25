## Find the ten most frequent words

Write a C program to find the ten most frequently used words in a given file. Your executable file must be named wordcount. It should take as argument the name of a file and outputs ten lines, one for each of the ten most frequently used words in the file in sorted order. If two words w1, w2 have the same occurance counts and w1 is alphabetically smaller than w2, then w1 precedes w2. Each line shows the occurance counts, followed by a whitespace, and then followed by the corresponding word.

For example, suppose the contents of the example.txt are given below:
```
$ cat example.txt
A potential victory for Donald J. Trump may hinge on one important (and large) 
group of Americans: whites who did not attend college.

Polls have shown a deep division between whites of different education levels and 
economic circumstances. A lot rides on how large these groups will be on Election Day: 
All pollsters have their own assessment of who will show up, and their predictions 
rely on these evaluations.
```

Then, the expected output of the `./wordcount example.txt` command should be:
```
$ ./wordcount example.txt
2 large
2 their
2 these
2 whites
2 who
2 will
3 a
3 and
3 of
4 on
```

In order to count the words, your program needs to transform the given textfile into a collection of words. First, you need to split the textfile into words. Each word refers to a consecutive sequence of alpha-numeric characters. Words are separated by one or more non-alphanumeric characters. (Hint: usage the C library function isalnum. For usage, type man isalnum) Second, you need to ``canonicalize`` (aka ``normalize'') each word by turning any uppercase letters into a lower case one.

To count the occurances of words, there are several strategies.

You can implement a hash table to store the mapping from each word (C strings) to its occurance counter. C's standard library does not have hash tables nor dictionary, so you'll have to implement your own. After you've counted all the words, you'll need to sort them by their occurance counters.
You can sort all words first. For sorting, you should learn to use the library function qsort (type man qsort to learn how to use it). You can then sum consecutive identical words in the sorted list to count them. Last, you need to sort words by their occurance counts.
Like in exercise 1, you should write a Makefile to compile your program and write your own unit tests to check the correctness. During grading, we'll test your program using text files of our own choosing.

Note that we will test your program using large text files. Your program must not run slower than O(n*log_n), otherwise, you will not be able to pass the test.

Once finished, add relevant source files and Makefile to your git repo by typing `git add *.c *.h Makefile`. Commit and push `git commit -am "complete wordcount"; git push origin`
