#include "leptjson.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <math.h>
#include <errno.h>


#define EXPECT(context, ch)       do { assert(*context->json == (ch)); context->json++;} while(0)
#define ISDIGIT(ch)         ((ch) >= '0' && (ch) <= '9')
#define ISDIGIT1TO9(ch)     ((ch) >= '1' && (ch) <= '9')

typedef struct
{
    const char *json;
} lept_context;

static void lept_parse_whitespace(lept_context *c);
static lept_parse_status lept_parse_literal(lept_context *c, lept_value *v, const char *literal, lept_type type);
lept_parse_status lept_parse(lept_value *v, const char *json);
lept_type lept_get_type(const lept_value *v);
static lept_parse_status lept_parse_value(lept_context *c, lept_value *v);
double lept_get_number(const lept_value *v);
static lept_parse_status lept_parse_number(lept_context *c, lept_value *v);

static void lept_parse_whitespace(lept_context *c)
{
    const char *p = c->json;
    while(*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r')
        ++p;
    c->json = p;
}

lept_parse_status lept_parse(lept_value *v, const char *json)
{
    lept_context c;
    assert(v != NULL);
    c.json = json;
    v->type = LEPT_NULL;
    lept_parse_whitespace(&c);
    return lept_parse_value(&c, v);
}

lept_type lept_get_type(const lept_value *v)
{
    assert(v != NULL);
    return v->type;
}

double lept_get_number(const lept_value *v)
{
    assert(v != NULL && v->type == LEPT_NUMBER);
    return v->n;
}

static lept_parse_status lept_parse_literal(lept_context *c, lept_value *v, const char *literal, lept_type type)
{
    size_t i;
    EXPECT(c, literal[0]);
    for (i = 0; literal[i + 1]; i++)
        if (c->json[i] != literal[i + 1])
            return LEPT_PARSE_INVALID_VALUE;
    c->json += i;
    v->type = type;
    return LEPT_PARSE_OK;
}

static lept_parse_status lept_parse_number(lept_context *c, lept_value *v)
{
    const char *p = c->json;
    if (*p == '-')
        ++p;
    if (*p == '0')
        ++p;
    else
    {
        if(!ISDIGIT1TO9(*p)) return LEPT_PARSE_INVALID_VALUE;
        ++p;
        while(ISDIGIT(*p))
            p++;
    }

    if (*p == '.')
    {
        ++p;
        if (!ISDIGIT(*p)) return LEPT_PARSE_INVALID_VALUE;
        ++p;
        while(ISDIGIT(*p))
            p++;
    }
    if (*p == 'e' || *p == 'E')
    {
        ++p;
        if (*p == '+' || *p == '-')
            ++p;
        if (!ISDIGIT(*p)) return LEPT_PARSE_INVALID_VALUE;
        ++p;
        while(ISDIGIT(*p))
            p++;
    }
    errno = 0;
    v->n = strtod(c->json, NULL);
    if (errno == ERANGE && (v->n == HUGE_VAL || v->n == -HUGE_VAL))
        return LEPT_PARSE_NUMBER_TOO_BIG;
    v->type = LEPT_NUMBER;
    c->json = p;
    return LEPT_PARSE_OK;
}

static lept_parse_status lept_parse_value(lept_context *c, lept_value *v)
{
    lept_parse_status status;
    switch(*c->json)
    {
        case 'n':  status = lept_parse_literal(c, v, "null", LEPT_NULL);       break;
        case 't':  status = lept_parse_literal(c, v, "true", LEPT_TRUE);       break;
        case 'f':  status = lept_parse_literal(c, v, "false", LEPT_FALSE);     break;
        case '\0': status = LEPT_PARSE_EXPECT_VALUE;                           break;
        default :  status = lept_parse_number(c, v);                           break;
    }
    lept_parse_whitespace(c);
    if(status == LEPT_PARSE_OK && c->json[0] != '\0')
    {
        status = LEPT_PARSE_ROOT_NOT_SINGULAR;
        v->type = LEPT_NULL;
    }
    return status;
}

