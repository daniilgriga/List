#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "../include/color_print.h"

#define SIZE         10
#define FREE_PLACE   -1
#define POISON_PLACE 666

typedef int elem_t;

struct list_t
{
    elem_t* data;
    int* next;
    int* prev;

    int head;
    int tail;

    int free_elem;
};

int list_logfile (struct list_t* list);

int list_ctor (struct list_t* list);

int list_dtor (struct list_t* list);

int load_free_places (struct list_t* list);

int load_elements (struct list_t* list);

int insert_after (struct list_t* list, const int position, const elem_t elem);

int graph_dump (struct list_t* list);

int main (void)
{
    struct list_t list = {};

    if ( list_ctor (&list) == 1 )
        return 1;

    //list_logfile (&list);

    insert_after (&list, 2, 25);

    list_logfile (&list);

    graph_dump (&list);

    list_dtor (&list);

    return 0;
}

int graph_dump (struct list_t* list)
{
    assert (list);

    FILE* graph_file = fopen ("../graphviz/dump.dot", "w");
    if (graph_file == NULL)
    {
        printf(RED_TEXT("ERROR open graph_file\n"));
        return 1;
    }

    fprintf (graph_file, "digraph\n{\n");
    fprintf (graph_file, "rankdir = \"LR\";\n");
    for (int i = 0; i < SIZE; i++)
        fprintf (graph_file, "node00%d [shape=Mrecord; label = \"  00%d | data = %3d | next = %3d | prev = %3d  \"];\n",
                 i, i, list->data[i], list->next[i], list->prev[i]);
    fprintf (graph_file, "\n");
    for (int i = 0; i < SIZE - 1; i++)
        fprintf (graph_file, "node00%d -> node00%d [ weight=1000; color=white; ];\n",
                 i, i + 1);

    fprintf (graph_file, "\nnode000 -> node000 [ label = \"lol\"];\n\n");

    for (int i = 1; i <= list->head; i++)
        fprintf (graph_file, "node00%d -> node00%d [ color=red; ]\n",
                 i, list->next[i]);
    fprintf (graph_file, "\n");

    for (int i = 1; i <= list->head; i++)
        fprintf (graph_file, "node00%d -> node00%d [ color=blue; ]\n",
                 i, list->prev[i]);
    fprintf (graph_file, "}");

    fclose (graph_file);

    return 0;
}

int insert_after (struct list_t* list, const int position, const elem_t elem)
{
    assert (list);

    int count = 0;

    for (count; count < SIZE; count++)
    {
        if (list->next[count] == -1)
            break;
    }

    list->free_elem = count;

    list->head = list->free_elem;

    list->data[list->free_elem] = elem;

    list->next[list->free_elem] = list->next[position];

    list->prev[list->free_elem] = position;

    list->prev[list->next[position]] = list->free_elem;

    list->next[position] = list->free_elem;

    return 0;
}

int list_ctor (struct list_t* list)
{
    assert (list);

    list->data = (elem_t*) calloc ( (size_t) SIZE , sizeof(list->data[0]) );
    if (list->data == NULL)
    {
        printf(RED_TEXT("ERROR: ")BLUE_TEXT("[list->data]")" is "BLUE_TEXT("[NULL]\n"));
        return 1;
    }

    list->head = 0;
    list->tail = 0;

    list->free_elem = 0;

    list->next = (elem_t*) calloc ( (size_t) SIZE , sizeof(list->next[0]) );
    if (list->next == NULL)
    {
        printf(RED_TEXT("ERROR: ")BLUE_TEXT("[list->next]")" is "BLUE_TEXT("[NULL]\n"));
        return 1;
    }

    list->prev = (elem_t*) calloc ( (size_t) SIZE , sizeof(list->prev[0]) );
    if (list->prev == NULL)
    {
        printf(RED_TEXT("ERROR: ")BLUE_TEXT("[list->prev]")" is "BLUE_TEXT("[NULL]\n"));
        return 1;
    }

    load_free_places (list);

    load_elements (list);

    return 0;
}

int load_free_places (struct list_t* list)
{
    assert (list);

    for (int i = 1; i < SIZE; i++)
        list->data[i] = POISON_PLACE;

    for (int i = 1; i < SIZE; i++)
        list->next[i] = FREE_PLACE;

    for (int i = 1; i < SIZE; i++)
        list->prev[i] = FREE_PLACE;


    return 0;
}

int load_elements (struct list_t* list)
{
    assert (list);

    for (int i = 1; i <= SIZE - 5; i++) // NOTE
        list->data[i] = 10 * i;

    list->head = 5;
    list->tail = 1;

    for (int i = 1; i <= SIZE - 5; i++)
        list->next[i] = i + 1;
    list->next[SIZE - 5] = 0;

    for (int i = 1; i <= SIZE - 5; i++)
        list->prev[i] = i - 1;

    return 0;
}

int list_dtor (struct list_t* list)
{
    assert (list);

    list->head = 0;
    list->tail = 0;

    list->free_elem = 0;

    free (list->data);
    free (list->next);
    free (list->prev);

    return 0;
}

int list_logfile (struct list_t* list)
{
    assert (list);

    FILE* logfile = fopen ("../graphviz/data.html", "w");
    if (logfile == NULL)
    {
        printf(RED_TEXT("ERROR open file\n"));
        return 1;
    }

    fprintf (logfile, "<pre>\n");
    fprintf (logfile, "\n\n\n\ndata: 000  001  002  003  004  005  006  007  008  009  010  011  012  013  014  015\n      ");
    for (int i = 0; i < SIZE; i++)
        fprintf(logfile, "%3d  ", list->data[i]);
    fprintf (logfile, "\n  %*s^ tail = %03d   %*s^ head = %03d\n", list->tail * 5, "", list->tail, (list->head - 3)  * 5, "", list->head);

    fprintf (logfile, "\n\n\n\nnext: 000  001  002  003  004  005  006  007  008  009  010  011  012  013  014  015\n      ");

    for (int i = 0; i < SIZE; i++)
        fprintf (logfile, "%3d  ", list->next[i]);

    fprintf (logfile, "\n\n\n\nprev: 000  001  002  003  004  005  006  007  008  009  010  011  012  013  014  015\n      ");

    for (int i = 0; i < SIZE; i++)
        fprintf (logfile, "%3d  ", list->prev[i]);

    fprintf (logfile, "\n\n<img src=\"dump.png\">");

    //fflush  (logfile);
    //getchar ();

    fclose (logfile);

    return 0;
}

