#include <dwarf-os/pgm_textfile_generator.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define SRC_DIR "src"
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

char * toCamelCase(char * allUpperCaseNaming) {
    uint16_t length = strlen(allUpperCaseNaming);
    char * stringToReturn = malloc(length + 1);
    if (stringToReturn == NULL) { return NULL; }

    strcpy(stringToReturn, allUpperCaseNaming);
    int indexToPlace = 0;
    for (uint16_t i = 0; i < length; i++, indexToPlace++) {
        stringToReturn[indexToPlace] = tolower(stringToReturn[i]);
        //we delete the underline
        if (stringToReturn[indexToPlace] == '_') {
            i++;
            stringToReturn[indexToPlace] = toupper(stringToReturn[i]);
        }
    }
    stringToReturn[indexToPlace] = '\0';
    return stringToReturn;
}


char * createFileName(char * prefix, char fileEnding) {
    uint16_t lengthPrefix = strlen(prefix);

    char * stringToReturn = malloc(lengthPrefix + 4);
    if (stringToReturn == NULL) { return NULL; }

    for (uint16_t i = 0; i < lengthPrefix; i++) {
        stringToReturn[i] = tolower(prefix[i]);
    } // 4 extra chars for "../"
    stringToReturn[lengthPrefix++] = 's';
    stringToReturn[lengthPrefix++] = '.';
    stringToReturn[lengthPrefix++] = fileEnding;
    stringToReturn[lengthPrefix] = '\0';
    return stringToReturn;
}

void calcMaxAmountOfIndexArray(const uint8_t datasets, Entry * const * entries, const uint8_t * entry_counts,
                               uint8_t * maxIndizes) {
    for (uint8_t i = 0; i < datasets - 1; i++) {
        maxIndizes[i] = 0;
        for (uint8_t j = 0; j < entry_counts[i]; j++) {
            if (entries[i][j].index_count > maxIndizes[i]) {
                maxIndizes[i] = entries[i][j].index_count;
            }
        }
    }
}

void
writeProgMemArrays(const char * prefix, const uint8_t datasets, Entry * const * entries, const uint8_t * entry_counts,
                   const char * camelCase, uint8_t withIndexArray, FILE * file) {
    for (int i = 0; i < datasets - 1; i++) {
        fprintf(file, "\ntypedef struct {\n");
        if (withIndexArray) {
            fprintf(file, "\tuint8_t numbers[%s_%d_INDEX_ARRAY_SIZE];\n", prefix, i + 1);
        }
        fprintf(file, "\tchar stringInProgramMem[%s_%d_STRING_LENGTH];\n", prefix, i + 1);
        fprintf(file, "} %s_%d;\n\n", prefix, i + 1);
        fprintf(file, "const __attribute__((__progmem__)) %s_%d %ss_%d[AMOUNT_%s_%d_STRINGS] = {\n", prefix,
                i + 1, camelCase, i + 1,
                prefix, i + 1);
        for (int j = 0; j < entry_counts[i]; j++) {
            fprintf(file, "\t\t{");

            if (withIndexArray) {
                fprintf(file, "{");
                // print indizes
                for (int k = 0; k < entries[i][j].index_count; k++) { fprintf(file, "%d,", entries[i][j].indices[k]); }
                fprintf(file, "},");
            }

            // print String
            fprintf(file, "\"%s\"},\n", entries[i][j].text);
        }
        fprintf(file, "};\n");
    }
}

void writeSingleRemainingStrings(const char * prefix, const uint8_t datasets, Entry * const * entries,
                                 const uint8_t * entry_counts, const char * camelCase, FILE * file) {
    for (uint8_t i = 0; i < entry_counts[datasets - 1]; i++) {
        fprintf(file, "#define %s_%d_STRING_LENGTH %u\n",
                prefix,
                entries[datasets - 1][i].indices[0],
                (strlen(entries[datasets - 1][i].text)) + 1);
        fprintf(file, "const __attribute__((__progmem__)) char %s_%d[%s_%d_STRING_LENGTH] = \"%s\";\n",
                camelCase,
                entries[datasets - 1][i].indices[0],
                prefix,
                entries[datasets - 1][i].indices[0],
                entries[datasets - 1][i].text);
        fprintf(file, "\n");
    }
}

