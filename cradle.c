#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

int lookahead;
void expression(void);

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
        c = lookahead;
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
    } else {
        c = lookahead;
        getch();
    }

    return c;
}

void
emit(char *s)
{
    printf("\t%s", s);
}

void
emitln(char *s)
{
    emit(s);
    printf("\n");
}

void
init(void)
{
    getch();
}

void
ident(void)
{
    char name = getname();
    char outputstr[64];

    if (lookahead == '(') {
        match('(');
        match(')');

        snprintf(outputstr, 64, "BSR %c", name);
        emitln(outputstr);
    } else {
        snprintf(outputstr, 64, "MOVE %c(PC),D0", name);
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
        snprintf(outputstr, 64, "MOVE #%c,D0", getnumber());
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
    char name = getname();

    match('=');
    expression();

    snprintf(outputstr, 64, "LEA %c(PC),A0", name);
    emitln(outputstr);
    emitln("MOVE D0,(A0)");
}

int
main(void)
{
    init();
    expression();
    if (lookahead != '\n') {
        expected("newline");
    }

    return 0;
}
