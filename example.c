#include <stdio.h>

#include "sfilter.h"


const char test1[] = "begin hello world end ";
const char test2[] = "begin1234endbeioinweend";
const char test3[] = "beginabcdend";
const char test4[] = "begin1234endbegin";

int main(int argc, char* argv[])
{
    sf_t sf;
    sf_t sf1;
    int32_t  result;
    kvec_char_t kv;
    result = kvec_init_char(&kv);
    if(result < 0){
        printf("kv init error\n");
        return 1;
    }

    result = sf_init(&sf,"begin","end",0);
    if(result < 0){
        printf("sf init error\n");
        return 1;
    }
    result = sf_init(&sf1,"begin",NULL,0);
    if(result < 0){
        printf("sf init error\n");
        return 1;
    }

    result = sf_input(&sf,test1);
    if(result < 0){ printf("sf input error\n"); return 1; }
    result = sf_input(&sf,test2);
    if(result < 0){ printf("sf input error\n"); return 1; }
    result = sf_input(&sf,test3);
    if(result < 0){ printf("sf input error\n"); return 1; }
    result = sf_input(&sf,test4);
    if(result < 0){ printf("sf input error\n"); return 1; }

    for(int i =0;i<10;++i) {
        result = sf_extract(&sf, &kv);
        if (result < 0) {
            printf("sf extract error\n");
            return 1;
        }
        else if(result == 0){
            printf("sf extract null\n");
        }
        else {
            printf("sf extract ok\n");
            for(int j = 0;j<kvec_size(kv);++j) {
                putchar(kvec_A(kv,j));
            }
            putchar('\n');
        }
    }

//    sf_reset(&sf);

    sf_input_and_extract(&sf,test1,&kv);
    for(int j = 0;j<kvec_size(kv);++j) {
        putchar(kvec_A(kv,j));
    }
    putchar('\n');


    kvec_destroy_char(&kv);
    sf_destory(&sf);

    return 0;
}