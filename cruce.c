#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>    //Necesario para semget, shmget, msgget
#include <sys/sem.h>    //Necesario para semget
#include <sys/shm.h>    //Necesario para shmget
#include <sys/msg.h>    //Necesario para msgget
#include <signal.h>     //Necesario para sigaction
#include <sys/types.h>  //Necesario para wait
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#include "cruce.h" 

//variables globales (son necesarias para usarlas en la manjeradora cuando recibimos CTRL + C)
int semaforo = -1, buzon = -1, memoria = -1, padre;
void *p_memoria;

struct mensaje {
    long tipo_mensaje;
    int PID;
    int num_solicitudes;
};

union semun {
    int val;
    struct semid_ds *buf;
    //ushort_t *array;
} union_s;

//Prototipos de función
void manejadora();
void n_pausas(int n);
void poner_semaforo(int tipo_semaforo, int valor);
int generar_coordenada_reserva(int coord_x, int coord_y);
void enviar_mensaje(long tipo_mensaje, int PID, int num_solicitudes); 
struct mensaje recibir_mensaje(long tipo_mensaje, int flags, int *cod_error); 
void operacion_semaforo(int num_sem, int operacion, int flag);
void finalizar_programa();

int main(int argc, char *argv[]) {
    int i, hijo, esperar_posicion_siguiente = 0;
    int n_max_procesos = -1;
    int uso_cpu = -1;
    int cod_error;
    struct posiciOn pos, pos_actual, pos_actual1, pos_actual2, pos_actual3, pos_actual4, pos_actual5;
    pos_actual1.x = 10000;
    pos_actual1.y = 10000;
    pos_actual2.x = 10000;
    pos_actual2.y = 10000;
    pos_actual3.x = 10000;
    pos_actual3.y = 10000;
    pos_actual4.x = 10000;
    pos_actual4.y = 10000;
    pos_actual5.x = 10000;
    pos_actual5.y = 10000;
    struct sigaction comp_nuevo, comp_inicial;
    int valor_devuelto_fork;
    int entre_en_el_cruce1 = 0, entre_en_el_cruce2 = 0, entre_en_el_cruce3 = 0, entre_en_el_cruce4 = 0;
    int sali_del_cruce1 = 0, sali_del_cruce2 = 0, sali_del_cruce3 = 0;
    int posicion_conflictiva = 0;
    
    comp_nuevo.sa_handler = manejadora;
    sigemptyset(&comp_nuevo.sa_mask);
    comp_nuevo.sa_flags = 0;
    
    if (sigaction(SIGINT, &comp_nuevo, &comp_inicial) == -1) {
        perror("sigaction:");
        finalizar_programa();
    }
            
    //--------Bloque de comprobación de argumentos------------------------------------------------
    if (argc != 3) {
        printf("Error: Numero de parametros incorrecto\n");
        printf("Uso: ./cruce <numero_maximo_de_procesos_simultaneos> <numero_positivo_para_indicar_la_velocidad_de ejecucion>\n"); 
        finalizar_programa();
    }
    else {
        if (atoi(argv[2]) < 0) { 
            printf("Error: El segundo parámetro debe ser un número entero mayor o igual a cero\n");
            finalizar_programa();
        }
        else {   
            n_max_procesos = atoi(argv[1]);
            uso_cpu = atoi(argv[2]);
        }
    }       
    //---------------------------------------------------------------- 
    size_t tamano_memoria = 276;
    int num_semaforos = 8;
    padre = getpid(); 
    
    semaforo = semget(IPC_PRIVATE, num_semaforos, IPC_CREAT | 0600);
    if (semaforo == -1) {
        perror("semget:");
        finalizar_programa();
    }

    buzon = msgget(IPC_PRIVATE, IPC_CREAT | 0600);
    if (buzon == -1) {
        perror("msgget:");
        finalizar_programa();
    }

    memoria = shmget(IPC_PRIVATE, tamano_memoria, IPC_CREAT | 0600);
    if (memoria == -1) {
        perror("shmget:");
        finalizar_programa();
    }
    
    p_memoria = shmat(memoria, 0, 0); 
    if (p_memoria == (void *) -1) {
        perror("shmget:");
        finalizar_programa();
    }


    int *procesos_activos = p_memoria + 256;
    *procesos_activos = 2; 
    short *estado_cruce1 = p_memoria + 260;
    short *estado_cruce2 = p_memoria + 264;
    short *estado_cruce3 = p_memoria + 268;
    short *estado_cruce4 = p_memoria + 272;
    *estado_cruce1 = 0;
    *estado_cruce2 = 0;
    *estado_cruce3 = 0;
    *estado_cruce4 = 0;
    
    union_s.val = 1;
    
    if (semctl(semaforo, 1, SETVAL, union_s) == -1) {
        perror("semctl:");
        finalizar_programa();
    }

    if (semctl(semaforo, 2, SETVAL, union_s) == -1) {
        perror("semctl:");
        finalizar_programa();
    } 
    
    if (semctl(semaforo, 3, SETVAL, union_s) == -1) { 
        perror("semctl:");
        finalizar_programa();
    } 
    
    if (semctl(semaforo, 4, SETVAL, union_s) == -1) { 
        perror("semctl:");
        finalizar_programa();
    } 
    
    if (semctl(semaforo, 5, SETVAL, union_s) == -1) { 
        perror("semctl:");
        finalizar_programa();
    } 
    
    if (semctl(semaforo, 6, SETVAL, union_s) == -1) { 
        perror("semctl:");
        finalizar_programa();
    }
    enviar_mensaje(5, 0, 0); 
    enviar_mensaje(6, 0, 0); 
    enviar_mensaje(7, 0, 0); 
    enviar_mensaje(8, 0, 0); 

    if (CRUCE_inicio(uso_cpu, n_max_procesos, semaforo, p_memoria) == -1) {
        perror("CRUCE_inicio:");
        finalizar_programa();
    }
    
    valor_devuelto_fork = fork(); 
    
    if (valor_devuelto_fork != 0) {
        for(;;) {           
            if (n_max_procesos == *procesos_activos) {
                wait(NULL);
            }  
            else {       
            hijo = CRUCE_nuevo_proceso();
            if (CRUCE_nuevo_proceso() == -1) {
                perror("CRUCE_nuevo_proceso:");
                finalizar_programa();
            }
            operacion_semaforo(2, -1, 0); 
            *procesos_activos = *procesos_activos + 1;
            operacion_semaforo(2, 1, 0); 
                      
            switch(fork()) {
                case -1: 
                    perror("fork:");
                    finalizar_programa();
                case 0:
                    if (hijo == COCHE) {
                        int tipo_mensaje_coche;
                        pos = CRUCE_inicio_coche();                   
                        struct mensaje ubicacion_coche;
                        do {
                            if (pausa_coche() == -1) {
                                perror("pausa_coche:");
                                finalizar_programa();
                            }
                            if (pos.x == 13) { 
                                recibir_mensaje(2, 0, &cod_error); 
                                posicion_conflictiva = 2;
                            }
                            if (pos.y == 6) { 
                                recibir_mensaje(1, 0, &cod_error);
                                posicion_conflictiva = 1;
                            }
                            if (pos.x >= 25 && pos.y >= 6 && pos.y <= 15 && entre_en_el_cruce3 == 0) { 
                                recibir_mensaje(7, 0, &cod_error); 
                                entre_en_el_cruce3 = 1;
                            }
                            if (pos.y == 16 && sali_del_cruce3 == 0) { //si no estamos en las coordenadas del cruce marcar el cruce como libre                                
                                enviar_mensaje(7, 0, 0); 
                                sali_del_cruce3 = 1;
                            }
                                                        
                            if (pos.x >= 13 && pos.y >= 6 && entre_en_el_cruce4 == 0) {
                                operacion_semaforo(6, -1, 0);
                                
                                *estado_cruce4 = *estado_cruce4 + 1;
                                recibir_mensaje(8, 1, &cod_error);
                                entre_en_el_cruce4 = 1;
                                
                                operacion_semaforo(6, 1, 0);                              
                            }
                               
                            
                            if (esperar_posicion_siguiente == 1) { 
                                recibir_mensaje(generar_coordenada_reserva((pos.x + 7) * 100, pos.y), 0, &cod_error);
                                esperar_posicion_siguiente = 0;
                            }
                            
                            operacion_semaforo(1, -1, 0);
                            
                            ubicacion_coche = recibir_mensaje(generar_coordenada_reserva(pos.x, pos.y), 1, &cod_error);                
                            if (cod_error != -1) {                               
                                enviar_mensaje(generar_coordenada_reserva(pos.x, pos.y), getpid(), 0);
                                esperar_posicion_siguiente = 1;
                            }                            
                            else { 
                                enviar_mensaje(generar_coordenada_reserva(pos.x, pos.y), getpid(), 0);                                   
                                ubicacion_coche = recibir_mensaje(generar_coordenada_reserva(pos_actual5.x, pos_actual5.y), 1, &cod_error);                    
                                if (cod_error != -1) { 
                                    if (getpid() != ubicacion_coche.PID) { 
                                        enviar_mensaje(generar_coordenada_reserva((pos_actual5.x + 7) * 100, pos_actual5.y), getpid(), 0);
                                    }
                                }
                                pos_actual5 = pos_actual4;
                                pos_actual4 = pos_actual3;
                                pos_actual3 = pos_actual2;               
                                pos_actual2 = pos_actual1;
                                                                                                                                                                                    
                                pos_actual1 = pos; 
                                pos = CRUCE_avanzar_coche(pos);
                            }
                            
                            if (posicion_conflictiva != 0) {
                                enviar_mensaje(posicion_conflictiva, 0, 0);
                                posicion_conflictiva = 0;
                            }
                           
                            if (pos.y >= 0) {
                                operacion_semaforo(1, 1, 0);
                            }                                                      
                        } while (pos.y >= 0);

                        ubicacion_coche = recibir_mensaje(generar_coordenada_reserva(pos_actual1.x, pos_actual1.y), 1, &cod_error);
                        ubicacion_coche = recibir_mensaje(generar_coordenada_reserva(pos_actual2.x, pos_actual2.y), 1, &cod_error);
                        ubicacion_coche = recibir_mensaje(generar_coordenada_reserva(pos_actual3.x, pos_actual3.y), 1, &cod_error);
                        ubicacion_coche = recibir_mensaje(generar_coordenada_reserva(pos_actual4.x, pos_actual4.y), 1, &cod_error);
                        ubicacion_coche = recibir_mensaje(generar_coordenada_reserva(pos_actual5.x, pos_actual5.y), 1, &cod_error);
                        
                        if (getpid() != ubicacion_coche.PID) { 
                            enviar_mensaje(generar_coordenada_reserva((pos_actual5.x + 7) * 100, pos_actual5.y), getpid(), 0);
                        }
                        
                        operacion_semaforo(6, -1, 0);
                            
                        *estado_cruce4 = *estado_cruce4 - 1;
                        if (*estado_cruce4 == 0) {
                            enviar_mensaje(8, 0, 0);
                        }
                                
                        operacion_semaforo(6, 1, 0);
                        
                         
                        operacion_semaforo(2, -1, 0);
                        if (CRUCE_fin_coche() == -1) {
                             perror("CRUCE_fin_coche:");
                             finalizar_programa();
                        }
                        *procesos_activos = *procesos_activos - 1;
                        operacion_semaforo(2, 1, 0);
                        
                        operacion_semaforo(1, 1, 0);
                        
                        exit(0);
                    }
                    else if (hijo == PEAToN) {
                            int tipo_mensaje_peaton;                                                                                  
                            struct mensaje ubicacion_peaton;
                            
                            operacion_semaforo(1, -1, 0);
                            
                            pos = CRUCE_inicio_peatOn_ext(&pos_actual);                             
                            enviar_mensaje(generar_coordenada_reserva(pos_actual.x, pos_actual.y), getpid(), 0);
                    
                            operacion_semaforo(1, 1, 0);

                        do {
                            if (pausa() == -1) {
                                perror("pausa:");
                                finalizar_programa();
                            }
                            if (pos.y == 11 && pos.x < 30) { 
                                recibir_mensaje(4, 0, &cod_error);
                                posicion_conflictiva = 4;
                            }
                            if (pos.x == 30) { 
                                recibir_mensaje(3, 0, &cod_error);
                                posicion_conflictiva = 3;
                            }                                

                            if (esperar_posicion_siguiente == 1) { 
                                recibir_mensaje(generar_coordenada_reserva((pos.x + 7) * 100, pos.y), 0, &cod_error);
                                esperar_posicion_siguiente = 0;
                            }
                            
                            operacion_semaforo(1, -1, 0);
    
                            ubicacion_peaton = recibir_mensaje(generar_coordenada_reserva(pos.x, pos.y), 1, &cod_error);                    
                            if (cod_error != -1) {                                
                                enviar_mensaje(generar_coordenada_reserva(pos.x, pos.y), getpid(), ubicacion_peaton.num_solicitudes + 1);
                                esperar_posicion_siguiente = 1;
                                                                                                
                            }                            
                            else { 
                                enviar_mensaje(generar_coordenada_reserva(pos.x, pos.y), getpid(), 0);                               
                                ubicacion_peaton = recibir_mensaje(generar_coordenada_reserva(pos_actual.x, pos_actual.y), 1, &cod_error);                     
                                if (cod_error != -1) { 
                                    if (getpid() != ubicacion_peaton.PID) { 
                                        for (i = 0; i < ubicacion_peaton.num_solicitudes; i++) { 
                                            enviar_mensaje(generar_coordenada_reserva((pos_actual.x + 7) * 100, pos_actual.y), getpid(), 0);
                                        }
                                    }
                                }           
                                if (pos.y == 11 && pos.x >= 22 && pos.x <= 26 && entre_en_el_cruce2 == 0) { 
                                    operacion_semaforo(4, -1, 0);
                                
                                    *estado_cruce2 = *estado_cruce2 + 1; 
                                    recibir_mensaje(6, 1, &cod_error); 
                                    entre_en_el_cruce2 = 1;
                                
                                    operacion_semaforo(4, 1, 0);
                                }
                                if (pos.y == 6 && pos.x < 30 && sali_del_cruce2 == 0) {
                                    operacion_semaforo(4, -1, 0);
                                
                                    *estado_cruce2 = *estado_cruce2 - 1; 
                                    if (*estado_cruce2 == 0) {
                                        enviar_mensaje(6, 0, 0); 
                                    }
                                    sali_del_cruce2 = 1;
                                
                                    operacion_semaforo(4, 1, 0);
                                }
                                if (pos.x == 30 && pos.y >= 13 && pos.y <= 15 && entre_en_el_cruce1 == 0) { 
                                    operacion_semaforo(5, -1, 0);
                                
                                    *estado_cruce1 = *estado_cruce1 + 1; 
                                    recibir_mensaje(5, 1, &cod_error); 
                                    entre_en_el_cruce1 = 1;
                                
                                    operacion_semaforo(5, 1, 0);
                                }
                                if (pos.x == 41 && sali_del_cruce1 == 0) {
                                    operacion_semaforo(5, -1, 0);
                                
                                    *estado_cruce1 = *estado_cruce1 - 1; 
                                    if (*estado_cruce1 == 0) {
                                        enviar_mensaje(5, 0, 0); 
                                    }
                                    sali_del_cruce1 = 1;
                                
                                    operacion_semaforo(5, 1, 0);
                                }                                                                                                                                                                     
                                pos_actual = pos; 
                                pos = CRUCE_avanzar_peatOn(pos);
                            }
                            
                            if (posicion_conflictiva != 0) {
                                enviar_mensaje(posicion_conflictiva, 0, 0);
                                posicion_conflictiva = 0;
                            }
                                                                        
                            if (pos.y >= 0) {
                                operacion_semaforo(1, 1, 0);
                            }                                                      
                        } while (pos.y >= 0);

                        recibir_mensaje(generar_coordenada_reserva(pos_actual.x, pos_actual.y), 1, &cod_error);
                        
                        if (getpid() != ubicacion_peaton.PID) { 
                            for (i = 0; i < ubicacion_peaton.num_solicitudes; i++) { 
                                enviar_mensaje(generar_coordenada_reserva((pos_actual.x + 7) * 100, pos_actual.y), getpid(), 0);
                            }
                        }
                        

                        operacion_semaforo(2, -1, 0);
                        if (CRUCE_fin_peatOn() == -1) {
                            perror("CRUCE_fin_peatOn:");
                            finalizar_programa();
                        }
                        *procesos_activos = *procesos_activos - 1;
                        operacion_semaforo(2, 1, 0);
                        
                        operacion_semaforo(1, 1, 0);
                        
                        exit(0);
                    }
                    break;
                default:
                    break;
            } 
            }                                           
        }
    }   
    else {
        //FASE 1
        for (;;) {
            poner_semaforo(SEM_C1, VERDE);
            poner_semaforo(SEM_C2, ROJO);
            poner_semaforo(SEM_P1, ROJO);
            poner_semaforo(SEM_P2, VERDE);
            enviar_mensaje(1, 0, 0);           
            enviar_mensaje(4, 0, 0);
            n_pausas(6);

            recibir_mensaje(1, 0, &cod_error); 
            recibir_mensaje(4, 0, &cod_error);
            poner_semaforo(SEM_C1, AMARILLO);
            poner_semaforo(SEM_P2, ROJO);
            n_pausas(2);
            
            recibir_mensaje(6, 0, &cod_error); 
            enviar_mensaje(6, 0, 0);
            recibir_mensaje(8, 0, &cod_error); 
            enviar_mensaje(8, 0, 0);
            poner_semaforo(SEM_C1, ROJO);

            //FASE 2
            poner_semaforo(SEM_C2, VERDE);
            enviar_mensaje(2, 0, 0);          
            n_pausas(9);
            
            recibir_mensaje(2, 0, &cod_error);            
            poner_semaforo(SEM_C2, AMARILLO);
            n_pausas(2);
            
            recibir_mensaje(8, 0, &cod_error); 
            enviar_mensaje(8, 0, 0);
            poner_semaforo(SEM_C2, ROJO);
             
            //FASE 3
            poner_semaforo(SEM_P1, VERDE);
            enviar_mensaje(3, 0, 0);
            n_pausas(12);
            
            recibir_mensaje(3, 0, &cod_error);
            poner_semaforo(SEM_P1, ROJO);
            poner_semaforo(SEM_C1, AMARILLO); 
            n_pausas(2);
            
            recibir_mensaje(5, 0, &cod_error);
            enviar_mensaje(5, 0, 0);
        }
    }       
    return 0;
}
 

