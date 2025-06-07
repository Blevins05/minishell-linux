# Trabajo Final de Prácticas: Sistemas Operativos 24-25: Minishell en C++
## AUTOR: Hugo Moncada Ruiz
## Descripción del Proyecto

La **minishell** es un intérprete de comandos desarrollado en C++, simulando una shell
para Linux, como proyecto final practico SOP. Con este programa en C++ 
se pueden ejecutar comandos basicos, con funcionalidades como redireccion E/S, ejecución mediante pipes, y guiones de mandatos. No incluye comandos internos, 
salvo cd para poder permitir a los usuarios cambiar de directorio de trabajo.
---

### Entrada y Salida
- **Entrada**: Puede ser interactiva o modo guion de comandos.
- **Salida**: El resultado de los comandos se muestra en la salida estándar.
- **Errores**: Los errores se notifican mediante la salida de error estándar.

### Prompt
En modo interactivo, la minishell muestra un **prompt** para indicar que está lista para recibir comandos. El prompt tiene el formato:

```shell
:~directorio_actual$
```

### Comandos
- Los comandos se ingresan como una lista de tokens separados por espacios o tabuladores.
- El primer token indica el nombre del comando, mientras que los siguientes tokens son los argumentos.
- Los comandos se ejecutan hasta que el usuario introduzca `salir`, en ese caso se termina la ejecucion del programa
- Los errores en los comandos no interrumpen la ejecución de la minishell.

### Ejecución de Guiones
La minishell puede ejecutar un fichero de comandos si se proporciona su nombre como argumento al iniciar el programa:

```bash
./minishell comandos.txt
```

### Redirección de Entrada y Salida
La minishell soporta redirección mediante los símbolos `>` y `<`:
- `comando < archivo`: Usa el archivo como entrada estándar.
- `comando > archivo`: Usa el archivo como salida estándar.

En caso de error durante la redirección, se notifica en la salida de error estándar y se suspende el comando.

### Tuberías
La minishell admite comandos encadenados mediante `|`. La salida del primer comando se conecta a la entrada del segundo comando:
Solo 1 tubería "|" máximo, no soporta mas.
```bash
comando1 | comando2
```

Se puede combinar redireccion con tuberias.

### Comando Interno `cd`
El comando `cd` permite cambiar el directorio de trabajo:

```bash
cd <path>
```
Si el directorio no es válido, se muestra un error en la salida de error estándar.

---

## Manual de Uso

### Construcción del Programa
1. Tener un compilador de C++ instalado (como `g++`, por ejemplo)
2. Compila el programa ejecutando:

```bash
g++ minishell.cpp -o minishell
```
Esto generará un ejecutable llamado `minishell`.

### Ejecución
#### Modo Interactivo
1. Inicia el programa:

```bash
./minishell
```

2. Introduce comandos según las especificaciones anteriores.
3. Para salir, escribe `salir`

#### Modo de Fichero de Comandos
1. Prepara un fichero con los comandos.
2. Inicia el programa pasando el fichero como argumento:

### Ejemplos de Uso

#### Comando Simple
```bash
ls -l
```

#### Redirección
```bash
ls -l > salida.txt
cat < entrada.txt
```

#### Tuberías
```bash
ls -l | grep "main"
```

#### Comando Interno `cd`
```bash
cd /home/usuario
```

#### Salir
```bash
salir
```
---

