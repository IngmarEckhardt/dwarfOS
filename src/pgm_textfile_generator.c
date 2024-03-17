#include "pgm_textfile_generator.h"
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define SRC_DIR "src/test"
#define FILE_EXTENSION 'c'

// structure to manage a single string
typedef struct {
    //if the string is several times in the array with strings given to the function, all of the indizes+1 will be listet here
    uint8_t indices[UINT8_MAX];
    const char * text;
    // amount of the indizes, just for convenience to ease the max-calculation
    int index_count;
} Entry;

char * addEscapeCharsToNonPrintableChars(const char * str) {
    char * result = malloc(strlen(str) * 2 + 1); // Allocate enough space for the replaced string
    char * dest = result;
    while (*str) {
        if (*str == '\n') {
            *dest++ = '\\';
            *dest++ = 'n';
        } else if (*str == '\"') {
            *dest++ = '\\';
            *dest++ = '\"';
        } else {
            *dest++ = *str;
        }
        str++;
    }
    *dest = '\0';
    return result;
}

char * toCamelCase(char * prefix) {
    uint16_t length = strlen(prefix);
    char * stringToReturn = malloc(length + 1);
    if (stringToReturn == NULL) { return NULL; }

    strcpy(stringToReturn, prefix);
    int indexToPlace = 0;
    for (uint16_t i = 0; i < length; i++, indexToPlace++) {
        stringToReturn[indexToPlace] = tolower(stringToReturn[i]);
        if (stringToReturn[indexToPlace] == '_') {
            i++;
            stringToReturn[indexToPlace] = toupper(stringToReturn[i]);
        }
    }
    stringToReturn[indexToPlace] = '\0';
    return stringToReturn;
}

char * createFileName(char * additionalPrefix, char * prefix, char fileEnding) {
    uint16_t lengthPrefix = strlen(prefix);
    uint16_t lengthAdditionalPrefix = strlen(additionalPrefix);
    char * stringToReturn = malloc(
            lengthPrefix + lengthAdditionalPrefix + 8); // 8 extra chars for "../", "/", "/", "s.", and null terminator
    if (stringToReturn == NULL) { return NULL; }

    strcpy(stringToReturn, "../");
    strcat(stringToReturn, additionalPrefix);
    strcat(stringToReturn, "/");
    strcat(stringToReturn, prefix);
    for (uint16_t i = 0; i < lengthPrefix; i++) {
        stringToReturn[i + lengthAdditionalPrefix + 4] = tolower(prefix[i]);
    } // 4 extra chars for "../"
    stringToReturn[lengthPrefix + lengthAdditionalPrefix + 4] = 's'; // 4 extra chars for "../"
    stringToReturn[lengthPrefix + lengthAdditionalPrefix + 5] = '.';
    stringToReturn[lengthPrefix + lengthAdditionalPrefix + 6] = fileEnding;
    stringToReturn[lengthPrefix + lengthAdditionalPrefix + 7] = '\0';
    return stringToReturn;
}

void calcMaxAmountOfIndexArray(const uint8_t datasets, Entry * const * entries, const int * entry_counts,
                               uint8_t * maxIndizes) {
    for (int i = 0; i < datasets - 1; i++) {
        maxIndizes[i] = 0;
        for (int j = 0; j < entry_counts[i]; j++) {
            if (entries[i][j].index_count > maxIndizes[i]) {
                maxIndizes[i] = entries[i][j].index_count;
            }
        }
    }
}

void writeProgMemArrays(const char * prefix, const uint8_t datasets, Entry * const * entries, const int * entry_counts,
                        const char * camelCase, FILE * file) {
    for (int i = 0; i < datasets - 1; i++) {
        fprintf(file, "\ntypedef struct {\n");
        fprintf(file, "\tuint8_t numbers[MAX_AMOUNT_OF_%s_DESCRIPTIONS_%d_WITH_SAME_LENGTH];\n", prefix, i + 1);
        fprintf(file, "\tchar stringInProgramMem[%s_DESCRIPTION_%d_LENGTH];\n", prefix, i + 1);
        fprintf(file, "} %s_%d;\n\n", prefix, i + 1);
        fprintf(file, "const __attribute__((__progmem__)) %s_%d %ss_%d[AMOUNT_OF_%s_DESCRIPTIONS_%d] = {\n", prefix,
                i + 1, camelCase, i + 1,
                prefix, i + 1);
        for (int j = 0; j < entry_counts[i]; j++) {
            fprintf(file, "\t\t{{");
            // print indizes
            for (int k = 0; k < entries[i][j].index_count; k++) { fprintf(file, "%d,", entries[i][j].indices[k]); }
            // print String
            fprintf(file, "},\"%s\"},\n", entries[i][j].text);
        }
        fprintf(file, "};\n");
    }
}

