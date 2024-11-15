#include "utility.h"
#include <errno.h>
#include <math.h>

// Reads data from a file and stores it in an array of WordData structs
WordData* read_lexicon_file(const char *filename, int *word_count) {
    FILE *file = fopen(filename, "r"); // Open the file in read mode

    // Error handling: If the file cannot be opened, print an error message and return NULL
    if (!file) {
        perror("Error opening file");
        return NULL;
    }

    int capacity = 100;  // Initial capacity for dynamic array
    WordData *words = malloc(capacity * sizeof(WordData)); // Allocate memory for WordData structs
    if (!words) {
        perror("Memory allocation failed");
        fclose(file);
        return NULL;
    }
    *word_count = 0; // Initialize word count to 0

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        // If the word count exceeds capacity, we need to expand the memory allocated for words
        if (*word_count >= capacity) {
            capacity *= 2; // Double the capacity
            WordData *temp = realloc(words, capacity * sizeof(WordData)); // Reallocate memory with new capacity
            if (!temp) {
                perror("Memory reallocation failed");
                free(words); // Free the original memory if reallocation fails
                fclose(file);
                return NULL;
            }
            words = temp; // Point to the newly allocated memory
        }

        // Extract the word and its sentiment scores from the line
        sscanf(line, "%s %f %f", words[*word_count].word, &words[*word_count].value1, &words[*word_count].value2);
        (*word_count)++; // Increment word count
    }

    fclose(file); // Close the file after reading all lines
    return words; // Return the dynamically allocated array of WordData structs
}

// Searches for a specific word in the WordData array
WordData find_data(WordData *data, char *word) {
    for (int i = 0; data[i].word[0] != '\0'; i++) {
        if (strcmp(data[i].word, word) == 0) {
            return data[i]; // Return the WordData struct if the word matches
        }
    }

    // If no match is found, return a WordData with an empty word
    WordData nullData;
    nullData.word[0] = '\0'; // Set the first character of word to null to indicate "not found"
    return nullData;
}

// Helper function to sift sentiment scores into positive, negative, and neutral components
void sift_sentiment_scores(float sentiments[], int count, float *pos_sum, float *neg_sum, int *neu_count) {
    *pos_sum = 0.0;  // Initialize positive score sum to zero
    *neg_sum = 0.0;  // Initialize negative score sum to zero
    *neu_count = 0;  // Initialize neutral count to zero

    // Loop through the array of sentiment scores
    for (int i = 0; i < count; i++) {
        if (sentiments[i] > 0) {
            *pos_sum += sentiments[i];  // Accumulate positive scores
        } else if (sentiments[i] < 0) {
            *neg_sum += sentiments[i];  // Accumulate negative scores
        } else {
            (*neu_count)++;  // Increment neutral count for zero scores
        }
    }
}

