/* 
 * CSCI 4460/6460 Large-Scale Programming and Testing
 * Homework 2 - Text Analysis (and more Regular Expressions) in C
 * Author: Siwen Zhang zhangs20@rpi.edu 661682039
 * Last Modified: 11/23/2020
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#define MAX_LINE_LENGTH 1025

// For Debugging
#ifdef DEBUG_MODE
printf( "the value of x is %d\n", x );
printf( "the value of q is %d\n", q );
printf( "why is my program crashing here?!\n" );
printf( "aaaaaaaaaaaaagggggggghhhh!\n" );
#endif


/* Text Analysis Part */
// global variable
// set flag for script tag
int scriptTag = 0;
// set flag for HTML tag
int HTMLtag = 0;

// remove special characters
void strRemove(char *str, const char *sub){
    int len = strlen(sub);
    if(len > 0){
        char *p = str;
        char *s = " ";
        while ((p = strstr(p, sub)) != NULL){
            memmove(p, s, 1);
            memmove(p+1, p+len, strlen(p+len)+1);
        }
    }
}

// remove all scripts
void scriptRemove(char *str){
    // if current text is within <script> tag
    if(scriptTag == 1){
        char *startpos = str;
        char *endScript = str;
        // <script> tag ends
        if((endScript = strstr(str, "</script>")) != NULL){
            // update <script> tag
            scriptTag = 0;
            memmove(startpos, endScript+9, strlen(endScript+9)+1);
        // current string still within <script> tag
        }else{
            // remove entire string
            strcpy(str, "");
        }
    }else{
    // if current text is not within <script> tag
        char *startScript = str;
        char *endScript = str;
        // <script> tag begins
        if((startScript = strstr(str, "<script")) != NULL){
            // update script tag
            scriptTag = 1;
            // <script> tag ends
            if((endScript = strstr(str, "</script>")) != NULL){
                // update script tag
                scriptTag = 0;
                char *s = " ";
                memmove(startScript, s, 1);
                memmove(startScript+1, endScript+9, strlen(endScript+9)+1);
            // <script> tag does not end
            }else{
                strcpy(startScript, "");
            }
        }
    }
}

// to remove all HTML tags
void tagRemove(char *str){
    char tmp[MAX_LINE_LENGTH] = "";
    for(int i = 0; str[i]; i++){
        if(HTMLtag){
            if(str[i] == '>'){
                // HTML tag ends
                // update HTML tag
                HTMLtag = 0;
            }  
        }else{
            if(str[i] == '<'){
                // HTML tag starts
                // update HTML tag
                HTMLtag = 1;
                // add a space to split words
                strncat(tmp, " ", 1);
            }else{
                // copy valid characters one by one 
                strncat(tmp, &str[i], 1); 
            }
        }
        strncat(tmp, "\0", 1);
    }
    strcpy(str, tmp);

}

// convert everything to lowercase
void toLower(char *str){
    for(int i = 0; str[i]; i++){
        str[i] = tolower(str[i]);
    }
}
// isolate all "words" by grouping every set of a-z and '
void wordSplit(char *str){
    char tmp[MAX_LINE_LENGTH] = "";
    for(int i = 0; str[i]; i++){
        if(isalpha(str[i]) || (str[i] == '\'')){
            strncat(tmp, &str[i], 1);
        }else if(str[i] == '\n'){
            strncat(tmp, "\n", 1);
        }else{
            strncat(tmp, " ", 1);
        }
    }
    strcpy(str, tmp);
}

// remove leading and trailing ' characters
void quoteRemove(char *str){
    int len = strlen(str);
    char tmp[MAX_LINE_LENGTH] = "";
    for(int i = 0; str[i]; i++){
        if((i == 0 || i == (len-1)) && str[i] == '\''){
            strncat(tmp, "", 1);
        }else if( (i > 0 && i < (len-1)) && (str[i] == '\'') && (!isalpha(str[i-1]) || !isalpha(str[i+1]))){
            strncat(tmp, " ", 1);
        }else{
            strncat(tmp, &str[i], 1);
        }
    }
    strcpy(str, tmp);
}

