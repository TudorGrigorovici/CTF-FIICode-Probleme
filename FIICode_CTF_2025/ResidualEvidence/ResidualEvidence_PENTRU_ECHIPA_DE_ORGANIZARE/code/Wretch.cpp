#include <Windows.h>
#include <stdio.h>

int main()
{
	HANDLE h = CreateFile("flag.txt", GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (h == INVALID_HANDLE_VALUE)
		return -1;
	BYTE buffer[1024];
	BYTE result[1024];
	
	memset(buffer, 0, 1024);
	memset(result, 0, 1024);
	DWORD readBytes;
	if (!ReadFile(h, buffer, 1024, &readBytes, NULL))
		return -2;
	if (readBytes < 1)
		return -3;

	int key1 = 0;
	int key2 = 0;
	int key3 = 0;


	if (key1 != 0 && key2 != 0)
	{
		if (key3 == 0)
		{
			printf("key3: count all visible fingers of the provided monsters!\n");
		}

		for (int i = 0; i < readBytes; i++)
		{
			result[i] = ((buffer[i] ^ key3) - key2) ^ key1 ;
		}
		HANDLE oh = CreateFile("wretch.sys", GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		WriteFile(oh, result, readBytes, NULL, NULL);
	}
	else
	{
		printf("I need the first and the second key!\n");
		return -4;
	}
	return 0;

}