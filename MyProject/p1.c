// 标准输入输出库
#include <stdio.h> 
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
// 系统IO库
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>

// 内存映射
#include <sys/mman.h>

#include "p1.h"
int *lcdpoint = NULL;
int lcdid = -1;
struct Node* head = NULL;
DIR *dir = NULL;

// 双向循环链表的节点结构
struct Node {
    const char* data;
    struct Node* prev;
    struct Node* next;
};

// 在链表的末尾插入一个节点
void append(struct Node** head_ref, const char* new_data) {
    // 创建新节点
    struct Node* new_node = (struct Node*)malloc(sizeof(struct Node));
    new_node->data = new_data;

    // 如果链表为空，则将新节点作为头节点
    if (*head_ref == NULL) {
        new_node->prev = new_node;
        new_node->next = new_node;
        *head_ref = new_node;
        return;
    }

    // 找到链表的最后一个节点
    struct Node* last_node = (*head_ref)->prev;

    // 将新节点链接到链表的末尾
    last_node->next = new_node;
    new_node->prev = last_node;
    new_node->next = *head_ref;
    (*head_ref)->prev = new_node;
}





void Draw_Pixel(int x,int y,int color)
{
    if (x<800&&y<480)
    {
        *(lcdpoint+(y*800+x)) = color;
    }
    else
    printf("not in sc!");
	
}

void OpDirAndGetBMP(const char *pathname){

    dir = opendir(pathname); // 打开指定路径
    if (dir == NULL) {
        perror("path error!");
        return;
    }
    
    struct dirent *dt;
    
    while ((dt = readdir(dir)) != NULL) {

        if (strcmp(dt->d_name, ".") != 0 && strcmp(dt->d_name, "..") != 0) {

            const char* file_name = dt->d_name;
            size_t file_name_len = strlen(file_name);
            
            if (file_name_len >= 4 && strcmp(file_name + file_name_len - 4, ".bmp") == 0) {
            // 处理或保留bmp文件
                // printf("%s\n", file_name);
                append(&head,file_name);
               }
        }     

    }
    return;
}

void CloseDir(){
    closedir(dir); // 关闭目录
}


void Init_lcd(){
    lcdid = open("/dev/fb0",O_RDWR);
    if(lcdid == -1)
    {
        perror("open lcd failed");
        return ;
    }

    lcdpoint = mmap(NULL,800*480*4,PROT_READ|PROT_WRITE,MAP_SHARED,lcdid,0);
    if(lcdpoint == MAP_FAILED)
    {
        perror("mmap failed");
        close(lcdid);
        return ;
    }
}

void UnInit(){
    munmap(lcdpoint,800*480*4);
    close(lcdid);
}


typedef struct {//------------------------------------------------------------
    long thread_id;
    int h;
    int screenX;
    int screenY;
    int color;
} ThreadArgs;

void *DrawOT_function(void *arg)
{
    ThreadArgs *args = (ThreadArgs *)arg;

    // 获取参数值
    long thread_id = args->thread_id;
    int h = args->h;
    int screenX = args->screenX;
    int screenY = args->screenY;
    int color = args->color;

    // 计算每个百叶窗的高度
    int shutter_height = abs(480) / 4; // 假设将屏幕分为4个百叶窗

    // 获取当前行在百叶窗中的位置
    int shutter_position = (h / shutter_height) % 2;

    // 确定是否绘制当前像素
    if (shutter_position == thread_id)
    {
        // 在指定位置绘制像素点
        Draw_Pixel(screenX, screenY, color);
    }

    return NULL;
}

