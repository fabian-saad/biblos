#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include <cjson/cJSON.h>
#include "parser.h"

static void print_usage(FILE *stream, const char *program)
{
    fprintf(stream,
            "Usage: %s [-e|-r] [-t translation]... [-v] passage...\n"
            "       %s -h\n"
            "       %s -T\n",
            program, program, program);

    fprintf(stream, "Default translation: Schlachter (German)\n\n");

    fprintf(stream,
            "Options:\n"
            "  -h                 Print usage\n"
            "  -e                 Print extended reference\n"
            "  -r                 No reference at the end\n"
            "  -t translation     Select translation (can be repeated)\n"
            "  -T                 List all available translations\n"
            "  -v                 No verse reference in text\n");
}

int main(int argc, char *argv[])
{

    char translations[100][20];
    int translationCount = 0;

    char *reference = NULL;

    char path[50];

    char book[4];
    int chapter = 1;
    int verse = 1;

    FILE *file = NULL;

    int noReference = 0;
    int noRefInText = 0;
    int longerRef = 0;

    int opt;
    while ((opt = getopt(argc, argv, "t:hrveT")) != -1)
    {
        switch (opt)
        {
        case 't':
            if (optarg == NULL || optarg[0] == '-')
            {
                fprintf(stderr, "Invalid argument for -t\n");
                return EXIT_FAILURE;
            }

            if (translationCount >= 100)
            {
                fprintf(stderr, "Too many translations\n");
                return EXIT_FAILURE;
            }

            char *translationRef = optarg;
            if (!parse_translation(&translationRef, translations[translationCount++]))
            {
                fprintf(stderr, "Unknown translation: %s\n", optarg);
                return EXIT_FAILURE;
            }

            break;
        case 'T':
            print_translations();
            return EXIT_SUCCESS;
        case 'h':
            print_usage(stdout, argv[0]);
            return EXIT_SUCCESS;
        case 'r':
            noReference = 1;
            break;
        case 'v':
            noRefInText = 1;
            break;
        case 'e':
            longerRef = 1;
            break;
        default:
            print_usage(stderr, argv[0]);
            return EXIT_FAILURE;
        }
    }

    if (!translationCount)
    {
        strcpy(translations[0], "deu_sch");
        translationCount = 1;
    }

    if (optind >= argc)
    {
        fprintf(stderr, "Missing bible passage\n");
        print_usage(stderr, argv[0]);
        return EXIT_FAILURE;
    }

    int passagesStart = optind;

    for (int t = 0; t < translationCount; t++)
    {

        char booksPath[100];

        snprintf(booksPath, sizeof(booksPath), "./api/%s/books.json", translations[t]);

        FILE *booksFile = fopen(booksPath, "r");
        if (booksFile == NULL)
        {
            fprintf(stderr, "Could not open 'books.json' for translation %s\n", translations[t]);
            return EXIT_FAILURE;
        }

        fseek(booksFile, 0, SEEK_END);
        long booksFileSize = ftell(booksFile);
        rewind(booksFile);
        if (booksFileSize < 0)
        {
            fprintf(stderr, "Could not determine file size for translation %s\n", translations[t]);
            fclose(booksFile);
            return EXIT_FAILURE;
        }

        char *booksJson = malloc(booksFileSize + 1);

        if (booksJson == NULL)
        {
            fprintf(stderr, "Memory allocation failed for translation %s\n", translations[t]);
            fclose(booksFile);
            return EXIT_FAILURE;
        }

        size_t booksBytesRead = fread(booksJson, 1, booksFileSize, booksFile);
        booksJson[booksBytesRead] = '\0';

        if (booksBytesRead != (size_t)booksFileSize)
        {
            fprintf(stderr, "Could not read books file completely\n");
            free(booksJson);
            fclose(booksFile);
            return EXIT_FAILURE;
        }

        fclose(booksFile);

        cJSON *booksRoot = cJSON_Parse(booksJson);
        free(booksJson);
        if (booksRoot == NULL)
        {
            fprintf(stderr, "Could not parse JSON for translation %s\n", translations[t]);
            return EXIT_FAILURE;
        }

        cJSON *allBooks = cJSON_GetObjectItem(booksRoot, "books");
        if (allBooks == NULL)
        {
            fprintf(stderr, "Missing 'books' for translation %s\n", translations[t]);
            cJSON_Delete(booksRoot);
            return EXIT_FAILURE;
        }

        optind = passagesStart;

        char prevBook[4] = "";
        while (optind < argc)
        {

            reference = argv[optind++];

            int hasBook = parse_book(&reference, book);

            if (hasBook)
            {
                strcpy(prevBook, book);
            }
            else
            {
                if (prevBook[0] == '\0')
                {
                    fprintf(stderr, "Missing/unknown book or missing chapter\n");
                    cJSON_Delete(booksRoot);
                    return EXIT_FAILURE;
                }
                strcpy(book, prevBook);
            }

            int maxChapter = 0;
            cJSON *allBooksObject;
            cJSON_ArrayForEach(allBooksObject, allBooks)
            {
                cJSON *bookId = cJSON_GetObjectItem(allBooksObject, "id");

                if (cJSON_IsString(bookId) && strcasecmp(bookId->valuestring, book) == 0)
                {
                    cJSON *numberOfChapters = cJSON_GetObjectItem(allBooksObject, "numberOfChapters");

                    if (numberOfChapters == NULL || !cJSON_IsNumber(numberOfChapters))
                    {
                        fprintf(stderr, "Missing 'numberOfChapters'\n");
                        cJSON_Delete(booksRoot);
                        return EXIT_FAILURE;
                    }

                    maxChapter = numberOfChapters->valueint;
                    break;
                }
            }

            if (maxChapter == 0)
            {
                fprintf(stderr, "Unknown book: %s\n", book);
                cJSON_Delete(booksRoot);
                return EXIT_FAILURE;
            }

            int i = 0;
            char strChapter[10];
            while (*reference != ':' && *reference != '\0' && i < (int)sizeof(strChapter) - 1)
            {
                strChapter[i++] = *reference++;
            }
            strChapter[i] = '\0';
            chapter = atoi(strChapter);

            if (chapter > maxChapter)
            {
                chapter = maxChapter;
            }

            snprintf(path, sizeof(path), "./api/%s/%s/%d.json", translations[t], book, chapter);

            file = fopen(path, "r");
            if (file == NULL)
            {
                fprintf(stderr, "Missing/unknown book or missing chapter\n");
                return EXIT_FAILURE;
            }

            fseek(file, 0, SEEK_END);
            long fileSize = ftell(file);
            rewind(file);

            if (fileSize < 0)
            {
                fprintf(stderr, "Could not determine file size\n");
                fclose(file);
                return EXIT_FAILURE;
            }

            char *json = malloc(fileSize + 1);
            if (json == NULL)
            {
                fprintf(stderr, "Memory allocation failed\n");
                fclose(file);
                return EXIT_FAILURE;
            }

            size_t bytesRead = fread(json, 1, fileSize, file);
            json[bytesRead] = '\0';

            if (bytesRead != (size_t)fileSize)
            {
                fprintf(stderr, "Could not read file completely\n");
                free(json);
                fclose(file);
                return EXIT_FAILURE;
            }

            fclose(file);

            cJSON *root = cJSON_Parse(json);
            free(json);
            if (root == NULL)
            {
                fprintf(stderr, "Could not parse JSON\n");
                return EXIT_FAILURE;
            }

            cJSON *numberOfVerses = cJSON_GetObjectItem(root, "numberOfVerses");
            if (numberOfVerses == NULL)
            {
                fprintf(stderr, "Missing 'numberOfVerses'\n");
                cJSON_Delete(root);
                return EXIT_FAILURE;
            }
            int maxVerse = numberOfVerses->valueint;

            int firstIteration = 1;
            while (*reference == ',' || firstIteration)
            {

                int toVerse;
                if (*reference == '\0')
                {
                    verse = 1;
                    toVerse = maxVerse;
                }
                else if (*reference == ':' || *reference == ',')
                {
                    reference++;
                    i = 0;
                    char strVerse[10];
                    while (*reference != '-' && *reference != '\0' && *reference != ',' && i < (int)sizeof(strVerse) - 1)
                    {
                        strVerse[i++] = *reference++;
                    }
                    strVerse[i] = '\0';
                    verse = atoi(strVerse);
                    if (verse == 0)
                    {
                        verse = 1;
                    }

                    toVerse = verse;
                    if (*reference == '-')
                    {

                        reference++;

                        i = 0;
                        char strToVerse[10];
                        while (*reference != '\0' && *reference != ',' && i < (int)sizeof(strToVerse) - 1)
                        {
                            strToVerse[i++] = *reference++;
                        }
                        strToVerse[i] = '\0';
                        toVerse = atoi(strToVerse);
                        if (strToVerse[0] == '\0')
                        {
                            toVerse = maxVerse;
                        }
                    }

                    if (toVerse > maxVerse)
                    {
                        toVerse = maxVerse;
                    }
                }
                else
                {
                    fprintf(stderr, "Invalid passage format\n");
                    return EXIT_FAILURE;
                }

                if (verse > maxVerse)
                {
                    verse = maxVerse;
                }

                cJSON *chapterObject = cJSON_GetObjectItem(root, "chapter");
                if (chapterObject == NULL)
                {
                    fprintf(stderr, "Missing 'chapter'\n");
                    cJSON_Delete(root);
                    return EXIT_FAILURE;
                }

                cJSON *content = cJSON_GetObjectItem(chapterObject, "content");
                if (content == NULL)
                {
                    fprintf(stderr, "Missing 'content'\n");
                    cJSON_Delete(root);
                    return EXIT_FAILURE;
                }

                cJSON *bookObject = cJSON_GetObjectItem(root, "book");
                if (bookObject == NULL)
                {
                    fprintf(stderr, "Missing 'book'\n");
                    cJSON_Delete(root);
                    return EXIT_FAILURE;
                }

                cJSON *bookName = cJSON_GetObjectItem(bookObject, "name");
                if (bookName == NULL)
                {
                    fprintf(stderr, "Missing book 'name'\n");
                    cJSON_Delete(root);
                    return EXIT_FAILURE;
                }

                cJSON *translationObject = cJSON_GetObjectItem(root, "translation");
                if (translationObject == NULL)
                {
                    fprintf(stderr, "Missing 'translation'\n");
                    cJSON_Delete(root);
                    return EXIT_FAILURE;
                }

                cJSON *translationShortName = cJSON_GetObjectItem(translationObject, "shortName");
                if (translationShortName == NULL)
                {
                    fprintf(stderr, "Missing translation 'shortName'\n");
                    cJSON_Delete(root);
                    return EXIT_FAILURE;
                }

                cJSON *translationLongName = cJSON_GetObjectItem(translationObject, "name");
                if (translationLongName == NULL)
                {
                    fprintf(stderr, "Missing translation 'name'\n");
                    cJSON_Delete(root);
                    return EXIT_FAILURE;
                }

                cJSON *languageName = cJSON_GetObjectItem(translationObject, "languageName");
                if (languageName == NULL)
                {
                    fprintf(stderr, "Missing translation 'languageName'\n");
                    cJSON_Delete(root);
                    return EXIT_FAILURE;
                }

                printf("\n");
                for (int i = verse; i <= toVerse; i++)
                {
                    cJSON *verseObject;
                    cJSON_ArrayForEach(verseObject, content)
                    {

                        cJSON *number = cJSON_GetObjectItem(verseObject, "number");

                        if (cJSON_IsNumber(number) && number->valueint == i)
                        {

                            cJSON *verseContent = cJSON_GetObjectItem(verseObject, "content");
                            if (verseContent == NULL)
                            {
                                fprintf(stderr, "Missing verse content\n");
                                cJSON_Delete(root);
                                return EXIT_FAILURE;
                            }

                            cJSON *verseText = NULL;
                            cJSON *contentObject = NULL;

                            cJSON_ArrayForEach(contentObject, verseContent)
                            {
                                if (cJSON_IsString(contentObject))
                                {
                                    verseText = contentObject;
                                    break;
                                }

                                if (cJSON_IsObject(contentObject))
                                {
                                    cJSON *text = cJSON_GetObjectItem(contentObject, "text");

                                    if (cJSON_IsString(text))
                                    {
                                        verseText = text;
                                        break;
                                    }
                                }
                            }

                            if (verseText == NULL)
                            {
                                fprintf(stderr, "No text found for verse %d\n", i);
                                cJSON_Delete(root);
                                return EXIT_FAILURE;
                            }

                            if (!noRefInText)
                            {
                                printf("%d ", i);
                            }

                            printf("%s ", verseText->valuestring);

                            break;
                        }
                    }
                }

                if (!noReference)
                {
                    printf("(%s %d", bookName->valuestring, chapter);
                    if ((verse != 1 || toVerse != maxVerse) || longerRef)
                    {
                        printf(":%d", verse);
                        if (verse != toVerse)
                        {
                            printf("-%d", toVerse);
                        }
                    }
                    if (longerRef)
                    {
                        printf(", %s, %s)", translationLongName->valuestring, languageName->valuestring);
                    }
                    else
                    {
                        printf(", %s)", translationShortName->valuestring);
                    }
                }
                printf("\n");

                firstIteration = 0;
            }
            cJSON_Delete(root);
        }
        cJSON_Delete(booksRoot);
        if (t != translationCount - 1)
        {
            printf("\n---\n");
        }
        else
        {
            printf("\n");
        }
    }
}