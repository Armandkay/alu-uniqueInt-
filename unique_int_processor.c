#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <stdbool.h>

#define MIN_INTEGER -1023
#define MAX_INTEGER 1023
#define ARRAY_SIZE (MAX_INTEGER - MIN_INTEGER + 1)

typedef struct {
    bool hasAppeared[ARRAY_SIZE];
} UniqueIntProcessor;

void initializeProcessor(UniqueIntProcessor *processor) {
    for (int i = 0; i < ARRAY_SIZE; i++) {
        processor->hasAppeared[i] = false;
    }
}

bool isValidIntegerLine(const char *line, int *number) {
    char *end;
    long value = strtol(line, &end, 10);
    if (end != line && *end == '\0' && value >= INT_MIN && value <= INT_MAX) {
        *number = (int)value;
        return true;
    }
    printf("Invalid integer line: %s\n", line);
    return false;
}

void readUniqueNumbers(UniqueIntProcessor *processor, const char *inputFilePath, int **distinctNumbers, int *count) {
    FILE *file = fopen(inputFilePath, "r");
    if (!file) {
        perror("Error opening input file");
        exit(EXIT_FAILURE);
    }

    char line[256];
    int number, index;
    *count = 0;
    *distinctNumbers = malloc(ARRAY_SIZE * sizeof(int));

    while (fgets(line, sizeof(line), file)) {
        char *strippedLine = strtok(line, "\r\n");
        if (strippedLine && isValidIntegerLine(strippedLine, &number)) {
            if (number >= MIN_INTEGER && number <= MAX_INTEGER) {
                index = number - MIN_INTEGER;
                if (!processor->hasAppeared[index]) {
                    processor->hasAppeared[index] = true;
                    (*distinctNumbers)[(*count)++] = number;
                }
            } else {
                printf("Number not in range: %d\n", number);
            }
        }
    }

    fclose(file);
}

void sortDistinctNumbers(int *numbers, int count) {
    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - i - 1; j++) {
            if (numbers[j] > numbers[j + 1]) {
                int temp = numbers[j];
                numbers[j] = numbers[j + 1];
                numbers[j + 1] = temp;
            }
        }
    }
}

void writeDistinctNumbers(int *numbers, int count, const char *outputFilePath) {
    FILE *file = fopen(outputFilePath, "w");
    if (!file) {
        perror("Error opening output file");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < count; i++) {
        fprintf(file, "%d\n", numbers[i]);
    }

    fclose(file);
}

void processFile(UniqueIntProcessor *processor, const char *inputFilePath, const char *outputFilePath) {
    initializeProcessor(processor);

    int *distinctNumbers;
    int count;
    readUniqueNumbers(processor, inputFilePath, &distinctNumbers, &count);
    sortDistinctNumbers(distinctNumbers, count);
    writeDistinctNumbers(distinctNumbers, count, outputFilePath);

    free(distinctNumbers);
}

void processDirectory(const char *inputFolder, const char *outputFolder) {
    DIR *dir;
    struct dirent *entry;

    if ((dir = opendir(inputFolder)) == NULL) {
        perror("Error opening input directory");
        exit(EXIT_FAILURE);
    }

    UniqueIntProcessor processor;
    char inputPath[512];
    char outputPath[512];

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG && strstr(entry->d_name, ".txt")) {
            snprintf(inputPath, sizeof(inputPath), "%s/%s", inputFolder, entry->d_name);
            snprintf(outputPath, sizeof(outputPath), "%s/%s_results.txt", outputFolder, entry->d_name);

            clock_t startTime = clock();
            processFile(&processor, inputPath, outputPath);
            clock_t endTime = clock();

            double timeSpent = (double)(endTime - startTime) / CLOCKS_PER_SEC;
            printf("Processed %s in %.2f seconds\n", entry->d_name, timeSpent);
        }
    }

    closedir(dir);
}

int main() {
    const char *inputFolder = "./Input";
    const char *outputFolder = "./Output";
    processDirectory(inputFolder, outputFolder);
    return 0;
}
