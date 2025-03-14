#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "color_print.h"
#include "list.h"

#define FREE_PLACE   -1
#define POISON_PLACE 666
#define CREATED_SIZE 5

enum errors
{
    NO_ERRORS                = 0,
    SIZE_BELOW_OR_EQUAL_ZERO = 1,
    HEAD_BELOW_ZERO          = 2,
    TAIL_BELOW_ZERO          = 3,
    FREE_ELEM_BELOW_ZERO     = 4,
    N_AND_PR_DOES_NOT_MATCH  = 5,
    PREV_ARE_NOT_IN_ORDER    = 6
};

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

int verificator (struct list_t* list)
{
    assert (list);

    if (list->size <= 0)
        return SIZE_BELOW_OR_EQUAL_ZERO;

    if (list->head < 0)
        return HEAD_BELOW_ZERO;

    if (list->tail < 0)
        return TAIL_BELOW_ZERO;

    if (list->free_elem < 0)
        return FREE_ELEM_BELOW_ZERO;

    for (int i = 0; i < list->size; i++)
        if ( list->next[list->prev[i]] != list->prev[list->next[i]] )
            return N_AND_PR_DOES_NOT_MATCH;

    for (int i = list->free_elem; i < list->size; i++)
        if ( list->prev[i] != FREE_PLACE )
            return PREV_ARE_NOT_IN_ORDER;

    return NO_ERRORS;
}

int list_assert (struct list_t* list, const char* file, int line, const char* func)
{
    int error = verificator (list);

    if (!error)
        return 0;

    place_info(file, line, func, "STACK_ASSERT");

    switch (error)
    {
        case SIZE_BELOW_OR_EQUAL_ZERO:
        {
            fprintf (stderr, "ERROR N%d: list->size below or equal zero",       SIZE_BELOW_OR_EQUAL_ZERO);
            break;
        }

        case HEAD_BELOW_ZERO:
        {
            fprintf (stderr, "ERROR N%d: list->head below zero",                HEAD_BELOW_ZERO);
            break;
        }

        case TAIL_BELOW_ZERO:
        {
            fprintf (stderr, "ERROR N%d: list->tail below zero",                TAIL_BELOW_ZERO);
            break;
        }

        case FREE_ELEM_BELOW_ZERO:
        {
            fprintf (stderr, "ERROR N%d: list->free_elem below zero",           FREE_ELEM_BELOW_ZERO);
            break;
        }

        case N_AND_PR_DOES_NOT_MATCH:
        {
            fprintf (stderr, "ERROR N%d: list->next does not match list->prev", N_AND_PR_DOES_NOT_MATCH);
            break;
        }

        case PREV_ARE_NOT_IN_ORDER:
        {
            fprintf (stderr, "ERROR N%d: the free_elements are not in order",   PREV_ARE_NOT_IN_ORDER);
            break;
        }

        default:
           fprintf (stderr, "ERROR N?: unknown error :(");
    }

    return 1;
}

void place_info (const char* file, int line, const char* func, const char* string)
{
    printf(PURPLE_TEXT("called from %s, name \"%s\" born at %s:%d:\n"),  file, string, func, line);
}

int increasing_size (struct list_t* list)
{

    int old_size = list->size;
    list->size *= 2;
    int new_size = list->size;

    list->data = (elem_t*) realloc ( list->data, (size_t) new_size * sizeof(list->data[0]) );
    if (list->data == NULL)
    {
        printf(RED_TEXT("ERROR: ")BLUE_TEXT("[list->data]")" is "BLUE_TEXT("[NULL]\n")"after realloc");
        return 1;
    }

    for (int i = old_size; i < new_size; i++)
        list->data[i] = POISON_PLACE;

    list->next = (elem_t*) realloc ( list->next, (size_t) new_size * sizeof(list->next[0]));
    if (list->next == NULL)
    {
        printf(RED_TEXT("ERROR: ")BLUE_TEXT("[list->next]")" is "BLUE_TEXT("[NULL]\n")"after realloc");
        return 1;
    }

    for (int i = old_size; i < new_size; i++)
        list->next[i] = i + 1;
    list->next[new_size - 1] = 0;

    list->prev = (elem_t*) realloc ( list->prev, (size_t) new_size * sizeof(list->prev[0]));
    if (list->prev == NULL)
    {
        printf(RED_TEXT("ERROR: ")BLUE_TEXT("[list->prev]")" is "BLUE_TEXT("[NULL]\n")"after realloc");
        return 1;
    }

    for (int i = old_size; i < new_size; i++)
        list->prev[i] = FREE_PLACE;

    list->free_elem = old_size;

    return 0;
}

