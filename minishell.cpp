#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <sched.h>
#include <sys/wait.h>
#include <unistd.h> // Librerias necesarias para poder utilizar el proyecto

using namespace std;

// Funcion para leer el prompt
void read(char str[], int tam)
{
  if (cin.fail())
  {
    cin.clear();
    cin.ignore(256, '\n');
  }
  cin.getline(str, tam + 1);
}

// Funcion que trocea la linea segun el delimitador delim. parts es una cadena donde se almacena el resultado de la ejecución
int trocear_linea(char *str, const char *delim, char **&parts)
{
  parts = (char **)malloc(sizeof(char *));
  parts[0] = NULL;

  if (str == NULL)
    return 0;

  int numParts = 0;
  int i = 0;
  while (str[i] != '\0')
  {
    int j = i;
    while (str[j] != '\0' && strchr(delim, str[j]) == NULL)
    {
      j++;
    }

    if (str[j] != '\0')
    {
      str[j] = '\0';
      j++;
    }

    if (str[i] != '\0')
    {
      numParts++;
      parts = (char **)realloc(parts, (numParts + 1) * sizeof(char *));
      parts[numParts - 1] = &str[i];
      parts[numParts] = NULL;
    }

    i = j;
  }

  return numParts;
}

// Funcion encargada de eliminar los espacios en blanco en un prompt.
char *strtrim(char *str)
{
  if (str == NULL)
    return NULL;

  int i = 0;
  while (str[i] == ' ' || str[i] == '\t')
  {
    i++;
  }

  int f = strlen(str) - 1;
  while (f > i && (str[f] == ' ' || str[f] == '\t'))
  {
    f--;
  }

  str[f + 1] = '\0';

  return &str[i];
}

// Función que maneja la ejecución de comandos con pipes.
void getredir(char *str, char *&in, char *&out)
{
  in = NULL;
  out = NULL;

  int ch = 0;
  while (str[ch] != '\0')
  {
    if (str[ch] == '<')
    {
      str[ch] = '\0';
      in = &str[ch + 1];
    }
    if (str[ch] == '>')
    {
      str[ch] = '\0';
      out = &str[ch + 1];
    }
    ch++;
  }

  in = strtrim(in);
  out = strtrim(out);

  if (in && *in == '\0')
  {
    in = nullptr;
  }
  if (out && *out == '\0')
  {
    out = nullptr;
  }
}

// Función que maneja la ejecución de comandos con pipes.
void ejectuar_pipe(char **trozos_pipe)
{
  int tuberia[2];

  if (pipe(tuberia))
  {
    perror("Error al crear la tuberia");
    exit(1);
  }
  char *in1, *out1;
  char *in2, *out2;

  getredir(trozos_pipe[0], in1, out1);
  getredir(trozos_pipe[1], in2, out2);

  char **trozos1, **trozos2;
  trocear_linea(trozos_pipe[0], " \t", trozos1);
  trocear_linea(trozos_pipe[1], " \t", trozos2);

  pid_t pid1 = fork();
  switch (pid1)
  {
  case -1:
    perror("ERROR: no he podido crear un proceso con fork");
    exit(1);

  case 0:
    close(tuberia[0]);
    if (dup2(tuberia[1], 1) == -1)
    {
      perror("No se puede redireccionar stdout");
      exit(2);
    }
    close(tuberia[1]);

    if (in1)
    {
      close(0);
      int fd_in = open(in1, O_RDONLY);

      if (fd_in == -1)
      {
        perror("Error. No es posible redireccionar la salida estandar.");
        exit(-1);
      }
    }

    execvp(trozos1[0], trozos1);
    perror("Error al ejecutar execvp()");
    free(trozos1);
    free(trozos2);
    exit(-1);
  }

  pid_t pid2 = fork();
  switch (pid2)
  {
  case -1:
    perror("ERROR: no he podido crear un proceso con fork");
    exit(4);

  case 0:
    close(tuberia[1]);
    if (dup2(tuberia[0], 0) == -1)
    {
      perror("No se puede redireccionar stdin");
      exit(5);
    }
    close(tuberia[0]);

    if (out2)
    {
      close(1);
      int fd_out = open(out2, O_WRONLY | O_CREAT | O_TRUNC, 0644);
      if (fd_out == -1)
      {
        perror("Error al abrir el archivo de entrada");
        exit(-1);
      }
    }
    execvp(trozos2[0], trozos2);
    perror("Error al ejecutar execvp()");
    free(trozos1);
    free(trozos2);
    exit(-1);
  }

  close(tuberia[0]);
  close(tuberia[1]);
  wait(NULL);
  wait(NULL);
}

