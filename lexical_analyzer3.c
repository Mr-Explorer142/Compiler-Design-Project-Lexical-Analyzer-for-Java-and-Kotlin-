/* File: lexer_interactive_colored.c
   Interactive Java/Kotlin lexical analyzer with pastel colors + minimal animation.

   Compile:
     gcc lexer_interactive_colored.c -o lexer -O2

   Run:
     ./lexer
*/

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h> /* usleep */
#include <time.h>

#define MAX_TOKENS 16000
#define MAX_COMMENTS 6000
#define MAX_ERRORS 6000
#define MAX_DECLS 6000

/* Data structures */
struct Symbol
{
    char token[512];
    int attribute;
    int line;
};
struct Decl
{
    char name[256];
    char type[128];
    int line;
};
struct Error
{
    char msg[512];
    int line;
};

static struct Symbol table[MAX_TOKENS];
static struct Decl decls[MAX_DECLS];
static struct Error errors[MAX_ERRORS];
static char comments[MAX_COMMENTS][1024];

static int tok_count = 0, decl_count = 0, err_count = 0, com_count = 0;

/* Combined Java + Kotlin keywords & types */
static const char *keywords[] = {
    "int", "float", "double", "char", "if", "else", "for", "while", "class",
    "public", "private", "return", "static", "void", "new",
    "fun", "var", "val", "when", "is", "in", "object", "null", "true", "false",
    "package", "import", "override", "data", "sealed", "lateinit",
    "Int", "Float", "Double", "Char", "String", "Boolean", "Long", "Short", "Byte"};
static int keyword_count = sizeof(keywords) / sizeof(keywords[0]);

/* Utilities */
static int min_int(int a, int b) { return a < b ? a : b; }
static int levenshtein(const char *a, const char *b)
{
    int n = (int)strlen(a), m = (int)strlen(b);
    if (n > 300 || m > 300)
        return abs(n - m) + 3;
    static int dp[601][601];
    for (int i = 0; i <= n; i++)
        dp[i][0] = i;
    for (int j = 0; j <= m; j++)
        dp[0][j] = j;
    for (int i = 1; i <= n; i++)
        for (int j = 1; j <= m; j++)
            dp[i][j] = (a[i - 1] == b[j - 1]) ? dp[i - 1][j - 1] : 1 + min_int(dp[i - 1][j - 1], min_int(dp[i - 1][j], dp[i][j - 1]));
    return dp[n][m];
}
static int isKeyword(const char *w)
{
    for (int i = 0; i < keyword_count; i++)
        if (strcmp(w, keywords[i]) == 0)
            return 1;
    return 0;
}
static int similarToKeyword(const char *w)
{
    int L = (int)strlen(w);
    if (L < 3)
        return 0;
    for (int i = 0; i < keyword_count; i++)
        if (levenshtein(w, keywords[i]) <= 2)
            return 1;
    return 0;
}
static int isDeclared(const char *id)
{
    for (int i = 0; i < decl_count; i++)
        if (strcmp(id, decls[i].name) == 0)
            return 1;
    return 0;
}
static const char *getType(const char *id)
{
    for (int i = 0; i < decl_count; i++)
        if (strcmp(id, decls[i].name) == 0)
            return decls[i].type;
    return "UNKNOWN";
}
static int isRelOp(const char *t)
{
    return (!strcmp(t, "<") || !strcmp(t, ">") || !strcmp(t, "<=") || !strcmp(t, ">=") || !strcmp(t, "==") || !strcmp(t, "!="));
}

/* token type checks */
static int isFloatValToken(const char *t) { return strchr(t, '.') != NULL || strchr(t, 'f') || strchr(t, 'F'); }
static int isCharLitToken(const char *t)
{
    int L = (int)strlen(t);
    return (L >= 3 && t[0] == '\'' && t[L - 1] == '\'');
}
static int isStringLitToken(const char *t)
{
    int L = (int)strlen(t);
    return (L >= 2 && t[0] == '"' && t[L - 1] == '"');
}
static int isIntToken(const char *t)
{
    if (!t)
        return 0;
    int L = (int)strlen(t);
    if (L == 0)
        return 0;
    for (int i = 0; i < L; i++)
    {
        if (i == 0 && (t[i] == '+' || t[i] == '-'))
            continue;
        if (!isdigit((unsigned char)t[i]))
            return 0;
    }
    return 1;
}

