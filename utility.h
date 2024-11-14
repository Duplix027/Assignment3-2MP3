#ifndef UTILITY_H
#define UTILITY_H

// Include necessary libraries
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include <math.h>

// Define general constants
#define MAX_STRING_LENGTH 200      // Maximum length for strings

// Declare Positive Amplifiers that amplify positive sentiment
#define POSITIVE_INTENSIFIERS_SIZE (sizeof(positive_intensifiers) / sizeof(positive_intensifiers[0]))

// Declare Negative Amplifiers that slightly reduce positive or amplify negative sentiment
#define NEGATIVE_INTENSIFIERS_SIZE (sizeof(negative_intensifiers) / sizeof(negative_intensifiers[0]))

// Declare words indicating negation, which invert the sentiment of the following word
#define NEGATIONS_SIZE (sizeof(negation_words) / sizeof(negation_words[0]))

// Constants for sentiment adjustment
#define INTENSIFIER 0.293          // Multiplier for intensifiers (positive or negative)
#define EXCLAMATION 0.292          // Boost from exclamation marks
#define CAPS 1.5                   // Boost for words in all caps
#define NEGATION -0.5              // Factor to invert sentiment on negated words

// Structure to hold word data, including sentiment scores
typedef struct {
    char word[MAX_STRING_LENGTH];  // Word string
    float value1;                  // Primary sentiment score
    float value2;                  // Secondary sentiment score
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
WordData find_data(WordData *data, char *word);            // Searches for a word in the WordData array

#endif
