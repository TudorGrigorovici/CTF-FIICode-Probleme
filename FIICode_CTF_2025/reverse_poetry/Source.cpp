#include <windows.h>
#include <stdio.h>

typedef void(*my_encryptor)(BYTE*,DWORD,DWORD*);

void Decoder1(BYTE *data, DWORD size)
{
	DWORD key = 0;
	DWORD key1 = 0xFA29516E;
	int i, j;

	key = *(DWORD*)(data + size - 4);
	*(DWORD*)(data + size - 4) = key ^ key1;

	for (i = 0; i < size - 4; i++)
	{
		DWORD x = data[i];
		data[i] = 0;
		while ((data[i] * (key + 'YO') + 'DUDE') % 256 != x)
			data[i]++;
		key = key * 256 + key / 16777216;
	}
}

void Encoder1(BYTE *data, DWORD size, DWORD *size2)
{
	DWORD key = 0;
	DWORD key1 = 0xFA29516E;
	int i, j;

	for (i = 0; i < 4; i++)
		key = (key << 8) | data[size - 1 - i];

	key ^= key1;
	key1 = key;

	for (i = 0; i < size - 4; i++)
	{
		data[i] = (data[i] * (key + 'YO') + 'DUDE') % 256;
		key = key * 256 + key / 16777216;
	}

	for (i, j = 0; j < 4; i++, j++)
		data[i] = ((BYTE*)&key1)[j];

	*size2 = size;
}

void Encoder2(BYTE *data, DWORD size, DWORD *size2)
{
	WORD key;
	key = (data[0] << 8) ^ data[size - 1] ^ 'BAU';
	int i;

	for (i = 1; i < size - 2; i += 2)
		((WORD*)(data + i))[0] ^= key;

	*size2 = size;
}

void Encoder3(BYTE *data, DWORD size, DWORD *size2)
{
	DWORD indexes[4];
	int parts = size / 4;
	BYTE cuernt;
	DWORD pos = 0;
	DWORD key;
	
	indexes[0] = size % parts;
	indexes[1] = size % (parts + 2);
	indexes[2] = size % (parts + 7);
	indexes[3] = size % (parts + 11);

	key = indexes[0] | (indexes[1] << 8) | (indexes[2] << 16) | (indexes[3] << 24);

	while (pos < size)
	{
		if (pos == indexes[0] ||
			pos == indexes[1] ||
			pos == indexes[2] ||
			pos == indexes[3])
		{
			pos++;
			continue;
		}
		data[pos] ^= ((key % 15) * 16) + (key / 16);
		key = (key * 16) + (key / 268435456);
		pos++;
	}

	*size2 = size;
}

void Encoder4(BYTE *data, DWORD size, DWORD *size2)
{
	BYTE *nou = (BYTE*)VirtualAlloc(0, size * 2, MEM_COMMIT, PAGE_READWRITE);
	int i;
	char alfabet[] = "q1w2e3r4t5y6u7i8o9p0-=_+{}[]|asdfghjkl;':zxcvbnm,./<>?";
	char x, y;

	for (i = 0; i < size; i++)
	{
		x = data[i] / strlen(alfabet);
		y = data[i] % strlen(alfabet);
		nou[i * 2] = alfabet[x];
		nou[i * 2 + 1] = alfabet[y];
	}

	memcpy(data, nou, size * 2);
	VirtualFree(nou, 0, MEM_RELEASE);

	*size2 = size * 2;
}

void Encrypt(BYTE *data, DWORD size, DWORD *size2)
{
	int i;
	BYTE *chunks[4];
	DWORD pos, old_pos, cumulat;
	DWORD sizes[4];
	my_encryptor encs[4] = { Encoder1, Encoder2, Encoder3, Encoder4 };
	BYTE *uniform;

	pos = 0;
	i = 0;
	old_pos = 0;
	while (pos < size)
	{
		if (*(DWORD*)(data + pos) == 0x0A0D0A0D)
		{
			pos += 4;
			sizes[i] = pos - old_pos;
			chunks[i] = (BYTE*)VirtualAlloc(0, sizes[i], MEM_COMMIT, PAGE_READWRITE);
			memcpy(chunks[i], data + old_pos, sizes[i]);
			i++;
			old_pos = pos;
		}
		pos++;
	}
	sizes[i] = pos - old_pos;
	chunks[i] = (BYTE*)VirtualAlloc(0, sizes[i], MEM_COMMIT, PAGE_READWRITE);
	memcpy(chunks[i], data + old_pos, sizes[i]);

	for (i = 0; i < 4; i++)
		encs[i](chunks[i], sizes[i], sizes + i);

	uniform = (BYTE*)VirtualAlloc(0, sizes[0] + sizes[1] + sizes[2] + sizes[3], MEM_COMMIT, PAGE_READWRITE);
	cumulat = 0;
	for (i = 0; i < 4; i++)
	{		
		memcpy(uniform + cumulat, chunks[i], sizes[i]);
		cumulat += sizes[i];
	}

	memcpy(data, uniform, cumulat);
	VirtualFree(uniform, 0, MEM_RELEASE);

	*size2 = cumulat;
}

void Decrypt(BYTE *data, DWORD size)
{

}

BYTE *LoadFile(char *fname, DWORD *size)
{
	HANDLE f;
	DWORD read;
	BYTE *buffer = 0;

	f = CreateFile(fname, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (f == INVALID_HANDLE_VALUE)
		return 0;
	*size = GetFileSize(f, 0);
	buffer = (BYTE*)VirtualAlloc(0, *size, MEM_COMMIT, PAGE_READWRITE);
	if (!ReadFile(f, buffer, *size, &read, NULL))
	{
		CloseHandle(f);
		VirtualFree(buffer, 0, MEM_RELEASE);
	}
	CloseHandle(f);
	return buffer;
}

BYTE *WriteToFile(char *fname, BYTE *buffer, DWORD size)
{
	HANDLE f;
	DWORD write;

	f = CreateFile(fname, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (f == INVALID_HANDLE_VALUE)
		return 0;
	WriteFile(f, buffer, size, &write, NULL);
	CloseHandle(f);
}

int main()
{
	BYTE *my_data;
	DWORD size, size2;

	my_data = LoadFile("input.txt", &size);
	if (my_data)
	{
		Encrypt(my_data, size, &size2);
		WriteToFile("result.bin", my_data, size2);
		Decoder1(my_data, 0x83);
		VirtualFree(my_data, 0, MEM_RELEASE);
	}

	return 0;
}