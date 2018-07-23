#pragma once

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus


#include <stdint.h>

#include "kdq.h"
#include "kvec.h"

KVEC_INIT(char);
KDQ_INIT(char);

typedef struct
{
    uint32_t i; //字符搜索状态
    kvec_char_t text;
}sf_stream_find_t;

enum sf_stream_filter_status { sf_enum_begin, sf_enum_body, sf_enum_end };
typedef struct sf_stream_filter
{
    sf_stream_find_t head;
    sf_stream_find_t tail;
    enum sf_stream_filter_status state;
    size_t body_size;
    kdq_char_t buffer;     //缓存
}sf_t;

//初始化对象,若成功,返回0,否则返回-1
//buffer_bits 取值范围 5-30
int32_t sf_init(sf_t *sf,const char *head,const char *tail,int32_t buffer_bits);
//销毁对象
void sf_destory(sf_t *sf);
//重置搜索状态,清空缓存
void sf_reset(sf_t *sf);
//将数据填入缓冲区,返回当前缓冲区已有数据的大小
int32_t sf_input(sf_t *sf,const char* input);
//从缓冲区提取报文,若找到完整报文,返回报文正文字符数,若还未找到完整报文,返回0
//若找到完整报文,但是无法将报文从内部缓冲区完整拷贝出来,返回 -1
int32_t sf_extract(sf_t *sf, kvec_char_t * output);
int32_t sf_input_and_extract(sf_t *sf, const char* input,kvec_char_t * output);

#ifdef __cplusplus
}
#endif // __cplusplus
