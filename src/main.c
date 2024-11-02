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

int list_log_file (struct list_t* list);

int list_ctor (struct list_t* list);

int list_dtor (struct list_t* list);

int load_free_places (struct list_t* list);

int load_elements_fr_dbg (struct list_t* list);

int insert_after (struct list_t* list, const int position, const elem_t elem);

const char* graph_dump (struct list_t* list);

int delete (struct list_t* list, const int position);

int insert_first_elem (struct list_t* list, const elem_t elem);

FILE* open_log_file (const char* filename);


FILE* LOG_FILE = NULL;

int main (void)
{
    LOG_FILE = open_log_file ("../graphviz/log_dump.html");

    struct list_t list = {};

    if ( list_ctor (&list) == 1 )
        return 1;

    insert_first_elem (&list, 10);
    insert_after (&list, 1, 20);
    insert_after (&list, 2, 30);
    insert_after (&list, 2, 25);

    delete (&list, 2);
    delete (&list, 3);
    delete (&list, 4);
    //delete (&list, 1);

    list_log_file (&list);

    insert_after (&list, 1, 111);

    list_log_file (&list);

    insert_after (&list, 4, 211);
    insert_after (&list, 3, 311);
    //insert_after (&list, 4, 411);
    //insert_after (&list, 2, 511);
    //insert_after (&list, 6, 611);

    //graph_dump (&list);

    list_log_file (&list);

    list_dtor (&list);

    fclose (LOG_FILE);

    return 0;
}

FILE* open_log_file (const char* filename)
{
    FILE* log_file = fopen (filename, "w");
    if (log_file == NULL)
    {
        printf(RED_TEXT("ERROR open log_dump file\n"));
        return NULL;
    }

    return log_file;
}


int insert_first_elem (struct list_t* list, const elem_t elem)
{
    insert_after (list, 0, elem);

    list->head    = 1;
    list->next[0] = list->head;

    return 0;
}

const char* graph_dump (struct list_t* list)
{
    assert (list);

    FILE* graph_file = fopen ("../graphviz/dump.dot", "w");
    if (graph_file == NULL)
    {
        printf(RED_TEXT("ERROR open graph_file\n"));
        return NULL;
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

    for (int i = 0; i < SIZE; i++)
    {
        if (list->prev[i] == -1)
        {
            fprintf (graph_file, "node00%d -> node00%d [ color=green; ]\n",
                     i, list->next[i]);

            fprintf (graph_file, "\n");
        }
        else
        {
            fprintf (graph_file, "node00%d -> node00%d [ color=red; ]\n",
                     i, list->next[i]);

            fprintf (graph_file, "\n");

            fprintf (graph_file, "node00%d -> node00%d [ color=blue; ]\n",
                     i, list->prev[i]);
        }

    }

    fprintf (graph_file, "}");
    fprintf (graph_file, "\n");

    fclose  (graph_file);

    static int dump_number = 1;
    static char filename[50] = {};
    char    command_name[100] = {};

    sprintf (filename, "../graphviz/dump%d.png", dump_number++);
    sprintf (command_name, "dot ../graphviz/dump.dot -Tpng -o %s", filename);

    system  (command_name);

    return filename;
}

int insert_after (struct list_t* list, const int position, const elem_t elem)
{
    assert (list);

    int new_elem_position = list->free_elem;

    list->free_elem = list->next[new_elem_position];

    if (list->tail == position)
    {
        list->prev[0] = new_elem_position;
        list->tail    = new_elem_position;
    }


    int old_next = list->next[position];

    list->data[new_elem_position] = elem;

    list->prev[new_elem_position] = position;

    list->next[new_elem_position] = old_next;

    list->next[position] = new_elem_position;

    list->prev[old_next] = new_elem_position;


    return 0;
}

int delete (struct list_t* list, const int position)
{
    assert (list);

    list->data[position] = POISON_PLACE;

    int old_next = list->next[position];
    int old_prev = list->prev[position];

    list->next[position] = list->free_elem;
    list->free_elem = position;

    if (list->prev[0] == position)
    {
        list->prev[0] = list->prev[position];
        list->tail    = list->prev[0];
    }
    else
    {
        list->prev[old_next] = old_prev;
    }

    if (list->prev[0] == position)
    {
        list->next[0] = list->next[position];
        list->head    = list->next[0];

        printf ("%d \n", list->next[position]);
    }
    else
    {
        list->next[old_prev] = old_next;
    }

    list->prev[position] = FREE_PLACE;
    /*
    list->prev[list->next[position]] = list->prev[position];

    list->next[list->prev[position]] = list->next[position];

    list->prev[position] = FREE_PLACE;

    //NOTE - if ???

    int old_free = list->free_elem;

    list->free_elem = position;

    list->next[position] = old_free;
    */
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

    list->free_elem = 1;

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

    //load_elements_fr_dbg (list);

    return 0;
}

int load_free_places (struct list_t* list)
{
    assert (list);

    for (int i = 1; i < SIZE; i++)
        list->data[i] = POISON_PLACE;

    list->next[0] = 0;
    for (int i = 1; i < SIZE - 1; i++)
        list->next[i] = i + 1;
    list->next[SIZE - 1] = 0;

    for (int i = 1; i < SIZE; i++)
        list->prev[i] = FREE_PLACE;

    return 0;
}

#if 0
int load_elements_fr_dbg (struct list_t* list)
{
    assert (list);

    for (int i = 1; i <= SIZE - 5; i++)
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
#endif

int list_dtor (struct list_t* list)
{
    assert (list);

    free (list->data);
    free (list->next);
    free (list->prev);

    list->head = 0;
    list->tail = 0;

    list->free_elem = 0;

    return 0;
}

int list_log_file (struct list_t* list)
{
    assert (list);

    fprintf (LOG_FILE, "<pre>\n");
    fprintf (LOG_FILE, "\n\n\n\ndata: 000  001  002  003  004  005  006  007  008  009  010  011  012  013  014  015\n      ");
    for (int i = 0; i < SIZE; i++)
        fprintf(LOG_FILE, "%3d  ", list->data[i]);
    fprintf (LOG_FILE, "\n  %*s^ free = %03d\n", (list->free_elem + 1) * 5, "", list->free_elem);

    fprintf (LOG_FILE, "\n\n\n\nnext: 000  001  002  003  004  005  006  007  008  009  010  011  012  013  014  015\n      ");

    for (int i = 0; i < SIZE; i++)
        fprintf (LOG_FILE, "%3d  ", list->next[i]);
    fprintf (LOG_FILE, "\n  %*s^ head = %03d\n", (list->head + 1) * 5, "", list->head);

    fprintf (LOG_FILE, "\n\n\n\nprev: 000  001  002  003  004  005  006  007  008  009  010  011  012  013  014  015\n      ");
    for (int i = 0; i < SIZE; i++)
        fprintf (LOG_FILE, "%3d  ", list->prev[i]);
    fprintf (LOG_FILE, "\n  %*s^ tail = %03d\n", (list->tail + 1) * 5, "", list->tail);

    const char* filename = graph_dump (list);

    fprintf (LOG_FILE, "\n\n<img src=\"%s\">", filename);

    fflush  (LOG_FILE);
    //getchar ();

    return 0;
}