/* Pastel (soft) color palette using 256-color ANSI codes */
#define COL_RESET "\033[0m"
#define PASTEL_IDENT "\033[38;5;120m"                /* soft green */
#define PASTEL_NUMBER "\033[38;5;159m"               /* soft cyan */
#define PASTEL_OPERATOR "\033[38;5;228m"             /* soft yellow */
#define PASTEL_KEYWORD "\033[38;5;170m"              /* soft magenta */
#define PASTEL_SEP "\033[38;5;246m"                  /* soft gray */
#define PASTEL_STRING "\033[38;5;215m"               /* soft peach */
#define PASTEL_CHAR "\033[38;5;180m"                 /* soft purple */
#define PASTEL_NS "\033[38;5;244m"                   /* slate */
#define PASTEL_COMMENT "\033[38;5;153m"              /* soft blue */
#define PASTEL_ERROR1 "\033[38;5;203m"               /* soft red */
#define PASTEL_ERROR2 "\033[38;5;208m"               /* soft orange */
#define PASTEL_HDR_BG "\033[48;5;236m\033[38;5;225m" /* soft header background/text */

/* map attribute to pastel color and label */
static const char *attrLabel(int a)
{
    switch (a)
    {
    case 1:
        return "KEYWORD";
    case 2:
        return "IDENTIFIER";
    case 3:
        return "NUMBER";
    case 4:
        return "OPERATOR";
    case 5:
        return "SEPARATOR";
    case 6:
        return "STRING";
    case 7:
        return "CHAR";
    case 8:
        return "NAMESPACE";
    default:
        return "UNKNOWN";
    }
}
static const char *attrColor(int a)
{
    switch (a)
    {
    case 1:
        return PASTEL_KEYWORD;
    case 2:
        return PASTEL_IDENT;
    case 3:
        return PASTEL_NUMBER;
    case 4:
        return PASTEL_OPERATOR;
    case 5:
        return PASTEL_SEP;
    case 6:
        return PASTEL_STRING;
    case 7:
        return PASTEL_CHAR;
    case 8:
        return PASTEL_NS;
    default:
        return COL_RESET;
    }
}

/* robust getc/ungetc with newline accounting */
static int getc_nl(FILE *fp, int *line)
{
    int c = fgetc(fp);
    if (c == '\r')
        return getc_nl(fp, line);
    if (c == '\n')
    {
        (*line)++;
        return c;
    }
    return c;
}
static void ungetc_nl(int c, FILE *fp, int *line)
{
    if (c == EOF)
        return;
    if (c == '\r')
        return;
    if (c == '\n')
    {
        if (*line > 1)
            (*line)--;
    }
    ungetc(c, fp);
}

/* helper to record declaration */
static void add_decl(const char *name, const char *type, int line)
{
    if (decl_count >= MAX_DECLS)
        return;
    if (isDeclared(name))
        return;
    strncpy(decls[decl_count].name, name, sizeof(decls[decl_count].name) - 1);
    strncpy(decls[decl_count].type, type, sizeof(decls[decl_count].type) - 1);
    decls[decl_count].line = line;
    decl_count++;
}

/* report error */
static void report_error(const char *msg, int line)
{
    if (err_count >= MAX_ERRORS)
        return;
    strncpy(errors[err_count].msg, msg, sizeof(errors[err_count].msg) - 1);
    errors[err_count].line = line;
    err_count++;
}