void writeGetterFunction(const char * prefix, const uint8_t datasets, Entry * const * entries,
                         const uint8_t * entry_counts, const char * camelCase, const char * pascalCase,
                         uint8_t desiredWithIndexArray, FILE * file) {

    fprintf(file, "#define LOAD_FROM(NUM) \\\n");
    fprintf(file, "\tstringToReturn = helper->createFileString_P(&(TextFile) { \\\n");
    fprintf(file, "\t\t.pointerToArray = addressOf(%ss_##NUM), \\\n", camelCase);
    fprintf(file, "\t\t.maxLengthOfStrings = %s_##NUM##_STRING_LENGTH, \\\n", prefix);
    if (desiredWithIndexArray) {
        fprintf(file, "\t\t.sizeOfIndexArray = %s_##NUM##_INDEX_ARRAY_SIZE, \\\n", prefix);
    } else {
        fprintf(file, "\t\t.sizeOfIndexArray = 0, \\\n");
    }

    fprintf(file, "\t\t.amountOfEntries = AMOUNT_%s_##NUM##_STRINGS, \\\n", prefix);
    fprintf(file, "\t}, %sNumber, helper); \\\n", camelCase);
    fprintf(file, "\tif (stringToReturn != NULL) { return stringToReturn; }\n\n");


    fprintf(file, "#define PUT_FROM(NUM) \\\n");
    fprintf(file, "\tif (helper->putFileString_P(&(TextFile) { \\\n");
    fprintf(file, "\t\t.pointerToArray =  addressOf(%ss_##NUM), \\\n", camelCase);

    fprintf(file, "\t\t.maxLengthOfStrings = %s_##NUM##_STRING_LENGTH, \\\n", prefix);
    if (desiredWithIndexArray) {
        fprintf(file, "\t\t.sizeOfIndexArray = %s_##NUM##_INDEX_ARRAY_SIZE, \\\n", prefix);
    } else {
        fprintf(file, "\t\t.sizeOfIndexArray = 0, \\\n");
    }
    fprintf(file, "\t\t.amountOfEntries = AMOUNT_%s_##NUM##_STRINGS, \\\n", prefix);
    fprintf(file, "\t}, %sNumber, helper) == 0) { return 0;}; \\\n\n", camelCase);


    fprintf(file, "char * load%s(FlashHelper * helper, uint8_t %sNumber) {\n", pascalCase, camelCase);
    fprintf(file, "\tchar * stringToReturn = NULL;\n\n");
    for (int j = 0; j < entry_counts[datasets - 1]; j++) {
        fprintf(file, "\tif (%sNumber == %d) {\n", camelCase,
                entries[datasets - 1][j].indices[0]);
        fprintf(file, "\t\tstringToReturn = (char *) malloc(%s_%d_STRING_LENGTH);\n", prefix,
                entries[datasets - 1][j].indices[0]);
        fprintf(file, "\t\thelper->copyString_P(stringToReturn,  addressOf(%s_%d));\n",
                camelCase, entries[datasets - 1][j].indices[0]);

        fprintf(file, "\t}\n");
    }
    for (uint8_t i = 0; i < datasets - 1; i++) { fprintf(file, "\tLOAD_FROM(%d)\n", i + 1); }
    fprintf(file, "\n");
    fprintf(file, "\treturn stringToReturn;\n");
    fprintf(file, "}\n\n");


    fprintf(file, "int16_t putFileStr%s(FlashHelper * helper, uint8_t %sNumber) {\n", pascalCase, camelCase);
    for (int j = 0; j < entry_counts[datasets - 1]; j++) {
        fprintf(file, "\tif (%sNumber == %d) {\n", camelCase,
                entries[datasets - 1][j].indices[0]);

        fprintf(file, "\t\treturn helper->putString_P(addressOf(%s_%d));\n",
                    camelCase, entries[datasets - 1][j].indices[0]);

        fprintf(file, "\t}\n");
    }
    for (uint8_t i = 0; i < datasets - 1; i++) { fprintf(file, "\tPUT_FROM(%d)\n", i + 1); }
    fprintf(file, "\n");
    fprintf(file, "\treturn -1;\n");
    fprintf(file, "}\n\n");


    fprintf(file, "//getter function expects an initialized instance of FlashHelper as reachable global reference\n");
    fprintf(file, "char * get%s(int %sNumber) {\n", pascalCase, camelCase);
    fprintf(file, "\treturn load%s(flashHelper, %sNumber);\n", pascalCase, camelCase);
    fprintf(file, "}\n");


    fprintf(file, "//getter function expects an initialized instance of FlashHelper as reachable global reference\n");
    fprintf(file, "char * put%s(int %sNumber) {\n", pascalCase, camelCase);
    fprintf(file, "\treturn putFileStr%s(flashHelper, %sNumber);\n", pascalCase, camelCase);
    fprintf(file, "}");
}

