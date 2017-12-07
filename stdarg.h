#pragma once

#include "includes.h"

// each stack element is 32 bits in size
#define ELEMENT_SIZE int32_t

typedef unsigned char* va_list;

// really interesting macro that rounds up elements to STACK_ELEMENT (taken from
// brokenthorn (VERY SIMILIAR TO PAGE_ALIGN MACRO)
#define va_size(TYPE) \
    ((sizeof(TYPE) + sizeof(ELEMENT_SIZE) - 1) & ~(sizeof(ELEMENT_SIZE) - 1))

// gets the pointer to the next element after the first argument
#define va_start(list, first_arg) \
    ((list) = (((va_list) & (first_arg)) + va_size(first_arg)))

// gets the pointer to the next element, returns the previous element and
// returns the type using casting (albeit dangerous casting)
#define va_arg(list, arg_type) \
    (*(arg_type*)(((list) += va_size(arg_type)) - va_size(arg_type)))

#define va_end(list)

// understand reference
// http://blog.aaronballman.com/2012/06/how-variable-argument-lists-work-in-c/