/* check assignment type compatibility and report E1 if mismatch */
static void check_assignment_type(const char *decl_type, const char *value_token, int line, const char *varname)
{
    if (!decl_type || strcmp(decl_type, "UNKNOWN") == 0)
        return;
    char dtype[128];
    strncpy(dtype, decl_type, sizeof(dtype) - 1);
    dtype[sizeof(dtype) - 1] = 0;
    int L = (int)strlen(dtype);
    if (L > 0 && dtype[L - 1] == '?')
        dtype[L - 1] = 0; /* nullable strip */

    if (strcmp(dtype, "int") == 0 || strcmp(dtype, "Int") == 0 || strcmp(dtype, "Long") == 0 || strcmp(dtype, "Short") == 0 || strcmp(dtype, "Byte") == 0)
    {
        if (isStringLitToken(value_token) || isCharLitToken(value_token))
        {
            char buf[256];
            snprintf(buf, sizeof(buf), "E1-TypeMismatch: %s '%s' cannot take '%s'", dtype, varname, value_token);
            report_error(buf, line);
            return;
        }
        if (strchr(value_token, '.') != NULL)
        {
            char buf[256];
            snprintf(buf, sizeof(buf), "E1-TypeMismatch: %s '%s' cannot take '%s'", dtype, varname, value_token);
            report_error(buf, line);
            return;
        }
        int lenv = (int)strlen(value_token);
        if (lenv > 0 && isalpha((unsigned char)value_token[lenv - 1]))
        {
            char buf[256];
            snprintf(buf, sizeof(buf), "E1-TypeMismatch: %s '%s' cannot take '%s'", dtype, varname, value_token);
            report_error(buf, line);
            return;
        }
    }
    else if (strcmp(dtype, "float") == 0 || strcmp(dtype, "Float") == 0 || strcmp(dtype, "double") == 0 || strcmp(dtype, "Double") == 0)
    {
        if (isCharLitToken(value_token) || isStringLitToken(value_token))
        {
            char buf[256];
            snprintf(buf, sizeof(buf), "E1-TypeMismatch: %s '%s' cannot take '%s'", dtype, varname, value_token);
            report_error(buf, line);
            return;
        }
    }
    else if (strcmp(dtype, "char") == 0 || strcmp(dtype, "Char") == 0)
    {
        if (!isCharLitToken(value_token))
        {
            char buf[256];
            snprintf(buf, sizeof(buf), "E1-TypeMismatch: %s '%s' must take a char literal, got '%s'", dtype, varname, value_token);
            report_error(buf, line);
            return;
        }
    }
    else if (strcmp(dtype, "String") == 0)
    {
        return;
    }
}

