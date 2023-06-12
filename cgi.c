#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

/* CGI Environment Variablen */
#define SERVER_NAME getenv("SERVER_NAME")
#define SERVER_PROTOCOL getenv("SERVER_PROTOCOL")
#define SERVER_PORT getenv("SERVER_PORT")
#define PATH_INFO getenv("PATH_INFO")
#define PATH_TRANSLATED getenv("PATH_TRANSLATED")
#define SCRIPT_NAME getenv("SCRIPT_NAME")
#define REMOTE_HOST getenv("REMOTE_HOST")
#define REMOTE_ADDR getenv("REMOTE_ADDR")
#define CONTENT_TYPE getenv("CONTENT_TYPE")
#define REQUEST_METHOD getenv("REQUEST_METHOD")
#define CONTENT_LENGTH getenv("CONTENT_LENGTH")
#define QUERY_STRING getenv("QUERY_STRING")

struct LinListe
  {
  char *name;
  char *value;
  struct LinListe *next;
  };

/* Funktionsprototypen */
char x2c(char hex[]);
void unescape(char *url);
struct LinListe *CGIReadInput(struct LinListe *root);
struct LinListe *Enter(struct LinListe *root, char *Name, char *Value);
void Drucke(struct LinListe *root);
struct LinListe *Suche(struct LinListe *root, char *SuchName);

struct LinListe *root = NULL;

int main(void)
  {
 struct LinListe *Erg;

  printf("Content-Type: text/html\r\n\r\n");
  printf("<HTML><HEAD><TITLE>Formular-Eingabe</TITLE></HEAD>\n");
  printf("<BODY><H1>Wiki-Eingabe</H1>\n");

  root = CGIReadInput(root);
  Drucke(root);

  Erg = Suche(root,"Autor");
  if (Erg == NULL)
    printf("Nicht gefunden\n");
  else
    printf("Suchergebnis: %s\n",Erg->value);

  printf("</BODY></HTML>\n");
  return EXIT_SUCCESS;
  }

struct LinListe *Enter(struct LinListe *root, char *Name, char *Value)
  {
  struct LinListe *Neu;

  Neu = (struct LinListe *) malloc(sizeof(struct LinListe));
  if (Neu == NULL)
    {
    printf("Speicher voll, Abbruch...\n");
    exit(1);
    }
  Neu->name = (char *) malloc(strlen(Name)+1);
  Neu->value = (char *) malloc(strlen(Value)+1);
  Neu->next = NULL;
  strcpy(Neu->name,Name);
  strcpy(Neu->value,Value);
 /* Element vor root einhaengen, root wird neues Element */
  Neu->next = root;
  root = Neu;
  return(root);
  }

void Drucke(struct LinListe *root)
  {
  struct LinListe *Tail = root;
  while (Tail != NULL)
    {
    printf("%s: %s\n", Tail->name, Tail->value);
    Tail = Tail->next;
    }
  }

struct LinListe *Suche(struct LinListe *root, char *SuchName)
  {
  struct LinListe *Tail = root;
  while (Tail != NULL)
    {
    if (strcmp(SuchName,Tail->name) == 0)
      return(Tail);
    Tail = Tail->next;
    }
  return(NULL);
  }

struct LinListe *CGIReadInput(struct LinListe *root)
  /* Bearbeiten der CGI-Daten und Aufspalten in
     die Paare Name/Wert. Es werden GET- und
     POST-Requests verarbeitet. */
  {
  int i, j, content_length, maxlen;
  short NM = 1; /* Statusvariable, NM=1: Name, NM=0: Wert */
  char *input, *name, *value, *filename;

  /* Eingabe prüfen */
  if (REQUEST_METHOD == NULL)
    {
    printf("Error: REQUEST_METHOD is null\n");
    exit(1);
    }
  if (CONTENT_LENGTH == NULL)
    {
    return(NULL);
    }

  /* Benoetigte Werte holen und Speicher reservieren */
  content_length = atoi(CONTENT_LENGTH);
  maxlen = content_length - 1;
  /* genügend Speicher fuer die Eingabe reservieren */
  input = malloc(sizeof(char) * content_length + 1);
  /* Name und Wert koennen maximal so lang sein wie die Eingabe */
  name = malloc(sizeof(char) * content_length + 1);
  value = malloc(sizeof(char) * content_length + 1);
  filename = malloc(sizeof(char) * content_length + 1);
  if (!strcmp(REQUEST_METHOD,"POST"))
    { /* Daten von der Standardeingabe lesen */
    if (fread(input,sizeof(char),content_length,stdin) != content_length)
      {
      /* consistency error. */
      printf("Error: input length < CONTENT_LENGTH\n");
      exit(1);
      }
    }
  else if (!strcmp(REQUEST_METHOD,"GET"))
    { /* Daten aus der Umgebungsvariablen QUERY_STRING holen */
    if (QUERY_STRING == NULL)
      {
      printf("Error: QUERY_STRING is null\n");
      exit(1);
      }
    strcpy(input,QUERY_STRING);
    }
  else
    { /* error: invalid request method */
    printf("Error: REQUEST_METHOD invalid\n");
    exit(1);
    }

  /* Daten aufsplitten */
  j = 0;
  for (i = 0; i < content_length; i++)
    {
    if (input[i] == '=')
      { /* Name zuende, nun Wert verarbeiten */
      name[j] = '\0';
      unescape(name);
      if (i == maxlen)
        { /* Ende der Eingabe, kein Wert */
        strcpy(value,"");
        /* abspeichern */
        root = Enter(root,name,value);
        printf ("%s --> '%s'<br>\n",name, value);

        }
      j = 0; NM = 0;
      }
    else if ((input[i] == '&') || (i == maxlen))
      { /* Wertangabe zuende */
      if (i == maxlen)
        { /* Ende der Eingabe, letzter Wert */
        value[j] = input[i];
        j++;
        }
      value[j] = '\0';
      unescape(value);
      /* abspeichern */
      root = Enter(root,name,value);
      printf ("%s --> '%s'<br>\n",name, value);
      j = 0; NM = 1;
      }
    else if (NM)
      { /* Name wird gerade gelesen */
      name[j] = input[i];
      j++;
      }
    else if (!NM)
      { /* Wert wird gerade gelesen */
      value[j] = input[i];
      j++;
      }
    }
  return(root);
  }
