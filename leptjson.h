#ifndef LEPTJSON_H__
#define LEPTJSON_H__

typedef enum
{
    LEPT_NULL,
    LEPT_TRUE,
    LEPT_FALSE,
    LEPT_NUMBER,
    LEPT_STRING,
    LEPT_ARRAY,
    LEPT_OBJECT
} lept_type;

typedef struct
{
    lept_type type;
} lept_value;

typedef enum
{
    LEPT_PARSE_OK,
    LEPT_PARSE_EXPECT_VALUE,
    LEPT_PARSE_INVALID_VALUE,
    LEPT_PARSE_ROOT_NOT_SINGULAR
} lept_parse_status;

lept_parse_status lept_parse(lept_value *v, const char *json);

lept_type lept_get_type(lept_value *v);

#endif