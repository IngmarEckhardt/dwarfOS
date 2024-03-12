#include <pgm_textfile_generator.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>


typedef struct {
    uint8_t indices[UINT8_MAX];
    const char * text;
    int index_count;
} AdventEntry;


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
    if (stringToReturn == NULL) {return NULL; }

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
    char * stringToReturn = malloc(lengthPrefix + lengthAdditionalPrefix + 8); // 8 extra chars for "../", "/", "/", "s.", and null terminator
    if (stringToReturn == NULL) { return NULL; }

    strcpy(stringToReturn, "../");
    strcat(stringToReturn, additionalPrefix);
    strcat(stringToReturn, "/");
    strcat(stringToReturn, prefix);
    strcat(stringToReturn, "/");
    for (uint16_t i = 0; i < lengthPrefix; i++) {stringToReturn[i + lengthAdditionalPrefix + 4] = tolower(prefix[i]);} // 4 extra chars for "../"
    stringToReturn[lengthPrefix + lengthAdditionalPrefix + 4] = 's'; // 4 extra chars for "../"
    stringToReturn[lengthPrefix + lengthAdditionalPrefix + 5] = '.';
    stringToReturn[lengthPrefix + lengthAdditionalPrefix + 6] = fileEnding;
    stringToReturn[lengthPrefix + lengthAdditionalPrefix + 7] = '\0';
    return stringToReturn;
}

