#include "sfilter.h"

#include <string.h>
#include <stdbool.h>
#include <assert.h>


//初始化对象,若成功,返回0,否则返回-1
static int32_t sf_stream_find_init(sf_stream_find_t *sf,const char *s)
{
    assert(sf != NULL);
    sf->i = 0;
    int result;
    result = kvec_init_char(&sf->text);
    if (result != 0)
        return -1;
    if(s == NULL){
        //允许字符串为空
        kvec_destroy_char(&sf->text);
        return 0;
    }
    size_t len = strlen(s);
    result = kvec_resize_char(&sf->text, len + 1);
    if (result != 0)
        return -1;
    for (size_t l = 0; l < len; ++l) {
        result = kvec_push_char(&sf->text, s[l]);
        if (result != 0)
            return -1;
    }
    sf->text.a[len] = 0;
    return 0;
}

//销毁对象
static void sf_stream_find_destory(sf_stream_find_t *sf)
{
    assert(sf != NULL);
    assert(kvec_size(sf->text) > 0);
    kvec_destroy_char(&sf->text);
    return;
}

//若在字符流中找到指定字符串,返回 0,否则返回-1
static int32_t sf_stream_find_input_char(sf_stream_find_t *sf, char c)
{
    assert(sf != NULL);
    assert(sf->i <= kvec_size(sf->text));

    if(kvec_size(sf->text) == 0)
        //如果目标字符串为空,则不进行搜索
        return -1;
    if (c == kvec_A(sf->text, sf->i)) {
        sf->i += 1;
    }
    else if (c == kvec_A(sf->text, 0)) {
        sf->i = 1;
    }
    else {
        sf->i = 0;
    }
    if (sf->i == kvec_size(sf->text)) {
        sf->i = 0;
        return 0;
    }
    else return -1;
}

//重置搜索状态
static void sf_stream_find_reset(sf_stream_find_t*sf)
{
    assert(sf != NULL);
    sf->i = 0;
}

int32_t sf_init(sf_t *sf,const char *head,const char *tail,int32_t buffer_bits)
{
    assert(sf != NULL);
    assert(head != NULL);
    assert(buffer_bits >= 0);
    assert(buffer_bits < 31);
    int result;
    buffer_bits = buffer_bits < 5 ? buffer_bits = 5: buffer_bits;
    sf_stream_find_init(&(sf->head), head);
    sf_stream_find_init(&(sf->tail), tail);//tail可以为NULL
    sf->state = sf_enum_begin;
    sf->body_size = 0;
    result =kdq_init_char(&sf->buffer);
    if (result != 0) return -1;
    result = kdq_resize_char(&sf->buffer, buffer_bits);
    if (result != 0) return -1;
    return 0;
}

void sf_destory(sf_t *sf)
{
    assert(sf != NULL);
    kdq_destroy_char(&sf->buffer);
    sf_stream_find_destory(&sf->head);
    sf_stream_find_destory(&sf->tail);
    return;
}

void sf_reset(sf_t *sf)
{
    assert(sf != NULL);
    kdq_clean_char(&sf->buffer);
    sf->state = sf_enum_begin;
    sf_stream_find_reset(&sf->head);
    sf_stream_find_reset(&sf->tail);
    return;
}

int32_t sf_input(sf_t *sf,const char* input)
{
    assert(sf != NULL);
    assert(input != NULL);
    size_t len = strlen(input);
    int result;
    for (size_t l = 0; l < len; ++l) {
        result = kdq_push_char(&sf->buffer, input[l]);
        if (result != 0)
            return -1;
    }
    return (int32_t)kdq_size(&sf->buffer);
}

int32_t sf_extract(sf_t *sf, kvec_t(char)* output)
{
    assert(sf != NULL);
    assert(output != NULL);
    char c = 0;
    char *pc = NULL;
    int result;
    sf_stream_find_t *find = NULL;
    while (true) {
        switch (sf->state) {
            case sf_enum_begin:
                pc = kdq_shift_char(&sf->buffer);
                if (pc == NULL)
                    return 0;
                c = *pc;
                result = sf_stream_find_input_char(&sf->head, c);
                if (result == 0) {
                    sf->state = sf_enum_body;
                    sf->body_size = 0;
                }
                break;
            case sf_enum_body:
                if ((sf->body_size) == kdq_size(&sf->buffer)) {
                    //已经扫描完buffer
                    return 0;
                }
                c = kdq_at(&sf->buffer, sf->body_size);
                find = (kvec_size(sf->tail.text) > 0) ? &sf->tail : &sf->head;
                result = sf_stream_find_input_char(find, c);
                if (result == 0) {
                    sf->state = sf_enum_end;
                    //计算出正文的长度
                    sf->body_size = sf->body_size - kvec_size(find->text) + 1;
                }
                else
                    sf->body_size += 1;
                break;
            case sf_enum_end:
                kvec_clean_char(output);
                for (size_t i = 0; i < sf->body_size; ++i) {
                    pc = kdq_shift_char(&sf->buffer);
                    result = kvec_a_char(output,i,*pc);
                    if(result != 0)
                        return -1;
                }
                for (size_t i = 0; i < kvec_size(sf->tail.text); ++i) {
                    kdq_shift_char(&sf->buffer);
                }
                sf->state = sf_enum_begin;
                return (int32_t)sf->body_size;
        }
    }
}


int32_t sf_input_and_extract(sf_t *sf, const char* input,kvec_char_t * output)
{
    int32_t  result;
    result = sf_input(sf,input);
    if(result == -1)
        return -1;
    result=sf_extract(sf,output);
    if(result == -1)
        return -1;
    return 0;
}
