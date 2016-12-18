#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TAB "    "

int lookahead;

void getch()
{
    lookahead = getchar();
}

void error(char *errstr)
{
    assert(errstr);

    fprintf(stderr, "error: %s", errstr);
}

void halt(char *errstr)
{
    assert(errstr);

    error(errstr);
    exit(EXIT_FAILURE);
}

void expected(char *exp)
{
    char expected_str[32];

    snprintf(expected_str, 32, "%s expected\n", exp);
    halt(expected_str);
}

void match(int c)
{
    char expected_str[4];

    if (lookahead == c) {
        getch();
    } else {
        snprintf(expected_str, 4, "`%c'", c);
        expected(expected_str);
    }
}

int is_alpha(int c)
{
    return isalpha(c);
}

int is_digit(int c)
{
    return isdigit(c);
}

int getname()
{
    int c;

    if (!is_alpha(lookahead)) {
        expected("name");
    } else {
        c = lookahead;
        getch();
        return c;
    }
}

int getnumber()
{
    int c;

    if (!is_digit(lookahead)) {
        expected("number");
    } else {
        c = lookahead;
        getch();
        return c;
    }
}

void emit(char *s)
{
    printf("%s%s", TAB, s);
}

void emitln(char *s)
{
    emit(s);
    printf("\n");
}

void init()
{
    getch();
}

void expression()
{
    char c, outputstr[64];

    snprintf(outputstr, 64, "MOVE #%c,D0", getnumber(c));
    emitln(outputstr);
}

int
main(void)
{
    init();
    expression();

    return 0;
}
