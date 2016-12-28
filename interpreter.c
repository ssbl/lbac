#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

int lookahead, table[26] = {0};
int expression(void);

void
getch(void)
{
    lookahead = getchar();
}

void
error(char *errstr)
{
    assert(errstr);

    fprintf(stderr, "error: %s\n", errstr);
}

void
halt(char *errstr)
{
    assert(errstr);

    error(errstr);
    exit(EXIT_FAILURE);
}

void
expected(char *exp)
{
    assert(exp);

    char expected_str[32];

    snprintf(expected_str, 32, "%s expected", exp);
    halt(expected_str);
}

void
match(int c)
{
    char expected_str[4];

    if (lookahead == c) {
        getch();
    } else {
        snprintf(expected_str, 4, "`%c'", c);
        expected(expected_str);
    }
}

int
isaddop(int c)
{
    return (c == '+' || c == '-');
}

int
getname(void)
{
    int c = 0;

    if (!isalpha(lookahead)) {
        expected("name");
    } else {
        c = toupper(lookahead);
        getch();
    }

    return c;
}

int
getnumber(void)
{
    int c = 0;

    if (!isdigit(lookahead)) {
        expected("number");
    }
    while (isdigit(lookahead)) {
        c = 10 * c + lookahead - '0';
        getch();
    }

    return c;
}

void
init(void)
{
    getch();
}

int
factor(void)
{
    int value;

    if (lookahead == '(') {
        match('(');
        value = expression();
        match(')');
    } else if (isalpha(lookahead)) {
        value = table[getname() - 'A'];
    } else {
        value = getnumber();
    }

    return value;
}

int
term(void)
{
    int value = factor();

    while (lookahead == '*' || lookahead == '/') {
        switch (lookahead) {
        case '*':
            match('*');
            value *= factor();
            break;
        case '/':
            match('/');
            value /= factor();
            break;
        default:
            expected("mulop");
        }
    }

    return value;
}

void
newline(void)
{
    if (lookahead == '\r') {
        getch();
    }
    if (lookahead == '\n') {
        getch();
    }
}

void
input(void)
{
    match('?');
    table[getname() - 'A'] = getchar();
}

void
output(void)
{
    match('!');
    printf("%d\n", table[getname() - 'A']);
}

int
expression(void)
{
    int value;

    if (isaddop(lookahead)) {
        value = 0;
    } else {
        value = term();
    }

    while (isaddop(lookahead)) {
        switch (lookahead) {
        case '+':
            match('+');
            value += term();
            break;
        case '-':
            match('-');
            value -= term();
            break;
        default:
            expected("addop");
        }
    }

    return value;
}

void
assignment(void)
{
    char name = getname();

    match('=');
    table[name - 'A'] = expression();
}

int
main(void)
{
    init();

    while (lookahead != '.') {
        switch (lookahead) {
        case '?':
            input(); break;
        case '!':
            output(); break;
        default:
            assignment();
        }
        newline();
    }

    return 0;
}
