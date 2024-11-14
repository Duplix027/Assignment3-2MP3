#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utility.h"

// Function to test the sentiment analysis implementation
int main() {
    int word_count = 0;

    // Read the lexicon file
    WordData *lexicon = read_lexicon_file("vader_lexicon.txt", &word_count);

    if (lexicon == NULL) {
        printf("Failed to load lexicon.\n");
        return 1;
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
    for (int i = 0; i < sizeof(test_sentences) / sizeof(test_sentences[0]); i++) {
        SentimentResult sentiment_score = calculate_sentiment_score(test_sentences[i], lexicon, word_count);
        printf("Sentence: \"%s\"\n", test_sentences[i]);
        printf("{'neg': %.3f, 'neu': %.3f, 'pos': %.3f, 'compound': %.4f}\n\n",
               sentiment_score.neg, sentiment_score.neu, sentiment_score.pos, sentiment_score.compound);
    }

    // Free the allocated memory
    free(lexicon);
    return 0;
}
