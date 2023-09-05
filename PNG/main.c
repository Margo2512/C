#include "return_codes.h"

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#if defined(ZLIB)
#include <stdlib.h>
#include <zlib.h>
#define DECOMPRESS(BUFFER, SIZE, DATA, NEW_SIZE)                                                                       \
	(uncompress((Bytef *)DATA, (uLongf *)&NEW_SIZE, (const Bytef *)BUFFER, (uLong)SIZE) != Z_OK)
#else
#error("Don't have lib")
#define DECOMPRESS(BUFFER, SIZE, DATA, NEW_SIZE) false
#endif

unsigned char *buffer;
size_t size = 0;
size_t capacity = 1;
bool flag = 0;

struct png_ihdr
{
	size_t width;
	size_t height;
	unsigned char bit_depth;
	unsigned char color_type;
};

bool skip(FILE *file_png, size_t len)
{
	unsigned char garbage[1];
	for (size_t i = 0; i < len; i++)
	{
		if (fread(garbage, sizeof(unsigned char), 1, file_png) != 1)
		{
			return false;
		}
	}
	return true;
}

size_t char_to_int(unsigned char *buf)
{
	size_t result = 0;
	for (int i = 0; i < 4; i++)
	{
		result = (result << 8) | buf[i];
	}
	return result;
}

int read_ihdr_chunk(FILE *file_png, struct png_ihdr *ihdr)
{
	unsigned char ihdr_chunk[25];

	if (fread(ihdr_chunk, 1, 25, file_png) != 25)
	{
		return ERROR_DATA_INVALID;
	}

	if (memcmp(ihdr_chunk + 4, "IHDR", 4) != 0)
	{
		return ERROR_DATA_INVALID;
	}

	ihdr->width = char_to_int(ihdr_chunk + 8);
	ihdr->height = char_to_int(ihdr_chunk + 12);
	ihdr->bit_depth = *(ihdr_chunk + 16);
	if (ihdr->bit_depth != 8)
	{
		return ERROR_DATA_INVALID;
	}
	ihdr->color_type = *(ihdr_chunk + 17);
	if (ihdr->color_type != 0 && ihdr->color_type != 2 && ihdr->color_type != 3)
	{
		return ERROR_DATA_INVALID;
	}
	return 0;
}

int idat_chunk(FILE *file_png)
{
	unsigned char idat_chunk[8];
	unsigned char len_block_bytes[4];
	size_t len_block;
	buffer = (unsigned char *)malloc(sizeof(unsigned char) * 1);
	if (buffer == NULL)
	{
		return ERROR_OUT_OF_MEMORY;
	}
	while (1)
	{
		if (fread(idat_chunk, sizeof(unsigned char), 8, file_png) != 8)
		{
			return ERROR_DATA_INVALID;
		}

		memcpy(len_block_bytes, idat_chunk, 4);
		len_block = char_to_int(len_block_bytes);

		if (memcmp(idat_chunk + 4, "IDAT", 4) == 0)
		{
			while (size + len_block > capacity)
			{
				capacity *= 2;
			}

			unsigned char *tmp = realloc(buffer, sizeof(unsigned char) * capacity);
			if (tmp == NULL)
			{
				free(buffer);
				return ERROR_OUT_OF_MEMORY;
			}
			buffer = tmp;

			if (fread(buffer + size, sizeof(unsigned char), len_block, file_png) != len_block)
			{
				free(buffer);
				return ERROR_OUT_OF_MEMORY;
			}
			size += len_block;
		}
		else if (memcmp(idat_chunk + 4, "IEND", 4) == 0)
		{
			break;
		}
		else if (memcmp(idat_chunk + 4, "PLTE", 4) == 0)
		{
			skip(file_png, len_block);
			flag = 1;
		}
		else
		{
			skip(file_png, len_block);
		}
		skip(file_png, 4);
	}
	return 0;
}

