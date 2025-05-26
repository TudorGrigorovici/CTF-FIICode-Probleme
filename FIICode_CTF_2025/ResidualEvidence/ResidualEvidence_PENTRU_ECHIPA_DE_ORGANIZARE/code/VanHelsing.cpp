#include <stdio.h>
#include <windows.h>

int main()
{
    DWORD dwValue = 0, cbData = sizeof(dwValue);
    LONG status = RegGetValue(HKEY_LOCAL_MACHINE,
        "Software\\VanHelsing",
        "Inga",
        RRF_RT_REG_DWORD,
        nullptr,
        &dwValue,
        &cbData
    );

    if (dwValue != 123)
    {
        printf("Please make sure that you understood the assignment!\n");
        return -1;
    }

    printf("I'm sorry to inform you, but the key is not in this folder!");
    return 0;
}