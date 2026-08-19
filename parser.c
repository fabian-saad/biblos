#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include <cjson/cJSON.h>
#include "parser.h"

static const Alias bookAliases[] = {

    // Old Testament
    {"GEN", {"1m", "ge", NULL}},
    {"EXO", {"2m", "ex", NULL}},
    {"LEV", {"3m", "le", NULL}},
    {"NUM", {"4m", "nu", NULL}},
    {"DEU", {"5m", "de", NULL}},
    {"JOS", {"jos", NULL}},
    {"JDG", {"ri", "judg", "jdg", NULL}},
    {"RUT", {"ru", NULL}},
    {"1SA", {"1s", NULL}},
    {"2SA", {"2s", NULL}},
    {"1KI", {"1ki", NULL}},
    {"2KI", {"2ki", NULL}},
    {"1CH", {"1ch", NULL}},
    {"2CH", {"2ch", NULL}},
    {"EZR", {"esr", "ezr", NULL}},
    {"NEH", {"ne", NULL}},
    {"EST", {"est", NULL}},
    {"JOB", {"hi", "job", NULL}},
    {"PSA", {"ps", NULL}},
    {"PRO", {"sp", "pr", NULL}},
    {"ECC", {"pre", "koh", "ec", NULL}},
    {"SNG", {"hl", "hoh", "so", "sn", NULL}},
    {"ISA", {"jes", "i", NULL}},
    {"JER", {"jer", NULL}},
    {"LAM", {"kl", "la", NULL}},
    {"EZK", {"hes", "eze", "ezk", NULL}},
    {"DAN", {"da", NULL}},
    {"HOS", {"ho", NULL}},
    {"JOL", {"joe", "jol", NULL}},
    {"AMO", {"am", NULL}},
    {"OBA", {"ob", NULL}},
    {"JON", {"jon", NULL}},
    {"MIC", {"mi", NULL}},
    {"NAM", {"na", NULL}},
    {"HAB", {"hab", NULL}},
    {"ZEP", {"zep", NULL}},
    {"HAG", {"hag", NULL}},
    {"ZEC", {"sa", "zec", NULL}},
    {"MAL", {"mal", NULL}},

    // New Testament
    {"MAT", {"mt", "mat", NULL}},
    {"MRK", {"mk", "mar", NULL}},
    {"LUK", {"lk", "lu", NULL}},
    {"JHN", {"jn", "jh", "joh", NULL}},
    {"ACT", {"apos", "apg", "ac", NULL}},
    {"ROM", {"rö", "ro", NULL}},
    {"1CO", {"1ko", "1co", NULL}},
    {"2CO", {"2ko", "2co", NULL}},
    {"GAL", {"ga", NULL}},
    {"EPH", {"ep", NULL}},
    {"PHP", {"phili", "php", NULL}},
    {"COL", {"c", "kol", NULL}},
    {"1TH", {"1th", NULL}},
    {"2TH", {"2th", NULL}},
    {"1TI", {"1ti", NULL}},
    {"2TI", {"2ti", NULL}},
    {"TIT", {"t", NULL}},
    {"PHM", {"ph", NULL}},
    {"HEB", {"he", NULL}},
    {"JAS", {"ja", NULL}},
    {"1PE", {"1p", NULL}},
    {"2PE", {"2p", NULL}},
    {"1JN", {"1j", NULL}},
    {"2JN", {"2j", NULL}},
    {"3JN", {"3j", NULL}},
    {"JUD", {"juda", "jude", NULL}},
    {"REV", {"of", "apok", "apk", "re", NULL}}

    // Deuterocanonical/Apocrypha (maybe in future)
};

const int bookAliasCount = sizeof(bookAliases) / sizeof(bookAliases[0]);

int parse_book(char **reference, char *book)
{
    for (int i = 0; i < bookAliasCount; i++)
    {
        for (int j = 0; bookAliases[i].aliases[j] != NULL; j++)
        {
            size_t len = strlen(bookAliases[i].aliases[j]);

            if (strncasecmp(*reference, bookAliases[i].aliases[j], len) == 0)
            {
                strcpy(book, bookAliases[i].code);

                *reference += len;

                while (**reference != '\0' && !isdigit((unsigned char)**reference))
                {
                    (*reference)++;
                }

                return 1;
            }
        }
    }
    return 0;
}

