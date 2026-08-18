#ifndef PARSER_H
#define PARSER_H

typedef struct {
    const char *code;
    const char *aliases[10];
} Alias;

int parse_book(char **reference, char *book);
int parse_translation(char **reference, char *translation);
void print_translations(void);

#endif