void writeSingleRemainingStrings(const char * prefix, const uint8_t datasets, Entry * const * entries,
                                 const int * entry_counts, const char * camelCase, FILE * file) {
    for (int i = 0; i < entry_counts[datasets - 1]; i++) {
        fprintf(file, "#define %s_DESCRIPTION_%d_LENGTH %u\n",
                prefix,
                entries[datasets - 1][i].indices[0],
                strlen(entries[datasets - 1][i].text));
        fprintf(file, "const __attribute__((__progmem__)) char %s_%d[%s_DESCRIPTION_%d_LENGTH] = \"%s\";\n",
                camelCase,
                entries[datasets - 1][i].indices[0],
                prefix,
                entries[datasets - 1][i].indices[0],
                entries[datasets - 1][i].text);
        fprintf(file, "\n");
    }
}

void writeGetterFunction(const char * prefix, const uint8_t datasets, Entry * const * entries,
                         const int * entry_counts, const char * camelCase, const char * pascalCase, FILE * file) {

    fprintf(file, "#define LOAD_FROM(NUM) \\\n");
#ifdef __AVR_HAVE_ELPM__
    fprintf(file, "\tstringToReturn = helper->loadFarStringFromFile(&(FarTextFile) { \\\n");
#else
    fprintf(file, "\tstringToReturn = helper->loadNearStringFromFile(&(NearTextFile) { \\\n");
#endif
#ifdef __AVR_HAVE_ELPM__
    fprintf(file, "\t\t.farPointer = pgm_get_far_address(%ss_##NUM), \\\n", camelCase);
#else
    fprintf(file, "\t\t.pointerToNearProgMemString = (void *) %ss_##NUM,\\\n", camelCase);
#endif
    fprintf(file, "\t\t.maxLengthOfStrings = %s_DESCRIPTION_##NUM##_LENGTH, \\\n", prefix);
    fprintf(file, "\t\t.sizeOfIndexArray = MAX_AMOUNT_OF_%s_DESCRIPTIONS_##NUM##_WITH_SAME_LENGTH, \\\n", prefix);
    fprintf(file, "\t\t.amountOfEntries = AMOUNT_OF_%s_DESCRIPTIONS_##NUM, \\\n", prefix);
    fprintf(file, "\t}, %sNumber); \\\n", camelCase);
    fprintf(file, "\tif (stringToReturn != NULL) { return stringToReturn; }\n\n");
    fprintf(file, "char * load%s(FlashHelper * helper, uint8_t %sNumber) {\n", pascalCase, camelCase);
    fprintf(file, "\tchar * stringToReturn = NULL;\n\n");
    for (int j = 0; j < entry_counts[datasets - 1]; j++) {
        fprintf(file, "\tif (%sNumber == %d) {\n", camelCase,
                entries[datasets - 1][j].indices[0]);
        fprintf(file, "\t\tstringToReturn = (char *) malloc(%s_DESCRIPTION_%d_LENGTH);\n", prefix,
                entries[datasets - 1][j].indices[0]);
#ifdef __AVR_HAVE_ELPM__
        fprintf(file, "\t\thelper->loadFarStringFromFlash(stringToReturn, pgm_get_far_address(%s_%d));\n",
                camelCase, entries[datasets - 1][j].indices[0]);
#else
        fprintf(file, "\t\thelper->loadNearStringFromFlash(stringToReturn, %s_%d);\n",
        camelCase, entries[datasets - 1][j].indices[0]);
#endif
        fprintf(file, "\t}\n");
    }
    for (int i = 0; i < datasets - 1; i++) { fprintf(file, "\tLOAD_FROM(%d)\n", i + 1); }
    fprintf(file, "\n");
    fprintf(file, "\treturn stringToReturn;\n");
    fprintf(file, "}\n\n");
    fprintf(file, "char * get%s(int %sNumber) {\n", pascalCase, camelCase);
    fprintf(file, "\treturn load%s(flashHelper, %sNumber);\n", pascalCase, camelCase);
    fprintf(file, "}");
}

