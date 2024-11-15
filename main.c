#include <stdio.h>     // Standard Input/Output library for printing output to the console
#include <stdlib.h>    // Standard Library for memory management (malloc, free)
#include <string.h>    // String handling functions (strcpy, strcmp, etc.)
#include <stddef.h>    // Added for size_t, needed for array indexing, a good practice to use size_t to avoid overflow
#include "utility.h"   // Our custom header file containing definitions, structures, and function prototypes

int main() {
    int word_count = 0;  // Variable to keep track of the number of words in our lexicon

    // Read the lexicon file
    // Here, we call the `read_lexicon_file()` function to load the sentiment lexicon data from the file.
    // If there's an error (e.g., the file is missing), it will print an error message and exit the program.
    WordData *lexicon = read_lexicon_file("vader_lexicon.txt", &word_count);
    if (lexicon == NULL) {
        // If the file couldn't be read, print error and exit.
        printf("Error: Unable to rread lexicon file.\n");
        return 1;  // Returning a non-zero value here indicates abnormal termination due to error
    }

    // Test Sentences
    const char *test_sentences[] = {
        "VADER is smart, handsome, and funny.",
        "VADER is smart, handsome, and funny!",
        "VADER is very smart, handsome, and funny.",
        "VADER is VERY SMART, handsome, and FUNNY.",
        "VADER is VERY SMART, handsome, and FUNNY!!!",
        "VADER is VERY SMART, uber handsome, and FRIGGIN FUNNY!!!",
        "VADER is not smart, handsome, nor funny.",
        "At least it isn’t a horrible book.",
        "The plot was good, but the characters are uncompelling and the dialog is not great.",
        "Make sure you :) or :D today!",
        "Not bad at all"
    };

    // Run sentiment analysis on each test sentence
    // The loop runs through each test sentence and performs sentiment analysis.
    // We use `sizeof(test_sentences) / sizeof(test_sentences[0])` to get the total number of sentences,
    // which ensures compatibility if the array size changes.
    for (size_t i = 0; i < sizeof(test_sentences) / sizeof(test_sentences[0]); i++) {
        // Calculate the sentiment score for each sentence using our sentiment analysis function
        // `calculate_sentiment_score()` returns a `SentimentResult` structure containing `neg`, `neu`, `pos`, and `compound` scores.
        SentimentResult sentiment_score = calculate_sentiment_score(test_sentences[i], lexicon, word_count);

        // Printing the results of the sentiment analysis
        // We use `%f` here to format the floating-point scores. Since VADER produces four key scores (positive, negative, neutral, and compound),
        // we display all of them for each sentence.
        printf("Sentence: \"%s\"\n", test_sentences[i]);
        printf("{'neg': %f, 'neu': %f, 'pos': %f, 'compound': %f}\n\n",
               sentiment_score.neg, sentiment_score.neu, sentiment_score.pos, sentiment_score.compound);
    }

    // Free the allocated memory
    // Since we allocated memory for `lexicon` dynamically in `read_lexicon_file`, we need to free it after we're done.
    free(lexicon);
    
    return 0; // Returning 0 indicates successful completion of the program
}