void manejadora() { 
    if (CRUCE_fin() == -1) {
        perror("CRUCE_fin:");
        finalizar_programa();
    }
    
    if (padre == getpid()) {
        if (semctl(semaforo, 0, IPC_RMID) == -1) {
            perror("semctl:");
            exit(3);
        } 
        if (msgctl(buzon, IPC_RMID, 0) == -1) {
            perror("msgctl:");
            exit(4);
        }
        if (shmdt(p_memoria) == -1) {
            perror("shmdt:");
            exit(6);
        }
        if (shmctl(memoria, IPC_RMID, NULL) == -1) {
            perror("shmctl:");
            exit(5);
        }
        while ((wait(NULL)) > 0) {}
    }
    
    exit(0);   
}

void n_pausas(int n) {
    int i;
    for (i = 0; i < n; i++) {
        if (pausa() == -1) {
            perror("pausa:");
            finalizar_programa();
        }
    }
}

void poner_semaforo(int tipo_semaforo, int valor) {
    if (CRUCE_pon_semAforo(tipo_semaforo, valor) == -1) {
        perror("CRUCE_pon_semAforo:");
        finalizar_programa();
    }
}

int generar_coordenada_reserva(int coord_x, int coord_y) { 
    int tipo_mensaje;
    tipo_mensaje = coord_x * 100 + coord_y + 777;
    return tipo_mensaje;
}