void calcMaxAmountOfIndexArray(const uint8_t datasets, AdventEntry * const * entries, const int * entry_counts,
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

void writeProgMemArrays(const char * prefix, const uint8_t datasets, AdventEntry * const * entries, const int * entry_counts,
                        const char * camelCase, FILE * file) {
    for (int i = 0; i < datasets - 1; i++) {
        fprintf(file, "\ntypedef struct {\n");
        fprintf(file, "    uint8_t numbers[MAX_AMOUNT_OF_%s_DESCRIPTIONS_%d_WITH_SAME_LENGTH];\n", prefix, i + 1);
        fprintf(file, "    char stringInProgramMem[%s_DESCRIPTION_%d_LENGTH];\n", prefix, i + 1);
        fprintf(file, "} %s_%d;\n\n", prefix, i + 1);
        fprintf(file, "#ifndef CCA_TEST\n");
        fprintf(file, "const %s_%d %ss_%d[AMOUNT_OF_%s_DESCRIPTIONS_%d] PROGMEM = {\n", prefix, i + 1, camelCase, i + 1,
                prefix, i + 1);
        fprintf(file, "#else\n");
        fprintf(file, "const %s_%d %ss_%d[AMOUNT_OF_%s_DESCRIPTIONS_%d] = {\n", prefix, i + 1, camelCase, i + 1,
                prefix, i + 1);
        fprintf(file, "#endif\n");

        for (int j = 0; j < entry_counts[i]; j++) {
            fprintf(file, "\t\t{{");
            // print indizes
            for (int k = 0; k < entries[i][j].index_count; k++) {fprintf(file, "%d,", entries[i][j].indices[k]); }
            // print String
            fprintf(file, "},\"%s\"},\n", entries[i][j].text);
        }
        fprintf(file, "};\n");
    }
}

void writeSingleRemainingStrings(const char * prefix, const uint8_t datasets, AdventEntry * const * entries,
                                 const int * entry_counts, const char * camelCase, FILE * file) {
    for (int i = 0; i < entry_counts[datasets - 1]; i++) {
        fprintf(file, "#define %s_DESCRIPTION_%d_LENGTH %u\n", prefix, entries[datasets - 1][i].indices[0],
                strlen(entries[datasets - 1][i].text));
    }
    fprintf(file, "\n");
    for (int j = 0; j < entry_counts[datasets - 1]; j++) {
        fprintf(file, "#ifndef CCA_TEST\n");
        fprintf(file, "const char %s_%d[%s_DESCRIPTION_%d_LENGTH] PROGMEM = \"%s\";\n", camelCase,
                entries[datasets - 1][j].indices[0], prefix, entries[datasets - 1][j].indices[0],
                entries[datasets - 1][j].text);
        fprintf(file, "#else\n");
        fprintf(file, "const char %s_%d[%s_DESCRIPTION_%d_LENGTH] = \"%s\";\n", camelCase,
                entries[datasets - 1][j].indices[0], prefix, entries[datasets - 1][j].indices[0],
                entries[datasets - 1][j].text);
        fprintf(file, "#endif\n");
    }
}

void writeGetterFunction(const char * prefix, const uint8_t datasets, AdventEntry * const * entries,
                         const int * entry_counts, const char * camelCase, const char * camelCaseCopy, FILE * file) {
    fprintf(file, "char * load%s(StringRepository * stringRepository, FlashHelper * flashHelper, uint8_t %sNumber) {\n", camelCaseCopy, camelCase);
    fprintf(file, "    char * stringToReturn = NULL;\n\n");
    for (int j = 0; j < entry_counts[datasets - 1]; j++) {
        fprintf(file, "    if (%sNumber == %d) {\n", camelCase, entries[datasets - 1][j].indices[0]);
        fprintf(file, "        stringToReturn = (char *) malloc(%s_DESCRIPTION_%d_LENGTH);\n", prefix,
                entries[datasets - 1][j].indices[0]);
        fprintf(file, "        flashHelper->loadFromFlash(stringToReturn, %s_%d);\n", camelCase,
                entries[datasets - 1][j].indices[0]);
        fprintf(file, "    }\n");
    }
    for (int i = 0; i < datasets - 1; i++) {
        fprintf(file, "    if (stringToReturn != NULL) { return stringToReturn; }\n\n");
        fprintf(file, "    stringToReturn = stringRepository->loadStringFromFile(&(TextFile) {\n");
        fprintf(file, "            .entries = (void *) %ss_%d,\n", camelCase, i + 1);
        fprintf(file, "            .maxLengthOfStrings = %s_DESCRIPTION_%d_LENGTH,\n", prefix, i + 1);
        fprintf(file, "            .sizeOfIndexArray = MAX_AMOUNT_OF_%s_DESCRIPTIONS_%d_WITH_SAME_LENGTH,\n", prefix,
                i + 1);
        fprintf(file, "            .amountOfEntries = AMOUNT_OF_%s_DESCRIPTIONS_%d,\n", prefix, i + 1);
        fprintf(file, "    }, flashHelper, %sNumber);\n", camelCase);

    }
    fprintf(file, "\n");
    fprintf(file, "\treturn stringToReturn;\n");
    fprintf(file, "}\n");

    fprintf(file, "\n");
    fprintf(file, "char * get%s(int %sNumber) {\n", camelCaseCopy, camelCase);
    fprintf(file, "#ifndef CCA_TEST\n");
    fprintf(file, "    return load%s(stringRepository, flashHelper, %sNumber);\n", camelCaseCopy, camelCase);
    fprintf(file, "#else\n");
    fprintf(file, "    StringRepository * stringRepository = dOS_initStringRepository(0);\n");
    fprintf(file, "    FlashHelper * flashHelper = dOS_initFlashHelper();\n");
    fprintf(file, "    char * result = load%s(stringRepository, flashHelper, %sNumber);\n", camelCaseCopy, camelCase);
    fprintf(file, "    free(stringRepository);\n");
    fprintf(file, "    free(flashHelper);\n");
    fprintf(file, "    return result;\n");
    fprintf(file, "#endif\n");
    fprintf(file, "}\n");
}

void writeSourceFile(const uint16_t * bordersArray, const char * prefix, const uint8_t datasets,
                     AdventEntry * const * entries, const int * entry_counts,
                     const char * camelCase, const char * pascalCase, FILE * file) {
    uint8_t maxIndizes[datasets];
    calcMaxAmountOfIndexArray(datasets, entries, entry_counts, maxIndizes);

    fprintf(file, "#include <stdlib.h>\n");
    fprintf(file, "#include <stdint.h>\n");
    fprintf(file, "#ifndef CCA_TEST\n");
    fprintf(file, "#include <avr/pgmspace.h>\n");
    fprintf(file, "#include <advent.h>\n");
    fprintf(file, "#include <advdec.h>\n");
    fprintf(file, "#else\n");
    fprintf(file, "#include <string_repository.h>\n");
    fprintf(file, "#endif\n\n");

        for (int i = 0; i < datasets - 1; i++) {fprintf(file, "#define %s_DESCRIPTION_%d_LENGTH %d\n", prefix, i + 1, bordersArray[i]+1);}
    for (int i = 0; i < datasets - 1; i++) {fprintf(file, "#define AMOUNT_OF_%s_DESCRIPTIONS_%d %d\n", prefix, i + 1, entry_counts[i]);}
    for (int i = 0; i < datasets - 1; i++) {fprintf(file, "#define MAX_AMOUNT_OF_%s_DESCRIPTIONS_%d_WITH_SAME_LENGTH %d\n", prefix, i + 1, maxIndizes[i]);}

    writeProgMemArrays(prefix, datasets, entries, entry_counts, camelCase, file);
    fprintf(file, "\n");

    writeSingleRemainingStrings(prefix, datasets, entries, entry_counts, camelCase, file);
    fprintf(file, "\n");

    writeGetterFunction(prefix, datasets, entries, entry_counts, camelCase, pascalCase, file);
}

void parseDatas(const char * const * textsArray, uint16_t amount, const uint16_t * bordersArray, uint8_t amountBorders,
                uint8_t * datasets, AdventEntry *** entries, int ** entry_counts) {
    (*datasets) = amountBorders + 1;
    (*entries) = malloc(*datasets * sizeof(AdventEntry *));
    (*entry_counts) = calloc(*datasets, sizeof(int));
    for (int i = 0; i < (*datasets); i++) {
        (*entries)[i] = malloc(amount * sizeof(AdventEntry));
    }
    for (uint16_t indexOfComputedString = 0; indexOfComputedString < amount; indexOfComputedString++) {

        if (textsArray[indexOfComputedString] == NULL) { continue; }

        uint16_t strLength = strlen(textsArray[indexOfComputedString]);

        uint8_t placed = 0;

        for (int indexOfDataset = 0; indexOfDataset < (*datasets); indexOfDataset++) {

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

                    AdventEntry entry = {
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

void convertTextArrayToProgMemTextFiles(const char * textsArray[], uint16_t amount, const uint16_t bordersArray[],
                                        uint8_t amountBorders, char * prefix) {
    uint8_t datasets;
    AdventEntry ** entries;
    int * entry_counts;
    parseDatas(textsArray, amount, bordersArray, amountBorders, &datasets, &entries, &entry_counts);

    char * fileNameC = createFileName("src", prefix, 'c');
    char * camelCase = toCamelCase(prefix);
    char * pascalCase = malloc(strlen(camelCase) + 1);
    strcpy(pascalCase, camelCase);
    pascalCase[0] = toupper(camelCase[0]);


    FILE * file = fopen(fileNameC, "w");
    if (file == NULL) {
        printf("Error opening file!\n");
        return;
    }
    writeSourceFile(bordersArray, prefix, datasets, entries, entry_counts, camelCase, pascalCase, file);
    fclose(file);

    for (int i = 0; i < amountBorders; i++) {free(entries[i]);}
    free(entries);
    free(entry_counts);
    free(fileNameC);
    free(camelCase);
    free(pascalCase);
}