void DrawPicture(const char *pathname,int xg)
{
    int picture_id = open(pathname,O_RDONLY);
    if(picture_id == -1)
    {  
	   perror("picture open failed");
	   return;
    }

    // 读取BMP图片的一些属性信息
    // 读取文件魔数，用于描述该文件是什么类型的文件
    unsigned char data[4]={0};
    read(picture_id,data,2);
    if(data[0]!='B'||data[1]!='M') // if(data[0] != 0x42||data[1] != 0x4d)
    {
        printf("this picture not bmp\n");
        close(picture_id);
        return;
    }

    // 读取像素数组的偏移量
    lseek(picture_id,0x0a,SEEK_SET); // 偏移到像素数组偏移量数据的起始位置
    read(picture_id,data,4);

    // 1000 0000 | 0100 0000 | 0010 0000 | 0001 0000
    /*
                                      0001 0000 data[0]
                            0010 0000 0000 0000 data[1]
                  0100 0000 0000 0000 0000 0000 data[2]
        1000 0000 0000 0000 0000 0000 0000 0000 data[3]
    */
    int offset = data[3] << 24 | data[2] << 16  | data[1] << 8 | data[0];
    
    // 读取宽高
    lseek(picture_id,0x12,SEEK_SET);
    read(picture_id,data,4);
    int width = data[3] << 24 | data[2] << 16  | data[1] << 8 | data[0];

    // printf("width = %d\n",width);

    read(picture_id,data,4);
    int height = data[3] << 24 | data[2] << 16  | data[1] << 8 | data[0];

    // printf("height = %d",height);

    // 读取色深，只有知道色深才知道每个图片的像素点所占字节数
    lseek(picture_id,0x1c,SEEK_SET);
    read(picture_id,data,2);
    int depth = data[1] << 8 | data[0];

    // 计算图片的填充字节数
    int fills = 0; // 填充字节默认为零

    if((width*(depth/8))%4)
    {
        fills = 4 - ((width*(depth/8))%4);
    }

    // 求出每一行真实字节数
    int real_bytes =  (width*(depth/8)) + fills;

    // 搞一个动态数据用来保存像素数组的数据
    unsigned char *color_array = (unsigned char *)malloc(real_bytes*abs(height));
    unsigned char *color_point = color_array;
    lseek(picture_id,offset,SEEK_SET);
    read(picture_id,color_array,real_bytes*abs(height));
    pthread_t threads[4];
            
    int rc;
    long t = 0;
    ThreadArgs args[4];


switch (xg){
case 1://垂直覆盖
    for(int h = 0; h < abs(height); h++) {

    for(int w = 0; w < width; w++) {

        
        unsigned char a, r, g, b;
        b = *color_point++;
        g = *color_point++;
        r = *color_point++;
        a = (depth == 24) ? 0 : *color_point++;
        int color = a << 24 | r << 16 | g << 8 | b;
        
        int screenX = w + (800 - width) / 2;
        int screenY = ((height > 0) ? height - 1 - h : abs(height)) + (480 - abs(height)) / 2;
        
        Draw_Pixel(screenX, screenY, color);
    }
    // 跳过填充字节
    color_point += fills;
    }
    break;

case 2://---------------------------------------------------------------------
    for(int h = 0; h < abs(height); h++) {
        

           for(int w = 0; w < width; w++)
            { 

                unsigned char a, r, g, b;
                b = *color_point++;
                g = *color_point++;
                r = *color_point++;
                a = (depth == 24) ? 0 : *color_point++;
                int color = a << 24 | r << 16 | g << 8 | b;

            int screenX = w + (800 - width) / 2;
            int screenY = ((height > 0) ? height - 1 - h : abs(height)) + (480 - abs(height)) / 2;

            // pthread_t threads[4];
            
            // int rc;
            // long t = 0;
            // ThreadArgs args[4];


            for (t = 0; t < 4; t++) {
                args[t].thread_id = t;
                args[t].h = h;  // 初始化其他参数
                args[t].screenX = w + (800 - width) / 2;
                args[t].screenY = ((height > 0) ? height - 1 - h : abs(height)) + (480 - abs(height)) / 2;
                args[t].color = a << 24 | r << 16 | g << 8 | b;
                
            printf("Main: creating thread %ld\n", t);
           
            }
            // ThreadArgs *args;
           for (t = 0; t < 4; t++) {
              rc = pthread_create(&threads[t], NULL, DrawOT_function, (void *)&args[t]);
             if (rc) {
                printf("ERROR; return code from pthread_create() is %d\n", rc);
                return;
                }
            }
        for (t = 0; t < 4; t++) {
          pthread_join(threads[t], NULL);

        }
        // for (t = 0; t < 4; t++) {
        //   pthread_join(threads[t], NULL);

        // }
        
        }
        
        // 跳过填充字节
        color_point += fills;
    }

    break;

default:
    for(int h = 0; h < abs(height); h++) {
    for(int w = 0; w < width; w++) {
        unsigned char a, r, g, b;
        b = *color_point++;
        g = *color_point++;
        r = *color_point++;
        a = (depth == 24) ? 0 : *color_point++;
        int color = a << 24 | r << 16 | g << 8 | b;
        
        int screenX = w + (800 - width) / 2;
        int screenY = ((height > 0) ? height - 1 - h : abs(height)) + (480 - abs(height)) / 2;
        Draw_Pixel(screenX, screenY, color);
    }
    // 跳过填充字节
    color_point += fills;
    
    }

    break;
}


    
    free(color_array);
    close(picture_id);
}




void printList(struct Node* head) {
    struct Node* current = head;

    // 链表为空，直接返回
    if (head == NULL) {
        printf("链表为空\n");
        return;
    }

    // 遍历链表，并打印每个节点的值
    do {
        
        DrawPicture(current->data,1);
        current = current->next;
        sleep(2);
    } while (1);

    printf("\n");
}

int xc(char *argv){
    
    OpDirAndGetBMP(argv);
    Init_lcd();
    printList(head);
    UnInit();
    CloseDir();
    return 0;
}