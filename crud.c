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

void updatePatient() {
    FILE *fp = fopen(PATIENT_FILE, "rb+");
    if (!fp) return;

    int id, found = 0;
    printf("Enter Patient ID to update: ");
    scanf("%d", &id);

    Patient p;
    while (fread(&p, sizeof(Patient), 1, fp)) {
        if (p.id == id) {
            printf("Enter new Name: "); scanf("%s", p.name);
            printf("Enter new Age: "); scanf("%d", &p.age);
            printf("Enter new Gender: "); scanf("%s", p.gender);
            printf("Enter new Phone: "); scanf("%s", p.phone);
            printf("Enter new Disease: "); scanf("%s", p.disease);

            fseek(fp, -sizeof(Patient), SEEK_CUR);
            fwrite(&p, sizeof(Patient), 1, fp);
            found = 1;
            break;
        }
    }

    fclose(fp);
    if (!found) printf("Record not found.\n");
}

void deletePatient() {
    FILE *fp = fopen(PATIENT_FILE, "rb");
    FILE *temp = fopen("data/temp.dat", "wb");
    if (!fp || !temp) return;

    int id, found = 0;
    printf("Enter Patient ID to delete: ");
    scanf("%d", &id);

    Patient p;
    while (fread(&p, sizeof(Patient), 1, fp)) {
        if (p.id != id)
            fwrite(&p, sizeof(Patient), 1, temp);
        else
            found = 1;
    }

    fclose(fp);
    fclose(temp);
    remove(PATIENT_FILE);
    rename("data/temp.dat", PATIENT_FILE);

    if (!found) printf("Record not found.\n");
}

void addDoctor() {
    FILE *fp = fopen(DOCTOR_FILE, "ab+");
    if (!fp) return;

    Doctor d;
    printf("Enter ID: "); scanf("%d", &d.id);
    printf("Name: "); scanf("%s", d.name);
    printf("Specialization: "); scanf("%s", d.specialization);
    printf("Phone: "); scanf("%s", d.phone);

    fwrite(&d, sizeof(Doctor), 1, fp);
    fclose(fp);
}

void viewDoctors() {
    FILE *fp = fopen(DOCTOR_FILE, "rb");
    if (!fp) return;

    Doctor d;
    printf("\n%-5s %-15s %-20s %-15s\n",
           "ID", "Name", "Specialization", "Phone");
    while (fread(&d, sizeof(Doctor), 1, fp))
        printf("%-5d %-15s %-20s %-15s\n",
               d.id, d.name, d.specialization, d.phone);

    fclose(fp);
}

void updateDoctor() {
    FILE *fp = fopen(DOCTOR_FILE, "rb+");
    if (!fp) return;

    int id, found = 0;
    printf("Enter Doctor ID to update: ");
    scanf("%d", &id);

    Doctor d;
    while (fread(&d, sizeof(Doctor), 1, fp)) {
        if (d.id == id) {
            printf("Enter new Name: "); scanf("%s", d.name);
            printf("Enter new Specialization: "); scanf("%s", d.specialization);
            printf("Enter new Phone: "); scanf("%s", d.phone);

            fseek(fp, -sizeof(Doctor), SEEK_CUR);
            fwrite(&d, sizeof(Doctor), 1, fp);
            found = 1;
            break;
        }
    }

    fclose(fp);
    if (!found) printf("Record not found.\n");
}

void deleteDoctor() {
    FILE *fp = fopen(DOCTOR_FILE, "rb");
    FILE *temp = fopen("data/temp.dat", "wb");
    if (!fp || !temp) return;

    int id, found = 0;
    printf("Enter Doctor ID to delete: ");
    scanf("%d", &id);

    Doctor d;
    while (fread(&d, sizeof(Doctor), 1, fp)) {
        if (d.id != id)
            fwrite(&d, sizeof(Doctor), 1, temp);
        else
            found = 1;
    }

    fclose(fp);
    fclose(temp);
    remove(DOCTOR_FILE);
    rename("data/temp.dat", DOCTOR_FILE);

    if (!found) printf("Record not found.\n");
}


int main() {
    
    int choice;
    while (1) {
        printf("\n1. Add Patient\n2. View Patients\n3. Update Patient\n4. Delete Patient\n");
        printf("5. Add Doctor\n6. View Doctors\n7. Update Doctor\n8. Delete Doctor\n9. Exit\n");
        printf("Enter choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: addPatient(); break;
            case 2: viewPatients(); break;
            case 3: updatePatient(); break;
            case 4: deletePatient(); break;
            case 5: addDoctor(); break;
            case 6: viewDoctors(); break;
            case 7: updateDoctor(); break;
            case 8: deleteDoctor(); break;
            case 9: exit(0);
            default: printf("Invalid choice.\n");
        }
    }
}
