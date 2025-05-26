#define __USE_GNU
#include <stdio.h>

int secret_function(void) {
    asm("jmp %esp");
}

void disable_buffering() {
    setvbuf(stdin, NULL, _IONBF, 0);
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);
}

void vuln(void)
{
    char buffer[130];

    printf("Buffer starts here 0x%x\n:",(void*)buffer);
    puts("Try and get the flag\n");
    gets(buffer);
    return;
}

int main(void)
{
    disable_buffering();
    puts("My cup runneth over\nLike blood from a stone...\n");
    vuln();
    return 0;
}