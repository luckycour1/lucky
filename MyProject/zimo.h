#ifndef __ZIMO_H__
#define __ZIMO_H__

#define RED 0xFF0000
#define GREEN 0x00FF00
#define BLUE 0x0000FF
#define WHITE 0xFFFFFF
#define BLACK 0x000000

extern unsigned char wen[];
extern unsigned char du[];
extern unsigned char shi[];
extern unsigned char qi[];
extern unsigned char ya[];
extern unsigned char guang[];
extern unsigned char zhao[];
extern unsigned char hai[];
extern unsigned char ba[];
extern unsigned char yan[];
extern unsigned char wu[];
extern unsigned char maohao[];
extern unsigned char sheshidu[];
extern unsigned char baifenhao[];
extern unsigned char p[];
extern unsigned char a[];
extern unsigned char L[];
extern unsigned char u[];
extern unsigned char x[];
extern unsigned char m[];
extern unsigned char dian[];
extern unsigned char num_ling_jiu[10][48];

void Draw_ZI(int x, int y,  unsigned char zi[], int color, int length);
void Draw_FUHAO(int x, int y, unsigned char fuhao[], int color, int length);
void Draw_NUM(int x, int y, unsigned char num[], int color, int length);

#endif
