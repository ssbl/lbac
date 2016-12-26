#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int lookahead;
void expression(void);

void
getch(void)
{
    lookahead = getchar();
}

void
skip_white(void)
{
    while (lookahead == ' ' || lookahead == '\t') {
        getch();
    }
}

void
error(const char *errstr)
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
expected(const char *exp)
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
        skip_white();
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

char *
getname(void)
{
    int i = 0;
    static char token[32];

    memset(token, 0, 32);
    if (!isalpha(lookahead)) {
        expected("name");
    } 

    while (isalnum(lookahead)) {
        token[i++] = lookahead;
        getch();
    }

    token[i] = 0;
    skip_white();

    return token;
}

char *
getnumber(void)
{
    int i = 0;
    static char number[32];

    memset(number, 0, 32);
    if (!isdigit(lookahead)) {
        expected("number");
    } 

    while (isdigit(lookahead)) {
        number[i++] = lookahead;
        getch();
    }

    number[i] = 0;
    skip_white();

    return number;
}

void
emit(const char *s)
{
    printf("\t%s", s);
}

void
emitln(const char *s)
{
    emit(s);
    printf("\n");
}

void
init(void)
{
    getch();
    skip_white();
}

void
ident(void)
{
    char *name = getname();
    char outputstr[64];

    if (lookahead == '(') {
        match('(');
        match(')');

        snprintf(outputstr, 64, "BSR %s", name);
        emitln(outputstr);
    } else {
        snprintf(outputstr, 64, "MOVE %s(PC),D0", name);
        emitln(outputstr);
    }
}

void
factor(void)
{
    char outputstr[64];

    if (lookahead == '(') {
        match('(');
        expression();
        match(')');
    } else if (isalpha(lookahead)) {
        ident();
    } else {
        snprintf(outputstr, 64, "MOVE #%s,D0", getnumber());
        emitln(outputstr);
    }
}

void
multiply(void)
{
    match('*');
    factor();
    emitln("MULS (SP)+,D0");
}

void
divide(void)
{
    match('/');
    factor();
    emitln("MOVE (SP)+,D1");
    emitln("DIVS D1,D0");
}

void
term(void)
{
    factor();

    while (lookahead == '*' || lookahead == '/') {
        emitln("MOVE D0,-(SP)");
        switch (lookahead) {
        case '*':
            multiply();
            break;
        case '/':
            divide();
            break;
        default:
            expected("mulop");
        }
    }
}

void
add(void)
{
    match('+');
    term();
    emitln("ADD (SP)+,D0");
}

void
subtract(void)
{
    match('-');
    term();
    emitln("SUB (SP)+,D0");
    emitln("NEG D0");
}

void
expression(void)
{
    if (isaddop(lookahead)) {
        emitln("CLR D0");
    } else {
        term();
    }

    while (isaddop(lookahead)) {
        emitln("MOVE D0,-(SP)");

        switch (lookahead) {
        case '+':
            add();
            break;
        case '-':
            subtract();
            break;
        default:
            expected("addop");
        }
    }
}

void
assignment(void)
{
    char outputstr[64];
    char *name = getname();

    match('=');
    expression();

    snprintf(outputstr, 64, "LEA %s(PC),A0", name);
    emitln(outputstr);
    emitln("MOVE D0,(A0)");
}

int
main(void)
{
    init();
    assignment();
    if (lookahead != '\n') {
        expected("newline");
    }

    return 0;
}
