/*
 * common.c - Common utility functions
 */

#include "common.h"

void clearStdin(void)
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
        ;
}

void getInput(char *buffer, int size)
{
    if (!buffer || size <= 0)
        return;
    if (fgets(buffer, size, stdin) == NULL)
    {
        buffer[0] = '\0';
        return;
    }
    buffer[strcspn(buffer, "\n")] = '\0';
}

int getNextId(const char *filename, size_t struct_size)
{
    FILE *fp = fopen(filename, "rb");
    if (!fp)
        return 1;

    int max_id = 0;
    void *buf = malloc(struct_size);
    if (!buf)
    {
        fclose(fp);
        return 1;
    }

    while (fread(buf, struct_size, 1, fp) == 1)
    {
        int rec_id;
        memcpy(&rec_id, buf, sizeof(int));
        if (rec_id > max_id)
            max_id = rec_id;
    }

    free(buf);
    fclose(fp);
    return max_id + 1;
}
