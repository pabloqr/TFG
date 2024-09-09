# TFG

## Descripción del proyecto

Se trata de un proyecto desarrollado en _Unreal Engine_ en el que se ofrece un entorno para la ejecución de un juego de estrategia por turnos en el que participa un jugador humano, así como una serie de agentes que implementan un algoritmo de IA que les permite tomar las decisiones más adecuadas en cada instante. En este sentido, algunas de las funciones más relevantes son:

- **Desarrollo de una partida:** el usuario puede jugar una partida, por defecto, contra 3 agentes en la que todos luchan para que la facción que controlan sea la última en pie.
- **Generación de mundos:** de forma adicional, en cada una de las partidas que juega el usuario, se creará un mapa diferente, ofreciendo rejugabilidad y variabilidad.
- **Edición de mundos:** la aplicación ofrece la posibilidad de que el usuario, además, cree mapas a su gusto modificando cada una de las casillas y recursos que estos pueden contener.
- **Gestión de archivos de guardado:** otra posibilidad interesante es la de almacenar mapas y partidas para cargarlas posteriormente.

## Sobre el repositorio

### Contenido

Los archivos que se pueden econtrar aquí incluyen todo el código desarrollado a lo largo de todo el proyecto: archivos con el código C++, archivos de _blueprints_, así como todos los _assets_ necesarios para poder ejecutar el juego.

Por tanto, la distribución de contenido es la siguiente:

- **Config:** contiene los archivos de configuración del proyecto.
- **Content:** almacena todo el contenido del proyecto exceptuando el código C++:
  - _Assets_: entre los que se incluyen los iconos, materiales, modelos y texturas.
  - _Blueprints_: divididas en función del modo de juego en que son empleadas. Aquellas que se requieren en múltiples modos, se encuentran en la carpeta **_Core_**.
  - _Levels_: para los que se incluye uno por modo de juego. Contienen los elementos necesarios para poder llevar a cabo todas las acciones del modo correspondiente.
  - _StarterContent_: donde se encuentran algunos elementos propios de _Unreal_ empleados en el proyecto.
- **Source:** contiene todos los archivos con código C++. Los nombres de estos siguen los estándares de _Unreal_ pero, para poder distinguir su finalidad más fácilmente, se indica el tipo en el nombre. Así, según su comienzo se clasifican:
  - _Actor_: se corresponden con los elementos principales de la partida y que contienen la lógica de estos.
  - _C_: son los que contienen la lógica de los controladores (para el jugador humano y los agentes).
  - _F_: se trata de estructuras que definen los atributos de todas las clases implementadas.
  - _G_: hace referencia a la palabra _Game_, sin embargo no define el contenido de los archivos.
  - _Interface_: definen las interfaces implementadas en el proyecto.
  - _Library_: se trata de archivos que contienen métodos genéricos con utilidades para otras clases.
  - _M_: estos archivos contienen la lógica de cada uno de los modos de juego. En concreto, _MMain_ es de importancia ya que contiene la lógica de turnos de la partida.
  - _Pawn_: los "peones" son los elementos que representan al jugador en el juego.
  - _S_: hace referencia a la palabra _State_ y, por tanto, define el estado de diferentes conceptos del programa.
  - _Save_: contienen la definición de los campos que se almacenan en los archivos de guardado.
- Adicionalmente, se incluyen algunos archivos que definen propiedades de _Unreal_ y que son generados por este.

### Importación de los archivos

Es posible crear un proyecto para _Unreal Engine 4.27_ descargando los archivos aquí contenidos. Bien, generando un comprimido pulsando en **_Code &rarr; Download ZIP_**, bien descargando el código fuente de la sección **_Releases_**.

A continuación, se debe extraer el contenido de cualquiera de los comprimidos en la ruta **_Documentos &rarr; Unreal Projects_** y, una vez iniciado el lanzador del editor, seleccionar la opción **_More_**. Esto mostrará los proyectos disponibles para cargar en el editor, de manera que este se encargará de generar el resto de archivos necesarios para su correcta ejecución.

![EditorLauncher_0](https://github.com/user-attachments/assets/a8f83dd0-1da9-4c1a-9825-9445731ab73a)
![EditorLauncher_1](https://github.com/user-attachments/assets/8b808d47-4c2c-475a-b05a-9e678bf69f6a)

--- 

## Manual de usuario

### Descarga de la aplicación

Para poder ejecutar fácilmente el programa, la mejor opción es descargar el ejecutable de la sección **_Releases_**. En concreto, se debe descargar el archivo comprimido **_TFG_Windows_** y, después, descomprimirlo en la ubicación que se prefiera.

Llegados a este punto, basta con ejecutar el archivo **_TFG.exe_**. Sin embargo, cabe destacar que sólo se ha generado el empaquetado para el sistema operativo _Windows_.

### Modo de uso

#### Modo Menú

Al lanzar el programa, se muestra el menú principal desde el que es posible:

- Crear una nueva partida.
- Cargar el editor de mundos.
- Salir del juego.

![menu_mode](https://github.com/user-attachments/assets/f15b8e59-23cf-4ee0-9d3e-371faed4623d)

--- 

#### Modo Editor

Desde este modo, el usuario puede modificar el mapa que se genera de cualquiera de las siguientes formas:

1. Cambiando los atributos del mapa y regenerándolo.
2. Modificando manualmente las casillas generadas.
3. Cambiando el recurso situado sobre las casillas.

![editor_mode_tile](https://github.com/user-attachments/assets/88307334-2609-4d29-825b-affe346e7788)
![editor_mode_resource](https://github.com/user-attachments/assets/4e29296f-21cc-477f-afb1-66efecc90f68)

Además, a través del menú de pausa, puede guardar el mapa actual y cargar aquellos guardados previamente.

![editor_mode_pausemenu_mapsaves](https://github.com/user-attachments/assets/1f2061a8-4cac-4b9a-8b04-ba8b6cba0c9a)

--- 

#### Modo Principal

Finalmente, en este modo en el que se desarrolla la partida, el jugador, en su turno, puede realizar diferentes acciones para lograr la victoria.