/* PASS 1: tokenize & initial decls (kept robust) */
int tokenize_and_build(const char *filename)
{
    FILE *fp = fopen(filename, "r");
    if (!fp)
        return 0;
    tok_count = decl_count = err_count = com_count = 0;
    int line = 1;
    int ch;
    while ((ch = getc_nl(fp, &line)) != EOF)
    {
        if (ch == ' ' || ch == '\t' || ch == '\v' || ch == '\f')
            continue;

        /* comments */
        if (ch == '/')
        {
            int nxt = getc_nl(fp, &line);
            if (nxt == '/')
            {
                char buf[1024];
                int idx = 0;
                buf[idx++] = '/';
                buf[idx++] = '/';
                int c2;
                while ((c2 = getc_nl(fp, &line)) != EOF && c2 != '\n')
                {
                    if (idx < (int)sizeof(buf) - 1)
                        buf[idx++] = (char)c2;
                }
                buf[idx] = 0;
                if (com_count < MAX_COMMENTS)
                    strcpy(comments[com_count++], buf);
                continue;
            }
            else if (nxt == '*')
            {
                char buf[1024];
                int idx = 0;
                buf[idx++] = '/';
                buf[idx++] = '*';
                int prev = 0, c2;
                while ((c2 = getc_nl(fp, &line)) != EOF)
                {
                    if (idx < (int)sizeof(buf) - 1)
                        buf[idx++] = (char)c2;
                    if (prev == '*' && c2 == '/')
                        break;
                    prev = c2;
                }
                buf[idx] = 0;
                if (com_count < MAX_COMMENTS)
                    strcpy(comments[com_count++], buf);
                continue;
            }
            else
            {
                ungetc_nl(nxt, fp, &line);
            }
        }

        /* identifier / keyword */
        if (isalpha(ch) || ch == '_')
        {
            char buf[512];
            int idx = 0;
            buf[idx++] = (char)ch;
            int c2;
            while ((c2 = getc_nl(fp, &line)) != EOF && (isalnum(c2) || c2 == '_'))
            {
                if (idx < (int)sizeof(buf) - 1)
                    buf[idx++] = (char)c2;
            }
            if (c2 != EOF)
                ungetc_nl(c2, fp, &line);
            buf[idx] = 0;
            strcpy(table[tok_count].token, buf);
            table[tok_count].attribute = isKeyword(buf) ? 1 : 2;
            table[tok_count].line = line;
            tok_count++;

            /* package/import namespace capture */
            if (isKeyword(buf) && (strcmp(buf, "package") == 0 || strcmp(buf, "import") == 0))
            {
                int pch;
                while ((pch = getc_nl(fp, &line)) != EOF && isspace(pch) && pch != '\n')
                    ;
                if (pch == '\n' || pch == EOF)
                {
                    if (pch != EOF)
                        ungetc_nl(pch, fp, &line);
                    continue;
                }
                char nb[512];
                int nidx = 0;
                while (pch != EOF && pch != '\n' && pch != ';')
                {
                    if (nidx < (int)sizeof(nb) - 1)
                        nb[nidx++] = (char)pch;
                    pch = getc_nl(fp, &line);
                }
                nb[nidx] = 0;
                int s = 0, e = nidx - 1;
                while (s <= e && isspace((unsigned char)nb[s]))
                    s++;
                while (e >= s && isspace((unsigned char)nb[e]))
                    e--;
                char final[512];
                int fidx = 0;
                for (int k = s; k <= e; k++)
                    final[fidx++] = nb[k];
                final[fidx] = 0;
                if (fidx > 0)
                {
                    strcpy(table[tok_count].token, final);
                    table[tok_count].attribute = 8;
                    table[tok_count].line = line;
                    tok_count++;
                }
                continue;
            }

            /* immediate Java-style declaration detection */
            if (tok_count >= 2 && table[tok_count - 2].attribute == 1 && table[tok_count - 1].attribute == 2)
            {
                add_decl(table[tok_count - 1].token, table[tok_count - 2].token, table[tok_count - 1].line);
            }
            continue;
        }

        /* numbers */
        if (isdigit(ch))
        {
            char buf[512];
            int idx = 0;
            buf[idx++] = (char)ch;
            int c2;
            while ((c2 = getc_nl(fp, &line)) != EOF && (isdigit(c2) || c2 == '.'))
            {
                if (idx < (int)sizeof(buf) - 1)
                    buf[idx++] = (char)c2;
            }
            while (c2 != EOF && isalpha(c2))
            {
                if (idx < (int)sizeof(buf) - 1)
                    buf[idx++] = (char)c2;
                c2 = getc_nl(fp, &line);
            }
            if (c2 != EOF)
                ungetc_nl(c2, fp, &line);
            buf[idx] = 0;
            strcpy(table[tok_count].token, buf);
            table[tok_count].attribute = 3;
            table[tok_count].line = line;
            tok_count++;
            continue;
        }

        /* char literal */
        if (ch == '\'')
        {
            char buf[128];
            int idx = 0;
            buf[idx++] = '\'';
            int c2 = getc_nl(fp, &line);
            if (c2 == '\\')
            {
                if (idx < (int)sizeof(buf) - 1)
                    buf[idx++] = '\\';
                int c3 = getc_nl(fp, &line);
                if (c3 != EOF && idx < (int)sizeof(buf) - 1)
                    buf[idx++] = (char)c3;
            }
            else if (c2 != EOF)
            {
                if (idx < (int)sizeof(buf) - 1)
                    buf[idx++] = (char)c2;
            }
            int cend = getc_nl(fp, &line);
            if (cend == '\'' && idx < (int)sizeof(buf) - 1)
                buf[idx++] = '\'';
            buf[idx] = 0;
            strcpy(table[tok_count].token, buf);
            table[tok_count].attribute = 7;
            table[tok_count].line = line;
            tok_count++;
            continue;
        }

        /* string literal */
        if (ch == '"')
        {
            char buf[1024];
            int idx = 0;
            buf[idx++] = '"';
            int c2;
            while ((c2 = getc_nl(fp, &line)) != EOF && c2 != '"')
            {
                if (c2 == '\\')
                {
                    if (idx < (int)sizeof(buf) - 2)
                    {
                        buf[idx++] = (char)c2;
                        int c3 = getc_nl(fp, &line);
                        if (c3 != EOF)
                            buf[idx++] = (char)c3;
                    }
                }
                else
                {
                    if (idx < (int)sizeof(buf) - 1)
                        buf[idx++] = (char)c2;
                }
            }
            if (c2 == '"' && idx < (int)sizeof(buf) - 1)
                buf[idx++] = '"';
            buf[idx] = 0;
            strcpy(table[tok_count].token, buf);
            table[tok_count].attribute = 6;
            table[tok_count].line = line;
            tok_count++;
            continue;
        }

        /* operators / punctuation - include Kotlin combos, treat ':' as separator */
        if (strchr("+-*/%=<>!&|?:.()", ch) || ch == ':')
        {
            char buf[16];
            int idx = 0;
            buf[idx++] = (char)ch;
            int n = getc_nl(fp, &line);
            if (n != EOF)
            {
                if ((buf[0] == '?' && n == '.') || (buf[0] == '?' && n == ':') || (buf[0] == '.' && n == '.') ||
                    (buf[0] == '=' && n == '=') || (buf[0] == '!' && n == '=') || (buf[0] == '<' && n == '=') || (buf[0] == '>' && n == '=') ||
                    (buf[0] == '&' && n == '&') || (buf[0] == '|' && n == '|'))
                {
                    buf[idx++] = (char)n;
                }
                else
                {
                    if (buf[0] == '=' && n == '=')
                    {
                        int n2 = getc_nl(fp, &line);
                        if (n2 == '=')
                        {
                            buf[idx++] = (char)n;
                            buf[idx++] = (char)n2;
                        }
                        else
                        {
                            ungetc_nl(n2, fp, &line);
                            ungetc_nl(n, fp, &line);
                        }
                    }
                    else
                        ungetc_nl(n, fp, &line);
                }
            }
            buf[idx] = 0;
            int attr = 4;
            if (buf[0] == ':' && idx == 1)
                attr = 5;
            strcpy(table[tok_count].token, buf);
            table[tok_count].attribute = attr;
            table[tok_count].line = line;
            tok_count++;
            continue;
        }

        /* separators */
        if (strchr("{}[];,", ch))
        {
            char buf[4] = {(char)ch, 0, 0, 0};
            strcpy(table[tok_count].token, buf);
            table[tok_count].attribute = 5;
            table[tok_count].line = line;
            tok_count++;
            continue;
        }

        /* else ignore */
    } /* end while */

    fclose(fp);

    /* Kotlin var/val detection & E1 check */
    for (int i = 0; i < tok_count; i++)
    {
        if (table[i].attribute == 1 && (strcmp(table[i].token, "var") == 0 || strcmp(table[i].token, "val") == 0))
        {
            if (i + 1 < tok_count && table[i + 1].attribute == 2)
            {
                const char *name = table[i + 1].token;
                if (i + 2 < tok_count && strcmp(table[i + 2].token, ":") == 0 && i + 3 < tok_count)
                {
                    const char *type = table[i + 3].token;
                    char typbuf[128];
                    strncpy(typbuf, type, sizeof(typbuf) - 1);
                    typbuf[sizeof(typbuf) - 1] = 0;
                    int lt = (int)strlen(typbuf);
                    if (lt > 0 && typbuf[lt - 1] == '?')
                        typbuf[lt - 1] = 0;
                    add_decl(name, typbuf, table[i + 1].line);
                    if (i + 4 < tok_count && strcmp(table[i + 4].token, "=") == 0 && i + 5 < tok_count)
                    {
                        const char *valtok = table[i + 5].token;
                        check_assignment_type(typbuf, valtok, table[i + 1].line, name);
                    }
                }
                else if (i + 2 < tok_count && strcmp(table[i + 2].token, "=") == 0)
                {
                    add_decl(name, "UNKNOWN", table[i + 1].line);
                }
            }
        }
    }

    return 1;
}