static const Alias translationAliases[] = {

    // German
    {"deu_sch", {"s", "ger", "de", NULL}},
    {"deu_l12", {"lu", "l1", NULL}},
    {"deu_elo", {"elo", "unr", NULL}},
    {"deu_tkw", {"ka", "wei", "tk", NULL}},
    {"deu_elbbk", {"elb", NULL}},

    // English
    {"eng_kjv", {"kj", "ki", "en", NULL}},
    {"eng_asv", {"as", "am", NULL}},
    {"eng_dby", {"db", "da", NULL}},
    {"eng_lsv", {"ls", "li", NULL}},
    {"eng_net", {"ne", NULL}},
    {"eng_ylt", {"yl", "yo", NULL}},
    {"eng_bbe", {"bb", "ba", NULL}},
    {"eng_fbv", {"fb", "fr", NULL}},
    {"eng_gnv", {"gn", "gen", NULL}},
    {"eng_msb", {"ms", "ma", "st", NULL}},
    {"eng_ojb", {"oj", "or", "je", "jw", "to", NULL}},
    {"eng_t4t", {"t4", "tr", NULL}},
    {"eng_ulb", {"ul", "un", NULL}},
    {"eng_wbs", {"wb", "no", "nw", NULL}},
    {"eng_wmu", {"wmu", "wmbb", "messianicb", "worldmessianicb", "meb", NULL}},
    {"eng_wmb", {"wm", "me", "worldm", NULL}},
    {"eng_webpb", {"we", "br", "wo", NULL}},

    // Other languages and translations (maybe in future)
};

static const int translationCount = sizeof(translationAliases) / sizeof(translationAliases[0]);

int parse_translation(char **reference, char *translation)
{
    for (int i = 0; i < translationCount; i++)
    {
        for (int j = 0; translationAliases[i].aliases[j] != NULL; j++)
        {
            size_t len = strlen(translationAliases[i].aliases[j]);

            if (strncasecmp(*reference, translationAliases[i].aliases[j], len) == 0)
            {
                strcpy(translation, translationAliases[i].code);

                *reference += len;

                while (**reference != '\0' && **reference != ' ')
                {
                    (*reference)++;
                }

                while (**reference == ' ')
                {
                    (*reference)++;
                }

                return 1;
            }
        }
    }
    return 0;
}

void print_translations(void)
{
    for (int i = 0; i < translationCount; i++)
    {
        char path[100];

        snprintf(path, sizeof(path), "./api/%s/books.json", translationAliases[i].code);

        FILE *file = fopen(path, "r");
        if (file == NULL)
        {
            fprintf(stderr, "Could not open 'books.json' for translation %s\n", translationAliases[i].code);
            exit(EXIT_FAILURE);
        }

        fseek(file, 0, SEEK_END);
        long fileSize = ftell(file);
        rewind(file);
        if (fileSize < 0)
        {
            fprintf(stderr, "Could not determine file size for translation %s\n", translationAliases[i].code);
            fclose(file);
            exit(EXIT_FAILURE);
        }

        char *json = malloc(fileSize + 1);

        if (json == NULL)
        {
            fprintf(stderr, "Memory allocation failed for translation %s\n", translationAliases[i].code);
            fclose(file);
            exit(EXIT_FAILURE);
        }

        size_t bytesRead = fread(json, 1, fileSize, file);
        json[bytesRead] = '\0';

        if (bytesRead != (size_t)fileSize)
        {
            fprintf(stderr, "Could not read file completely\n");
            free(json);
            fclose(file);
            exit(EXIT_FAILURE);
        }

        fclose(file);

        cJSON *root = cJSON_Parse(json);
        free(json);
        if (root == NULL)
        {
            fprintf(stderr, "Could not parse JSON for translation %s\n", translationAliases[i].code);
            exit(EXIT_FAILURE);
        }

        cJSON *translation = cJSON_GetObjectItem(root, "translation");
        if (translation == NULL)
        {
            fprintf(stderr, "Missing 'translation' for translation %s\n", translationAliases[i].code);
            cJSON_Delete(root);
            exit(EXIT_FAILURE);
        }

        cJSON *name = cJSON_GetObjectItem(translation, "name");
        if (name == NULL)
        {
            fprintf(stderr, "Missing 'name' for translation %s\n", translationAliases[i].code);
            cJSON_Delete(root);
            exit(EXIT_FAILURE);
        }

        cJSON *shortName = cJSON_GetObjectItem(translation, "shortName");
        if (shortName == NULL)
        {
            fprintf(stderr, "Missing 'shortName' for translation %s\n", translationAliases[i].code);
            cJSON_Delete(root);
            exit(EXIT_FAILURE);
        }

        cJSON *languageEnglishName = cJSON_GetObjectItem(translation, "languageEnglishName");
        if (languageEnglishName == NULL)
        {
            fprintf(stderr, "Missing 'languageEnglishName' for translation %s\n", translationAliases[i].code);
            cJSON_Delete(root);
            exit(EXIT_FAILURE);
        }

        printf("%s (%s), %s\n", name->valuestring, shortName->valuestring, languageEnglishName->valuestring);

        cJSON_Delete(root);
    }
}