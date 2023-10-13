# Simulación del tráfico de un cruce mediante programación concurrente

# - Introducción

Proyecto realizado en la asignatura de SSOO II del grado de Ingenieria Informática de la Universidad de Salamanca. El enunciado del proyecto se encuentra en el siguiente enlace:  

http://avellano.usal.es/~labssoo/pract209.htm
  
El principal objetivo de este proyecto es la realización de un programa en C que simule el tráfico de coches y peatones en un cruce donde cada peatón y cada coche será simulado por un proceso.
De esta manera, el objetivo es establecer una correcta sincronización y comunicación entre los distintos procesos para que se cumplan las normas de tráfico en el cruce.
Para ello, se utilizarán distintos mecanismos IPC de comunicación entre procesos como los semáforos, los buzones o la memoria compartida.

# - Comentarios sobre el entorno de ejecución

Para ejecutar este programa, se requerira de una distribución del Sistema Operativo **GNU/Linux**.    

Para poder compilar correctamente el programa, se deberá tener instalada una version del compilador gcc o similar. En el caso de no tener gcc, se puede instalar facilmente con el siguiente comando:

```sudo apt install gcc```

# - Parámetros de ejecución

Para ejecutar el programa es necesario proporcionar 2 argumentos. 

El primer argumento se trata del **número máximo de procesos** (peatones y coches) que pueden existir de manera simultanea en el cruce. Este primer argumento puede tomar valores del ***1 al 40***.

El segundo argumento hace referencia a la **velocidad** con la que se desplazaran los peatones y los coches. Este argumento podrá tomar valores del ***0 al 40*** de forma que la velocidad del programa será mas rápida cuanto mas pequeño sea el número introducido.  
Si el segundo argumento es 0, el programa se ejecutará a ***máxima velocidad***, lo que implicará un alto consumo de CPU.

Si los parámetros introducidos no respetan las reglas anteriores, el programa lo detectará, informará al usuario y acabará.

# Comentarios sobre el mateial adjuntado

El proyecto cuenta con los siguientes ficheros:

- Un fichero llamado ***cruce.c*** que contendrá el código necesario para poner en marcha la infraestructura del cruce, leer y interpretar los argumentos introducidos y crear los peatones y coches correspondientes.
- Un fichero llamado ***libcruce.a*** el cual se trata de una biblioteca estática de funciones que contendrá una serie de funciones para gestionar la creación y finalización del cruce así como la gestión del valor de los semáforos creación y movimiento de peatones y coches.
- Un fichero llamado ***cruce.h*** que contiene las cabeceras de las funciones definidas en la biblioteca *libcruce.a* así como una serie de macros que utiliza la biblioteca.

# - Funciones de la biblioteca estática de funciones ***libcruce.a***

La biblioteca estática de funciones ***libcruce.a*** cuenta con las siguientes funciones:

- **int CRUCE_inicio(int ret, int maxProcs, int semAforos, char * zona)**: Esta función deberá ser ejecutada por el proceso principal despues de crear los mecanismos IPC correspodientes y antes de haber creado algún peatón o coche. Esta función recibe los siguientes parámetros:
  
  - **ret**: Número entero que hace referencia a la velocidad de representación de los peatones y coches en el cruce (segundo argumento pasado en la línea de órdenes).
  - **maxProcs**: Número entero que hace referencia al número máximo de procesos (peatones y coches) que pueden existir de manera simultanea en el cruce (primer argumento pasado en la línea de órdenes).
  - **semAforos**: Número entero que hace referencia al identificador de un conjunto o array de semáforos que se deberá haber creado previamente donde el primer semáforo será utilizado por la biblioteca y el resto pueden ser utilizados para realizar tareas de sincronización.
  - **zona**: Puntero a una zona de memoria compartida que se deberá haber creado previamente donde los primeros 256 bytes serán utilizados por la biblioteca mientras el resto pueden ser utilizados para realizar tareas de sincronización.
 
- **int CRUCE_pon_semAforo(int sem, int color)**: Esta función pone el semáforo pasado en el primer parámetro al color indicado en el segundo parámetro. Esta función recibe los siguientes parámetros:

  - **sem**: Número entero que hace referencia a un semáforo del cruce. Este parámetro puede tomar los valores *SEM_P1*, *SEM_P2*, *SEM_C1*, *SEM_C2* dependiendo del semáforo al que se desea cambiar el color.
  - **color**: Número entero que hace referencia al color al que se desea cambiar el semáforo pasado como primer parámetro. Este parámetro puede tomar los valores *ROJO*, *AMARILLO* (solamente para los semaforos de los coches) y *VERDE*.
 
Nota: *SEM_P1*, *SEM_P2*, *SEM_C1*, *SEM_C2*, *ROJO*, *AMARILLO* y *VERDE* son macros definidas en ***cruce.h***.

- **int CRUCE_nuevo_proceso(void)**: Esta función crea un nuevo proceso que puede ser un coche o un peatón. Esta función devuelve *COCHE* en el caso de que el proceso creado se trate de un coche y *PEAToN* en el caso de que el proceso creado se trate de un peatón.

Nota: *COCHE* y *PEAToN* son macros definidas en ***cruce.h***.

- **struct posiciOn CRUCE_inicio_coche(void)**: Cada vez que se crea un nuevo proceso de tipo *COCHE*, debe llamar a esta función. Esta función devuelve las coordenadas de la posición siguiente que ocupará el coche a través de un tipo de datos llamado *posiciOn*.

Nota: *posiciOn* es un tipo de datos definido en ***cruce.h*** y cuenta con dos enteros que hacen referencia a la coordenada x y a la coordenada y.

- **struct posiciOn CRUCE_inicio_peatOn_ext(struct posiciOn *posNacimiento)**: Cada vez que se crea un nuevo proceso de tipo *PEAToN*, debe llamar a esta función. Esta función devuelve las coordenadas de la posición siguiente que ocupará el peatón a través de un tipo de datos llamado *posiciOn*. Esta función cuenta con un único parámetro en el cual se devolverá por referencia la posición actual que ocupa el peatón a través de un tipo de datos llamado *posiciOn*.

  
# - Pasos necesarios para ejecutar el programa

**Paso 1: Compilar el programa**  

Para ello se debe introducir el siguiente comando:    

```gcc lallevas.c -o lallevas```

Tras ejecutar este comando, se generará un fichero ejecutable llamado *lallevas*

**Paso 2: Ejecutar el programa**  

Para ello se debe introducir el siguente comando:    

```./lallevas 6```

Tras ejecutar este comando, el programa se habra ejecutado correctamente, siempre y cuendo se hayan introducido los argumentos correspondientes.

# - Ejemplo de ejecución
