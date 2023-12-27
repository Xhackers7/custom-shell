#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{   
    char *inp_buffer = NULL;
    size_t size=0;
    while (1)
    {
        printf("dash> ");
        getline(&inp_buffer, &size, stdin);
        printf("%s", inp_buffer);
        if (!strcmp(inp_buffer, "exit\n")) exit(0);
    }
    
    return 0;
}