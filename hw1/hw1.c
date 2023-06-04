/* 
 * CSCI 4460/6460 Large-Scale Programming and Testing
 * Homework 1 - Regular Expressions and Pattern Matching in C
 * Author: Siwen Zhang zhangs20@rpi.edu 661682039
 * Last Modified: 10/13/2020
 * The program is to implement a rudimentary grep tool
 * that supports line-based regular expressions 
 * and simple pattern matching.
 * Reference[1]: shorturl.at/rKUX1
 * Reference[2]: shorturl.at/jrKNW
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#define MAX_LINE_LENGTH 257

// For Debugging
#ifdef DEBUG_MODE
printf( "the value of x is %d\n", x );
printf( "the value of q is %d\n", q );
printf( "why is my program crashing here?!\n" );
printf( "aaaaaaaaaaaaagggggggghhhh!\n" );
#endif

/*
 * This function is to match the special characters after '\\'
 * with the input string characters. 
 * Input: "input" means the character of the input string 
 * and "pattern" means the special character in the regex string
 * Output: return 1(true) if there is a match found
 *         return 0(false) if there is no match found
 */
int CheckSpecialMatch(int input, int pattern){
    if((pattern == 'd' &&  isdigit(input)) || 
       (pattern == 'D' && !isdigit(input)) || 
       (pattern == 'w' &&  isalpha(input)) || 
       (pattern == 'W' && !isalpha(input)) || 
       (pattern == 's' &&  isspace(input)) || 
       (pattern == '\\' && input == '\\')){
        return 1;
    }else{
        return 0;
    }
}
/*
 * This function is to find the character string[i] in the input string
 * match with the start point for the given regular expression string
 * then set the dp[i][0] as true(1) 
 * in order to start to check if the following substring 
 * starting from string[i] match with the entire regex pattern string
 * Input: input string, regular expression string, index
 * Output: return 1(true) if there is a matching start point found
 *         return 0(false) if there is no matching start point found
*/
int FindStartPoint(char *string, char *pattern, int index){
    if(pattern[1] == '?'           ||
       string[index] == pattern[0] ||
       pattern[0] == '.'           ||
       pattern[0] == '\\'){
        return 1;
    }else{
        return 0;
    }
}
/*
 * check if any substring of the input string
 * match the given regular expression pattern
 * Input: input string and matching pattern
 * Output: return 1(true) if there is a match found
 *         return 0(false) if there is no match found 
 */
