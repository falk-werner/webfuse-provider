#include "webfuse_provider/impl/json/reader.h"
#include <string.h>

void
wfp_impl_json_reader_init(
    struct wfp_json_reader * reader,
    char * contents,
    size_t length)
{
    reader->contents =contents;
    reader->length = length;
    reader->pos = 0;
}

void
wfp_impl_json_reader_skip_whitespace(
    struct wfp_json_reader * reader)
{
    char c = reader->contents[reader->pos];

    while ((' ' == c) || ('\n' == c) || ('\t' == c)|| ('\r' == c))
    {
        reader->pos++;
        c = reader->contents[reader->pos];
    }
}


char
wfp_impl_json_reader_peek(
    struct wfp_json_reader * reader)
{
    char result = '\0';
    if (reader->pos < reader->length)
    {
        result = reader->contents[reader->pos];
    }

    return result;
}

char
wfp_impl_json_reader_read_char(
    struct wfp_json_reader * reader)
{
    char result = '\0';
    if (reader->pos < reader->length)
    {
        result = reader->contents[reader->pos];
        reader->pos++;
    }

    return result;
}

void
wfp_impl_json_reader_unget_char(
    struct wfp_json_reader * reader)
{
    if (0 < reader->pos)
    {
        reader->pos--;
    }
}


bool
wfp_impl_json_reader_read_const(
    struct wfp_json_reader * reader,
    char const * value,
    size_t length)
{
    size_t const remaining = reader->length - reader->pos; 
    bool const result = ((remaining >= length) && (0 == strncmp(&(reader->contents[reader->pos]), value, length)));
    {
        reader->pos += length;
    }
    
    return result;
}

int
wfp_impl_json_reader_read_int(
    struct wfp_json_reader * reader,
    char first)
{
    bool const is_signed = ('-' == first);
    int value = is_signed ? 0 : first - '0';

    while (reader->pos < reader->length) 
    {
        char c = reader->contents[reader->pos];
        if (('0' <= c) && (c <= '9'))
        {
            value *= 10;
            value += (c - '0');
            reader->pos++;
        }
        else
        {
            break;
        }
        
    }

    return (is_signed ? -value : value);
}

extern bool
wfp_impl_json_reader_read_string(
    struct wfp_json_reader * reader,
    char * * value)
{
    wfp_impl_json_reader_skip_whitespace(reader);
    char c = wfp_impl_json_reader_read_char(reader);
    if ('\"' != c) { return false; }

    size_t p = reader->pos;
    *value = &(reader->contents[p]);
    c = wfp_impl_json_reader_read_char(reader);
    while (('\"' != c) && ('\0' != c))
    {
        if ('\\' != c)
        {
            reader->contents[p++] = c;
        }
        else
        {
            c = wfp_impl_json_reader_read_char(reader);
            switch (c)
            {
                case '\"':
                    reader->contents[p++] = '\"';            
                    break;
                case '\\':
                    reader->contents[p++] = '\\';            
                    break;
                case '/':
                    reader->contents[p++] = '/';            
                    break;
                case 'b':
                    reader->contents[p++] = '\b';            
                    break;
                case 'f':
                    reader->contents[p++] = '\f';            
                    break;
                case 'n':
                    reader->contents[p++] = '\n';            
                    break;
                case 'r':
                    reader->contents[p++] = '\r';            
                    break;
                case 't':
                    reader->contents[p++] = '\t';            
                    break;
                default:
                    return false;
            }
        }
        c = wfp_impl_json_reader_read_char(reader);
    }

    bool const result = ('\"' == c);
    if (result)
    {
        reader->contents[p] = '\0';
    }
    return result;
}