void calcMaxLengthOfStrings(const uint8_t datasets, Entry * const * entries, const int * entry_counts,
                            uint16_t * maxLengthOfStrings) {
    for (int i = 0; i < datasets - 1; i++) {
        maxLengthOfStrings[i] = 0;
        for (int j = 0; j < entry_counts[i]; j++) {
            uint16_t intermedLength = strlen(entries[i][j].text);
            if (intermedLength  > maxLengthOfStrings[i]) {
                maxLengthOfStrings[i] = intermedLength;
            }
        }
    }
}
void writeSourceFile(const char * prefix, const uint8_t datasets,
                     Entry * const * entries, const int * entry_counts,
                     const char * camelCase, const char * pascalCase, FILE * file) {

    uint8_t maxIndizes[datasets];
    uint16_t maxLengthOfStrings[datasets];
    calcMaxAmountOfIndexArray(datasets, entries, entry_counts, maxIndizes);

    calcMaxLengthOfStrings(datasets, entries, entry_counts,maxLengthOfStrings);

    fprintf(file, "//#include \"%ss.h\"\n", camelCase);
    fprintf(file, "#include <stdlib.h>\n");
    fprintf(file, "#include <avr/pgmspace.h>\n");
    fprintf(file, "#include \"advent.h\"\n");
    fprintf(file, "#include \"advdec.h\"\n\n");
    for (int i = 0; i < datasets - 1; i++) {
        fprintf(file, "#define %s_DESCRIPTION_%d_LENGTH %d\n", prefix,
                i + 1,  maxLengthOfStrings[i] + 1);
    }
    for (int i = 0; i < datasets - 1; i++) {
        fprintf(file, "#define AMOUNT_OF_%s_DESCRIPTIONS_%d %d\n", prefix,
                i + 1, entry_counts[i]);
    }
    for (int i = 0; i < datasets - 1; i++) {
        fprintf(file, "#define MAX_AMOUNT_OF_%s_DESCRIPTIONS_%d_WITH_SAME_LENGTH %d\n", prefix,
                i + 1,maxIndizes[i]);
    }

    writeProgMemArrays(prefix, datasets, entries, entry_counts, camelCase, file);
    fprintf(file, "\n");

    writeSingleRemainingStrings(prefix, datasets, entries, entry_counts, camelCase, file);
    fprintf(file, "\n");

    writeGetterFunction(prefix, datasets, entries, entry_counts, camelCase, pascalCase, file);
}

void parseDatas(const char ** textsArray, uint16_t amount, const uint16_t * bordersArray, uint8_t amountBorders,
                uint8_t * datasets, Entry *** entries, int ** entry_counts) {
    (*datasets) = amountBorders + 1;
    (*entries) = malloc(*datasets * sizeof(Entry *));
    (*entry_counts) = calloc(*datasets, sizeof(int));
    for (int i = 0; i < (*datasets); i++) {
        (*entries)[i] = malloc(amount * sizeof(Entry));
    }
    for (uint16_t indexOfComputedString = 0; indexOfComputedString < amount; indexOfComputedString++) {

        if (textsArray[indexOfComputedString] == NULL) { continue; }

        uint16_t strLength = strlen(textsArray[indexOfComputedString]);

        uint8_t placed = 0;

        for (int indexOfDataset = 0; indexOfDataset < (*datasets); indexOfDataset++) {
            //placement with string length or as fallback placing in the last dataset
            if ((strLength <= bordersArray[indexOfDataset]) || (*datasets) - 1 == indexOfDataset) {
                char * convertedString = addEscapeCharsToNonPrintableChars(textsArray[indexOfComputedString]);
                for (int indexOfEntryInTheDataset = 0;
                     indexOfEntryInTheDataset < (*entry_counts)[indexOfDataset]; indexOfEntryInTheDataset++) {

                    if (strcmp(convertedString, (*entries)[indexOfDataset][indexOfEntryInTheDataset].text) == 0) {
                        (*entries)[indexOfDataset][indexOfEntryInTheDataset].indices[(*entries)[indexOfDataset][indexOfEntryInTheDataset].index_count++] =
                                indexOfComputedString + 1;
                        placed = 1;
                        break;
                    }
                }
                if (!placed) {

                    Entry entry = {
                            .text = convertedString,
                            .index_count = 1,
                            .indices = {indexOfComputedString + 1}
                    };
                    (*entries)[indexOfDataset][(*entry_counts)[indexOfDataset]++] = entry;
                }
                break;
            }
        }
    }
}

void convertTextArrayToProgMemTextFiles(const char ** textsArray, uint16_t amount, const uint16_t bordersArray[],
                                        uint8_t amountBorders, char * prefix) {
    uint8_t datasets;
    Entry ** entries;
    int * entry_counts;

    parseDatas(textsArray, amount, bordersArray, amountBorders, &datasets, &entries, &entry_counts);

    //write the file into the /src directory, if this is getting started from a subfolder of project root /
    char * fileNameC = createFileName(SRC_DIR, prefix, FILE_EXTENSION);
    char * camelCase = toCamelCase(prefix);
    char * pascalCase = malloc(strlen(camelCase) + 1);
    strcpy(pascalCase, camelCase);
    pascalCase[0] = toupper(camelCase[0]);


    FILE * file = fopen(fileNameC, "w");
    if (file == NULL) {
        free(fileNameC);
        free(camelCase);
        free(pascalCase);
        for (int i = 0; i < amountBorders; i++) { free(entries[i]); }
        free(entries);
        free(entry_counts);
        printf("Error opening file!\n");
        return;
    }

    writeSourceFile(prefix, datasets, entries, entry_counts, camelCase, pascalCase, file);


    fclose(file);
    free(fileNameC);
    free(camelCase);
    free(pascalCase);
    for (int i = 0; i < amountBorders; i++) { free(entries[i]); }
    free(entries);
    free(entry_counts);
}