int IsMatch(char *string, char *pattern){
    // define the length of the input string and regex pattern string
    int stringLen = strlen(string), patternLen = strlen(pattern);
    /*
     * define a 2D table to simulate the matching process 
     * using dynamic programming algorithm
     * define dp[i][j] to be true(1) if string[0..i) matches with pattern[0..j)
     * and false(0) otherwise
     * initialize all elements in the 2D table with false(0) 
     */
    int dp[stringLen+1][patternLen+1];
    for(int i = 0; i <= stringLen; i++){
        for(int j = 0; j <= patternLen; j++){
            dp[i][j] = 0;
        }
    }
    // dp[0][0] means the match status of two empty strings
    // dp[0][0] should always be set as true(1) 
    dp[0][0] = 1;
    /*
     * traverse the input string and pattern string
     * from index 0 to the end of the string 
     * update each step match status 
     */
    for(int i = 0; i <= stringLen; i++){
        for(int j = 1; j <= patternLen; j++){
            // find matching start point
            // initialize the start point as true(1)
            if( FindStartPoint(string, pattern, i) ){
                dp[i][0] = 1;
            }
            /* #1 special case:
             * regex string contains '?' at index j-1 
             * if the '?' matches 0 instance
             * then current match status dp[i][j] can be updated 
             * by the match status dp[i][j-2]
             * if the '?' matches 1 instance
             * then current match status dp[i][j] can be updated 
             * by the match status dp[i][j-1]
             */
            if(j > 1 && pattern[j-1] == '?'){
                dp[i][j] = dp[i][j-1] + dp[i][j-2];
            /*
             * #2 special case:
             * regex string contains '\' at index j-2
             * current match status dp[i][j] can be updated 
             * by the match status dp[i-1][j-2]
             * also need to check special match pattern
             * between string[i-1] and pattern[j-1]
             */
            }else if( j > 1 && pattern[j-2] == '\\'){
                dp[i][j] = i && dp[i-1][j-2] && CheckSpecialMatch(string[i-1], pattern[j-1]);
            /*
             * #3 special case:
             * regex string contains '*' at index j-1
             * then there are two situations to be considered
             * if index j-3 is '\'
             * and if '*' matches 0 instance
             * then current match status dp[i][j] can be updated
             * by the match status dp[i][j-3]
             * if '*' matches more instances
             * then current match status dp[i][j] can be updated
             * by the match status dp[i-1][j], also need to check
             * special match pattern between string[i-1] and pattern[j-2]
             * else if index j-3 is NOT '\'
             * and if '*' matches 0 instances
             * then current match status dp[i][j] can be updated
             * by the match status dp[i][j-2]
             * if '*' matches more instances
             * then current match status dp[i][j] can be updated
             * by the match status dp[i-1][j] and also need to check 
             * if the character at string[i-1] matches with 
             * the character at pattern[j-2] or is '.' at pattern[j-2]
             */
            }else if( j > 1 && pattern[j-1] == '*'){
                if(j > 2 && pattern[j-3] == '\\'){
                    dp[i][j] = dp[i][j-3] || ( i && CheckSpecialMatch(string[i-1], pattern[j-2]) && dp[i-1][j]);
                }else{
                    dp[i][j] = dp[i][j-2] || ( i && ( pattern[j-2] == '.' || pattern[j-2] == string[i-1]) && dp[i-1][j]);
                }
           /*
             * #4 special case:
             * regex string contains '+' at index j-1
             * then there also be two situations to be considered
             * if index j-3 is '\'
             * since '+' can only match one or more instances
             * current match status dp[i][j] can be updated
             * by the match status dp[i-1][j] or dp[i-1][j-3]
             * also need to check special match pattern 
             * between string[i-1] and pattern[j-2]
             * else if index j-3 is NOT '\'
             * then current match status dp[i][j] can be updated
             * by the match status dp[i-1][j] or dp[i-1][j-2] 
             * and check if the character at string[i-1] matches with 
             * the character at pattern[j-2] or is '.' at pattern[j-2]
             */
            }else if( j > 1 && pattern[j-1] == '+'){
                if(j > 2 && pattern[j-3] == '\\'){
                    dp[i][j] =  i && CheckSpecialMatch(string[i-1],pattern[j-2]) && (dp[i-1][j] || dp[i-1][j-3]);
                }else{
                    dp[i][j] =  i && (pattern[j-2] == '.' || string[i-1] == pattern[j-2]) && (dp[i-1][j] || dp[i-1][j-2]);
                }
            /*
             * #5 case
             * this is an exact matching pattern
             * if there is no special character in the regex string
             * the current match status dp[i][j] can be updated 
             * by the match status dp[i-1][j-1] 
             * and also check if the character string[i-1] 
             * matches with character pattern[j-1]
             * or pattern[j-1] is '.' which can match any character
             */ 
            }else{
                dp[i][j] = i && dp[i-1][j-1] && ( string[i-1] == pattern[j-1] || pattern[j-1] == '.');
            }
        }
        /*
         * dp[i][patternLen] means string[0..i) matches with pattern[0..patternLen) 
         * there exists a substring in the input string matching with the entire regex string
         * function IsMatch() return true
         */
        if(dp[i][patternLen] == 1) return 1;
    }
    return 0;
}

int main(int argc, char *argv[])
{
    // ensure buffered output
    setvbuf(stdout, NULL, _IONBF, 0);
    /* 
     * check if command-line arguments are given properly
     * there are 2 expected command-line arguments:
     * bash$ ./a.out regex.txt hw1-input01.txt
     * for errors involving, report an error message 
     * to stderr and abort further program execution
     */
    if (argc != 3){
        fprintf( stderr, "ERROR: Invalid arguments\n");
        fprintf( stderr, "USAGE: a.out <regex-file> <input-file>\n");
        return -1;
    }
    // declare file pointers to the regex file and input file
    FILE *fpRegex, *fpInput;
    // define the pointer to an array of chars 
    // where the string read from the file is stored
    char pattern[MAX_LINE_LENGTH];
    char string[MAX_LINE_LENGTH];
    // open the regex file for reading
    fpRegex = fopen(argv[1], "r");
    /*
     * check if the regex file can be opened
     * if the file does not exist
     * report error message and 
     * abort further program execution
     */
    if (fpRegex == NULL){
        fprintf(stderr, "ERROR: Read regex file failed\n");
        return EXIT_FAILURE;
    }
    /* 
     * use fegts to read the line from the FILE stream fpRegex
     * if the End-of-File is encountered and no characters have been read
     * then report error message then aborting further program execution
     * if fgets succeed
     * pattern points to the char array where the string read is stored
     */
    if (fgets(pattern, 257, fpRegex) == NULL){
        fprintf(stderr, "ERROR: fgets() failed\n");
        return EXIT_FAILURE;
    }
    // only take the regular expression before "\n" to match the input strings
    strtok(pattern, "\n");
    /*
     * check if the input file can be opened
     * if the file does not exist
     * report error message and 
     * abort further program execution
     */
    fpInput = fopen(argv[2], "r");
    if (fpInput == NULL){
        fprintf(stderr, "ERROR: Read input file failed\n");
        return EXIT_FAILURE;
    }
    /*
     * get each readable line from the input file
     * use IsMatch() function to check
     * if the current line contains any substring 
     * match the given regular expression 
     */
    while(fgets(string, 257, fpInput) != NULL)
    {
        if(IsMatch(string, pattern)){
            printf("%s", string);
        }
    }
    // after reading files completed
    // close the file stream 
    fclose(fpRegex);
    fclose(fpInput);
    return EXIT_SUCCESS;
}
