NAME: Cameron Schulz
ID: 1191172

INSTRUCTIONS:
Compile: make
Run program: ./a3 [<OPTIONS>] <datafile> [ <datafile> ... ]
Help: ./a3
Clean files: make clean

To create hashtable -- data-byname.txt, data-bynumber.txt, smalldata-mixed.txt
To query hashtable -- key.txt
To delete data from hashtable -- delete.txt

Example commands under -- commands.txt

RESOURCES:

CIS2520 Hash Table Slides

memcpy:
https://www.tutorialspoint.com/c_standard_library/c_function_memcpy.htm 

memcmp:
https://www.tutorialspoint.com/c_standard_library/c_function_memcmp.htm

General tutorial on hash tables in c:
https://www.youtube.com/watch?v=2Ti5yvumFTU&t=704s


STATE OF IMPLEMENTATION:
COMPLETE


SUMMARY OF NEW HASHING ALGORITHM --- hashByAscii --- OPTIONS: -H ascii
This hashing algorithm will get the last character and the first character in the key and convert it to its ASCII value. If the key is originally a string, the ASCII value of the first character subtracted from the last will be returned. If the key is originally an int, then the key will be returned. 

Must: This hash is able to locate any value given a matching key

Should: Uses all space in the table 

Ideally: This hash creates clustering -- about half as much as hashbyLength, but still a lot compared to hashBySum. The hash will create no clustering if the key is an int.