/* PASS 2: detect errors E1..E4 */
void detect_errors_pass2(void)
{
    for (int i = 0; i < tok_count; i++)
    {
        struct Symbol *t = &table[i];

        /* E2 - misspelled keyword */
        if (t->attribute == 2)
        {
            if (!isDeclared(t->token) && similarToKeyword(t->token))
            {
                int prev_is_keyword = (i > 0 && table[i - 1].attribute == 1);
                if (!prev_is_keyword)
                {
                    char buf[256];
                    snprintf(buf, sizeof(buf), "E2-MisspelledKeyword: '%s' resembles a keyword", t->token);
                    report_error(buf, t->line);
                }
            }
        }

        /* E3 - identifier used before declaration */
        if (t->attribute == 2)
        {
            int prev_is_decl_keyword = (i > 0 && table[i - 1].attribute == 1 && (strcmp(table[i - 1].token, "var") == 0 || strcmp(table[i - 1].token, "val") == 0 || isKeyword(table[i - 1].token)));
            if (!prev_is_decl_keyword && !isDeclared(t->token))
            {
                char buf[256];
                snprintf(buf, sizeof(buf), "E3-IdentifierError: '%s' used before declaration", t->token);
                report_error(buf, t->line);
            }
        }

        /* E1 - assignment */
        if (i + 2 < tok_count && strcmp(table[i + 1].token, "=") == 0 && table[i].attribute == 2)
        {
            const char *id = table[i].token;
            const char *valtok = table[i + 2].token;
            if (!isDeclared(id))
            {
                char buf[256];
                snprintf(buf, sizeof(buf), "E3-IdentifierError: '%s' used before declaration", id);
                report_error(buf, table[i].line);
            }
            else
            {
                const char *dtype = getType(id);
                check_assignment_type(dtype, valtok, table[i].line, id);
            }
        }

        /* E4 - relational operator misuse */
        if (isRelOp(t->token))
        {
            if (i == 0 || i == tok_count - 1)
            {
                char buf[256];
                snprintf(buf, sizeof(buf), "E4-RelationalError: Operator '%s' at invalid position", t->token);
                report_error(buf, t->line);
            }
            else
            {
                int left_ok = (table[i - 1].attribute == 2 || table[i - 1].attribute == 3 || table[i - 1].attribute == 6 || table[i - 1].attribute == 7);
                int right_ok = (table[i + 1].attribute == 2 || table[i + 1].attribute == 3 || table[i + 1].attribute == 6 || table[i + 1].attribute == 7);
                if (!left_ok || !right_ok)
                {
                    char buf[256];
                    snprintf(buf, sizeof(buf), "E4-RelationalError: Operator '%s' has invalid operands", t->token);
                    report_error(buf, t->line);
                }
            }
        }
    }
}

