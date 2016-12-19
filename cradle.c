#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TAB "    "

int lookahead;
void expression(void);

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

int is_addop(int c)
{
    return (c == '+' || c == '-');
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

void factor()
{
    char c, outputstr[64];

    if (lookahead == '(') {
        match('(');
        expression();
        match(')');
    } else {
        snprintf(outputstr, 64, "MOVE #%c,D0", getnumber(c));
        emitln(outputstr);
    }
}

void multiply()
{
    match('*');
    factor();
    emitln("MULS (SP)+,D0");
}

void divide()
{
    match('/');
    factor();
    emitln("MOVE (SP)+,D1");
    emitln("DIVS D1,D0");
}

void term()
{
    factor();

    while (lookahead == '*' || lookahead == '/') {
        emitln("MOVE D0,-(SP)");
        switch(lookahead) {
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

void add()
{
    match('+');
    term();
    emitln("ADD (SP)+,D0");
}

void subtract()
{
    match('-');
    term();
    emitln("SUB (SP)+,D0");
    emitln("NEG D0");
}

void expression()
{
    if (is_addop(lookahead)) {
        emitln("CLR D0");
    } else {
        term();
    }

    while (is_addop(lookahead)) {
        emitln("MOVE D0,-(SP)");

        switch(lookahead) {
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

int
main(void)
{
    init();
    expression();

    return 0;
}
