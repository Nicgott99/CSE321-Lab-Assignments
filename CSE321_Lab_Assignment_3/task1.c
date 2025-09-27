#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>

struct shared{
    char sel[100];
    int b;
};

int main(){
    key_t key = ftok(".", 's');
    int shm_id = shmget(key, sizeof(struct shared), IPC_CREAT | 0666);
    if(shm_id == -1){
        perror("shmget");
        exit(1);
    }
    
    struct shared *shm_ptr = (struct shared *)shmat(shm_id, NULL, 0);
    if(shm_ptr == (void *)-1){
        perror("shmat");
        exit(1);
    }
    
    int fd[2];
    if(pipe(fd) == -1){
        perror("pipe");
        exit(1);
    }
    
    shm_ptr->b = 1000;
    
    printf("Provide Your Input From Given Options:\n");
    printf("1. Type a to Add Money\n");
    printf("2. Type w to Withdraw Money\n");
    printf("3. Type c to Check Balance\n");
    
    char input[100];
    scanf("%s", input);
    
    strcpy(shm_ptr->sel, input);
    printf("Your selection: %s\n", shm_ptr->sel);
    
    pid_t pid = fork();
    if(pid < 0){
        perror("fork");
        exit(1);
    }
    
    if(pid == 0){
        close(fd[0]);
        
        if(strcmp(shm_ptr->sel, "a") == 0){
            printf("Enter amount to be added:\n");
            int amount;
            scanf("%d", &amount);
            
            if(amount > 0){
                shm_ptr->b += amount;
                printf("Balance added successfully\n");
                printf("Updated balance after addition:\n");
                printf("%d\n", shm_ptr->b);
            } else {
                printf("Adding failed, Invalid amount\n");
            }
        } 
        else if(strcmp(shm_ptr->sel, "w") == 0){
            printf("Enter amount to be withdrawn:\n");
            int amount;
            scanf("%d", &amount);
            
            if(amount > 0 && amount <= shm_ptr->b){
                shm_ptr->b -= amount;
                printf("Balance withdrawn successfully\n");
                printf("Updated balance after withdrawal:\n");
                printf("%d\n", shm_ptr->b);
            } else {
                printf("Withdrawal failed, Invalid amount\n");
            }
        }
        else if(strcmp(shm_ptr->sel, "c") == 0){
            printf("Your current balance is:\n");
            printf("%d\n", shm_ptr->b);
        }
        else {
            printf("Invalid selection\n");
        }
        
        char msg[] = "Thank you for using";
        write(fd[1], msg, strlen(msg) + 1);
        
        close(fd[1]);
        exit(0);
    } 
    else {
        close(fd[1]);
        
        wait(NULL);
        
        char buffer[100];
        read(fd[0], buffer, sizeof(buffer));
        printf("%s\n", buffer);
        
        close(fd[0]);
        
        shmdt(shm_ptr);
        shmctl(shm_id, IPC_RMID, NULL);
    }
    
    return 0;
}
