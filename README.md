Bugs:
1. Problem with  fgets() function: 
	A newline character is considered a valid character by it and included in the string copied to str. So I should convert "\n" to "\0".
2. String has no end:
	In the strcpy() function, we should copy "strlen() + 1" character into command to include '\o' at the end.
3. Segmentation fault:
	In the parse() function, we should put the str.push_back() before the strtok() function, for the last string would be NULL.

