/*
 * doctors.c - Doctor CRUD Operations
 * Handles create, read, update, and delete operations for doctor records
 */

#include "common.h"
#include "doctors.h"

/* Case-insensitive string compare */
static int ci_cmp_str(const char *a, const char *b)
{
    while (*a && *b)
    {
        unsigned char ca = (unsigned char)tolower((unsigned char)*a);
        unsigned char cb = (unsigned char)tolower((unsigned char)*b);
        if (ca != cb)
            return ca - cb;
        a++;
        b++;
    }
    return (unsigned char)tolower((unsigned char)*a) - (unsigned char)tolower((unsigned char)*b);
}

/* Doctor comparator: sort by name (case-insensitive) */
static int cmpDoctorByName(const void *x, const void *y)
{
    const Doctor *a = x, *b = y;
    return ci_cmp_str(a->name, b->name);
}

void addDoctor()
{
    Doctor d;
    printf("Enter ID: ");
    scanf("%d", &d.id);
    clearStdin();

    if (doctorExists(d.id))
    {
        printf("Doctor with ID %d already exists.\n", d.id);
        return;
    }

    printf("Name: ");
    getInput(d.name, sizeof(d.name));
    printf("Specialization: ");
    getInput(d.specialization, sizeof(d.specialization));

    /* Phone: require at least 7 digits */
    while (1)
    {
        printf("Phone: ");
        getInput(d.phone, sizeof(d.phone));
        int digits = 0;
        for (size_t i = 0; i < strlen(d.phone); ++i)
            if (isdigit((unsigned char)d.phone[i]))
                digits++;
        if (digits < 7)
        {
            printf("Phone must contain at least 7 digits.\n");
            continue;
        }
        break;
    }

    FILE *fp = fopen(DOCTOR_FILE, "ab+");
    if (!fp)
        return;
    fwrite(&d, sizeof(Doctor), 1, fp);
    fclose(fp);
    printf("Doctor added successfully.\n");
}

void viewDoctors()
{
    FILE *fp = fopen(DOCTOR_FILE, "rb");
    if (!fp)
        return;

    Doctor *arr = NULL;
    size_t n = 0, cap = 0;
    Doctor d;
    while (fread(&d, sizeof(Doctor), 1, fp))
    {
        if (n == cap)
        {
            cap = cap ? cap * 2 : 16;
            arr = realloc(arr, cap * sizeof(Doctor));
        }
        arr[n++] = d;
    }
    fclose(fp);
    if (n == 0)
        return;

    qsort(arr, n, sizeof(Doctor), cmpDoctorByName);

    printf("\n%-5s %-15s %-20s %-15s\n",
           "ID", "Name", "Specialization", "Phone");

    for (size_t i = 0; i < n; ++i)
        printf("%-5d %-15s %-20s %-15s\n",
               arr[i].id, arr[i].name, arr[i].specialization, arr[i].phone);

    free(arr);
}

void updateDoctor()
{
    FILE *fp = fopen(DOCTOR_FILE, "rb+");
    if (!fp)
        return;

    int id, found = 0;
    printf("Enter Doctor ID to update: ");
    scanf("%d", &id);
    clearStdin();

    Doctor d;
    while (fread(&d, sizeof(Doctor), 1, fp))
    {
        if (d.id == id)
        {
            char buf[128];
            printf("Enter new Name (press Enter to keep '%s'): ", d.name);
            getInput(buf, sizeof(buf));
            if (buf[0] != '\0')
                strncpy(d.name, buf, sizeof(d.name));

            printf("Enter new Specialization (press Enter to keep '%s'): ", d.specialization);
            getInput(buf, sizeof(buf));
            if (buf[0] != '\0')
                strncpy(d.specialization, buf, sizeof(d.specialization));

            while (1)
            {
                printf("Enter new Phone (press Enter to keep '%s'): ", d.phone);
                getInput(buf, sizeof(buf));
                if (buf[0] == '\0')
                    break;
                int digits = 0;
                for (size_t i = 0; i < strlen(buf); ++i)
                    if (isdigit((unsigned char)buf[i]))
                        digits++;
                if (digits < 7)
                {
                    printf("Phone must contain at least 7 digits.\n");
                    continue;
                }
                strncpy(d.phone, buf, sizeof(d.phone));
                break;
            }

            if (fseek(fp, -(long)sizeof(Doctor), SEEK_CUR) != 0)
                perror("fseek");
            fwrite(&d, sizeof(Doctor), 1, fp);
            found = 1;
            printf("Doctor updated.\n");
            break;
        }
    }

    fclose(fp);
    if (!found)
        printf("Record not found.\n");
}

void deleteDoctor()
{
    FILE *fp = fopen(DOCTOR_FILE, "rb");
    FILE *temp = fopen("data/temp.dat", "wb");
    if (!fp || !temp)
        return;

    int id, found = 0;
    printf("Enter Doctor ID to delete: ");
    scanf("%d", &id);
    clearStdin();

    Doctor d;
    while (fread(&d, sizeof(Doctor), 1, fp))
    {
        if (d.id != id)
            fwrite(&d, sizeof(Doctor), 1, temp);
        else
            found = 1;
    }

    fclose(fp);
    fclose(temp);

    remove(DOCTOR_FILE);
    rename("data/temp.dat", DOCTOR_FILE);

    if (found)
        printf("Doctor deleted.\n");
    else
        printf("Record not found.\n");
}

int doctorExists(int doctor_id)
{
    FILE *fp = fopen(DOCTOR_FILE, "rb");
    if (!fp)
        return 0;

    Doctor d;
    while (fread(&d, sizeof(Doctor), 1, fp))
    {
        if (d.id == doctor_id)
        {
            fclose(fp);
            return 1;
        }
    }
    fclose(fp);
    return 0;
}
