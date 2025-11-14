/*
 * patients.c - Patient CRUD Operations
 * Handles create, read, update, and delete operations for patient records
 */

#include "common.h"
#include "patients.h"

/* Case-insensitive string compare (returns <0,0,>0) */
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

/* Patient comparator: sort by name (case-insensitive) */
static int cmpPatientByName(const void *x, const void *y)
{
    const Patient *a = x, *b = y;
    return ci_cmp_str(a->name, b->name);
}

void addPatient()
{
    FILE *fp = fopen(PATIENT_FILE, "ab+");
    if (!fp)
    {
        perror("Error opening patient file");
        return;
    }

    Patient p;
    printf("Enter ID: ");
    scanf("%d", &p.id);
    clearStdin();

    printf("Name: ");
    getInput(p.name, sizeof(p.name));

    /* Age validation: must be integer 1..120 */
    while (1)
    {
        char agebuf[16];
        printf("Age: ");
        getInput(agebuf, sizeof(agebuf));
        if (agebuf[0] == '\0')
        {
            printf("Age cannot be empty.\n");
            continue;
        }
        int age;
        if (sscanf(agebuf, "%d", &age) != 1 || age <= 0 || age > 120)
        {
            printf("Invalid age. Enter a number between 1 and 120.\n");
            continue;
        }
        p.age = age;
        break;
    }

    /* Gender: basic validation (alphabetic) */
    while (1)
    {
        printf("Gender: ");
        getInput(p.gender, sizeof(p.gender));
        if (strlen(p.gender) == 0)
        {
            printf("Gender cannot be empty.\n");
            continue;
        }
        int ok = 0;
        for (size_t i = 0; i < strlen(p.gender); ++i)
            if (isalpha((unsigned char)p.gender[i]) || p.gender[i] == ' ')
                ok = 1;
        if (!ok)
        {
            printf("Invalid gender. Enter Male/Female/Other or similar.\n");
            continue;
        }
        break;
    }

    /* Phone: require at least 7 digits */
    while (1)
    {
        printf("Phone: ");
        getInput(p.phone, sizeof(p.phone));
        int digits = 0;
        for (size_t i = 0; i < strlen(p.phone); ++i)
            if (isdigit((unsigned char)p.phone[i]))
                digits++;
        if (digits < 7)
        {
            printf("Phone must contain at least 7 digits.\n");
            continue;
        }
        break;
    }

    fwrite(&p, sizeof(Patient), 1, fp);
    fclose(fp);
    printf("Patient added successfully.\n");
}

void viewPatients()
{
    FILE *fp = fopen(PATIENT_FILE, "rb");
    if (!fp)
        return;

    Patient *arr = NULL;
    size_t n = 0, cap = 0;
    Patient p;
    while (fread(&p, sizeof(Patient), 1, fp))
    {
        if (n == cap)
        {
            cap = cap ? cap * 2 : 16;
            arr = realloc(arr, cap * sizeof(Patient));
        }
        arr[n++] = p;
    }
    fclose(fp);

    if (n == 0)
        return;

    qsort(arr, n, sizeof(Patient), cmpPatientByName);

    printf("\n%-5s %-20s %-5s %-10s %-15s\n",
           "ID", "Name", "Age", "Gender", "Phone");

    for (size_t i = 0; i < n; ++i)
    {
        printf("%-5d %-20s %-5d %-10s %-15s\n",
               arr[i].id, arr[i].name, arr[i].age, arr[i].gender, arr[i].phone);
    }
    free(arr);
}

void updatePatient()
{
    FILE *fp = fopen(PATIENT_FILE, "rb+");
    if (!fp)
        return;

    int id, found = 0;
    printf("Enter Patient ID to update: ");
    scanf("%d", &id);
    clearStdin();

    Patient p;
    while (fread(&p, sizeof(Patient), 1, fp))
    {
        if (p.id == id)
        {
            char buf[128];
            printf("Enter new Name (press Enter to keep '%s'): ", p.name);
            getInput(buf, sizeof(buf));
            if (buf[0] != '\0')
                strncpy(p.name, buf, sizeof(p.name));

            while (1)
            {
                printf("Enter new Age (press Enter to keep '%d'): ", p.age);
                getInput(buf, sizeof(buf));
                if (buf[0] == '\0')
                    break;
                int newAge;
                if (sscanf(buf, "%d", &newAge) != 1 || newAge <= 0 || newAge > 120)
                {
                    printf("Invalid age. Enter a number between 1 and 120.\n");
                    continue;
                }
                p.age = newAge;
                break;
            }

            while (1)
            {
                printf("Enter new Gender (press Enter to keep '%s'): ", p.gender);
                getInput(buf, sizeof(buf));
                if (buf[0] == '\0')
                    break;
                int ok = 0;
                for (size_t i = 0; i < strlen(buf); ++i)
                    if (isalpha((unsigned char)buf[i]) || buf[i] == ' ')
                        ok = 1;
                if (!ok)
                {
                    printf("Invalid gender. Enter Male/Female/Other or similar.\n");
                    continue;
                }
                strncpy(p.gender, buf, sizeof(p.gender));
                break;
            }

            while (1)
            {
                printf("Enter new Phone (press Enter to keep '%s'): ", p.phone);
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
                strncpy(p.phone, buf, sizeof(p.phone));
                break;
            }

            if (fseek(fp, -(long)sizeof(Patient), SEEK_CUR) != 0)
                perror("fseek");
            fwrite(&p, sizeof(Patient), 1, fp);
            found = 1;
            printf("Patient updated.\n");
            break;
        }
    }

    fclose(fp);
    if (!found)
        printf("Record not found.\n");
}

void deletePatient()
{
    FILE *fp = fopen(PATIENT_FILE, "rb");
    FILE *temp = fopen("data/temp.dat", "wb");
    if (!fp || !temp)
        return;

    int id, found = 0;
    printf("Enter Patient ID to delete: ");
    scanf("%d", &id);
    clearStdin();

    Patient p;
    while (fread(&p, sizeof(Patient), 1, fp))
    {
        if (p.id != id)
            fwrite(&p, sizeof(Patient), 1, temp);
        else
            found = 1;
    }

    fclose(fp);
    fclose(temp);

    remove(PATIENT_FILE);
    rename("data/temp.dat", PATIENT_FILE);

    if (found)
        printf("Patient deleted.\n");
    else
        printf("Record not found.\n");
}

int patientExists(int patient_id)
{
    FILE *fp = fopen(PATIENT_FILE, "rb");
    if (!fp)
        return 0;

    Patient p;
    while (fread(&p, sizeof(Patient), 1, fp))
    {
        if (p.id == patient_id)
        {
            fclose(fp);
            return 1;
        }
    }
    fclose(fp);
    return 0;
}
