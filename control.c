#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <string.h>
#include <errno.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/types.h>

#define KEY 23005
#define KEY2 34567

int print_error(int result){
  if(result == -1){
    printf("Error: %s\n", strerror(errno));
    return -1;
  }
  return 0;
}

int* create_shmem(int data){
  int d = shmget(KEY2, sizeof(int), 0777|IPC_CREAT);
  if(print_error(d)) return NULL;
  int* pointer = shmat(d, 0, 0);
  return pointer;
}

int get_shmem(){
  int d = shmget(KEY2, sizeof(int), 0777);
  if(print_error(d)) return -1;
  int* pointer = shmat(d, 0, SHM_RDONLY);
  if(print_error(*pointer)) return -1;
  return *pointer;
}

int det_shmem(){
  int d = shmget(KEY2, sizeof(int), 0777);
  if(print_error(d)) return -1;
  int* pointer = shmat(d, 0, SHM_RDONLY);
  int r = shmdt(pointer);
  if(print_error(r)) return -1;
  return 0;
}

int view(){
  int data = get_shmem();
  if(print_error(data)) return -1;
  //printf("Data: %d\n", data);
  int val = semctl(data, 0, GETVAL);
  //printf("value: %d\n", val);
  if(print_error(val)) return -1;
  return val;
}

int create(char* val){
  int d = semget(KEY, 1, 0777|IPC_CREAT|IPC_EXCL);
  if(d == -1){
     printf("Semaphore Exists!\n");
     return -1;
  }
  int* data = create_shmem(d);
  *data = d;
  int converted;
  sscanf(val, "%d", &converted);
  semctl(d, 0, SETVAL, converted);
  printf("Semaphore created: %d\n",d);
  printf("Value Set: %d\n", converted);
  return 0;
}

int remove_sem(){
  int d = semctl(get_shmem(), 0, IPC_RMID, 0);
  if(print_error(d)) return -1;
  return det_shmem();
}

int main(int argc, char* argv[]){
  if(argc == 1){
    printf("Enter an Argument\n");
    return -1;
  }
  char* argument = argv[1];
  if(!strcmp(argument, "-v")){
    printf("Semaphore Value: %d\n", view());
  }
  else if(!strcmp(argument, "-c")){
    if(argc == 2){
      printf("Need Initial Value\n");
      return -1;
    }
    create(argv[2]);
  }
  else if(!strcmp(argument, "-r")){
    printf("Semaphore removed: %d\n", remove_sem());
  }
  else{
    printf("Invalid Argument\n");
    return -1;
  }
  return 0;
}
