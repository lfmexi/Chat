#include <sys/shm.h>
#include <pthread.h>
#include <errno.h>
#include "estruct.h"

int id_turno=-1;
key_t key_proc,key_turno,key_lectores;
int mem_id_proc,mem_id_turno,mem_id_lectores;
char usuario[20];
MENSAJE*memoria1=NULL;
MENSAJE*memoria2=NULL;
void * func_thread_lectura_simple_mem1(void*par);
void * func_thread_lectura_simple_mem2(void*par);
void * func_thread_lectura_grupal(void*par);
int isNumeric(char*);
void conv_simple();
void conv_grupal();
void escritura_grupal();
void escrituraMem1();
void escrituraMem2();
void agregaProceso();
void levanta_proceso();
int P();
int V();
int needQuit(pthread_mutex_t *mtx);

int main()
{
    id_proceso=-1;
    key_t key,key2,key3;
    int mem_id,mem_id2,mem_id3;
    int * creada=NULL;

    key=ftok("/bin/cat",63);
    if(key==-1){
        printf("se arruinó \n");
        return 1;
    }

    mem_id=shmget(key,sizeof(int)*2,0777);
    if(mem_id==-1){
        mem_id=shmget(key,sizeof(int)*2,0777|IPC_CREAT);
    }

    if(mem_id==-1){
        printf("falló al crear la memoria compartida\n");
        return 1;
    }

    creada=(int*)shmat(mem_id,(char*)0,0);

    printf("%d",creada[0]);

    key2=ftok("/bin/ls",88);
    if(key2==-1){
        printf("se arruinó \n");
        return 1;
    }

    mem_id2=shmget(key2,sizeof(MENSAJE[1000]),0777);
    if(mem_id2==-1){
        mem_id2=shmget(key2,sizeof(MENSAJE[1000]),0777|IPC_CREAT);
    }

    if(mem_id2==-1){
        printf("falló al crear la memoria compartida\n");
        return 1;
    }

    memoria1=(MENSAJE*)shmat(mem_id2,(char*)0,0);

    if(memoria1==NULL){
        printf("falló al crear la memoria compartida\n");
        return 1;
    }

    key3=ftok("/bin/cp",99);
    if(key3==-1){
        printf("se arruinó \n");
        return 1;
    }

    mem_id3=shmget(key3,sizeof(MENSAJE[1000]),0777);
    if(mem_id3==-1){
        mem_id3=shmget(key3,sizeof(MENSAJE[1000]),0777|IPC_CREAT);
    }

    if(mem_id3==-1){
        printf("falló al crear la memoria compartida\n");
        return 1;
    }

    memoria2=(MENSAJE*)shmat(mem_id3,(char*)0,0);

    if(memoria2==NULL){
        printf("falló al crear la memoria compartida\n");
        return 1;
    }

    if(creada[0]!=1){
        creada[0]=1;
        int i;
        for(i=0;i<1000;i++){
            memoria1[i].vacio=1;
            memoria2[i].vacio=1;
        }
    }
    printf("Ingrese el nombre de usuario a utilizar: ");
    scanf("%s",usuario);
    int acepta=0;
    while(!acepta){
        char str[5];
        printf("Ingrese la opción deseada:\n");
        printf("1. Conversación simple\n");
        printf("2. Conversación grupal\n");
        printf("3. Salir\n->");
        scanf("%s",str);
        if(isNumeric(str)){
            acepta=atoi(str);
            switch (acepta){
                case 1:{
                    conv_simple();
                }break;
                case 2:{
                    conv_grupal();
                }
            }
        }else{
            printf("ingrese una opción válida!\n");
        }
        if(acepta==3){
            if(mem_id!=-1){
                shmdt((char*)creada);
                shmdt((char*)memoria1);
                shmdt((char*)memoria2);
                shmctl(mem_id,IPC_RMID,(struct shmid_ds*)NULL);
                shmctl(mem_id2,IPC_RMID,(struct shmid_ds*)NULL);
                shmctl(mem_id3,IPC_RMID,(struct shmid_ds*)NULL);
            }
            return 0;
        }
    }

    if(mem_id!=-1){
        shmdt((char*)creada);
        shmdt((char*)memoria1);
        shmdt((char*)memoria2);
        shmctl(mem_id,IPC_RMID,(struct shmid_ds*)NULL);
        shmctl(mem_id2,IPC_RMID,(struct shmid_ds*)NULL);
    }
    if(mem_id_proc!=-1){
        shmdt((char*)procs);
        shmdt((char*)turnos);
        shmctl(mem_id_proc,IPC_RMID,(struct shmid_ds*)NULL);
        shmctl(mem_id_turno,IPC_RMID,(struct shmid_ds*)NULL);
    }
    return 0;
}

