#ifndef LIST_H_
#define LIST_H_

typedef int elem_t;

struct list_t
{
    elem_t* data;
    int* next;
    int* prev;

    int head;
    int tail;

    int size;

    int free_elem;
};

void place_info (const char* file, int line, const char* func, const char* string);

int list_ctor (struct list_t* list);

int list_dtor (struct list_t* list);

int load_free_places (struct list_t* list);

int insert_after (struct list_t* list, const int position, const elem_t elem);

const char* graph_dump (struct list_t* list);

int delete (struct list_t* list, const int position);

int insert_first_elem (struct list_t* list, const elem_t elem);

FILE* open_log_file (const char* filename);

int verificator (struct list_t* list);

int increasing_size (struct list_t* list);

int list_assert (struct list_t* list, const char* file, int line, const char* func);

#endif // LIST_H_
