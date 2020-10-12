#include  <stdio.h>
#include  <stdlib.h>
#include  <sys/types.h>
#include  <sys/ipc.h>
#include  <sys/shm.h>
#include <unistd.h>
#include <sys/wait.h>

int randForSleep(){
  srand((time(NULL)));
  int randNum = ((rand()%5)+1);//up to 10 seconds
  return randNum;
}

int randForMoney_P(){//parent
  srand((time(NULL)*390));
  int randNum = ((rand()%100)+1);//generate random number between 0-100
  return randNum;
}

int randForMoney_C(){//child
  srand((time(NULL)*390));
  int randNum = ((rand()%50)+1);//generate random number between 0-50
  return randNum;
}

int main(){
  int BankAccount = 0;
  int Turn = 0;
  int balance;
  int ShmID;
  int *ShmPTR;
  
  ShmID = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666);
  ShmPTR = (int *) shmat(ShmID, NULL, 0);
     if (*ShmPTR == -1) {
          printf("*** shmat error (server) ***\n");
          exit(1);
     }
  ShmPTR[0] = BankAccount;
  ShmPTR[1] = Turn;

  pid_t pid;
  pid = fork();
  
  if (pid < 0)
    printf("Fork error. \n");
  
  //Parent process
  else if (pid > 0){
    int i = 1;
    for (i = 1; i <= 25; i++){
      sleep(randForSleep());
      while (ShmPTR[1] != 0);
      if (BankAccount <= 100){
        //Deposit Money
        balance = randForMoney_P();
        if (balance % 2 == 0){//if number is even
          //Deposit Money in Bank Account
          ShmPTR[0] += balance;
          //ShmPTR[0] += balance;
          printf("Dear old Dad: Deposits $%d / Balance = $%d\n", balance, ShmPTR[0]);
          //ShmPTR[1] = 1;
          //printf("Turn value is %d\n", ShmPTR[1]);
        }
        if (balance % 2 != 0){//if number is odd
          printf("Dear old Dad: Doesn't have any money to give\n");
          //ShmPTR[1] = 1;
         // printf("Turn value is %d\n", ShmPTR[1] );
        }
        //wait(NULL);//comment next run
        ShmPTR[1] = 1;
      }
      else {
        printf("Dear old Dad: Thinks Student has enough Cash ($%d)\n", ShmPTR[0]);
      }

     }
    exit(0);
  }
  //Child process
  else {
    int i = 1;
    for (i = 1; i <= 25; i++){
      sleep(randForSleep());
      while (ShmPTR[1] != 1);
      balance = randForMoney_C();
      printf("Poor Student needs $%d\n", balance);
      if (balance <= ShmPTR[0]){
        //balance <= BankAccount
        //Withdraw money from bank account
        ShmPTR[0] -= balance;
        printf("Poor Student:Withdraws $%d / Balance = $%d\n", balance, ShmPTR[0]);
       }
      if (balance > ShmPTR[0]){
        printf("Poor Student: Not Enough Cash ($%d)\n", ShmPTR[0]);
       }
       ShmPTR[1] = 0;
       }
     exit(0);
  }
    
  }
  
  
  