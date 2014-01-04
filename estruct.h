#ifndef ESTRUCT_H_INCLUDED
#define ESTRUCT_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
typedef struct msj{
    int vacio;
    char emisor[20];
    char destinatario[20];
    char mensaje[128];
    char fecha[20];
}MENSAJE;

void escribe_msg(MENSAJE**msg,char*emisor,char*destinatario,char*msj);

void lee_mensajes(MENSAJE*,char*dest);

void lee_mensajes_grupal(MENSAJE*,char*);
int * turnos;
int * procs;
int * lectores;
int id_proceso;
int verifica_lectores();
int agrega_lector();
#endif // ESTRUCT_H_INCLUDED
