#include "stdio.h"

int snprintf(char * s, size_t n, const char * format, ...)
{
    va_list list;
    va_start(list, format);

	char* end = (char*)((uintptr_t)s + n);
    for (size_t i = 0; i < strlen(format); i++)
    {
    	//this check does not cover all possible ways that buffer can oveflow
    	if(s >= end)
	{
		break;
	}
        switch (format[i])
        {
        case '%':
        {
            switch (format[i + 1])
            {
            case 'c':
            {
                char c = va_arg(list, char);
                *s++ = c;
                i++;
                break;
            }
            case 's':
            {
                char* buf = va_arg(list, char*);
		//don't have strncpy
		uint32_t str_len = strlen(buf);
		memcpy(s, buf, str_len);
		s += str_len;
                
		i++;
                break;
            }
            case 'd':
            case 'i':
            {
                int32_t integer = va_arg(list, int32_t);
                char buf[32]    = { 0 };
                itoa(integer, buf, 10);
	
		uint32_t str_len = strlen(buf);
                memcpy(s, buf, str_len);
		s += str_len;
                
		i++;
                break;
            }
            case 'x':
            case 'X':
            {
                uint32_t integer = va_arg(list, uint32_t);
                char buf[32]     = { 0 };
                itoa_u(integer, buf, 16);
		
		uint32_t str_len = strlen(buf);
                memcpy(s, buf, str_len);
		s += str_len;
                
		i++;
		break;
            }
            default:
            {
		*s++ = format[i];
                i++;
                break;
            }
            }
            break;
        }
        default:
        {
	*s++ = format[i];
            break;
        }
        }
    }

    va_end(list);
    return 1;
}

int sscanf(const char * s, const char * format, ...)
{
    va_list list;
    va_start(list, format);
	int results = 0;
	for(int i = 0; *s && i < strlen(format); i++)
	{
		switch (format[i])
		{
		case '%':
		{
		    switch (format[i + 1])
		    {
		    case 'c':
		    {
			break;
		    }
		    case 's':
		    {
		    	char* end = s;
		    	while(!isspace(*end))
				end++;

			char* str = va_arg(list, char*);

			//don't have strncpy
			uint32_t str_len = (uint32_t)(end - s);
			memcpy(str, s, str_len);
			str[str_len + 1] = '\0';
			s = end;
			
			i++;
			results++;
			break;
		    }
		    case 'd':
		    case 'i':
		    {
		    	char* end = s;
		    	while(!isspace(*end))
				end++;
			
			int32_t* integer = va_arg(list, int32_t*);
	
			char saved_char = *end;
			*end = '\0';
			*integer = atoi(s);
			*end = saved_char;

			i++;
			results++;
			break;
		    }
		    case 'x':
		    case 'X':
		    {
		    	char* end = s;
		    	while(!isspace(*end))
				end++;
			
			int32_t* integer = va_arg(list, int32_t*);
	
			char saved_char = *end;
			*end = '\0';
			*integer = atoi(s);
			*end = saved_char;

			i++;
			results++;
			break;
		    }
		    default:
		    {
			i++;
			break;
		    }
		    }
		    break;
		}
		default:
		{
		    break;
		}
		}
	}
    va_end(list);
	return results;
}

