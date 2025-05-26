#include <stdio.h>
#include <stdlib.h>
#include "tdas/list.h"
#include "tdas/extra.h"
#include <string.h>

// Cada ítem tiene su nombre, cuánto suma al puntaje y el peso que carga
typedef struct Item {
    char* nombre;
    int valor;
    int peso;
} Item;

// Un escenario del laberinto, habitaciones y cosas para agarrar
typedef struct Fase {
    int id; // Número único (como en el CSV)
    char* nombre; // Ej: "Cocina", "Sótano tenebroso"
    char* descripcion; // Texto que describe el lugar
    List* items;       // Lista de ítems que hay aquí (punteros a Item)
    struct Fase* arriba; // Punteros a otros escenarios conectados
    struct Fase* abajo;
    struct Fase* izquierda;
    struct Fase* derecha;
    int esFinal; // 1 si aquí está la salida del laberinto
} Fase;

// Todo lo que lleva el jugador durante la partida
typedef struct {
    int puntaje; // Puntos totales 
    List* inventario;  // Lista de items que carga el jugador 
    int tiempo;  // Tiempo restante 
    Fase* current; // Dónde está el jugador ahora mismo
} Juego;

// Carga el laberinto desde un archivo CSV, si tiene más de 16 items, fallaría :p
Fase* cargarLaberinto() {
    char direccion[500];
    printf("ingrese la direccion del laberinto:\n");
    scanf("%s", direccion);
    FILE* archivo = fopen(direccion, "r");
    if (archivo == NULL) 
    {
        perror("Error al abrir el archivo");
        return NULL;
    }
    // Leer y omitir la cabecera
    leer_linea_csv(archivo, ',');
    Fase** fases = calloc(17, sizeof(Fase*)); // Array temporal para 16 fases (ID 1-16)
    char** campos;
    // Lee cada línea del CSV y crea las fases
    while ((campos = leer_linea_csv(archivo, ',')) != NULL) {
        Fase* fase = malloc(sizeof(Fase));
        fase->id = atoi(campos[0]);
        fase->nombre = strdup(campos[1]);
        fase->descripcion = strdup(campos[2]);
        fase->esFinal = (strcmp(campos[8], "Si") == 0);  // Es la salida?

        // Procesa los ítems del escenario (si los tiene)
        fase->items = list_create();
        if (strlen(campos[3]) > 0) {
            List* items_str = split_string(campos[3], ";"); // Separa "item1;item2;..."
            for (char* item_str = list_first(items_str); item_str != NULL; item_str = list_next(items_str)) 
            {
                List* partes = split_string(item_str, ","); // Divide "nombre,valor,peso"
                Item* item = malloc(sizeof(Item));
                item->nombre = strdup(list_first(partes));
                item->valor = atoi(list_next(partes));
                item->peso = atoi(list_next(partes));
                list_pushBack(fase->items, item);
                list_clean(partes); // Limpia la lista de partes
                free(partes);
            }
            list_clean(items_str);
            free(items_str); // Libera la lista de strings de ítems
        }
        // Conexiones a otros escenarios (usando IDs como punteros temporales)  
        int arriba_id = atoi(campos[4]);
        int abajo_id = atoi(campos[5]);
        int izquierda_id = atoi(campos[6]);
        int derecha_id = atoi(campos[7]);
        // Asignar IDs temporales
        //Magia negra con punteros 👻 
        fase->arriba = (arriba_id != -1) ? (Fase*)(size_t)arriba_id : NULL; // Guarda este número como si fuera una dirección de memoria, pero en realidad es un ID disfrazado
        fase->abajo = (abajo_id != -1) ? (Fase*)(size_t)abajo_id : NULL; // Lo mismo con lo demás
        fase->izquierda = (izquierda_id != -1) ? (Fase*)(size_t)izquierda_id : NULL;
        fase->derecha = (derecha_id != -1) ? (Fase*)(size_t)derecha_id : NULL; 
        fases[fase->id] = fase; // Guarda la fase en el array según su ID
    }

    // Reemplaza los IDs con punteros reales a las fases
    for (int i = 1; i <= 16; i++) 
    {
        Fase* fase = fases[i];
        if (!fase) continue; // Convierte el ID guardado (como puntero) a un puntero real
        // Magia negra con punteros 👻 
        fase->arriba = (fase->arriba) ? fases[(size_t)fase->arriba] : NULL; //Aquí se reemplaza el "puntero falso" (que era una ID) por el puntero real de la fase
        fase->abajo = (fase->abajo) ? fases[(size_t)fase->abajo] : NULL; // Lo mismo con lo demás   
        fase->izquierda = (fase->izquierda) ? fases[(size_t)fase->izquierda] : NULL;
        fase->derecha = (fase->derecha) ? fases[(size_t)fase->derecha] : NULL;
    }
    fclose(archivo);
    Fase* inicio = fases[1]; // El inicio siempre va a ser 1
    free(fases); //Se libera el array
    return inicio;
}
//Calcula cuánto pesa el inventario del jugador
int calcular_peso(Juego* juego) 
{
    int peso = 0;
    // Recorre cada ítem en el inventario y suma su peso
    for (Item* item = list_first(juego->inventario); item != NULL; item = list_next(juego->inventario)) 
    {
        peso += item->peso;
    }
    return peso;
}