void calcMaxLengthOfStrings(const uint8_t datasets, Entry * const * entries, const uint8_t * entry_counts,
                            uint16_t * maxLengthOfStrings) {
    for (uint8_t i = 0; i < datasets - 1; i++) {
        maxLengthOfStrings[i] = 0;
        for (uint8_t j = 0; j < entry_counts[i]; j++) {
            uint16_t intermedLength = strlen(entries[i][j].text);
            if (intermedLength > maxLengthOfStrings[i]) {
                maxLengthOfStrings[i] = intermedLength;
            }
        }
    }
}

// Is there a way how this function or its helpers can fail and should report this?

void writeSourceFile(const char * prefix, const uint8_t datasets,
                     Entry * const * entries, const uint8_t * entry_counts,
                     const char * camelCase, const char * pascalCase, FILE * file, uint8_t desiredWithIndexArray) {

    uint8_t maxIndizes[datasets];
    uint16_t maxLengthOfStrings[datasets];
    calcMaxAmountOfIndexArray(datasets, entries, entry_counts, maxIndizes);

    calcMaxLengthOfStrings(datasets, entries, entry_counts, maxLengthOfStrings);
    char * prefixLowercase = calloc(strlen(prefix) + 1, 1);
    for (uint16_t i = 0; i < strlen(prefix); i++) {
        prefixLowercase[i] = tolower(prefix[i]);
    }
    fprintf(file, "//#include \"%ss.h\"\n", prefixLowercase);
    fprintf(file, "#include <stdlib.h>\n");
    fprintf(file, "#include <dwarf-os/flash_helper.h>\n\n");
    for (uint8_t i = 0; i < datasets - 1; i++) {
        fprintf(file, "#define %s_%d_STRING_LENGTH %d\n", prefix,
                i + 1, maxLengthOfStrings[i] + 1);
    }
    for (uint8_t i = 0; i < datasets - 1; i++) {
        fprintf(file, "#define AMOUNT_%s_%d_STRINGS %d\n", prefix,
                i + 1, entry_counts[i]);
    }
    if (desiredWithIndexArray) {
        for (uint8_t i = 0; i < datasets - 1; i++) {
            fprintf(file, "#define %s_%d_INDEX_ARRAY_SIZE %d\n", prefix,
                    i + 1, maxIndizes[i]);
        }
    }


    writeProgMemArrays(prefix, datasets, entries, entry_counts, camelCase, desiredWithIndexArray, file);
    fprintf(file, "\n");

    writeSingleRemainingStrings(prefix, datasets, entries, entry_counts, camelCase, file);
    fprintf(file, "\n");

    writeGetterFunction(prefix, datasets, entries, entry_counts, camelCase, pascalCase, desiredWithIndexArray, file);
}

// Did we have to clean the allocated memory, work everytime with calloc?

