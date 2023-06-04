/* 
 * CSCI 4460/6460 Large-Scale Programming and Testing
 * Homework 3 - The Regular Expressions Reboot
 * Author: Siwen Zhang zhangs20@rpi.edu 661682039
 * Last Modified: 12/14/2020
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#define MAX_LINE_LENGTH 256

// For Debugging
#ifdef DEBUG_MODE
printf( "the value of x is %d\n", x );
printf( "the value of q is %d\n", q );
printf( "why is my program crashing here?!\n" );
printf( "aaaaaaaaaaaaagggggggghhhh!\n" );
#endif


/*
 * This function is to match the classes of characters 
 * and also escaped literals after a backslash character
 * Input: "input" means the character of the input string 
 * and "pattern" means the special character in the regex string
 * Output: return 1(true) if there is a match found
 *         return 0(false) if there is no match found
 */
int checkSpecialMatch(int input, int pattern){
    if((pattern == 'd'  &&  isdigit(input)) || 
       (pattern == 'D'  && !isdigit(input)) || 
       (pattern == 'w'  &&  isalpha(input)) || 
       (pattern == 'W'  && !isalpha(input)) || 
       (pattern == 's'  &&  isspace(input)) || 
       (pattern == 'S'  && !isspace(input)) ||
       (pattern == '.'  &&  input == '.'  ) ||
       (pattern == '\\' &&  input == '\\' ) ||
       (pattern == '^'  &&  input == '^'  ) ||
       (pattern == '$'  &&  input == '$'  ) ||
       (pattern == '?'  &&  input == '?'  ) ||
       (pattern == '+'  &&  input == '+'  ) ||
       (pattern == '*'  &&  input == '*'  ) ||
       (pattern == '{'  &&  input == '{'  ) ||
       (pattern == '}'  &&  input == '}'  ) ||
       (pattern == '['  &&  input == '['  ) ||
       (pattern == ']'  &&  input == ']'  ) ||
       (pattern == '('  &&  input == '('  ) ||
       (pattern == ')'  &&  input == ')'  )){
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
int findStartPoint(char *string, char *pattern, int index){
    if(pattern[1] == '?'           ||
       pattern[1] == '*'           ||
       string[index] == pattern[0] ||
       pattern[0] == '.'           ||
       pattern[0] == '['           ||
       pattern[0] == '('           ||
       pattern[0] == '\\'){
        return 1;
    }else{
        return 0;
    }
}
/*
 * This function is deal with bracket expressions 
 * Any set of verbatim characters or regex characters 
 * can be combined within square brackets
 * to indicate that one character must be matched 
 * Input: input string, regex macthing pattern, 
 * index i in string and index j in regex pattern
 * index k means the start position of bracket expression
 * found is a flag to indicate the string matches the regex or not
 * Output: returns the found flag 
 *         return 1(true) if there is a match found
 *         return 0(false) if there is no match found
 */
int bracketMatch(char *string, char *pattern, int i, int j, int *k){
    int found = 0;
    for(*k = j-2; *k >=0; (*k)--){
        if(pattern[*k] == '[') break;
        if( (pattern[*k] == '\\') || (pattern[*k] == '^') ) continue;
        if(*k && pattern[(*k)-1] == '\\'){
            if(i && checkSpecialMatch(string[i-1],pattern[*k])) found = 1;
        }else{
            if(i && (pattern[*k] == '.' || pattern[*k] == string[i-1])) found = 1; 
        }
    }
    if(pattern[(*k)+1] == '^') found = 1 - found;
    return found;
}
/*
 * check if matching with the beginning or(and) the end of a line
 * Input: begin_regex is the flag to indicate if matching the beginning  
 * Output: returns the exact match flag 
 *         return 1(true) if there is a match found
 *         return 0(false) if there is no match found
 */
int checkExactMatch(int begin_regex, int end_regex, int begin_pos, int end_pos, int str_len){
    int match_status = 0;
    int begin_match = 0, end_match = 0;
    if(begin_regex){
        if(begin_pos == 0) begin_match = 1;
    }else{
        begin_match = 1;
    }
    if(end_regex){
        if((begin_pos + end_pos) == str_len) end_match = 1;
    }else{
        end_match = 1;
    }
    if(begin_match && end_match) match_status = 1;
    return match_status;
}
/* count the number of matching instances within a group */
void countGroupMatch(int string_end, int pattern_end, int pattern_start, int stringLen, int patternLen, int dp[stringLen+1][patternLen+1], int * group_count){
    for(int m = pattern_end; m > pattern_start; m--){
        for(int k = string_end; k >= 0; k--){
            if(dp[k][m] == 1){
                *group_count += 1;
                continue;
            }
        }
    }
}
/*
 * check if any substring of the input string
 * match the given regular expression pattern
 * Input: input string and matching pattern
 * Output: return 1(true) if there is a match found
 *         return 0(false) if there is no match found 
 */
int isMatch(char *string, char *pattern, char *** store_groups, int * captured_groups_count){
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
    int i; // index of input string
    int end_match = 0; // set a flag to indicate the matching ending point
    // for case {n,}
    int match_more = 0; // set a flag to indicate the exact matching status
    // for case {n,m}
    int match_least = 0; // set a flag to indicate the least matching status
    int match_count = 0; // count the matching number up to the most matching
    int repeat_count = 0; // count the number of repeated instances

    /* for capturing groups */
    int group_start_pos = 0; // find the start position of parenthese
    int group_end_pos = 0; // find the end position of parenthese
    int group_count = 0; // count the matching characters within parenthese
    int group_match_plus = 0; // record the status of greedy group matching
    // intialize a temporary 2D array to record matching groups 
    *store_groups = calloc(15, sizeof(char *));
    for( int i = 0; i < 15; i++){
        (*store_groups)[i] = calloc(MAX_LINE_LENGTH, sizeof(char));
    }
    int group_idx = 0; // count the number of identified matching groups
    /*
     * traverse the input string and pattern string
     * from index 0 to the end of the string 
     * update each step match status 
     */
    for(i = 0; i <= stringLen; i++){
        for(int j = 1; j <= patternLen; j++){ 
            /* repetition through '?'
             * '?' match zero or one instance
             */
            if( j && pattern[j-1] == '?'){
                // append with bracket matching
                if(pattern[j-2] == ']'){
                    // set k to record the bracket start position
                    int k;
                    for(k = j-2; k>= 0; k--){
                        if(pattern[k] == '[') break;
                    }
                    dp[i][j] = dp[i][k] + dp[i][j-1];
                // append with group matching 
                }else if(pattern[j-2] == ')'){
                    // set k to record the parenthese start position
                    int k;
                    for(k = j-2; k>= 0; k--){
                        if(pattern[k] == '(') break;
                    }
                    dp[i][j] = dp[i][j-1] || dp[i][k];
                }else{
                    if( j > 2  && pattern[j-3] == '\\'){
                        dp[i][j] = dp[i][j-3] + dp[i][j-1];
                    }else{
                        dp[i][j] = dp[i][j-2] + dp[i][j-1];
                    }
                }
            /* matching classes of characters
             * regex string contains '\' at index j-2
             * current match status dp[i][j] can be updated 
             * by the match status dp[i-1][j-2]
             * also need to check special match pattern
             * between string[i-1] and pattern[j-1]
             */
            }else if( j > 1 && pattern[j-2] == '\\'){
                dp[i][j] = i && dp[i-1][j-2] && checkSpecialMatch(string[i-1], pattern[j-1]);
            /* repetition through '*'
             * '*' match zero or more instances
             */
            }else if( j && pattern[j-1] == '*'){
                // append with bracket matching
                if(pattern[j-2] == ']'){
                    int k;
                    int found = bracketMatch(string, pattern, i, j, &k);
                    dp[i][j] = dp[i][k] || (i && found && dp[i-1][j]);
                }else if(pattern[j-2] == ')'){
                    // set k to record the parenthese start position
                    int k;
                    for(k = j-2; k>= 0; k--){
                        if(pattern[k] == '(') break;
                    }
                    dp[i][j] = dp[i][k];
                }else{
                    if(j > 2 && pattern[j-3] == '\\'){
                        dp[i][j] = dp[i][j-3] || ( i && checkSpecialMatch(string[i-1], pattern[j-2]) && dp[i-1][j]);
                    }else{
                        dp[i][j] = dp[i][j-2] || ( i && ( pattern[j-2] == '.' || pattern[j-2] == string[i-1]) && dp[i-1][j]);
                    }
                }
            /* repetition through '+'
             * '+' match one or more instances
             */
            }else if( j && pattern[j-1] == '+'){
                // append with bracket matching
                if(pattern[j-2] == ']'){
                    int k;
                    int found = bracketMatch(string, pattern, i, j, &k);
                    dp[i][j] = i && found && (dp[i-1][j] || dp[i-1][k]);
                }else{
                    if(j > 2 && pattern[j-3] == '\\'){
                        dp[i][j] =  i && checkSpecialMatch(string[i-1],pattern[j-2]) && (dp[i-1][j] || dp[i-1][j-3]);
                    }else{
                        dp[i][j] =  i && (pattern[j-2] == '.' || string[i-1] == pattern[j-2]) && (dp[i-1][j] || dp[i-1][j-2]);
                    }
                }
            /* repetition through exact matching pattern
             * {n} match exactly n instances,
             * where n is a positive integer
             */
            }else if( j > 3 && (pattern[j-1] == '}' && pattern[j-3] == '{' )){
                int n = atoi(&pattern[j-2]); // n is the number of exact matching instances
                int match_repeat = 1;
                if( j > 4 && pattern[j-5] == '\\'){
                    for(int k = 1; k <= n; k++){
                        if( !checkSpecialMatch(string[i-k], pattern[j-4])) match_repeat = 0;
                    }
                    dp[i][j] = (i >= n) && (match_repeat) && (dp[i-n][j-5]);
                }else{
                    for(int k = 1; k <= n; k++){
                        if(pattern[j-4] != '.' && (string[i-k] != pattern[j-4])) match_repeat = 0;
                    }
                    dp[i][j] = (i >= n) && (match_repeat) && (dp[i-n][j-4]);
                }
            /* repetition through exact matching pattern
             * {n,} match at least n instances,
             * where n is a positive integer
             */
            }else if(j > 4 && (pattern[j-1] == '}' && pattern[j-4] == '{' )){
                int n = atoi(&pattern[j-3]); // n is the number of least matching instances
                if(!match_more){
                    int match_repeat = 1;
                    if( j > 5 && pattern[j-6] == '\\'){
                        for(int k = 1; k <= n; k++){
                            if( !checkSpecialMatch(string[i-k], pattern[j-5])) match_repeat = 0;
                        }
                        if(match_repeat) match_more = 1;
                        dp[i][j] = (i >= n) && (match_repeat) && (dp[i-n][j-6]);
                    }else{
                        for(int k = 1; k <= n; k++){
                            if(pattern[j-5] != '.' && (string[i-k] != pattern[j-5])) match_repeat = 0;
                        }
                        if(match_repeat) match_more = 1;
                        dp[i][j] = (i >= n) && (match_repeat) && (dp[i-n][j-5]);
                    }
                // match_more == 1, find the position in string to exactly match the regex 
                }else{
                    if( j > 5 && pattern[j-6] == '\\'){
                        if(!checkSpecialMatch(string[i-1], pattern[j-5])) match_more = 0;
                        else{
                            dp[i][j] = i && dp[i-1][j];
                        }
                    }else{
                        if(pattern[j-5] != '.' && (string[i-1] != pattern[j-5])) match_more = 0;
                        else{
                            dp[i][j] = i && dp[i-1][j];
                        }
                    }
                } 
            /* repetition through exact matching pattern
             * {n,m} match at least n, at most m instances,
             * n < m
             * special case: n == 0
             */
            }else if(j > 5 && (pattern[j-1] == '}' && pattern[j-5] == '{' )){
                int n = atoi(&pattern[j-4]); // at least matching instances
                int m = atoi(&pattern[j-2]); // at most matching instances
                // special case, match at least 0 instance
                if( n == 0 ){
                    int match_find = 0;
                    if( j > 6 && pattern[j-7] == '\\'){
                        if(checkSpecialMatch(string[i-1], pattern[j-6])) {
                            match_count += 1;
                            match_find = 1;
                        }
                        dp[i][j] = dp[i][j-7] || (i && match_find && (match_count <= m) && dp[i-1][j]);

                    }else{
                        if(pattern[j-6] == '.' || pattern[j-6] == string[i-1]){
                            match_count += 1;
                            match_find = 1;
                        }
                        dp[i][j] = dp[i][j-6] || ( i && match_find && (match_count <= m) && dp[i-1][j]);
                    }
                // match at least n instances, which n is a positive integer
                }else{
                    if( j > 5 && pattern[j-6] == ')'){
                        int group_n = n*2;
                        if(!match_least){
                            int match_repeat = 1;
                            for(int k = 1; k <= group_n; k++){
                                if( !checkSpecialMatch(string[i-k],   pattern[j-7]) || 
                                    !checkSpecialMatch(string[i-k-1], pattern[j-9])  ) match_repeat = 0;
                                k++;
                            }
                            if(match_repeat){
                                match_least = 1;
                                match_count += n;
                            }
                            dp[i][j] = (i >= group_n) && match_repeat && dp[i-group_n][j-11];
                        }else{
                            // check every 2 instances each time
                            if(repeat_count){
                                if(!checkSpecialMatch(string[i-1], pattern[j-7]) || 
                                   !checkSpecialMatch(string[i-2], pattern[j-9])  ) match_least = 0;
                                else{
                                    match_count += 1;
                                    dp[i][j] = (match_count <= m) && dp[i-2][j];
                                }
                                repeat_count = 0;
                            }else{
                                repeat_count += 1;
                                match_least = 1;
                            }
                        }
                    }
                    // the matching instances haven't reached the least matching number 
                    if(!match_least){
                        int match_repeat = 1;
                        if( j > 6 && pattern[j-7] == '\\'){
                            for(int k = 1; k <= n; k++){
                                if( !checkSpecialMatch(string[i-k], pattern[j-6])) match_repeat = 0;
                            }
                            // the matching instances reached the least matching number
                            if(match_repeat) {
                                match_least = 1;
                                match_count += n;
                            }
                            dp[i][j] = (i >= n) && (match_repeat) && (dp[i-n][j-7]);
                        }else{
                            for(int k = 1; k <= n; k++){
                                if(pattern[j-6] != '.' && (string[i-k] != pattern[j-6])) match_repeat = 0;
                            }
                        
                            if(match_repeat) {
                                match_least = 1;
                                match_count += n;
                            } 
                            dp[i][j] = (i >= n) && (match_repeat) && (dp[i-n][j-6]);
                        }
                    // match_least == 1, find the position in string to at least match the regex 
                    }else{
                        if( j > 6 && pattern[j-7] == '\\'){
                            if(!checkSpecialMatch(string[i-1], pattern[j-6])) match_least = 0;
                            else{
                                // count match instances number
                                match_count += 1;
                                dp[i][j] = i && (match_count <= m) && dp[i-1][j];
                            }
                        }else{
                            if(pattern[j-6] != '.' && (string[i-1] != pattern[j-6])) match_least = 0;
                            else{
                                // count match instances number
                                match_count += 1; 
                                dp[i][j] = i && (match_count <= m) && dp[i-1][j];
                            }
                        }
                    } 
                }
            /* character set matching
             * support bracket expressions []
             * index k means the start position of the bracket expression
             * int "found" is a flag indicating if there is a match or not
             */
            }else if( j && pattern[j-1] == ']'){
                int k;
                int found = bracketMatch(string, pattern, i, j, &k);
                dp[i][j] = i && dp[i-1][k] && found;
            /* captured groups within a match 
             * using parentheses () to identify each group
             * identify the position of '('
             * record the start position of the group matching regex
             * skip the parenthese to match the string
             */
            }else if( j && pattern[j-1] == '('){
                dp[i][j] = dp[i][j-1];
                group_start_pos = j-1;
            /* capturing groups */ 
            }else if( j && pattern[j-1] == ')'){
                dp[i][j] = dp[i][j-1];
                // find the matching group in string
                if(dp[i][j] == 1 ){
                    // greedy matching as many as characters in groups
                    if(pattern[j-2] == '+'){
                        group_match_plus = 1;
                        group_end_pos = j-1;
                        if(string[i] == pattern[j]){
                            // within the parentheses, count the number of matched instances in string 
                            countGroupMatch(i-2, j-1, group_start_pos, stringLen, patternLen, dp, &group_count);
                            strncpy((*store_groups)[group_idx], string+(i-group_count), group_count);
                            group_idx++;
                            // reset the flag of finding the start position of matching groups as 0
                            group_start_pos = 0;
                            group_count = 0;

                        }
                    // for (\d{n}), the group matching count is the integer n
                    }else if(pattern[j-2] == '}'){
                        group_count = atoi(&pattern[j-3]);
                        strncpy((*store_groups)[group_idx], string+(i-group_count), group_count);
                        group_idx++;
                        // reset the flag of finding the start position of matching groups as 0
                        group_start_pos = 0;
                        group_count = 0; 
                    }else{
                        // within the parentheses, count the number of matched instances in string  
                        countGroupMatch(i-1, j-1, group_start_pos, stringLen, patternLen, dp, &group_count);
                        strncpy((*store_groups)[group_idx], string+(i-group_count), group_count);
                        group_idx++;
                        group_start_pos = 0;
                        group_count = 0; 
                    } 
                // find the last matching instance in the groups           
                }else if(j == group_end_pos +1 && group_match_plus ){
                    group_match_plus = 0; // reset the greedy matching flag
                    countGroupMatch(i-2, j-1, group_start_pos, stringLen, patternLen, dp, &group_count);   
                    strncpy((*store_groups)[group_idx], string+(i-group_count), group_count-1);
                    group_idx++;
                    // reset the flag of finding the start position of matching groups as 0
                    group_start_pos = 0;
                    group_count = 0; 
                }
            /* exact matching pattern
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
         * function isMatch() return true
         * record the last matching instances using greedy approach
         */
        if(!end_match && dp[i][patternLen] == 1){
            end_match = 1;
        }else if(end_match && dp[i][patternLen] == 0){
            *captured_groups_count = group_idx;
            return i-1;
        }
    }
    // find the last position in the string matching the regex 
    if(end_match == 1) {
        *captured_groups_count = group_idx;
        return i-1;
    }
    // can not find matched substring for the given string and regex
    return 0;
}
/*
 * apply regex to each line of the file specified by filename
 * store all matching lines in the dynamically allocated array matches
 * Input: filename, regex pattern, dynamically allocated 2D array, trim flag
 * trim to set is a flag to determine if each matched line need be trimmed or not
 * Output: return the number of matched lines (zero or more)
 *         or -1 if an error occurred
 */
int regex_match( const char * filename, const char * regex, char *** matches, int trim_to_match, 
                 char *** groups, int * captured_groups){
    
    /* check exact and positional matching */
    int begin_regex = 0; // indicate if has '^' in regex 
    int end_regex = 0; // indicate if has '$' in regex
    int len_regex = strlen(regex); // regex length
    char regex_copy[256]; // store the regex copy without ^ and $ 
    /* copy the const regex string to a temporary string */
    memset(regex_copy, '\0', sizeof(regex_copy));
    strcpy(regex_copy, regex);
    if(regex[0] == '^') {
        begin_regex = 1; // need to match the beginning of the line
        strcpy(regex_copy, regex_copy+1);
    }
    if(regex[len_regex-1] == '$') {
        end_regex = 1; // need to match the end of the line
        strtok(regex_copy, "$");
    }
    // set a temporary 2D array to store the matched groups
    char ** store_groups = NULL;
    // count the number of captured groups
    int captured_groups_count = 0;
    
    FILE * inputfile = fopen(filename, "r");

    if( inputfile == NULL){
        perror("fopen() failed");
        return EXIT_FAILURE;
    }
    // create an char array to save each readable line
    char string[MAX_LINE_LENGTH] = "";
    // count the number of matched lines
    int count = 0;
    // initialize the number of matched lines
    int table_length = 32;
    
    /* Initial memory allocation */
    // dynamically allocate  the 2D array char ** matches
    // matches stores all matching lines in the dynamically allocated space
    *matches = calloc(table_length, sizeof(char *));
    for( int i = 0; i < table_length; i++){
        (*matches)[i] = calloc(MAX_LINE_LENGTH, sizeof(char));
    }

    // read one line each time from the input file
    while(fgets(string, MAX_LINE_LENGTH, inputfile) != NULL){
        if(strcmp(string, "\n") == 0) continue;
        // trim the "\n" of the end of string 
        strtok(string, "\n");
        // resize the memory block of 2D matches array
        if(count == table_length){
            *matches = realloc(*matches, 1.2*table_length*sizeof(char*));
            // update the 2D array size
            table_length = 1.2*table_length;
            for(int i = count; i < table_length; i++){
                (*matches)[i] = calloc(MAX_LINE_LENGTH, sizeof(char));
            }
        }
        /* traverse the current input string  
         * find the first possible matching point
         * start to apply isMatch function 
         */
        int index = 0; // initialize the first matching position 
        while(string[index]){
            if(findStartPoint(string, regex_copy, index)){
                int end_pos = isMatch(string+index, regex_copy, &store_groups, &captured_groups_count);
                // the current string matches the given regex
                if(end_pos){
                    // no matching the beginning or the end characters in the line
                    if(!begin_regex && !end_regex){
                        // no trimming flag set
                        if(!trim_to_match){
                            // if matches, store the line in the matches array
                            strcpy((*matches)[count], string);
                            // increase the matched line number
                            count++; 
                            // clear 2D array groups   
                            *groups = calloc(15, sizeof(char *));
                            for( int i = 0; i < 15; i++){
                                (*groups)[i] = calloc(MAX_LINE_LENGTH, sizeof(char));
                            }
                            *captured_groups = captured_groups_count;
                            if( *captured_groups > 0 ){
                                for( int i = 0; i < *captured_groups; i++){
                                    strcpy((*groups)[i+1], store_groups[i]);
                                }
                            }
                            break;
                        /*
                         * set a trimming flag
                         * need to trim the matched line to 
                         * show only the matching substring
                         */
                        }else{
                            //int end_pos = isMatch(string+index, regex_copy);
                            strncpy((*matches)[count], string+index, end_pos);
                            count++;
                            // clear 2D array groups   
                            *groups = calloc(15, sizeof(char *));
                            for( int i = 0; i < 15; i++){
                                (*groups)[i] = calloc(MAX_LINE_LENGTH, sizeof(char));
                            }
                            *captured_groups = captured_groups_count;
                            if( *captured_groups > 0 ){
                                for( int i = 0; i < *captured_groups; i++){
                                    strcpy((*groups)[i+1], store_groups[i]);
                                }
                            }
                            break;
                        }
                    // need to match the beginning or/and the end of the string
                    }else{
                        int str_len = strlen(string);
                        // current string matched the given regex
                        if(checkExactMatch(begin_regex, end_regex, index, end_pos, str_len)){
                            // no trimming flag set
                            if(!trim_to_match){
                                // if matches, store the line in the matches array
                                strcpy((*matches)[count], string);
                                // increase the matched line number
                                count++;
                                // clear 2D array groups   
                                *groups = calloc(15, sizeof(char *));
                                for( int i = 0; i < 15; i++){
                                    (*groups)[i] = calloc(MAX_LINE_LENGTH, sizeof(char));
                                }
                                *captured_groups = captured_groups_count;
                                if( *captured_groups > 0 ){
                                    for( int i = 0; i < *captured_groups; i++){
                                        strcpy((*groups)[i+1], store_groups[i]);
                                    }
                                }
                                break;
                            /*
                             * set a trimming flag
                             * need to trim the matched line to 
                             * show only the matching substring
                             */
                            }else{
                                strncpy((*matches)[count], string+index, end_pos);
                                count++;
                                // clear 2D array groups   
                                *groups = calloc(15, sizeof(char *));
                                for( int i = 0; i < 15; i++){
                                    (*groups)[i] = calloc(MAX_LINE_LENGTH, sizeof(char));
                                }
                                *captured_groups = captured_groups_count;
                                if( *captured_groups > 0 ){
                                    for( int i = 0; i < *captured_groups; i++){
                                        strcpy((*groups)[i+1], store_groups[i]);
                                    }
                                }
                                break;
                            }
                        }
                    } 
                }
            }
            index++; // continue to find the next possible matching position
        } 
    }
    return count; // return the number of lines tat matched (zero or more)
}

#ifdef USE_MY_MAIN_FUNCTION
int main( int argc, char * argv[] )
{
  if ( argc != 3 && argc != 4 )
  {
    fprintf( stderr, "ERROR: Invalid arguments\n" );
    fprintf( stderr, "USAGE: %s <regex-file> <input-file> [<trim-to-match>]\n", argv[0] );
    return EXIT_FAILURE;
  }

  FILE * regexfile = fopen( argv[1], "r" );

  if ( regexfile == NULL )
  {
    perror( "fopen() failed" );
    return EXIT_FAILURE;
  }

  char * regex = NULL;

  if ( fscanf( regexfile, "%m[^\n]", &regex ) != 1 )
  {
    free( regex );
    fprintf( stderr, "ERROR: regex file is incorrect\n" );
    return EXIT_FAILURE;
  }

  fclose( regexfile );

  /* call regex_match() to find matches of a given regex   */
  /*  in a given file (argv[2]); lines must be dynamically */
  /*   allocated in the regex_match() function call        */
  char ** lines = NULL;
  int trim_to_match = 0;
  char ** groups = NULL;
  int captured_groups = 0;
  if ( argc == 4 ) trim_to_match = ( atoi( argv[3] ) ? 1 : 0 );
  int matched_lines = regex_match( argv[2], regex, &lines, trim_to_match, &groups, &captured_groups);

  free( regex );

  int i;

  printf( "MATCHED LINE%s (%d):\n", ( matched_lines == 1 ? "" : "S" ), matched_lines );

  /* display resulting matched lines */
  for ( i = 0 ; i < matched_lines ; i++ )
  {
    printf( "%s\n", lines[i] );
  }

   /* display matched groups on last match, if applicable */
  if ( captured_groups > 0 )
  {
    printf( "\nCAPTURED GROUP%s:", ( captured_groups == 1 ? "" : "S" ) );
    for ( i = 1 ; i <= captured_groups ; i++ )
    {
      printf( " [%s]", groups[i] );
    }
    printf( "\n" );

    /* deallocate dynamically allocated memory */
    for ( i = 1 ; i <= captured_groups ; i++ )
    {
      free( groups[i] );
    }

    free( groups );
  }

  /* deallocate dynamically allocated memory */
  for ( i = 0 ; i < matched_lines ; i++ )
  {
    free( lines[i] );
  }

  free( lines );

  return EXIT_SUCCESS;
}
#endif