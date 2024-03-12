#ifndef  __P1_H__
#define  __P1_H__

int xc(char *argv);
void Draw_Pixel(int x,int y,int color);
void CloseDir();
void Init_lcd();
void UnInit();
void *DrawOT_function(void *arg);
void DrawPicture(const char *pathname,int xg);


#endif