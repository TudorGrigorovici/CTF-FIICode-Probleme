#include <windows.h>
#include <stdio.h>
using namespace std;



int main()
{
	HANDLE h = CreateFile("werewolf_low_points.txt", GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (h == INVALID_HANDLE_VALUE)
		return -1;
	BYTE buffer[1024];
	DWORD read;
	if (!ReadFile(h, buffer, 1024, &read, NULL))
		return -2;

	switch (read % 10)
	{
	case 0:
		puts("appcompat\n");
		break;
	case 1:
		puts("apppatch\n");
		break;
	case 2:
		puts("AppReadiness\n");
		break;
	case 3:
		puts("DigitalLocker\n");
		break;
	case 4:
		puts("ELAMBKUP\n");
		break;
	case 5:
		puts("Fonts\n");
		break;
	case 6:
		puts("GameBarPresenceWriter\n");
		break;
	case 7:
		puts("Resources\n");
		break;
	case 8:
		puts("schemas\n");
		break;
	case 9:
		puts("Web\n");
		break;
	}

	return 0;
}