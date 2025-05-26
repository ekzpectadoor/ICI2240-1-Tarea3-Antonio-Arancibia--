Sistema de Juego GraphQuest
Descripción
GraphQuest es un juego de exploración de laberintos desarrollado en C, donde los jugadores navegan a través de diferentes fases, recolectan ítems, gestionan su inventario y el tiempo para alcanzar una fase final. El juego incorpora elementos de estrategia y gestión de recursos, ya que el peso de los ítems recolectados afecta el tiempo consumido al moverse.

Cómo compilar y ejecutar
Este sistema ha sido desarrollado en lenguaje C. Para comenzar a trabajar con el sistema en tu equipo local, sigue estos pasos:

Requisitos previos:
Tener instalado un compilador de C (como gcc). Si estás en Windows, se recomienda instalar MinGW o utilizar el entorno WSL.

Pasos para compilar y ejecutar:

Descarga los archivos: Asegúrate de tener main.c, extra.c, extra.h, list.c, y list.h en el mismo directorio.

Abre la terminal: Navega hasta el directorio donde guardaste los archivos.

Compila el código: En la terminal, compila el programa con el siguiente comando:

gcc main.c extra.c list.c -o graphquest

Si no funciona, especifica la dirección del archivo:

gcc main.c tdas/extra.c tdas/list.c -o graphquest

Ejecuta el programa: Una vez compilado, puedes ejecutar la aplicación con:

./graphquest.exe

Funcionalidades
Funcionando correctamente:

Cargar laberinto desde archivo: El programa puede cargar un laberinto desde un archivo CSV, parseando las fases, sus descripciones, ítems y conexiones. 
Movimiento entre fases: El jugador puede moverse entre fases (Arriba, Abajo, Izquierda, Derecha) si las conexiones existen. 
Recoger ítems: Los ítems disponibles en cada fase pueden ser recogidos y añadidos al inventario del jugador.  Al recoger un ítem, el puntaje del jugador aumenta según el valor del ítem. 
Descartar ítems: Los ítems del inventario pueden ser descartados, lo que reduce el puntaje del jugador y libera peso. 
Cálculo de peso: El juego calcula el peso total de los ítems en el inventario del jugador. 
Gestión del tiempo: El movimiento entre fases consume tiempo, que se calcula en función del peso del inventario. 
Estado del juego: Se muestra en pantalla el tiempo restante, el puntaje actual, el peso del inventario y los ítems en la fase actual. 
Final del juego: El juego termina si el tiempo se agota o si el jugador llega a una fase final. 
Reiniciar partida: Es posible reiniciar el juego, volviendo al estado inicial del laberinto, con el puntaje y tiempo restablecidos. 

Problemas conocidos:

Limpieza de pantalla: Al intentar limpiar la pantalla, puede que no se limpie del todo.
Acentos: Al añadir acentos a las letras estas se muestran de forma extraña, asi que el programa muestra texto sin acentos.
Validación de entrada en selección de ítems: Al seleccionar ítems para recoger o descartar, la validación de la entrada del usuario no es la mejor.  Si el usuario ingresa texto no numérico o números fuera del rango de opciones, el programa se puede comportar de formas no deseadas.
Compatibilidad: No se manejan casos de IDs corruptos o de archivos con más de 16 IDs :c


Ejemplo de uso

Paso 1: Cargar laberinto desde archivo
Al iniciar el programa, la primera opción es cargar el laberinto.

========================================
          LABERINTO GRAPHQUEST          
========================================
1) Cargar laberinto desde archivo
2) Iniciar partida
3) Salir
Ingrese alguna opcion: 1
Se te pedirá la dirección del archivo del laberinto (ej: graphquest.csv). 

Paso 2: Iniciar partida
Después de cargar el laberinto, puedes iniciar la partida.

========================================
          LABERINTO GRAPHQUEST          
========================================
1) Cargar laberinto desde archivo
2) Iniciar partida
3) Salir
Ingrese alguna opcion: 2


Paso 3: Interacción durante el juego
Una vez iniciada la partida, se mostrará la información de la fase actual y las opciones disponibles. Por ejemplo, al inicio estarás en la "Entrada principal".

=== Entrada principal ===
Descripcion: Una puerta rechinante abre paso a esta mansion olvidada por los dioses y los conserjes. El aire huele a humedad y a misterios sin resolver. [cite: 1]
Tiempo restante: 15 
Puntaje hasta ahora: 0 
Peso en el inventario: 0 kg 
Items en este entorno:
 (No hay items) 

Opciones:
1) Recoger un item
2) Descartar un item
3) Moverse
4) Reiniciar
5) Salir
Ingresa alguna opcion: 3


Paso 4: Moverse a otra fase
Si seleccionas la opción 3 (Moverse), el juego te mostrará las direcciones disponibles. Desde la "Entrada principal", solo puedes moverte "Abajo" a la "Cocina".

Direcciones disponibles:
1) Abajo 
Elije la direccion a seguir: 1
Movimiento valido. Tiempo usado: 1 
Ahora te encuentras en la "Cocina".

=== Cocina ===
Descripcion: Restos de una batalla culinaria. Cuchillos por aqui, migajas por alla. Alguien cocino… o fue cocinado. 
Tiempo restante: 14 
Puntaje hasta ahora: 0 
Peso en el inventario: 0 kg 
Items en este entorno:
  - Cuchillo (3 pts, 1 kg) 
  - Pan (2 pts, 1 kg)

Opciones:
1) Recoger un item
2) Descartar un item
3) Moverse
4) Reiniciar
5) Salir
Ingresa alguna opcion: 1


Paso 5: Recoger un ítem
Si seleccionas la opción 1 (Recoger un item), podrás elegir los ítems que deseas añadir a tu inventario. Por ejemplo, podrías recoger el "Cuchillo" y el "Pan".

items disponibles:
1) Cuchillo (3 pts, 1 kg) 
2) Pan (2 pts, 1 kg)
Selecciona algun item: 1,2
Items recogidos. 
Ahora, tu puntaje y el peso en el inventario se han actualizado. 

El juego continuará hasta que el tiempo se agote o el jugador llegue a la "Salida"

Programa donde 1 humano colaboró :p