// Menú para agarrar ítems del escenario actual
void recogerItems(Juego* juego) {
    List* items = juego->current->items;
    if (list_size(items) == 0) 
    {
        printf("No hay items aquí.\n");
        presioneTeclaParaContinuar();
        return;
    }

    printf("items disponibles:\n");
    int idx = 1;
    //Crear una lista temporal para mantener los ítems originales
    List* temp_items = list_create();
    for (Item* item = list_first(items); item != NULL; item = list_next(items)) 
    {
        printf("%d) %s (%d pts, %d kg)\n", idx++, item->nombre, item->valor, item->peso);
        list_pushBack(temp_items, item); //Copiar referencias
    }
    printf("Selecciona algun item: ");
    char input[256];
    fgets(input, 256, stdin);
    List* seleccion = split_string(input, ",");
    List* nuevos_items = list_create();
    List* items_a_recoger = list_create();
    idx = 1; //Reiniciar índice para la selección
    for (Item* item = list_first(temp_items); item != NULL; item = list_next(temp_items)) 
    {
        int seleccionado = 0;
        for (char* sel = list_first(seleccion); sel != NULL; sel = list_next(seleccion)) 
        {
            int num = atoi(sel);
            if (num == idx) {
                list_pushBack(items_a_recoger, item);
                seleccionado = 1;
                break;
            }
        }
        if (!seleccionado) 
        {
            list_pushBack(nuevos_items, item);
        }
        idx++;
    }

    // Actualizar items del escenario sin liberar memoria
    list_clean(juego->current->items);
    for (Item* item = list_first(nuevos_items); item != NULL; item = list_next(nuevos_items)) 
    {
        list_pushBack(juego->current->items, item);
    }

    //Añadir al inventario
    for (Item* item = list_first(items_a_recoger); item != NULL; item = list_next(items_a_recoger)) 
    {
        list_pushBack(juego->inventario, item);
        juego->puntaje += item->valor;
    }
    // Liberar listas auxiliares (no los ítems)
    list_clean(temp_items);
    free(temp_items);
    list_clean(seleccion);
    free(seleccion);
    list_clean(nuevos_items);
    free(nuevos_items);
    list_clean(items_a_recoger);
    free(items_a_recoger);
    printf("Ítems recogidos.\n");
    presioneTeclaParaContinuar();
} 
// Para descartar un item del inventario
void descartarItems(Juego* juego) {
    if (list_size(juego->inventario) == 0) 
    {
        printf("Inventario vacio.\n");
        presioneTeclaParaContinuar();
        return;
    }

    printf("Inventario:\n");
    int idx = 1;
    for (Item* item = list_first(juego->inventario); item != NULL; item = list_next(juego->inventario)) 
    {
        printf("%d) %s (%d pts, %d kg)\n", idx++, item->nombre, item->valor, item->peso);
    }
    printf("Selecciona el item a descartar: ");
    char input[256];
    fgets(input, 256, stdin);
    List* seleccion = split_string(input, ",");
    List* nuevo_inventario = list_create();
    idx = 1;
    for (Item* item = list_first(juego->inventario); item != NULL; item = list_next(juego->inventario)) 
    {
        int descartar = 0;
        for (char* sel = list_first(seleccion); sel != NULL; sel = list_next(seleccion)) 
        {
            if (atoi(sel) == idx) 
            {
                descartar = 1;
                juego->puntaje -= item->valor;
                free(item); // Libera memoria del ítem
                break;
            }
        }
        if (!descartar) list_pushBack(nuevo_inventario, item);
        idx++;
    }
    list_clean(juego->inventario);
    free(juego->inventario);
    juego->inventario = nuevo_inventario;
    list_clean(seleccion);
    free(seleccion);
    printf("El item ha sido descartado \n");
    presioneTeclaParaContinuar();
}
//Moverse a otro escenario 
void avanzar(Juego* juego) {
    printf("Direcciones disponibles:\n");
    int opciones = 0;
    if (juego->current->arriba) 
    {
        printf("%d) Arriba\n", ++opciones); 
    }
    if (juego->current->abajo)
    { 
        printf("%d) Abajo\n", ++opciones); 
    }
    if (juego->current->izquierda)
    { 
        printf("%d) Izquierda\n", ++opciones); 
    }
    if (juego->current->derecha)
    { 
        printf("%d) Derecha\n", ++opciones); 
    }
    if (opciones == 0) 
    {
        printf("No hay salidas :c\n");
        presioneTeclaParaContinuar();
        return;
    }

    printf("Elije la direccion a seguir: ");
    int dir;
    scanf("%d", &dir);
    getchar(); // Limpiar buffer
    Fase* destino = NULL;
    opciones = 0;
    if (juego->current->arriba && ++opciones == dir)
    {
        destino = juego->current->arriba;
    }
    if (juego->current->abajo && ++opciones == dir)
    {
        destino = juego->current->abajo;
    }
    if (juego->current->izquierda && ++opciones == dir)
    {
        destino = juego->current->izquierda;
    }
    if (juego->current->derecha && ++opciones == dir)
    {
        destino = juego->current->derecha;
    }
    if (!destino) 
    {
        printf("Direccion invalida.\n");
        presioneTeclaParaContinuar();
        return;
    }
    int peso = calcular_peso(juego);
    int tiempo_uso = (peso + 1 + 9) / 10; 
    juego->tiempo -= tiempo_uso;
    juego->current = destino;
    if (juego->tiempo < 0) juego->tiempo = 0;
    printf("Movimiento valido. Tiempo usado: %d\n", tiempo_uso);
    presioneTeclaParaContinuar();

}


