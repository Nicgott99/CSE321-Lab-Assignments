#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>

struct msg{
    long int type;
    char txt[6];
};

int main(){
    key_t key = ftok(".", 'm');
    int msgid = msgget(key, IPC_CREAT | 0666);
    if(msgid == -1){
        perror("msgget");
        exit(1);
    }
    
    printf("Please enter the workspace name:\n");
    char workspace[10];
    scanf("%s", workspace);
    
    if(strcmp(workspace, "cse321") != 0){
        printf("Invalid workspace name\n");
        msgctl(msgid, IPC_RMID, NULL);
        exit(0);
    }
    
    struct msg message;
    message.type = 1;
    strcpy(message.txt, workspace);
    msgsnd(msgid, &message, sizeof(message.txt), 0);
    printf("Workspace name sent to otp generator from log in: %s\n", message.txt);
    
    pid_t pid1 = fork();
    if(pid1 < 0){
        perror("fork");
        exit(1);
    }
    
    if(pid1 == 0){
        struct msg received_msg;
        msgrcv(msgid, &received_msg, sizeof(received_msg.txt), 1, 0);
        printf("OTP generator received workspace name from log in: %s\n", received_msg.txt);
        
        pid_t otp = getpid();
        char otp_str[6];
        snprintf(otp_str, sizeof(otp_str), "%d", otp);
        
        struct msg otp_msg;
        otp_msg.type = 2;
        strcpy(otp_msg.txt, otp_str);
        msgsnd(msgid, &otp_msg, sizeof(otp_msg.txt), 0);
        printf("OTP sent to log in from OTP generator: %s\n", otp_msg.txt);
        
        otp_msg.type = 3;
        msgsnd(msgid, &otp_msg, sizeof(otp_msg.txt), 0);
        printf("OTP sent to mail from OTP generator: %s\n", otp_msg.txt);
        
        pid_t pid2 = fork();
        if(pid2 < 0){
            perror("fork");
            exit(1);
        }
        
        if(pid2 == 0){
            struct msg mail_msg;
            msgrcv(msgid, &mail_msg, sizeof(mail_msg.txt), 3, 0);
            printf("Mail received OTP from OTP generator: %s\n", mail_msg.txt);
            
            mail_msg.type = 4;
            msgsnd(msgid, &mail_msg, sizeof(mail_msg.txt), 0);
            printf("OTP sent to log in from mail: %s\n", mail_msg.txt);
            
            exit(0);
        } 
        else {
            wait(NULL);
            exit(0);
        }
    } 
    else {
        wait(NULL);
        
        struct msg otp_from_generator;
        msgrcv(msgid, &otp_from_generator, sizeof(otp_from_generator.txt), 2, 0);
        printf("Log in received OTP from OTP generator: %s\n", otp_from_generator.txt);
        
        struct msg otp_from_mail;
        msgrcv(msgid, &otp_from_mail, sizeof(otp_from_mail.txt), 4, 0);
        printf("Log in received OTP from mail: %s\n", otp_from_mail.txt);
        
        if(strcmp(otp_from_generator.txt, otp_from_mail.txt) == 0){
            printf("OTP Verified\n");
        } else {
            printf("OTP Incorrect\n");
        }
        
        msgctl(msgid, IPC_RMID, NULL);
    }
    
    return 0;
}