/* comparator: sort by line then token */
int cmpSymbols(const void *a, const void *b)
{
    const struct Symbol *A = (const struct Symbol *)a;
    const struct Symbol *B = (const struct Symbol *)b;
    if (A->line != B->line)
        return A->line - B->line;
    return strcmp(A->token, B->token);
}

/* Small animation for drawing a horizontal line */
static void animated_hline(int width)
{
    for (int i = 0; i < width; i++)
    {
        putchar('-');
        fflush(stdout);
        usleep(1500); /* 1.5ms per char -> subtle */
    }
    putchar('\n');
}

/* Print Symbol Table first (colored per attribute & token) */
void print_symbol_table_box(void)
{
    /* sort tokens by line */
    qsort(table, tok_count, sizeof(struct Symbol), cmpSymbols);

    int col1 = 40, col2 = 18, col3 = 6;
    int total = col1 + col2 + col3 + 6;

    /* header */
    printf("\n%s", PASTEL_HDR_BG);
    animated_hline(total);
    printf("| %-*s | %-*s | %-*s |\n", col1, "TOKEN", col2, "ATTRIBUTE", col3, "LINE");
    animated_hline(total);
    printf("%s", COL_RESET);

    for (int i = 0; i < tok_count; i++)
    {
        const char *tokcol = attrColor(table[i].attribute);
        const char *attrcol = attrColor(table[i].attribute);
        char token_display[512];
        strncpy(token_display, table[i].token, col1 - 1);
        token_display[col1 - 1] = 0;

        printf("| %s%-*s%s | %s%-*s%s | %*d |\n",
               tokcol, col1, token_display, COL_RESET,
               attrcol, col2, attrLabel(table[i].attribute), COL_RESET,
               col3 - 1, table[i].line);
    }

    animated_hline(total);
}

/* Print comments second (soft blue) */
void print_comments_box(const char *fname)
{
    (void)fname;
    int width = 65;
    printf("\n");
    printf("%s", PASTEL_HDR_BG);
    animated_hline(width);
    printf("| %-*s |\n", width - 4, "COMMENTS");
    animated_hline(width);
    printf("%s", COL_RESET);

    if (com_count == 0)
    {
        printf("| %s(no comments found)%s\n", PASTEL_COMMENT, COL_RESET);
        animated_hline(width);
        return;
    }
    for (int i = 0; i < com_count; i++)
    {
        char buf[58];
        strncpy(buf, comments[i], sizeof(buf) - 1);
        buf[sizeof(buf) - 1] = 0;
        /* print comment line with pastel blue */
        printf("| %s%-58s%s |\n", PASTEL_COMMENT, buf, COL_RESET);
    }
    animated_hline(width);
}

