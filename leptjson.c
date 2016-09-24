#include "leptjson.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#define EXPECT(context, ch)       do { assert(*context->json == (ch)); context->json++;} while(0)


typedef struct
{
    const char *json;
} lept_context;

static void lept_parse_whitespace(lept_context *c);
static lept_parse_status lept_parse_null(lept_context *c, lept_value *v);
static lept_parse_status lept_parse_true(lept_context *c, lept_value *v);
static lept_parse_status lept_parse_false(lept_context *c, lept_value *v);
lept_parse_status lept_parse(lept_value *v, const char *json);
lept_type lept_get_type(lept_value *v);
static lept_parse_status lept_parse_value(lept_context *c, lept_value *v);

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

lept_type lept_get_type(lept_value *v)
{
    return v->type;
}

static lept_parse_status lept_parse_null(lept_context *c, lept_value *v)
{
    EXPECT(c, 'n');
    if (c->json[0] != 'u' || c->json[1] != 'l' || c->json[2] != 'l')
    {
        return LEPT_PARSE_INVALID_VALUE;
    }
    c->json += 3;
    v->type = LEPT_NULL;
    return LEPT_PARSE_OK;
}

static lept_parse_status lept_parse_true(lept_context *c, lept_value *v)
{
    EXPECT(c, 't');
    if (c->json[0] != 'r' || c->json[1] != 'u' || c->json[2] != 'e')
    {
        return LEPT_PARSE_INVALID_VALUE;
    }
    c->json += 3;
    v->type = LEPT_TRUE;
    return LEPT_PARSE_OK;
}

static lept_parse_status lept_parse_false(lept_context *c, lept_value *v)
{
    EXPECT(c, 'f');
    if (c->json[0] != 'a' || c->json[1] != 'l' || c->json[2] != 's' || c->json[3] != 'e')
    {
        return LEPT_PARSE_INVALID_VALUE;
    }
    c->json += 4;
    v->type = LEPT_FALSE;
    return LEPT_PARSE_OK;
}

static lept_parse_status lept_parse_value(lept_context *c, lept_value *v)
{
    lept_parse_status status;
    switch(*c->json)
    {
        case 'n':  status = lept_parse_null(c, v);       break;
        case 't':  status = lept_parse_true(c, v);       break;
        case 'f':  status = lept_parse_false(c, v);      break;
        case '\0': status = LEPT_PARSE_EXPECT_VALUE;     break;
        default :  status = LEPT_PARSE_INVALID_VALUE;    break;
    }
    lept_parse_whitespace(c);
    if(status == LEPT_PARSE_OK && c->json[0] != '\0')
    {
        status = LEPT_PARSE_ROOT_NOT_SINGULAR;
        v->type = LEPT_NULL;
    }
    return status;
}