// preprocess input documents
void prepInput(char *str){

    // remove special characters 
    strRemove(str, "&nbsp;");
    strRemove(str, "&quot;");
    strRemove(str, "&amp;");
    strRemove(str, "&lt;");
    strRemove(str, "&gt;");
    
    // remove <script>..</script>
    scriptRemove(str);

    // remove all HTML tags
    tagRemove(str);
    
    // convert everything to lowercase
    toLower(str);

    // isolate all "words"
    wordSplit(str);

    // remove leading and trailing ' 
    quoteRemove(str);
}

// get single word from input files
int getWords(){
    // count the number of words
    int wordNum = 0;
    FILE *f1, *f2;
    f1= fopen("validWords.txt", "r");
    f2 = fopen("wordsList.txt", "w");
    // use char array "word" to save words
    char word[MAX_LINE_LENGTH];
    // read words from input stream
    // printf words into a file
    while(fscanf(f1, "%s", word) == 1){
        if(strlen(word) > 1){
            wordNum++;
            fprintf(f2, "%s\n", word);
        }
    }
    fclose(f1);
    fclose(f2);
    return wordNum;
}
// get bigrams from input files
int getBigram(const char *stopWords){
    // count the number of Bigrams
    int biGramNum = 0;
    // create a file point f declare the file saving bigrams
    FILE *f1, *f2;
    f1 = fopen("wordsList.txt", "r");
    f2 = fopen("bigram.txt", "w");

    char word[MAX_LINE_LENGTH];

    // store bigram words
    char tmp[MAX_LINE_LENGTH] = "";
    // count word number to identify bigrams
    int count = 0;

    while(fscanf(f1, "%s", word) == 1){
        if(strstr(stopWords, word) == NULL && count == 0) {
            strcat(tmp, word);
            strcat(tmp, " ");
            count += 1;
        }else if(strstr(stopWords, word) == NULL && count == 1){
            strcat(tmp, word);
            strcat(tmp, "\n");
            biGramNum++;
            fputs(tmp, f2);
            strcpy(tmp, "");
            strcat(tmp, word);
            strcat(tmp, " ");
            count = 1;
        }else if(strstr(stopWords, word) != NULL){
            count = 0;
            strcpy(tmp, "");
        }
    }
    fclose(f1);
    fclose(f2);
    return biGramNum;
}

/* Regular Expression Part */

/*
 * This function is to match the special characters after '\\'
 * with the input string characters. 
 * Input: "input" means the character of the input string 
 * and "pattern" means the special character in the regex string
 * Output: return 1(true) if there is a match found
 *         return 0(false) if there is no match found
 */
