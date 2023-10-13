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
