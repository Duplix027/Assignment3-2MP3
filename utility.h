#ifndef UTILITY_H
#define UTILITY_H

// Define general constants
#define ARRAY_SIZE 20              // Array size for intArray in WordData struct
#define MAX_STRING_LENGTH 200      // Maximum length for strings
#define LINE_LENGTH 100            // Maximum length of a line in the file

// Include necessary libraries
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include <math.h>

// Positive intensifiers that amplify positive sentiment
#define POSITIVE_INTENSIFIERS_SIZE 11
static char *positive_intensifiers[] = {
   "absolutely",
   "completely",
   "extremely",
   "really",
   "so",
   "totally",
   "very",
   "particularly",
   "exceptionally",
   "incredibly",
   "remarkably"
};

// Negative intensifiers that slightly reduce positive or amplify negative sentiment
#define NEGATIVE_INTENSIFIERS_SIZE 9
static char *negative_intensifiers[] = {
   "barely",
   "hardly",
   "scarcely",
   "somewhat",
   "mildly",
   "slightly",
   "partially",
   "fairly",
   "pretty much"
};

// Words indicating negation, which invert the sentiment of the following word
#define NEGATIONS_SIZE 13
static char *negation_words[] = {
   "not",
   "isn't",
   "doesn't",
   "wasn't",
   "shouldn't",
   "won't",
   "cannot",
   "can't",
   "nor",
   "neither",
   "without",
   "lack",
   "missing"
};

// Constants for sentiment adjustment
#define INTENSIFIER 0.293          // Multiplier for intensifiers (positive or negative)
#define EXCLAMATION 0.292          // Boost from exclamation marks
#define CAPS 1.5                   // Boost for words in all caps
#define NEGATION -0.5              // Factor to invert sentiment on negated words

// Structure to hold word data, including sentiment scores and an integer array
typedef struct {
   char word[MAX_STRING_LENGTH];  // Word string
   float value1;                  // Primary sentiment score
   float value2;                  // Secondary sentiment score
   int intArray[ARRAY_SIZE];      // Integer array (customizable for additional data)
} WordData;

// Structure to hold the sentiment analysis results
typedef struct {
    float pos;
    float neg;
    float neu;
    float compound;
} SentimentResult;

// Function prototypes
WordData *read_lexicon_file(const char *filename, int *word_count); // Reads lexicon file
SentimentResult calculate_sentiment_score(const char *sentence, WordData *lexicon, int word_count); // Calculates sentiment score for a sentence
WordData find_data(WordData *data, char *word);          // Searches for a word in the WordData array
int is_all_caps(const char* word);                       // Returns true if word is all caps

#endif