int checkSpecialMatch(int input, int pattern){
    if((pattern == 'd' &&  isdigit(input)) || 
       (pattern == 'D' && !isdigit(input)) || 
       (pattern == 'w' &&  isalpha(input)) || 
       (pattern == 'W' && !isalpha(input)) || 
       (pattern == 's' &&  isspace(input)) || 
       (pattern == '.' &&  input == '.' )  ||
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
int findStartPoint(char *string, const char *pattern, int index){
    if(pattern[1] == '?'           ||
       string[index] == pattern[0] ||
       pattern[0] == '.'           ||
       pattern[0] == '['           ||
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
int bracketMatch(char *string, const char *pattern, int i, int j, int *k){
    int found = 0;
    for(*k = j-2; *k >=0; (*k)--){
        if(pattern[*k] == '[') break;
        if(*k && pattern[*k-1] == '\\'){
            if(i && checkSpecialMatch(string[i-1],pattern[*k])) found = 1;
        }else{
            if(i && (pattern[*k] == '.' || pattern[*k] == string[i-1])) found = 1; 
        }
    }
    if(pattern[*k+1] == '^') found = 1 - found;
    return found;
}
/*
 * check if any substring of the input string
 * match the given regular expression pattern
 * Input: input string and matching pattern
 * Output: return 1(true) if there is a match found
 *         return 0(false) if there is no match found 
 */
int isMatch(char *string, const char *pattern){
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
            if( findStartPoint(string, pattern, i) ){
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
                if(pattern[j-2] == ']'){
                    int k;
                    for(k = j-2; k>= 0; k--){
                        if(pattern[k] == '[') break;
                    }
                    dp[i][j] = dp[i][k] + dp[i][j-1];
                }else{
                    dp[i][j] = dp[i][j-2] + dp[i][j-1];
                }
                
            /*
             * #2 special case:
             * regex string contains '\' at index j-2
             * current match status dp[i][j] can be updated 
             * by the match status dp[i-1][j-2]
             * also need to check special match pattern
             * between string[i-1] and pattern[j-1]
             */
            }else if( j > 1 && pattern[j-2] == '\\'){
                dp[i][j] = i && dp[i-1][j-2] && checkSpecialMatch(string[i-1], pattern[j-1]);
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
                if(pattern[j-2] == ']'){
                    int k;
                    int found = bracketMatch(string, pattern, i, j, &k);
                    dp[i][j] = dp[i][k] || (i && found && dp[i-1][j]);
                }else{
                    if(j > 2 && pattern[j-3] == '\\'){
                        dp[i][j] = dp[i][j-3] || ( i && checkSpecialMatch(string[i-1], pattern[j-2]) && dp[i-1][j]);
                    }else{
                        dp[i][j] = dp[i][j-2] || ( i && ( pattern[j-2] == '.' || pattern[j-2] == string[i-1]) && dp[i-1][j]);
                    }
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
            /*
             * #5 special case
             * support bracket expressions
             * index k means the start position of the bracket expression
             * flag found means if there is a match or not
             */
            }else if( j > 1 && pattern[j-1] == ']'){
                int k;
                int found = bracketMatch(string, pattern, i, j, &k);
                dp[i][j] = i && dp[i-1][k] && found;
            }else{
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
                dp[i][j] = i && dp[i-1][j-1] && ( string[i-1] == pattern[j-1] || pattern[j-1] == '.');
            }
            
        }
        /*
         * dp[i][patternLen] means string[0..i) matches with pattern[0..patternLen) 
         * there exists a substring in the input string matching with the entire regex string
         * function isMatch() return true
         */
        if(dp[i][patternLen] == 1){
            return 1;
        }
    }
    // can not find matched substring for the given string and regex
    return 0;
}

int isMatchTrim(char *string, const char *pattern, int *startpos, int *endpos){
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
    
    int start_flag = 0, match_flag = 0;
    /*
     * traverse the input string and pattern string
     * from index 0 to the end of the string 
     * update each step match status 
     */
    for(int i = 0; i <= stringLen; i++){
        for(int j = 1; j <= patternLen; j++){
            // find matching start point
            // initialize the start point as true(1)
            if( !start_flag && findStartPoint(string, pattern, i) ){
                dp[i][0] = 1; 
                start_flag = 1;
                *startpos = i;
                match_flag = 1;
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
                if(pattern[j-2] == ']'){
                    int k;
                    for(k = j-2; k>= 0; k--){
                        if(pattern[k] == '[') break;
                    }
                    dp[i][j] = dp[i][k] + dp[i][j-1];
                }else{
                    dp[i][j] = dp[i][j-2] + dp[i][j-1];
                }
                
            /*
             * #2 special case:
             * regex string contains '\' at index j-2
             * current match status dp[i][j] can be updated 
             * by the match status dp[i-1][j-2]
             * also need to check special match pattern
             * between string[i-1] and pattern[j-1]
             */
            }else if( j > 1 && pattern[j-2] == '\\'){
                dp[i][j] = i && dp[i-1][j-2] && checkSpecialMatch(string[i-1], pattern[j-1]);
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
                if(pattern[j-2] == ']'){
                    int k;
                    int found = bracketMatch(string, pattern, i, j, &k);
                    dp[i][j] = dp[i][k] || (i && found && dp[i-1][j]);
                }else{
                    if(j > 2 && pattern[j-3] == '\\'){
                        dp[i][j] = dp[i][j-3] || ( i && checkSpecialMatch(string[i-1], pattern[j-2]) && dp[i-1][j]);
                    }else{
                        dp[i][j] = dp[i][j-2] || ( i && ( pattern[j-2] == '.' || pattern[j-2] == string[i-1]) && dp[i-1][j]);
                    }
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
            /*
             * #5 special case
             * support bracket expressions
             * index k means the start position of the bracket expression
             * flag found means if there is a match or not
             */
            }else if( j > 1 && pattern[j-1] == ']'){
                int k;
                int found = bracketMatch(string, pattern, i, j, &k);
                dp[i][j] = i && dp[i-1][k] && found;
            }else{
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
                dp[i][j] = i && dp[i-1][j-1] && ( string[i-1] == pattern[j-1] || pattern[j-1] == '.');
            }
            if(dp[i][j] == 1) match_flag = 1;
        }
        if(!match_flag){
            if(findStartPoint(string, pattern, i)){
                *startpos = i;
                start_flag = 0;
            }else{
                start_flag = 0;
            }
        }
        match_flag = 0;
        /*
         * dp[i][patternLen] means string[0..i) matches with pattern[0..patternLen) 
         * there exists a substring in the input string matching with the entire regex string
         * function isMatch() return true
         */
        if(dp[i][patternLen] == 1){
            *endpos = i;
            return 1;
        }
    }
    // can not find matched substring in the given string
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
int regex_match( const char * filename, const char * regex, char *** matches, int trim_to_match){

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
        /*
         * if trim to match flag is not set
         * use isMatch() function to check
         * if the current line contains any substring 
         * match the given regular expression 
         * show the whole matched line
         */ 
        if(!trim_to_match){
            if(isMatch(string, regex)){
                if(strcmp(string, "\n") == 0) continue;
                // if matches, store the line in the matches array
                strcpy((*matches)[count], string);
                // increase the matched line number
                count++;
            }
        /*
         * if trim to match flag is set
         * use isMatchTrim() function to check
         * if the current line contains any substring 
         * match the given regex
         * and trim the matched line to
         * the matching substring 
         */
        }else{
            int startpos, endpos;
            if(isMatchTrim(string, regex, &startpos, &endpos)){
                if(strcmp(string, "\n") == 0) continue;
                // if matches, store the trimmed matching substring
                strncpy((*matches)[count], string+startpos, endpos-startpos);
                count++;
            }
        } 
    }
    return count;  
}


#ifndef USE_SUBMITTY_MAIN
int main(int argc, char *argv[])
{
    // ensure buffered output
    setvbuf(stdout, NULL, _IONBF, 0);
    // check the number of input files
    printf("Total number of documents: %d\n", argc-1);

    FILE *inputFile, *outputFile;
    char str[MAX_LINE_LENGTH];
    

    inputFile = fopen(argv[1], "r");
    outputFile = fopen("validWords.txt", "w");

    if (inputFile == NULL){
        fprintf(stderr, "ERROR: Open input file failed\n");
        return EXIT_FAILURE;
    }
    if (outputFile == NULL){
        fprintf(stderr, "ERROR: Open output file failed\n");
        return EXIT_FAILURE;
    }


    while(fgets(str, MAX_LINE_LENGTH, inputFile) != NULL)
    {
        prepInput(str);

        char * token = strtok(str, " ");
        while( token != NULL){
            if(strlen(token) > 1){
                fputs(token, outputFile);
                fputs(" ", outputFile);
            }
            token = strtok(NULL, " ");
        }
    }
    fclose(inputFile);
    fclose(outputFile);

    // get single word lists
    int wordNum = getWords();
    printf("Total number of words: %d\n", wordNum);

    // list of "stop words" 
    // used for identifying bigram/trigram
    char stopWords[1024] = "the of to and in said for that was on"
                            "he is with at by it from as be were"
                            "an have his but has are not who they its"
                            "had will would about been this their new or"
                            "which we more after us percent up one people";
    
    int biGramNum = getBigram(stopWords);
    int uniWords= 0, uniBigram = 0, triGramNum = 0, uniTriGram = 0; 

    printf("Total number of unique words: %d\n", uniWords);
    printf("Total number of interesting bigrams: %d\n", biGramNum);
    printf("Total number of unique interesting bigrams: %d\n", uniBigram);
    printf("Total number of interesting trigrams: %d\n", triGramNum);
    printf("Total number of unique interesting trigrams: %d\n", uniTriGram);
    printf("\n");
    printf("Top 50 words:\n");

    return EXIT_SUCCESS;
}
#endif