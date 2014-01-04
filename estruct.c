#include "estruct.h"

void escribe_msg(MENSAJE**msg,char*emisor,char*destinatario,char*msj){
    int i=0;
    while(i<1000){
        if((*msg[i]).vacio==1)
            break;
        i++;
    }

    if(i<1000){
        time_t tiempo=time(0);
        struct tm * tlocal=localtime(&tiempo);
        char buffer[20];
        strftime(buffer,20,"%d/%m/%y %H:%M:%S",tlocal);
        (*msg[i]).vacio=0;
        sprintf((*msg[i]).emisor,"%s",emisor);
        sprintf((*msg[i]).destinatario,"%s",destinatario);
        sprintf((*msg[i]).mensaje,"%s",msj);
        sprintf((*msg[i]).fecha,"%s",buffer);
    }else{
        printf("Memoria de mensajes llena\n");
    }
}

void lee_mensajes(MENSAJE*cola,char*destinatario){
    if(cola!=NULL){
        int i;
        for(i=0;i<1000;i++){
            if(cola[i].vacio!=1){
                if(strcmp(cola[i].destinatario,destinatario)==0){
                    printf("De: %s. Hora:%s Mensaje: %s\n",cola[i].emisor,cola[i].fecha,cola[i].mensaje);
                    cola[i].vacio=1;
                    printf("->");
                }
            }
        }
    }
}

void lee_mensajes_grupal(MENSAJE*cola,char*usuario){
    if(cola!=NULL){
        int i;
        for(i=0;i<1000;i++){
            if(cola[i].vacio!=1){
                if(strcmp(cola[i].emisor,usuario)!=0){
                    printf("De: %s. Hora:%s Mensaje: %s\n",cola[i].emisor,cola[i].fecha,cola[i].mensaje);
                    printf("->");
                    usleep(20);
                    cola[i].vacio=1;
                }
            }
        }
    }
}

int verifica_lectores(){
    int i;
    for(i=0;i<100;i++){
        if(lectores[i]!=-1){
            if(lectores[i]==id_proceso)
                return 1;
        }else{
            return 0;
        }
    }
    return 0;
}

int agrega_lector(){
    int i;
    for(i=0;i<100;i++){
        if(lectores[i]==-1){
            lectores[i]=id_proceso;
            return 1;
        }
    }
    return 0;
}