/* Print errors last, colored by type */
void print_errors_and_summary_box(void)
{
    int width = 70;
    printf("\n");
    printf("%s", PASTEL_HDR_BG);
    animated_hline(width);
    printf("| %-*s |\n", width - 4, "ERROR REPORT");
    animated_hline(width);
    printf("%s", COL_RESET);

    if (err_count == 0)
    {
        printf("%sNo errors found.%s\n", PASTEL_IDENT, COL_RESET);
        animated_hline(width);
        return;
    }

    for (int i = 0; i < err_count; i++)
    {
        const char *col = strchr(errors[i].msg, 'E1') ? PASTEL_ERROR1 : (strchr(errors[i].msg, 'E2') ? PASTEL_ERROR2 : PASTEL_ERROR1);
        char msgbuf[64];
        strncpy(msgbuf, errors[i].msg, sizeof(msgbuf) - 1);
        msgbuf[sizeof(msgbuf) - 1] = 0;
        printf("| %s%-60s%s | %3d |\n", col, msgbuf, COL_RESET, errors[i].line);
    }

    animated_hline(width);

    /* summary counts */
    int e1 = 0, e2 = 0, e3 = 0, e4 = 0;
    for (int i = 0; i < err_count; i++)
    {
        if (strstr(errors[i].msg, "E1-"))
            e1++;
        if (strstr(errors[i].msg, "E2-"))
            e2++;
        if (strstr(errors[i].msg, "E3-"))
            e3++;
        if (strstr(errors[i].msg, "E4-"))
            e4++;
    }
    printf("%sSummary:%s E1=%d  E2=%d  E3=%d  E4=%d   Total=%d\n", PASTEL_IDENT, COL_RESET, e1, e2, e3, e4, err_count);
}

/* Trim helper */
static void trim_inplace(char *s)
{
    int n = (int)strlen(s);
    while (n > 0 && (s[n - 1] == '\n' || s[n - 1] == '\r' || isspace((unsigned char)s[n - 1])))
        s[--n] = 0;
    int i = 0;
    while (s[i] && isspace((unsigned char)s[i]))
        i++;
    if (i)
        memmove(s, s + i, strlen(s + i) + 1);
}

/* Interactive prompts */
static int prompt_language(char *out_filename, size_t maxlen)
{
    char buf[64];
    while (1)
    {
        printf("\nSelect language: (1) Java  (2) Kotlin  [enter 1 or 2]: ");
        if (!fgets(buf, sizeof(buf), stdin))
            return 0;
        trim_inplace(buf);
        if (strlen(buf) == 0)
            continue;
        if (buf[0] == '1')
        {
            strncpy(out_filename, "Input.java", maxlen - 1);
            out_filename[maxlen - 1] = 0;
            return 1;
        }
        if (buf[0] == '2')
        {
            strncpy(out_filename, "Input.kt", maxlen - 1);
            out_filename[maxlen - 1] = 0;
            return 1;
        }
        printf("Invalid choice. Please enter 1 or 2.\n");
    }
}
static int prompt_yesno(const char *msg)
{
    char buf[32];
    while (1)
    {
        printf("%s (y/n): ", msg);
        if (!fgets(buf, sizeof(buf), stdin))
            return 0;
        trim_inplace(buf);
        if (strlen(buf) == 0)
            continue;
        char c = (char)tolower((unsigned char)buf[0]);
        if (c == 'y')
            return 1;
        if (c == 'n')
            return 0;
        printf("Please answer y or n.\n");
    }
}

/* main loop */
int main(void)
{
    char filename[256];
    printf("%sLexical Analyzer for Java and Kotlin %s\n", PASTEL_HDR_BG, COL_RESET);
    while (1)
    {
        if (!prompt_language(filename, sizeof(filename)))
        {
            printf("Input error. Exiting.\n");
            return 1;
        }
        printf("Selected file: %s\n", filename);
        if (!prompt_yesno("Proceed with analysis on this file"))
            continue;

        if (!tokenize_and_build(filename))
        {
            printf("%sERROR:%s Could not open %s\n", PASTEL_ERROR1, COL_RESET, filename);
        }
        else
        {
            detect_errors_pass2();
            /* Print in user-required sequence:
               1) Symbol Table
               2) Comments
               3) Errors
            */
            print_symbol_table_box();
            print_comments_box(filename);
            print_errors_and_summary_box();
        }

        if (!prompt_yesno("Do you want to continue and analyze another file"))
        {
            printf("Exiting. Goodbye.\n");
            break;
        }
    }
    return 0;
}