void conv_simple(){
    pthread_t thread;
    pthread_mutex_t mutex;

    key_proc=ftok("/bin/echo",3);
    if(key_proc==-1){
        printf("se arruinó \n");
        return;
    }
    mem_id_proc=shmget(key_proc,sizeof(int)*4,0777);
    if(mem_id_proc==-1){
        mem_id_proc=shmget(key_proc,sizeof(int)*4,0777|IPC_CREAT);
        id_proceso=0;
    }else{
        id_proceso=1;
    }

    if(mem_id_proc==-1){
        printf("falló al crear la memoria compartidaP\n");
        return;
    }

    procs=(int*)shmat(mem_id_proc,(char*)0,0);

    key_turno=ftok("/bin/ps",33);
    if(key_turno==-1){
        printf("se arruinó \n");
        return;
    }
    mem_id_turno=shmget(key_turno,sizeof(int)*2,0777);
    if(mem_id_turno==-1){
        mem_id_turno=shmget(key_turno,sizeof(int)*2,0777|IPC_CREAT);
    }

    if(mem_id_turno==-1){
        printf("falló al crear la memoria compartidaT\n");
        return;
    }

    turnos=(int*)shmat(mem_id_turno,(char*)0,0);

    if(id_proceso==0){
        procs[0]=0;
        procs[1]=0;
        procs[2]=0;
        procs[3]=0;
        turnos[0]=1;
        turnos[1]=1;
    }
    char dest[20];
    printf("Ingrese el nombre del usuario con quien tendrá convesación:\n");
    scanf("%s",dest);
    printf("%d\n",id_proceso);

    pthread_mutex_init(&mutex,NULL);
    pthread_mutex_lock(&mutex);

    if(id_proceso==0){
        pthread_create(&thread,NULL,func_thread_lectura_simple_mem1,&mutex);
        escrituraMem2(dest);
    }else{
        pthread_create(&thread,NULL,func_thread_lectura_simple_mem2,&mutex);
        escrituraMem1(dest);
    }

    pthread_mutex_unlock(&mutex);
    pthread_join(thread,NULL);
}


void * func_thread_lectura_simple_mem2(void*par){
    pthread_mutex_t *mutex = par;
    while(!needQuit(mutex)){
        //algoritmo de Dekker 5
        procs[3]=1;
        while(procs[0]==1){
            if(turnos[0]==1){
                procs[3]=0;
                while(turnos[0]==1);
                procs[3]=1;
            }
        }
        //region crítica
        lee_mensajes(memoria2,usuario);
        turnos[0]=1;
        procs[3]=0;
    }
    return NULL;
}

void * func_thread_lectura_simple_mem1(void*par){
    pthread_mutex_t *mutex = par;
    while(!needQuit(mutex)){
        //algoritmo de Dekker 5
        procs[1]=1;
        while(procs[2]==1){
            if(turnos[1]==1){
                procs[1]=0;
                while(turnos[1]==1);
                procs[1]=1;
            }
        }
        //region crítica
        lee_mensajes(memoria1,usuario);
        turnos[1]=1;
        procs[1]=0;
    }
    return NULL;
}

void escrituraMem2(char*dest){
    while(1){
        char buffer[128];
        printf("->");
        gets(buffer);
        if(strcmp(buffer,"~q")==0){
            printf("adios\n");
            break;
        }else if(buffer[0]=='#'){
            switch(buffer[1]){
                case '1':
                    strcpy(buffer,":-)))");
                    break;
                case '2':
                    strcpy(buffer,":-(((");
                    break;
                case '3':
                    strcpy(buffer,":-DDD");
                    break;
                case '4':
                    strcpy(buffer,":-O");
                    break;
                case '5':
                    strcpy(buffer,";-)");
            }
        }
        procs[0]=1;
        while(procs[3]==1){
            if(turnos[0]==2){
                procs[0]=0;
                while(turnos[0]==2);
                procs[1]=1;
            }
        }
        //region crítica
        if(buffer[0]!='\0'){
            escribe_msg(&memoria2,usuario,dest,buffer);
        }
        procs[0]=0;
        turnos[0]=2;
    }
}

void escrituraMem1(char*dest){
    while(1){
        char buffer[128];
        printf("->");
        gets(buffer);
        if(strcmp(buffer,"~q")==0){
            printf("adios\n");
            break;
        }else if(buffer[0]=='#'){
            switch(buffer[1]){
                case '1':
                    strcpy(buffer,":-)))");
                    break;
                case '2':
                    strcpy(buffer,":-(((");
                    break;
                case '3':
                    strcpy(buffer,":-DDD");
                    break;
                case '4':
                    strcpy(buffer,":-O");
                    break;
                case '5':
                    strcpy(buffer,";-)");
            }
        }
        procs[2]=1;
        while(procs[1]==1){
            if(turnos[1]==2){
                procs[2]=0;
                while(turnos[1]==2);
                procs[2]=1;
            }
        }
        //region crítica
        if(buffer[0]!='\0'){
            escribe_msg(&memoria1,usuario,dest,buffer);
        }
        procs[2]=0;
        turnos[1]=2;
    }
}

