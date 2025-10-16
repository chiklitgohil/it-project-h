#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "crud.h"

void addPatient() {
    FILE *fp = fopen(PATIENT_FILE, "ab+");
    if (!fp) return;

    Patient p;
    printf("Enter ID: "); scanf("%d", &p.id);
    printf("Name: "); scanf("%s", p.name);
    printf("Age: "); scanf("%d", &p.age);
    printf("Gender: "); scanf("%s", p.gender);
    printf("Phone: "); scanf("%s", p.phone);
    printf("Disease: "); scanf("%s", p.disease);

    fwrite(&p, sizeof(Patient), 1, fp);
    fclose(fp);
}

void viewPatients() {
    FILE *fp = fopen(PATIENT_FILE, "rb");
    if (!fp) return;

    Patient p;
    printf("\n%-5s %-15s %-5s %-10s %-15s %-20s\n",
           "ID", "Name", "Age", "Gender", "Phone", "Disease");
    while (fread(&p, sizeof(Patient), 1, fp))
        printf("%-5d %-15s %-5d %-10s %-15s %-20s\n",
               p.id, p.name, p.age, p.gender, p.phone, p.disease);

    fclose(fp);
}
