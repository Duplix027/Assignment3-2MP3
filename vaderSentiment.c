#include "utility.h"
#include <errno.h>
#include <math.h>

// Reads data from a file and stores it in an array of WordData structs
WordData* read_lexicon_file(const char *filename, int *word_count) {
    FILE *file = fopen(filename, "r");

    // Error handling using perror for informative messages
    if (!file) {
        perror("Error opening file");
        return NULL;
    }

    int capacity = 100;  // Initial capacity for dynamic array
    WordData *words = malloc(capacity * sizeof(WordData));
    if (!words) {
        perror("Memory allocation failed");
        fclose(file);
        return NULL;
    }
    *word_count = 0;

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        // Expand memory allocation dynamically when needed
        if (*word_count >= capacity) {
            capacity *= 2;
            WordData *temp = realloc(words, capacity * sizeof(WordData));
            if (!temp) {
                perror("Memory reallocation failed");
                free(words);
                fclose(file);
                return NULL;
            }
            words = temp;
        }

        sscanf(line, "%s %f %f", words[*word_count].word, &words[*word_count].value1, &words[*word_count].value2);
        (*word_count)++;
    }

    fclose(file);
    return words;
}

// Searches for a specific word in the WordData array
WordData find_data(WordData *data, char *word) {
    for (int i = 0; data[i].word[0] != '\0'; i++) {
        if (strcmp(data[i].word, word) == 0) {
            return data[i];
        }
    }

    WordData nullData;
    nullData.word[0] = '\0';
    return nullData;
}

// Helper function to sift sentiment scores into positive, negative, and neutral components
void sift_sentiment_scores(float sentiments[], int count, float *pos_sum, float *neg_sum, int *neu_count) {
    *pos_sum = 0.0;
    *neg_sum = 0.0;
    *neu_count = 0;

    for (int i = 0; i < count; i++) {
        if (sentiments[i] > 0) {
            *pos_sum += (sentiments[i] + 1); // Compensation for positive sentiment
        } else if (sentiments[i] < 0) {
            *neg_sum += (sentiments[i] - 1); // Compensation for negative sentiment
        } else {
            (*neu_count)++;
        }
    }
}

// Calculates the sentiment score of a sentence and returns a SentimentResult struct
SentimentResult calculate_sentiment_score(const char *sentence, WordData *lexicon, int word_count) {
    float scores[MAX_STRING_LENGTH] = { 0.0 };
    int index = 0;
    float sentimentSum = 0.0;
    float pos_sum = 0.0, neg_sum = 0.0;
    int neu_count = 0;

    // Copy the sentence so we can tokenize it without modifying the original
    char sentence_copy[MAX_STRING_LENGTH];
    strcpy(sentence_copy, sentence);

    // Tokenize the sentence into individual words
    char *token = strtok(sentence_copy, " \n\t\v\f\r,.");

    // Flags to track if previous word was an intensifier or negation
    bool previous_word_is_intensifier = false;
    bool negation_active = false;

    // Loop through each token to analyze its sentiment
    while (token != NULL) {
        bool allCaps = true; // Variable to check if the word is in ALLCAPS
        int exclamation = 0; // Counter for exclamation marks

        // Convert token to lowercase for consistent lookup
        char lowerToken[MAX_STRING_LENGTH];
        strcpy(lowerToken, token);

        for (int i = 0; lowerToken[i] != '\0'; i++) {
            // Check if the current character is lowercase (to decide if ALLCAPS)
            if (islower(lowerToken[i])) {
                allCaps = false;
            }
            // Convert character to lowercase
            lowerToken[i] = tolower(lowerToken[i]);

            // Count and limit exclamation marks
            if (lowerToken[i] == '!') {
                exclamation++;
                lowerToken[i] = '\0'; // Remove the exclamation mark
                if (exclamation > 3) exclamation = 3; // Limit to a max of 3
            }
        }

        // Find the sentiment value of the word in the lexicon
        WordData wordData = find_data(lexicon, lowerToken);

        // Check if the token is an intensifier or negation
        bool is_intensifier = false;
        bool is_negation = false;

        // Check if the current word is an intensifier or negation
        for (int i = 0; i < POSITIVE_INTENSIFIERS_SIZE; i++) {
            if (strcmp(lowerToken, positive_intensifiers[i]) == 0) {
                is_intensifier = true;
                break;
            }
        }

        for (int i = 0; i < NEGATIONS_SIZE; i++) {
            if (strcmp(lowerToken, negation_words[i]) == 0) {
                is_negation = true;
                break;
            }
        }

        if (is_intensifier) {
            // Set flag for the next word
            previous_word_is_intensifier = true;
        } else if (is_negation) {
            // Activate negation effect for subsequent words
            negation_active = true;
        } else if (wordData.word[0] != '\0') {
            // Process the sentiment value if the word is in the lexicon
            float sentimentValue = wordData.value1;

            // Apply ALLCAPS amplification if needed
            if (allCaps) {
                sentimentValue *= CAPS;
            }

            // Apply previous intensifier if applicable
            if (previous_word_is_intensifier) {
                sentimentValue += sentimentValue * INTENSIFIER;
                previous_word_is_intensifier = false; // Reset flag
            }

            // Apply ongoing negation effect
            if (negation_active) {
                sentimentValue *= NEGATION;
            }

            // Adjust sentiment for exclamation marks (if any)
            if (sentimentValue > 0) {
                sentimentValue += (sentimentValue * EXCLAMATION * exclamation);
            } else {
                sentimentValue -= (sentimentValue * EXCLAMATION * exclamation);
            }

            // Store the sentiment value and add it to the cumulative sentiment sum
            scores[index] = sentimentValue;
            sentimentSum += sentimentValue;

            // Move to the next index for storing scores
            index++;
        }

        // Deactivate negation if a punctuation or conjunction is reached
        if (strcmp(lowerToken, "and") == 0 || strcmp(lowerToken, "or") == 0) {
            negation_active = false;
        }

        // Get the next word token
        token = strtok(NULL, " \n\t\v\f\r,.");
    }

    // Use sift_sentiment_scores to categorize the individual word sentiment scores
    sift_sentiment_scores(scores, index, &pos_sum, &neg_sum, &neu_count);

    // Calculate the compound score for the sentence
    float compound = sentimentSum / sqrt(pow(sentimentSum, 2) + 15);

    // Normalize positive, negative, and neutral scores based on total sentiment components
    float total = pos_sum + fabs(neg_sum) + neu_count;
    float pos = fabs(pos_sum / total);
    float neg = fabs(neg_sum / total);
    float neu = fabs((float)neu_count / total);

    // Return the sentiment analysis results as a struct
    SentimentResult result = {pos, neg, neu, compound};
    return result;
}