// Calculates the sentiment score of a sentence and returns a SentimentResult struct
SentimentResult calculate_sentiment_score(const char *sentence, WordData *lexicon, int word_count) {
    float scores[MAX_STRING_LENGTH] = { 0.0 }; // Array to store individual word sentiment scores
    int index = 0; // Index for storing sentiment scores
    float sentimentSum = 0.0; // Cumulative sum of all sentiment scores
    float pos_sum = 0.0, neg_sum = 0.0; // Sums for positive and negative scores
    int neu_count = 0; // Count of neutral scores

    // Copy the sentence into a new array so we can modify it without affecting the original
    char sentence_copy[MAX_STRING_LENGTH];
    strcpy(sentence_copy, sentence);

    // Tokenize the sentence into individual words
    char *token = strtok(sentence_copy, " \n\t\v\f\r,."); // Delimiters include spaces, tabs, punctuation

    // Flags for intensifier and negation handling
    bool previous_word_is_intensifier = false; // Tracks if the previous word was an intensifier
    bool negation_active = false; // Tracks if negation is currently active

    // Loop through each token (word) in the sentence
    while (token != NULL) {
        bool allCaps = true; // Flag to check if the word is in ALLCAPS
        int exclamation = 0; // Counter for the number of exclamation marks

        // Convert token to lowercase for consistent lexicon lookup
        char lowerToken[MAX_STRING_LENGTH];
        strcpy(lowerToken, token);

        for (int i = 0; lowerToken[i] != '\0'; i++) {
            // If the character is lowercase, then the word is not all caps
            if (islower(lowerToken[i])) {
                allCaps = false;
            }
            // Convert each character to lowercase
            lowerToken[i] = tolower(lowerToken[i]);

            // Count and remove exclamation marks, limiting to max of 3
            if (lowerToken[i] == '!') {
                exclamation++;
                lowerToken[i] = '\0'; // Remove exclamation marks from token
                if (exclamation > 3) exclamation = 3; // Cap the number of exclamation marks to 3
            }
        }

        // Find the sentiment value of the word in the lexicon
        WordData wordData = find_data(lexicon, lowerToken);

        // Check if the current token is an intensifier or negation word
        bool is_intensifier = false;
        bool is_negation = false;

        // Check against positive intensifiers
        for (int i = 0; i < POSITIVE_INTENSIFIERS_SIZE; i++) {
            if (strcmp(lowerToken, positive_intensifiers[i]) == 0) {
                is_intensifier = true;
                break;
            }
        }

        // Check against negation words
        for (int i = 0; i < NEGATIONS_SIZE; i++) {
            if (strcmp(lowerToken, negation_words[i]) == 0) {
                is_negation = true;
                break;
            }
        }

        // Handle intensifier flag
        if (is_intensifier) {
            previous_word_is_intensifier = true; // Set flag for next word
        } else if (is_negation) {
            negation_active = true; // Activate negation for following words
        } else {
            // Process the sentiment value if the word is in the lexicon
            float sentimentValue = 0.0;

            if (wordData.word[0] != '\0') { // Check if the word was found in the lexicon
                sentimentValue = wordData.value1; // Get the primary sentiment value

                // Amplify sentiment value if the word is in ALLCAPS
                if (allCaps) {
                    sentimentValue *= CAPS; // CAPS amplification factor is 1.5
                }

                // Apply intensifier effect from the previous word if applicable
                if (previous_word_is_intensifier) {
                    sentimentValue += sentimentValue * INTENSIFIER; // Amplify by 0.293 for intensifiers
                    previous_word_is_intensifier = false; // Reset the flag after applying
                }

                // Apply negation effect if active
                if (negation_active) {
                    sentimentValue *= NEGATION; // Negate the sentiment by multiplying with -0.5
                }

                // Adjust sentiment for exclamation marks if present
                if (exclamation > 0) {
                    if (sentimentValue > 0) {
                        sentimentValue += (sentimentValue * EXCLAMATION * exclamation); // Positive boost for positive words
                    } else {
                        sentimentValue -= (sentimentValue * EXCLAMATION * exclamation); // Negative boost for negative words
                    }
                }
            }

            // Store the sentiment value in the scores array
            scores[index] = sentimentValue;
            sentimentSum += sentimentValue; // Accumulate total sentiment value

            index++; // Move to the next index for storing scores

            // Deactivate negation if a conjunction or punctuation is encountered
            if (strcmp(lowerToken, "and") == 0 || strcmp(lowerToken, "or") == 0 || strcmp(lowerToken, ".") == 0) {
                negation_active = false; // Reset negation effect
            }

            // Reset the intensifier flag for the current word if it's not an intensifier
            previous_word_is_intensifier = false;
        }

        // Get the next word token
        token = strtok(NULL, " \n\t\v\f\r,.");
    }

    // Sift sentiment scores into positive, negative, and neutral sums
    sift_sentiment_scores(scores, index, &pos_sum, &neg_sum, &neu_count);

    // Calculate the compound score for the sentence using normalization
    float compound = sentimentSum / sqrt(sentimentSum * sentimentSum + 15);

    // Normalize the positive, negative, and neutral scores based on total components
    float total = pos_sum + fabs(neg_sum) + neu_count;
    if (total == 0.0) total = 1.0; // Prevent division by zero by setting total to 1

    float pos = fabs(pos_sum / total); // Calculate positive proportion
    float neg = fabs(neg_sum / total); // Calculate negative proportion
    float neu = (float)neu_count / total; // Calculate neutral proportion

    // Return the sentiment analysis results as a struct
    SentimentResult result = { pos, neg, neu, compound };
    return result; // Return the SentimentResult struct containing all calculated scores
}