int isNumeric(char*c){
    int i;
    for(i=0;i<sizeof(*c);i++){
        if(!(c[i]>=48 && c[i]<=57)){
            return 0;
        }
    }
    return 1;
}

int needQuit(pthread_mutex_t *mtx)
{
  switch(pthread_mutex_trylock(mtx)) {
    case 0:
      pthread_mutex_unlock(mtx);
      return 1;
    case EBUSY:
      return 0;
  }
  return 1;
}

void conv_grupal(){
    pthread_t thread;
    pthread_mutex_t mutex;

    key_proc=ftok("/bin/tar",33);
    if(key_proc==-1){
        printf("se arruinó \n");
        return;
    }
    mem_id_proc=shmget(key_proc,sizeof(int)*100,0777);
    if(mem_id_proc==-1){
        mem_id_proc=shmget(key_proc,sizeof(int)*100,0777|IPC_CREAT);
    }

    if(mem_id_proc==-1){
        printf("falló al crear la memoria compartidaP\n");
        return;
    }

    procs=(int*)shmat(mem_id_proc,(char*)0,0);

    key_turno=ftok("/bin/grep",33);
    if(key_turno==-1){
        printf("se arruinó \n");
        return;
    }
    mem_id_turno=shmget(key_turno,sizeof(int)*3,0777);
    if(mem_id_turno==-1){
        mem_id_turno=shmget(key_turno,sizeof(int)*3,0777|IPC_CREAT);
        id_proceso=0;
    }

    if(mem_id_turno==-1){
        printf("falló al crear la memoria compartidaT\n");
        return;
    }

    turnos=(int*)shmat(mem_id_turno,(char*)0,0);

    key_lectores=ftok("/bin/touch",30);
    if(key_lectores==-1){
        printf("se arruinó \n");
        return;
    }
    mem_id_lectores=shmget(key_lectores,sizeof(int)*100,0777);
    if(mem_id_lectores==-1){
        mem_id_lectores=shmget(key_lectores,sizeof(int)*100,0777|IPC_CREAT);
    }

    if(mem_id_lectores==-1){
        printf("falló al crear la memoria compartidaL\n");
        return;
    }

    lectores=(int*)shmat(mem_id_lectores,(char*)0,0);

    if(id_proceso==0){
        int i;
        for(i=0;i<100;i++){procs[i]=-1;lectores[i]=-1;}
        turnos[0]=1;
        turnos[1]=1;
        turnos[2]=0;
    }else{
        id_proceso=turnos[0];
        turnos[0]++;
    }
    printf("%d\n",id_proceso);
    pthread_mutex_init(&mutex,NULL);
    pthread_mutex_lock(&mutex);
    pthread_create(&thread,NULL,func_thread_lectura_grupal,&mutex);
    escritura_grupal();
    pthread_mutex_unlock(&mutex);
    pthread_join(thread,NULL);
}

void * func_thread_lectura_grupal(void*par){
    pthread_mutex_t *mutex = par;
    while(!needQuit(mutex)){
        lee_mensajes_grupal(memoria1,usuario);
        usleep(10);
    }
    return NULL;
}

void escritura_grupal(){
    while(1){
        char buffer[128];
        printf("->");
        gets(buffer);
        if(strcmp(buffer,"~q")==0){
            printf("adios\n");
            break;
        }else if(buffer[0]=='#'){
            switch(buffer[1]){
                case '1':
                    strcpy(buffer,":-)))");
                    break;
                case '2':
                    strcpy(buffer,":-(((");
                    break;
                case '3':
                    strcpy(buffer,":-DDD");
                    break;
                case '4':
                    strcpy(buffer,":-O");
                    break;
                case '5':
                    strcpy(buffer,";-)");
            }
        }
        if(buffer[0]!='\0'){
            int continua=0;
            int i=0;
            while(!continua){
                continua=P();
                if(!continua&&i==0){
                    agregaProceso();
                }else if(continua && i>0){
                    if(procs[0]!=id_proceso){
                        continua=0;
                    }
                }
                i++;
            }
            escribe_msg(&memoria1,usuario,"todos",buffer);
            usleep(10);
            if(V()){
                levanta_proceso();
            }
        }
    }
}

int P(){
    if(turnos[1]>0){
        turnos[1]--;
        return 1;
    }else{
        return 0;
    }
}

int V(){
    if(procs[0]==-1&&turnos[1]<1){
        turnos[1]++;
    }else if(procs[0]!=-1){
        return 1;
    }
    return 0;
}

void agregaProceso(){
    int i=0;
    while(i<100){
        if(procs[i]==-1)
            break;
        i++;
    }
    if(i<100){
        procs[i]=id_proceso;
    }
}

void levanta_proceso(){
    if(procs[0]!=-1){
        procs[0]=-1;
        int i;
        for(i=1;i<100;i++){
            if(procs[i]!=-1){
                procs[i-1]=procs[i];
                procs[i]=-1;
            }else{
                break;
            }
        }
    }
}
