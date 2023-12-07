#include "cstack.h"
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// экземпляр элемента стека
struct node
{
    struct node *prev;
    unsigned int size;
    char data[]; // член типа гибкого массива
};

typedef struct node *stack_t;

// экземпляр стека
struct stack_entry
{
    int reserved;
    stack_t stack;
};

typedef struct stack_entry stack_entry_t;

// таблица экземпляров стека
struct stack_entries_table
{
    unsigned int size;
    stack_entry_t **entries;
};

struct stack_entries_table g_table = {0u, NULL};

hstack_t stack_new(void)
{
    // хэндлер нового стека
    hstack_t new_hstack;

    // объявляем указатель на пустой стек
    stack_entry_t *new_stack;

    new_stack = malloc(sizeof(stack_entry_t));
    if (new_stack)
    {
        new_stack->reserved = 0;
        new_stack->stack = NULL;
    }
    else
        return -1;

    // добавляем указатель на новый стек в массив указателей на стек

    g_table.entries = realloc(g_table.entries, (g_table.size + 1) * sizeof(new_stack));
    if (g_table.entries)
    {
        g_table.size++;
        g_table.entries[g_table.size - 1] = new_stack;
        new_hstack = g_table.size - 1;
        return new_hstack;
    }
    return -1;
}

void stack_free(const hstack_t hstack)
{
    if (!stack_valid_handler(hstack))
    {
        stack_t temp;

        // удаляем элементы стека
        //------------------------------------------------//
        while(g_table.entries[hstack]->reserved)
        {
            // сохраняем текущий указатель стека
            temp = g_table.entries[hstack]->stack;

            // сохраняем в указатель на стек указатель на предыущий элемент
            g_table.entries[hstack]->stack = temp->prev;

            // удаляем текущий элемент
            free(temp);

            // декрементируем размер стека
            g_table.entries[hstack]->reserved--;

        }
        //------------------------------------------------//

        // удаляем стек из массива указателей на стек
        //------------------------------------------------//
        free(g_table.entries[hstack]);

        // если удаленый стек был единственным стеком,
        // то "очищаем" массив указателей на стек
        if (g_table.size == 1)
        {
            g_table.size = 0;
            free(g_table.entries);

            // для определенного поведения при новом выделении памяти
            // с помощью realloc()
            g_table.entries = NULL;
        }
        
        else
        {
            // если удаленный стек не был на последнем месте в массиве,
            // то в таблице указателей на стек выполняем смещение   
            if ((unsigned int)hstack != (g_table.size - 1))
            {
                for (unsigned int i = hstack; i < g_table.size - 1; i++)
                    g_table.entries[i] = g_table.entries[i+1];
            }
            g_table.entries[g_table.size - 1] = NULL;
            g_table.size--;
        }
        //------------------------------------------------//
    }

}

int stack_valid_handler(const hstack_t hstack)
{
    if ((unsigned int)hstack < g_table.size && hstack >= 0)
        return 0;

    return 1;
}

unsigned int stack_size(const hstack_t hstack)
{
    if (!stack_valid_handler(hstack))
    {
        return g_table.entries[hstack]->reserved;
    }
    return 0;
}

void stack_push(const hstack_t hstack, const void* data_in, const unsigned int size)
{
    if (!stack_valid_handler(hstack))
    {
        if (data_in && size)
        {
            stack_t ptr_new_node;

            // запрос области памяти для размещения структуры типа node и массива размера size
            // ptr_new_node ссылается на выделенную область для нового элемента стека
            ptr_new_node = malloc(sizeof(struct node) + (size_t)size);

            // заполняем выделенную область данными
            memcpy(ptr_new_node->data, data_in, (size_t)size);

            // сохранение указателя на предыдущий элемент
            ptr_new_node->prev = g_table.entries[hstack]->stack;

            // обновление указателя стека
            g_table.entries[hstack]->stack = ptr_new_node;

            // инкрементирование размера стека
            g_table.entries[hstack]->reserved++;
        }
    }
    
}

unsigned int stack_pop(const hstack_t hstack, void* data_out, const unsigned int size)
{
    if (!stack_valid_handler(hstack))
    {
        if (g_table.entries[hstack]->stack && size >= g_table.entries[hstack]->stack->size && data_out)
        {
            stack_t temp;

            // сохраняем текущий указатель стека
            temp = g_table.entries[hstack]->stack;

            // сохраняем в указатель на стек указатель на предыущий элемент
            g_table.entries[hstack]->stack = temp->prev;

            // заполняем буфер данными
            memcpy(data_out, temp->data, (size_t)size);

            // удаляем текущий элемент
            free(temp);

            // декрементируем размер стека
            g_table.entries[hstack]->reserved--;

            return size;
        }
    }
    return 0;
}