uint8_t parseData(const char ** textsArray, const uint16_t amount, const uint16_t * bordersArray,
                  const uint8_t datasets, Entry *** entries, uint8_t ** entry_counts) {

    (*entries) = malloc(datasets * sizeof(Entry *));
    if ((*entries) == NULL) {
        return 0;
    }
    (*entry_counts) = calloc(datasets, sizeof(uint8_t));
    if ((*entry_counts) == NULL) {
        free(*entries);
        return 0;
    }
    for (uint8_t i = 0; i < datasets; i++) {
        (*entries)[i] = malloc(amount * sizeof(Entry));
        if ((*entries)[i] == NULL) {
            for (uint8_t j = 0; j < i; j++) {
                free((*entries)[j]);
            }
            free(*entries);
            free(*entry_counts);
            return 0;
        }
    }

    //going through the strings
    for (uint16_t indexOfComputedString = 0; indexOfComputedString < amount; indexOfComputedString++) {

        // you can pass Array also without strings at any position
        if (textsArray[indexOfComputedString] == NULL) { continue; }

        uint16_t strLength = strlen(textsArray[indexOfComputedString]);

        uint8_t placed = 0;

        //trying if the string fits into the given maximum size of a dataset, starting with smallest
        for (uint8_t indexOfDataset = 0; indexOfDataset < datasets; indexOfDataset++) {

            //placement with string length or as fallback placing in the last dataset for single very large strings
            if ((strLength <= bordersArray[indexOfDataset]) || datasets - 1 == indexOfDataset) {

                //make sure string operations don't kill the escape characters
                char * convertedString = addEscapeCharsToNonPrintableChars(textsArray[indexOfComputedString]);

                // we look if we already have an identical string parsed before and can just add the additional index-information
                for (uint8_t indexOfEntryInTheDataset = 0;
                     indexOfEntryInTheDataset < (*entry_counts)[indexOfDataset]; indexOfEntryInTheDataset++) {
                    //we found the string
                    if (strcmp(convertedString, (*entries)[indexOfDataset][indexOfEntryInTheDataset].text) == 0) {
                        //add the new index to its index array
                        (*entries)[indexOfDataset][indexOfEntryInTheDataset].indices[(*entries)[indexOfDataset][indexOfEntryInTheDataset].index_count++] =
                                indexOfComputedString + 1;
                        placed = 1;
                        break;
                    }
                }
                // we didn't find the string and create a new Entry
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
    return 1;
}

void convertStringsToPGMTextFile(const char ** arrayWithStrings, uint16_t amountOfStrings,
                                 const uint16_t maxSizesOfStringsInPGMArrays[],
                                 uint8_t desiredAmountPGMArrays, char * desiredNamingAllUpperCase,
                                 uint8_t desiredWithIndexArray) {
    uint8_t datasets = desiredAmountPGMArrays + 1;
    Entry ** entries;
    uint8_t * entry_counts;
    char * fileNameC = createFileName(desiredNamingAllUpperCase, FILE_EXTENSION);
    char * camelCase = toCamelCase(desiredNamingAllUpperCase);
    char * pascalCase = malloc(strlen(camelCase) + 1);

    //try to parse the data, handle all possible fails in allocating memory in one step
    if (!parseData(arrayWithStrings, amountOfStrings, maxSizesOfStringsInPGMArrays, datasets, &entries,
                   &entry_counts) ||
        fileNameC == NULL || camelCase == NULL || pascalCase == NULL) {
        return;
    }
    strcpy(pascalCase, camelCase);
    pascalCase[0] = toupper(camelCase[0]);




    //write the file into the /src directory, if this is getting started from a subfolder of project root /
    FILE * file = fopen(fileNameC, "w");
    if (file == NULL) {
        free(fileNameC);
        free(camelCase);
        free(pascalCase);
        for (uint8_t i = 0; i < datasets; i++) { free(entries[i]); }
        free(entries);
        free(entry_counts);
        printf("Error opening file!\n");
        return;
    }

    writeSourceFile(desiredNamingAllUpperCase, datasets, entries, entry_counts, camelCase, pascalCase, file,
                    desiredWithIndexArray);


    fclose(file);
    free(fileNameC);
    free(camelCase);
    free(pascalCase);
    for (uint8_t i = 0; i < datasets; i++) { free(entries[i]); }
    free(entries);
    free(entry_counts);
}

// ToDo functions are too long, should be seperated into smaller pieces, a algorithm that decides the borders by themself to minimize progmem consumption