// Bucle principal del juego
void procesarJuego(Fase* laberinto) {
    Juego juego = {0, list_create(), 15, laberinto}; //Se empieza con 15 :p
    while (juego.tiempo > 0 && !juego.current->esFinal) {
        limpiarPantalla();
        //Muestra info del escenario actual
        // Lo demás es casi autoexplicatorio
        printf("=== %s ===\n", juego.current->nombre);
        printf("Descripcion: %s\n", juego.current->descripcion);
        printf("Tiempo restante: %d\n", juego.tiempo);
        printf("Puntaje hasta ahora: %d\n", juego.puntaje);
        printf("Peso en el inventario: %d kg\n", calcular_peso(&juego));
        printf("Items en este entorno:\n");
        if (list_size(juego.current->items) == 0) printf(" (No hay items)\n");
        else 
        {
            for (Item* item = list_first(juego.current->items); item != NULL; item = list_next(juego.current->items)) 
            {
                printf("  - %s (%d pts, %d kg)\n", item->nombre, item->valor, item->peso);
            }
        }
        printf("\nOpciones:\n");
        printf("1) Recoger un item\n");
        printf("2) Descartar un item\n");
        printf("3) Moverse\n");
        printf("4) Reiniciar\n");
        printf("5) Salir\n");
        printf("Ingresa alguna opcion: ");
        int opcion;
        scanf("%d", &opcion);
        getchar(); // Limpiar buffer
        switch(opcion) {
            case 1: 
                recogerItems(&juego); juego.tiempo--; 
                break;
            case 2: 
                descartarItems(&juego); juego.tiempo--; 
                break;
            case 3: 
                avanzar(&juego); 
                break;
            case 4: 
                list_clean(juego.inventario);
                juego.inventario = list_create();
                juego.puntaje = 0;
                juego.tiempo = 100;
                juego.current = laberinto;
                break;
            case 5: 
                juego.tiempo = 0; 
                break;
            default: printf("Opcion invalida.\n");
        }
    }
    limpiarPantalla();
    if (juego.current->esFinal) {
        printf("Has completado GraphQuest! C:\n");
    } else {
        printf("Tiempo agotado!\n");
    }
    printf("Puntaje final: %d\n", juego.puntaje);
    presioneTeclaParaContinuar();
    list_clean(juego.inventario);
    free(juego.inventario);
}

// Menú principal del programa
int main() {
    Fase* laberinto = NULL;
    int opcion;
    do {
        limpiarPantalla();
        printf("========================================\n");
        printf("          LABERINTO GRAPHQUEST          \n");
        printf("========================================\n");
        printf("1) Cargar laberinto desde archivo\n");
        printf("2) Iniciar partida\n");
        printf("3) Salir\n");
        printf("Ingrese alguna opcion: ");
        scanf("%d", &opcion);
        getchar(); //Limpieza de buffer

        switch(opcion) {
            case 1:
                if (laberinto)
                {
                    free(laberinto); //Por si acaso, se libera la memoria del laberinto anterior(Aunque debiese estar liberada)
                }
                laberinto = cargarLaberinto();
                printf("Laberinto cargado.\n");
                presioneTeclaParaContinuar();
                break;
            case 2:
                if (!laberinto) {
                    printf("Hay que cargar un laberinto valido primero\n");
                    presioneTeclaParaContinuar();
                } else {
                    procesarJuego(laberinto); // Inicia la partida
                }
                break;
            case 3:
                printf("Saliendo del programa...\n");
                break;
            default:
                printf("Opción inválida.\n");
                presioneTeclaParaContinuar();
        }
    } while (opcion != 3);
    free(laberinto); // Se libera la memoria
    return 0;
}