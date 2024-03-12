#include <stdio.h>
#include <stdlib.h>
#include "time.h"
#include <unistd.h>
#include <pthread.h>

#include "lcd.h"
#include "draw_pic.h"
#include "serial.h"
#include "zimo.h"
#include "beep_say.h"
#include "gy39.h"
#include "p1.h"
#include "touch_getV.h"

void* gy39_func_thread(void* arg) {
    // 执行 gy39_func 函数的相关代码
    gy39_func();
}

void* xc_thread(void* arg) {
    // 执行 xc 函数的相关代码
    char* path = (char*)arg;
    xc(path);
}

void* touch_thread(void* arg){
    //执行touch_GetV函数相关代码
    touch_GetV();
}

int main(int argc, const char *argv[]){
    lcd_Init();
    const char* thread_arg = argv[1];
    pthread_t t1,t2,t3;       
    pthread_create(&t1, NULL, gy39_func_thread, NULL);
    pthread_create(&t2, NULL, xc_thread, (void*)thread_arg);
    pthread_create(&t3, NULL, touch_thread,NULL);

    // 等待所有线程结束
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    pthread_join(t3, NULL);

    lcd_close();

    return 0;
}

