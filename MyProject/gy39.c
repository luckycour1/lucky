#include <stdio.h>
#include <stdlib.h>
#include "time.h"
#include <unistd.h>

#include "lcd.h"
#include "draw_pic.h"
#include "serial.h"
#include "zimo.h"
#include "beep_say.h"
#include "gy39.h"


int get_n(int num);
void print_shuzi(int num, int x, int y);

void gy39_func(){
    int gy39_fd = init_serial("./dev/ttySAC1", 9600);
    int gy39_fd2 = init_serial("./dev/ttySAC2", 9600);
    unsigned char cmd1[] = {0xA5, 0x81, 0x26};
    unsigned char cmd2[] = {0xA5, 0x83, 0x28};
    unsigned char data[20] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    unsigned char command[] = {0xFF, 0x01, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79};
    unsigned char response[9];

    float Lux = 0, T = 0, P = 0, Hum = 0,C = 0;
    int H = 0;

    write(gy39_fd, cmd2, 3);


    while(1){
        // Draw_Picture(0, 0, "./YSQD.bmp", Custom);
        read(gy39_fd, data, 15);
        if (data[2] == 0x15){       
            Lux = (data[4]<<24) | (data[5]<<16) | (data[6]<<8) | data[7];//光照
            Lux = Lux/100;
        }else if(data[2] == 0x45){
            T = (data[4]<<8) | data[5];//温度
            T = T/100;
            P = (data[6]<<24) | (data[7]<<16) | (data[8]<<8) | data[9];//压强
            P = P/100;
            Hum = (data[10]<<8) | data[11];//湿度
            Hum = Hum/100;      
            H = (data[12]<<8) | data[13];//海拔

        write(gy39_fd2, command, 9);
        sleep(1);
        int a = read(gy39_fd2, response, 9);
        
        if(a == 9){
            
            // 解析接收到的数据包
            if (response[0] == 0xFF && response[1] == 0x86) {
                // if (response[8] == 0xF5) { // 校验和校验
                    int high_byte = response[2];
                    int low_byte = response[3];
                    C = (high_byte << 8) | low_byte;
                
                // } else {
                //     printf("xy error\n");
                // }
                } else {
                    printf("data formate error\n");
                }
        };



        }




        //温度
        Draw_ZI(0, 24*0, wen, RED, 3*24);
        Draw_ZI(24, 24*0, du, RED, 3*24);
        Draw_FUHAO(48, 24*0, maohao, RED, 1*27);
        print_shuzi((int)T, 56, 24*0);
        Draw_FUHAO(56 + 16*get_n((int)T), 24*0, dian, RED, 1*27);
        print_shuzi(((int)(T*100))%100, 56 + 16*get_n((int)T) + 8, 24*0);
        Draw_FUHAO(56 + 16*get_n((int)T) + 8 + 16*2, 24*0, sheshidu, RED, 4*27);

        //气压
        Draw_ZI(0, 24*1, qi, RED, 3 * 24);
        Draw_ZI(24, 24*1, ya, RED, 3 * 24);
        Draw_FUHAO(48, 24*1, maohao, RED, 1*27);
        print_shuzi((int)P, 56, 24*1);
        Draw_FUHAO(56 + 16*get_n((int)P), 24*1, dian, RED, 1*27);
        print_shuzi(((int)(P*100))%100, 56 + 16*get_n((int)P) + 8, 24*1);
        Draw_FUHAO(56 + 16*get_n((int)P) + 8 + 16*2, 24*1, p, RED, 2*27);
        Draw_FUHAO(56 + 16*get_n((int)P) + 8 + 16*2 + 16, 24*1, a, RED, 2*27);

        //湿度
        Draw_ZI(0, 24*2, shi, RED, 3 * 24);
        Draw_ZI(24, 24*2, du, RED, 3 * 24);
        Draw_FUHAO(48, 24*2, maohao, RED, 1*27);
        print_shuzi((int)Hum, 56, 24*2);
        Draw_FUHAO(56 + 16*get_n((int)Hum), 24*2, dian, RED, 1*27);
        print_shuzi(((int)(Hum*100))%100, 56 + 16*get_n((int)Hum) + 8, 24*2);
        Draw_FUHAO(56 + 16*get_n((int)Hum) + 8 + 16*2, 24*2, baifenhao, RED, 3*27);

        //光照
        Draw_ZI(0, 24*3, guang, RED, 3 * 24);
        Draw_ZI(24, 24*3, zhao, RED, 3 * 24);
        Draw_FUHAO(48, 24*3, maohao, RED, 1*27);
        print_shuzi((int)Lux, 56, 24*3);
        Draw_FUHAO(56 + 16*get_n((int)Lux), 24*3, dian, RED, 1*27);
        print_shuzi(((int)(Lux*100))%100, 56 + 16*get_n((int)Lux) + 8, 24*3);
        Draw_FUHAO(56 + 16*get_n((int)Lux) + 8 + 16*2, 24*3, L, RED, 2*27);
        Draw_FUHAO(56 + 16*get_n((int)Lux) + 8 + 16*2 + 16, 24*3, u, RED, 2*27);
        Draw_FUHAO(56 + 16*get_n((int)Lux) + 8 + 16*2 + 16 + 16, 24*3, x, RED, 2*27);

        //海拔
        Draw_ZI(0, 24*4, hai, RED, 3 * 24);
        Draw_ZI(24, 24*4, ba, RED, 3 * 24);
        Draw_FUHAO(48, 24*4, maohao, RED, 1*27);
        print_shuzi(H, 56, 24*4);
        Draw_FUHAO(56 + 16*get_n(H), 24*4, m, RED, 3*27);

        //烟雾
        Draw_ZI(0, 24*5, yan, RED, 3 * 24);
        Draw_ZI(24, 24*5, wu, RED, 3 * 24);
        Draw_FUHAO(48, 24*5, maohao, RED, 1*27);
        print_shuzi(C, 56, 24*5);
        Draw_FUHAO(56 + 16*get_n((int)C) + 8 + 16*2, 24*5, p, RED, 2*27);
        Draw_FUHAO(56 + 16*get_n((int)C) + 8 + 16*3, 24*5, p, RED, 2*27);
        Draw_FUHAO(56 + 16*get_n(C) + 8 + 16*4, 24*5, m, RED, 3*27);


        printf("Lux=%.2flux, T=%.2fC, P=%.2fpa, Hum=%.2f%%, H=%dm,C=%.2fppm\n", Lux, T, P, Hum, H,C);

        beep_say(T);
        beep_say(C);
        
        for(int i=0;i<14;i++) { data[i] = 0; }
        sleep(1);
    }
}

// 显示数字
void print_shuzi(int num, int x, int y){
    int n = get_n(num);
    int offset = 0;

    while(num){
        offset = 16*(n-1);
        Draw_NUM(x + offset, y, num_ling_jiu[num%10], BLACK, 2*24);
        n--;num /= 10;
    }
}

int get_n(int num){
    int n = 1;
    while(num/10){
        n++;
        num /= 10;
    }
    return n;
}
