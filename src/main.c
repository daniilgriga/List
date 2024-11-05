#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "color_print.h"
#include "list.h"

int list_log_file (struct list_t* list);

FILE* LOG_FILE = NULL;

int main (void)
{
    LOG_FILE = open_log_file ("../graphviz/log_dump.html");

    struct list_t list = {};

    if ( list_ctor (&list) == 1 )
        return 1;

    list_log_file (&list);

    insert_first_elem (&list, 10);
    insert_after (&list, 1, 20);
    insert_after (&list, 2, 30);
    insert_after (&list, 2, 25);

    //delete (&list, 2);
    //delete (&list, 3);
    //delete (&list, 4);
    //delete (&list, 1);

    list_log_file (&list);

    insert_after (&list, 1, 111);
    list_log_file (&list);

    insert_after (&list, 4, 211);
    list_log_file (&list);

    insert_after (&list, 3, 311);
    list_log_file (&list);

    insert_after (&list, 4, 411);
    list_log_file (&list);

    insert_after (&list, 2, 511);
    list_log_file (&list);

    insert_after (&list, 6, 611);
    list_log_file (&list);

    insert_after (&list, 7, 611);
    list_log_file (&list);

    delete (&list, 7);
    list_log_file (&list);

    delete (&list, 11);
    list_log_file (&list);

    list_dtor (&list);

    fclose (LOG_FILE);

    return 0;
}

int list_log_file (struct list_t* list)
{
    assert (list);

    fprintf (LOG_FILE, "<pre>\n");

    fprintf (LOG_FILE, "\n\n\n\ndata: ");
    for (int i = 0; i < list->size; i++)
        fprintf (LOG_FILE, "%03d  ", i);
    fprintf (LOG_FILE, "\n      ");

    for (int i = 0; i < list->size; i++)
        fprintf(LOG_FILE, "%03d  ", list->data[i]);
    fprintf (LOG_FILE, "\n  %*s^ free = %03d\n", (list->free_elem + 1) * 5, "", list->free_elem);

    fprintf (LOG_FILE, "\n\n\n\nnext: ");
    for (int i = 0; i < list->size; i++)
        fprintf (LOG_FILE, "%03d  ", i);
    fprintf (LOG_FILE, "\n      ");

    for (int i = 0; i < list->size; i++)
        fprintf (LOG_FILE, "%03d  ", list->next[i]);
    fprintf (LOG_FILE, "\n  %*s^ head = %03d\n", (list->head + 1) * 5, "", list->head);

    fprintf (LOG_FILE, "\n\n\n\nprev: ");
    for (int i = 0; i < list->size; i++)
        fprintf (LOG_FILE, "%03d  ", i);
    fprintf (LOG_FILE, "\n      ");

    for (int i = 0; i < list->size; i++)
        fprintf (LOG_FILE, "%03d  ", list->prev[i]);
    fprintf (LOG_FILE, "\n  %*s^ tail = %03d\n", (list->tail + 1) * 5, "", list->tail);

    const char* filename = graph_dump (list);

    fprintf (LOG_FILE, "\n\n<img src=\"%s\">", filename);

    //fflush  (LOG_FILE);
    //getchar ();

    return 0;
}
