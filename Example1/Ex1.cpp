#include <pthread.h>
#include <iostream>
#include <string.h>
#include <unistd.h>
using namespace std;


struct Args{
    int id;
    char* buf;
    pthread_mutex_t* mutex;
};

int readString(char* string){
  //  char string[128]={0};
    cout<<"Input your string: " << endl;
    cin.getline (string, 128);
    if (strlen(string)>64){
        cout << "Sorry! It is large!";
        return 1;
    };
    //cout <<"Input str is" << string;
    return 0;
}

void* threadFunction(void *arg){
    while(true){
        
        struct Args * goodArgs =( struct Args *) arg;
        char string[128]={0};
         pthread_mutex_lock(goodArgs->mutex);
        //cout << "Strlen is"<< strlen(string)<<endl;
        readString(string);
        cout << "Hello from " << goodArgs->id << " thread!" << endl;
        cout << "1. String is "<<string <<endl;
       
        for (int i=0; i<strlen(string); i++)
            goodArgs->buf[i]=string[i];
        cout << "1. Current buffer is: " << goodArgs->buf << endl;
    //*(goodArgs->buf) = 100;
        pthread_mutex_unlock(goodArgs->mutex);
        sleep(0.25);
    }
}


void* threadFunction2(void *arg){
    while(true){
    struct Args * goodArgs =( struct Args *) arg;
    sleep(0.2);
    cout << "we wate mutex";
    pthread_mutex_lock(goodArgs->mutex);
    cout << "Hello from " << goodArgs->id << " thread!" << endl;
    
    cout << "2. Current buffer is: " << goodArgs->buf << endl;
    memset (goodArgs->buf, '\n', strlen(goodArgs->buf));
    //*(goodArgs->buf) = 2;
    pthread_mutex_unlock(goodArgs->mutex);
    }
}

int main(){
    pthread_t tid; /* идентификатор потока */
    pthread_t tid2; /* идентификатор потока */
    pthread_mutex_t lock;
    int id = 1;
    int id2 = 2;

    //
    char buffer[128] = {0};
    if (pthread_mutex_init(&lock, NULL) != 0)
    {
        printf("\n mutex init failed\n");
        return 1;
    }
    Args args1;
    args1.id = id;
    args1.buf = buffer;
    args1.mutex = &lock;
    Args args2;
    args2.id = id2;
    args2.buf = buffer;
    args2.mutex = &lock;
    pthread_create(&tid2, NULL, threadFunction, &args1);
    
    pthread_create(&tid, NULL, threadFunction2, &args2);
    
    pthread_join(tid,NULL);
    pthread_join(tid2,NULL);
    cout << "The thread execution is end! GoodBye!";
    return 0;
}