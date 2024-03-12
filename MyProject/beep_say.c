#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int beep_say(float x)
{
	int fd,ret;
	char beep_ctrl[1]; //0 --> 不响  1 --->响
	
	//[1]打开蜂鸣器的驱动设备文件
	fd = open("/dev/pwm",O_RDWR);
	if(fd < 0)
	{
		printf("open /dev/pwm failed\n");
		return -1;
	}
	//[2]---write 写入控制命令
	if(x > 20.00)
	{
		beep_ctrl[0] = 0;  //不响
		ret = write(fd,beep_ctrl,sizeof(beep_ctrl));
		if( ret == -1)
		{
			perror("write");
		}
		sleep(1);
		
		beep_ctrl[0] = 1; 	//响
		ret = write(fd,beep_ctrl,sizeof(beep_ctrl));
		if( ret == -1)
		{
			perror("write");
		}
		sleep(1);
	}

	 //[3] 
	 close(fd);
	return 0;
}