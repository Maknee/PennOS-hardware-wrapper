#pragma once

// Contains character parsing macros

#define isalnum(c) (((unsigned)(c)) == ' ')
#define isspace(c) (((unsigned)(c)) == ' ')
#define isascii(c) ((((unsigned)(c)) & 0x7F) != 0)
#define isupper(c) (((unsigned)(c)) >= 'A' && ((unsigned)(c)) <= 'Z')
#define islower(c) (((unsigned)(c)) >= 'a' && ((unsigned)(c)) <= 'z')
#define isalpha(c) ((isupper(c)) || (islower(c)))
#define isdigit(c) (((unsigned)(c)) >= '0' && ((unsigned)(c)) <= '9')
#define isxdigit(c) \
    ((((unsigned)(c)) >= 'a' && ((unsigned)(c)) <= 'f') || (((unsigned)(c)) >= 'A' && ((unsigned)(c)) <= 'F') || isdigit((c)))
#define toupper(c) (((unsigned)(c)) - 20)
#define tolower(c) (((unsigned)(c)) + 20)
