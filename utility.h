#ifndef UTILITY_H
#define UTILITY_H

// Define general constants
#define ARRAY_SIZE 20              // Array size for intArray in WordData struct; used to hold additional metadata for each word
#define MAX_STRING_LENGTH 200      // Maximum length for strings to accommodate long words or sentences
#define LINE_LENGTH 100            // Maximum length of a line in the file; this helps when reading files

// Include necessary libraries
#include <stdio.h>    // For file operations, input/output
#include <string.h>   // For string manipulation (strcpy, strcmp, etc.)
#include <stdlib.h>   // For dynamic memory allocation (malloc, free, etc.)
#include <ctype.h>    // For character checking and manipulation (tolower, isupper, etc.)
#include <stdbool.h>  // For boolean data type
#include <math.h>     // For mathematical calculations used in sentiment analysis

// Positive intensifiers that amplify positive sentiment
// The size is defined as POSITIVE_INTENSIFIERS_SIZE for easy reference
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
// Positive intensifiers are words that amplify positive sentiment in a sentence. For example, "really happy" is more positive than just "happy."

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
// Negative intensifiers weaken a positive sentiment or amplify a negative one. For instance, "barely good" is much less positive than just "good."

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
// Negation words are used to flip the sentiment of a following word. For example, "not happy" means the opposite of "happy."

// Constants for sentiment adjustment
#define INTENSIFIER 0.293          // Multiplier for intensifiers (positive or negative) — this value is empirically derived to adjust sentiment scores
#define EXCLAMATION 0.292          // Boost from exclamation marks to indicate emphasis, adding to the overall sentiment score
#define CAPS 1.5                   // Boost for words in all caps, indicating a higher intensity (e.g., "HAPPY" vs "happy")
#define NEGATION -0.5              // Factor to invert sentiment on negated words; a negative sentiment value is multiplied by this to invert its meaning

// Structure to hold word data, including sentiment scores and an integer array
typedef struct {
   char word[MAX_STRING_LENGTH];  // The word as a string
   float value1;                  // Primary sentiment score (usually positive or negative)
   float value2;                  // Secondary sentiment score (could be for other attributes, e.g., emotional intensity)
   int intArray[ARRAY_SIZE];      // Additional data related to the word; customizable for different use cases
} WordData;
// The WordData struct holds the core data for each word in the sentiment lexicon. This includes two float scores for sentiment and an integer array to hold extra info if needed.

// Structure to hold the sentiment analysis results
typedef struct {
    float pos;      // Positive sentiment score
    float neg;      // Negative sentiment score
    float neu;      // Neutral sentiment score
    float compound; // Compound score, representing the overall sentiment of the sentence
} SentimentResult;
// SentimentResult is used to store the result of the sentiment analysis for a given sentence. It includes individual positive, negative, and neutral scores, as well as a compound score.

// Function prototypes
WordData *read_lexicon_file(const char *filename, int *word_count); // Reads the lexicon file and returns an array of WordData with the count of words
SentimentResult calculate_sentiment_score(const char *sentence, WordData *lexicon, int word_count); // Analyzes the sentiment of a given sentence
WordData find_data(WordData *data, char *word);          // Searches for a specific word in the WordData array, returning the WordData struct for that word
int is_all_caps(const char* word);                       // Checks if the word is in all uppercase letters and returns true if it is

#endif