int defilter(unsigned char *info, size_t *pos, struct png_ihdr *ihdr)
{
	int pixel_len = (int)(ihdr->color_type + 1);
	size_t line_length = ihdr->width * pixel_len + 1;
	unsigned char filter_type = info[*pos];
	*pos += line_length;

	switch (filter_type)
	{
	case 0:
		return 0;

	case 1:
		for (size_t i = 1; i < line_length; i++)
			info[i + *pos] += info[i + *pos - pixel_len];
		return 0;

	case 2:
		for (size_t i = 1; i < line_length; i++)
		{
			if (*pos - line_length >= 0)
				info[i + *pos] += info[i + *pos - line_length];
		}
		return 0;

	case 3:
		for (size_t i = 1; i < line_length; i++)
		{
			int tmp = 0;
			if (i >= pixel_len + 1)
				tmp += info[i + *pos - pixel_len];
			if (*pos - line_length >= 0)
				tmp += info[i + *pos - line_length];

			info[i + *pos] += (unsigned char)(tmp / 2);
		}
		return 0;

	case 4:
		for (size_t i = pixel_len + 1; i < line_length; i++)
		{
			size_t first = info[i + *pos - pixel_len];
			size_t second = info[i + *pos - line_length];
			size_t third = info[i + *pos - line_length - pixel_len];

			if (abs(second - third) <= abs(first - third) && abs(second - third) <= abs(first + second - 2 * third))
				info[i + *pos] += first;
			else if (abs(first - third) <= abs(first + second - 2 * third))
				info[i + *pos] += second;
			else
				info[i + *pos] += third;
		}
		return 0;

	default:
		return ERROR_UNKNOWN;
	}
}

int main(int argc, char *argv[])
{
	if (argc != 3)
	{
		perror("ERROR_PARAMETER_INVALID");
		return ERROR_PARAMETER_INVALID;
	}
	struct png_ihdr *ihdr = malloc(sizeof(struct png_ihdr));

	FILE *file_png = fopen(argv[1], "rb");

	unsigned char header_png[8];
	if (fread(header_png, 1, 8, file_png) != 8)
	{
		fclose(file_png);
		perror("ERROR_DATA_INVALID");
		return ERROR_DATA_INVALID;
	}

	if (memcmp(header_png, "\x89PNG\r\n\x1a\n", 8) != 0)
	{
		fclose(file_png);
		perror("ERROR_DATA_INVALID");
		return ERROR_DATA_INVALID;
	}
	read_ihdr_chunk(file_png, ihdr);
	idat_chunk(file_png);
	fclose(file_png);

	size_t new_size = (ihdr->width * ihdr->height * (ihdr->color_type + 1) + ihdr->height);
	unsigned char *info = malloc(sizeof(unsigned char) * new_size);

	if (info == NULL)
	{
		free(buffer);
		perror("ERROR_OUT_OF_MEMORY");
		return ERROR_OUT_OF_MEMORY;
	}

	if (DECOMPRESS(buffer, size, info, new_size))
	{
		free(info);
		free(buffer);
		perror("ERROR_DATA_INVALID");
		return ERROR_DATA_INVALID;
	}

	free(buffer);

	size_t pos = 0;
	size_t result = 0;
	while (result == 0 && pos < new_size)
	{
		result = defilter(info, &pos, ihdr);
	}

	if (result != 0)
	{
		free(info);
		perror("ERROR_UNKNOWN");
		return ERROR_UNKNOWN;
	}

	FILE *file_pnm = fopen(argv[2], "wb");
	if (file_pnm == NULL)
	{
		fclose(file_png);
		perror("ERROR_CANNOT_OPEN_FILE");
		return ERROR_CANNOT_OPEN_FILE;
	}
	if ((ihdr->color_type == 0 || ihdr->color_type == 3) && !flag)
	{
		fprintf(file_pnm, "P5\n");
	}
	else if (ihdr->color_type == 2 || ihdr->color_type == 3 && flag)
	{
		fprintf(file_pnm, "P6\n");
	}
	else
	{
		fclose(file_pnm);
		free(info);
		remove(argv[2]);
		perror("ERROR_UNKNOWN");
		return ERROR_UNKNOWN;
	}

	fprintf(file_pnm, "%zu %zu\n255\n", ihdr->width, ihdr->height);
	size_t line_len = ihdr->width * (ihdr->color_type + 1);
	for (size_t i = 1; i < new_size; i += line_len + 1)
	{
		if (fwrite(&info[i], sizeof(unsigned char), line_len, file_pnm) != line_len)
		{
			fclose(file_pnm);
			free(info);
			remove(argv[2]);
			perror("ERROR_DATA_INVALID");
			return ERROR_DATA_INVALID;
		}
	}
	fclose(file_pnm);
	return 0;
}