void enviar_mensaje(long tipo_mensaje, int PID, int num_solicitudes) {
    struct mensaje m;
    m.tipo_mensaje = tipo_mensaje;
    m.PID = PID;
    m.num_solicitudes = num_solicitudes;
    if (msgsnd(buzon, &m, sizeof(struct mensaje) - sizeof(long), 0) == -1) {
        perror("msgsnd:");
        finalizar_programa();
    }
}

struct mensaje recibir_mensaje(long tipo_mensaje, int flags, int *cod_error) {
    struct mensaje m;
    if (flags == 0) {
        *cod_error = msgrcv(buzon, &m, sizeof(struct mensaje) - sizeof(long), tipo_mensaje, 0);
        if (*cod_error == -1) {
            perror("msgrcv:");
            finalizar_programa();
        }
    }
    else {
        *cod_error = msgrcv(buzon, &m, sizeof(struct mensaje) - sizeof(long), tipo_mensaje, IPC_NOWAIT);
    }
    return m;
}

void operacion_semaforo(int num_sem, int operacion, int flag) {
    struct sembuf sops;
    sops.sem_num = num_sem;
    sops.sem_op = operacion;
    sops.sem_flg = flag;
                            
    if (semop(semaforo, &sops, 1) == -1) {
         perror("semop:");
         finalizar_programa();
    } 
}

void finalizar_programa() {
    if (kill(0, SIGINT) == -1) {
        perror("kill:");
        printf("Se ha producido un error en el programa, pulsa Control + C");
    }
}

