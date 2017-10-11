#include <stdio.h>         
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <linux/input.h>
#include <sys/ioctl.h>
#include <errno.h>


#define ACTION_FILE_NAME "/data/misc/camera/action.txt"
#define MAX_LINE 1024
#define MAX_INTPUT_DEVICES 10

int fd[MAX_INTPUT_DEVICES] = {0};
int play_action();

typedef struct{
    int fd;
    char device[32];
    unsigned int sec;
    unsigned int usec;
    unsigned int type;
    unsigned int code;
    unsigned int value;
    int dev_idx;
}event_t;

void usage()
{
	printf("----------Usage:---------------\n");
    printf(" -r:for recording action\n");
    printf(" -p:for playing action\n");
}

int main(int argc, char *argv[])         
{        
    int ch;
    char cmd[64] = {0};

    if(argc != 2){
        usage();
    }

    while((ch=getopt(argc,argv,"rp"))!=-1)         
    { 
        switch(ch)       
        {         
            case 'r':        
                sprintf(cmd, "%s %s", "getevent -t >", ACTION_FILE_NAME);
                printf("cmd:%s\n", cmd);
                system(cmd);      
                break;         
            case 'p':         
                printf("option p: playing action\n");
		    play_action();
                break;         
            default:         
                usage();
        }        
    }  
  
    return 0;  
}

int play_action()
{
    FILE *fp;
    char strLine[MAX_LINE];
    char sec[16] = {0};
    char usec[16] = {0};
    long interval = 0;
    char cmd[64] = {0};
    int last_sec, last_usec = 0;
    unsigned int line_num = 0, i;
    int last_fd = -1;
    char last_device[32] = {0};
    int dev_idx = -1, ret = -1;
    int version;
    struct input_event event;

    event_t *rd_buf = (event_t *)malloc(sizeof(event_t)*1024*1024);
    memset(rd_buf, 0, sizeof(event_t)*1024*1024);

    if((fp = fopen(ACTION_FILE_NAME, "r")) == NULL)
    {   
        printf("Open Falied!");   
        goto ERROR1;   
    }

    while (!feof(fp))	//parse event info & calc line number
    {
        fgets(strLine, MAX_LINE, fp);
        if('[' == strLine[0]){
            sscanf(strLine, "%*[^ ] %[^.].%[^]]] %[^:]:%x %x %x", sec,    \
            usec, rd_buf[line_num].device, &rd_buf[line_num].type,    \
            &rd_buf[line_num].code, &rd_buf[line_num].value);
            rd_buf[line_num].sec = atoi(sec);
            rd_buf[line_num].usec = atoi(usec);
            sscanf(rd_buf[line_num].device, "/dev/input/event%d", &dev_idx);
            rd_buf[line_num].dev_idx = dev_idx;
            if(fd[dev_idx] == 0){
                fd[dev_idx] = rd_buf[line_num].fd = open(rd_buf[line_num].device, O_RDWR);
                printf("opening fd:%d %d\n", fd[dev_idx], fd[1]);
                if(fd[dev_idx] < 0){
                    printf("Could not open %s", rd_buf[line_num].device);
                    goto ERROR1;
                }
            }
            line_num++;
        }
    }
    printf("line_num:%d\n", line_num);
    last_sec = rd_buf[0].sec;
    last_usec = rd_buf[0].usec;

    for(i = 0; i < line_num; i++){
        interval = (rd_buf[i].sec - last_sec)*1000000LL + (rd_buf[i].usec - last_usec);
        usleep(interval);
        memset(&event, 0, sizeof(event));
        event.type = rd_buf[i].type;
        event.code = rd_buf[i].code;
        event.value = rd_buf[i].value;
        printf("fd:%d idx:%d %d %d %d\n", fd[1], rd_buf[i].dev_idx, event.type, event.code, event.value);
        ret = write(fd[rd_buf[i].dev_idx], &event, sizeof(event));
        if(ret < (ssize_t) sizeof(event)) {
            fprintf(stderr, "write event failed, %s\n", strerror(errno));
            goto ERROR1;
        }
        last_sec = rd_buf[i].sec;
        last_usec = rd_buf[i].usec;
    }

ERROR1:
    for(i = 0; i < MAX_INTPUT_DEVICES; i++){
        if(fd[i] > 0){
            close(fd[i]);
        }
    }
    fclose(fp);
ERROR2:    
    free(rd_buf);

    return 0;	
}



