// Funcion para ejecutar un solo comando (sin pipes)
static void ejecutar_comando(char comando[])
{
  char **trozos_cmd;
  char *in, *out;
  getredir(comando, in, out);
  int num = trocear_linea(comando, " \t", trozos_cmd);
  if (strcmp(trozos_cmd[0], "cd") == 0)
  {
    if (num != 2)
    {
      cerr << "El numero de argumentos introducidos es incorrecto" << endl;
    }
    else
    {
      if (chdir(trozos_cmd[1]) == -1)
      {
        perror("CD: Error al cambiar de directorio");
      }
      free(trozos_cmd);
    }
  }
  else
  {
    pid_t pid = fork();
    switch (pid)
    {
    case -1:
      perror("Error al crear un proceso\n");
      break;
    case 0:

      if (out != NULL)
      {
        close(1);
        int fd_out = open(out, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd_out == -1)
        {
          perror("Error al abrir el archivo de salida");
          exit(-1);
        }
      }

      if (in != NULL)
      {
        close(0);
        int fd_in = open(in, O_RDONLY);
        if (fd_in == -1)
        {
          perror("Error al abrir el archivo de entrada");
          exit(-1);
        }
      }

      execvp(trozos_cmd[0], trozos_cmd);
      perror("Error al ejecutar la funcion execvp");
      free(trozos_cmd);
      exit(-1);
    default:
      wait(NULL);
      free(trozos_cmd);
    }
  }
}

// Funcion genérica para ejecución de comandos. Si hay mas de 2 partes entonces hay pipe, si no es un comando simple
void ejecutar(char comando[])
{
  char **trozos_cmd;
  int np = trocear_linea(comando, "|", trozos_cmd);

  if (np == 1)
  {
    ejecutar_comando(trozos_cmd[0]);
  }
  else if (np == 2)
  {
    ejectuar_pipe(trozos_cmd);
  }
}

const int MAX_CARACTERES = 255;

int main(int argc, char *argv[])
{
  char comando[MAX_CARACTERES + 1];
  char **partes;
  char current_path[MAX_CARACTERES];

  switch (argc)
  {
  case 1:
    while (true)
    {
      // Mejora opcional: Mostrar prompt directorio de trabajo actual.

      // Esto es para que el compilador no saque un error/advertencia: uso y declaracion de variable basura
      auto a = getcwd(current_path, sizeof(current_path));
      (void)a;
      cout << ":~" << current_path << "$ ";
      read(comando, MAX_CARACTERES);
      if (strcmp(comando, "salir") == 0)
      {
        break;
      }
      // Mejora opcional. Ignorar comentarios (prompts precedidos por #)
      char *comment = strchr(comando, '#');
      if (comment)
      {
        *comment = '\0';
      }

      if (*comando == '\0')
        continue;

      // Mejora opcional: Ejecución de varios comandos en un mismo prompt,
      // separados por ";"
      int parts = trocear_linea(comando, ";", partes);
      for (int i = 0; i < parts; i++)
      {
        // Evita violaciones de segmento. Prompts vacíos.
        ejecutar(strtrim(partes[i]));
      }
    }
    break;

  case 2:
  {
    ifstream entrada;
    entrada.getline(comando, MAX_CARACTERES, '\n');
    entrada.open(argv[1]);
    if (entrada.fail())
    {
      cerr << "No puede abrirse el archivo " << argv[1] << endl;
      exit(2);
    }
    else
    {

      while (true)
      {
        // Mejora opcional: Mostrar prompt directorio de trabajo actual.
        entrada.getline(comando, MAX_CARACTERES);
        if (strcmp(comando, "salir") == 0)
        {
          break;
        }
        // Mejora opcional. Ignorar comentarios (prompts precedidos por #)
        char *comment = strchr(comando, '#');
        if (comment)
        {
          *comment = '\0';
        }

        if (*comando == '\0')
          continue;

        // Mejora opcional: Ejecución de varios comandos en un mismo prompt,
        // separados por ";"
        int parts = trocear_linea(comando, ";", partes);
        for (int i = 0; i < parts; i++)
        {
          // Evita violaciones de segmento. Prompts vacíos.
          ejecutar(strtrim(partes[i]));
        }
      }
      entrada.close();
    }
    break;
  }
  default:
    cerr << "Numero de argumentos incorrecto" << endl;
    exit(1);
  }
  exit(0);
}