int list_ctor (struct list_t* list)
{
    assert (list);

    list->size = CREATED_SIZE;

    list->data = (elem_t*) calloc ( (size_t) list->size , sizeof(list->data[0]) );
    if (list->data == NULL)
    {
        printf(RED_TEXT("ERROR: ")BLUE_TEXT("[list->data]")" is "BLUE_TEXT("[NULL]\n"));
        return 1;
    }

    list->head = 0;
    list->tail = 0;

    list->free_elem = 1;

    list->next = (elem_t*) calloc ( (size_t) list->size , sizeof(list->next[0]) );
    if (list->next == NULL)
    {
        printf(RED_TEXT("ERROR: ")BLUE_TEXT("[list->next]")" is "BLUE_TEXT("[NULL]\n"));
        return 1;
    }

    list->prev = (elem_t*) calloc ( (size_t) list->size , sizeof(list->prev[0]) );
    if (list->prev == NULL)
    {
        printf(RED_TEXT("ERROR: ")BLUE_TEXT("[list->prev]")" is "BLUE_TEXT("[NULL]\n"));
        return 1;
    }

    load_free_places (list);

    return 0;
}

int insert_first_elem (struct list_t* list, const elem_t elem)
{
    assert (list);

    insert_after (list, 0, elem);

    list->head    = 1;
    list->next[0] = list->head;

    return 0;
}

const char* graph_dump (struct list_t* list)
{
    assert (list);

    FILE* graph_file = fopen ("graphviz/dump.dot", "w");
    if (graph_file == NULL)
    {
        printf(RED_TEXT("ERROR open graph_file\n"));
        return NULL;
    }

    fprintf (graph_file, "digraph\n{\n");
    fprintf (graph_file, "rankdir = \"LR\";\n");

    for (int i = 0; i < list->size; i++)
        fprintf (graph_file, "node%03d [shape=Mrecord; label = \"  %03d | data = %3d | next = %3d | prev = %3d  \"];\n",
                 i, i, list->data[i], list->next[i], list->prev[i]);
    fprintf (graph_file, "\n");

    for (int i = 0; i < list->size - 1; i++)
        fprintf (graph_file, "node%03d -> node%03d [ weight=1000; color=white; ];\n",
                 i, i + 1);

    for (int i = 0; i < list->size; i++)
    {
        if (list->prev[i] == -1)
        {
            fprintf (graph_file, "node%03d -> node%03d [ color=\"#00FF00\"; ]\n",
                     i, list->next[i]);

            fprintf (graph_file, "\n");
        }
        else
        {
            fprintf (graph_file, "node%03d -> node%03d [ color=\"#0000FF\"; ]\n",
                     i, list->next[i]);

            fprintf (graph_file, "\n");

            fprintf (graph_file, "node%03d -> node%03d [ color=\"#00FFFF\"; ]\n",
                     i, list->prev[i]);
        }
    }

    fprintf (graph_file, "}");
    fprintf (graph_file, "\n");

    fclose  (graph_file);

    static int dump_number = 1;
    static char filename[50] = {};
    char    command_name[100] = {};

    sprintf (filename, "graphviz/dump%d.png", dump_number++);
    sprintf (command_name, "dot graphviz/dump.dot -Tpng -o %s", filename);

    system  (command_name);

    return filename;
}

int insert_after (struct list_t* list, const int position, const elem_t elem)
{
    assert (list);

    if (list->free_elem == 0)
        increasing_size (list);

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
    }
    else
    {
        list->next[old_prev] = old_next;
    }

    list->prev[position] = FREE_PLACE;

    return 0;
}


int load_free_places (struct list_t* list)
{
    assert (list);

    for (int i = 1; i < list->size; i++)
        list->data[i] = POISON_PLACE;

    list->next[0] = 0;
    for (int i = 1; i < list->size; i++)
        list->next[i] = i + 1;
    list->next[list->size - 1] = 0;

    for (int i = 1; i < list->size; i++)
        list->prev[i] = FREE_PLACE;

    return 0;
}

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
