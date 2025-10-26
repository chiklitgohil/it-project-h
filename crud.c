#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "crud.h"

// Helper function to safely read string input and clear the buffer
void getInput(char *buffer, int size)
{
    fgets(buffer, size, stdin);
    buffer[strcspn(buffer, "\n")] = 0; // Remove trailing newline
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
    char buffer[100]; // Buffer for reading string inputs

    printf("Enter ID: ");
    scanf("%d", &p.id);
    while (getchar() != '\n')
        ; // Clear input buffer

    printf("Name: ");
    scanf("%s", p.name);
    getInput(p.name, sizeof(p.name));
    printf("Age: ");
    scanf("%d", &p.age);
    while (getchar() != '\n')
        ; // Clear input buffer
    printf("Gender: ");
    getInput(p.gender, sizeof(p.gender));
    printf("Phone: ");
    getInput(p.phone, sizeof(p.phone));
    printf("Disease: ");
    getInput(p.disease, sizeof(p.disease));

    fwrite(&p, sizeof(Patient), 1, fp);
    fclose(fp);
}

void viewPatients()
{
    FILE *fp = fopen(PATIENT_FILE, "rb");
    if (!fp)
        return;

    Patient p;
    printf("\n%-5s %-15s %-5s %-10s %-15s %-20s\n",
           "ID", "Name", "Age", "Gender", "Phone", "Disease");
    while (fread(&p, sizeof(Patient), 1, fp))
        printf("%-5d %-15s %-5d %-10s %-15s %-20s\n",
               p.id, p.name, p.age, p.gender, p.phone, p.disease);

    fclose(fp);
}

void updatePatient()
{
    FILE *fp = fopen(PATIENT_FILE, "rb+");
    if (!fp)
        return;

    int id, found = 0;
    printf("Enter Patient ID to update: ");
    scanf("%d", &id);

    Patient p;
    while (fread(&p, sizeof(Patient), 1, fp))
    {
        if (p.id == id)
        {
            printf("Enter new Name: ");
            scanf("%s", p.name);
            printf("Enter new Age: ");
            scanf("%d", &p.age);
            printf("Enter new Gender: ");
            scanf("%s", p.gender);
            printf("Enter new Phone: ");
            scanf("%s", p.phone);
            printf("Enter new Disease: ");
            scanf("%s", p.disease);

            fseek(fp, -sizeof(Patient), SEEK_CUR);
            fwrite(&p, sizeof(Patient), 1, fp);
            found = 1;
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

    if (!found)
        printf("Record not found.\n");
}

void addDoctor()
{
    FILE *fp = fopen(DOCTOR_FILE, "ab+");
    if (!fp)
        return;

    Doctor d;
    printf("Enter ID: ");
    scanf("%d", &d.id);
    printf("Name: ");
    scanf("%s", d.name);
    printf("Specialization: ");
    scanf("%s", d.specialization);
    printf("Phone: ");
    scanf("%s", d.phone);

    fwrite(&d, sizeof(Doctor), 1, fp);
    fclose(fp);
}

void viewDoctors()
{
    FILE *fp = fopen(DOCTOR_FILE, "rb");
    if (!fp)
        return;

    Doctor d;
    printf("\n%-5s %-15s %-20s %-15s\n",
           "ID", "Name", "Specialization", "Phone");
    while (fread(&d, sizeof(Doctor), 1, fp))
        printf("%-5d %-15s %-20s %-15s\n",
               d.id, d.name, d.specialization, d.phone);

    fclose(fp);
}

void updateDoctor()
{
    FILE *fp = fopen(DOCTOR_FILE, "rb+");
    if (!fp)
        return;

    int id, found = 0;
    printf("Enter Doctor ID to update: ");
    scanf("%d", &id);

    Doctor d;
    while (fread(&d, sizeof(Doctor), 1, fp))
    {
        if (d.id == id)
        {
            printf("Enter new Name: ");
            scanf("%s", d.name);
            printf("Enter new Specialization: ");
            scanf("%s", d.specialization);
            printf("Enter new Phone: ");
            scanf("%s", d.phone);

            fseek(fp, -sizeof(Doctor), SEEK_CUR);
            fwrite(&d, sizeof(Doctor), 1, fp);
            found = 1;
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

    if (!found)
        printf("Record not found.\n");
}

// --- Appointment Management Functions (Skeletons) ---
void scheduleAppointment()
{
    printf("\n--- Schedule Appointment ---\n");
    // TODO: Check if patient and doctor IDs exist.
    // TODO: Get appointment details (date, reason).
    // TODO: Check for scheduling conflicts.
    // TODO: Save the new appointment to APPOINTMENT_FILE.
    printf("Function not yet implemented.\n");
}

void viewAppointments()
{
    printf("\n--- View Appointments ---\n");
    // TODO: Open APPOINTMENT_FILE and display all records.
    // TODO (Advanced): Add filtering by patient ID or doctor ID.
    printf("Function not yet implemented.\n");
}

void cancelAppointment()
{
    printf("\n--- Cancel Appointment ---\n");
    // TODO: Ask for appointment ID.
    // TODO: Find the appointment and change its status to "Cancelled".
    // This could be an update or a delete/re-add operation.
    printf("Function not yet implemented.\n");
}

// --- Medical Record Management Functions (Skeletons) ---
void addMedicalRecord()
{
    printf("\n--- Add Medical Record ---\n");
    // TODO: Ask for an appointment ID.
    // TODO: Verify the appointment exists.
    // TODO: Get diagnosis and prescription details.
    // TODO: Save the new medical record.
    printf("Function not yet implemented.\n");
}

void viewPatientMedicalHistory()
{
    printf("\n--- View Patient Medical History ---\n");
    // TODO: Ask for a patient ID.
    // TODO: Find all appointments for that patient.
    // TODO: For each appointment, find and display the corresponding medical record.
    printf("Function not yet implemented.\n");
}

int main()
{
    int choice;
    while (1)
    {
        printf("\n--- Hospital Management System ---\n");
        printf("\n-- Patient Management --\n");
        printf("1. Add Patient\n2. View Patients\n3. Update Patient\n4. Delete Patient\n");
        printf("\n-- Doctor Management --\n");
        printf("5. Add Doctor\n6. View Doctors\n7. Update Doctor\n8. Delete Doctor\n");
        printf("\n-- Appointments & Records --\n");
        printf("9. Schedule Appointment\n10. View Appointments\n11. Cancel Appointment\n");
        printf("12. Add Medical Record\n13. View Patient Medical History\n");
        printf("\n0. Exit\n");
        printf("Enter choice: ");
        scanf("%d", &choice);
        while (getchar() != '\n')
            ; // Clear input buffer after reading number

        switch (choice)
        {
        case 1:
            addPatient();
            break;
        case 2:
            viewPatients();
            break;
        case 3:
            updatePatient();
            break;
        case 4:
            deletePatient();
            break;
        case 5:
            addDoctor();
            break;
        case 6:
            viewDoctors();
            break;
        case 7:
            updateDoctor();
            break;
        case 8:
            deleteDoctor();
            break;
        case 9:
            scheduleAppointment();
            break;
        case 10:
            viewAppointments();
            break;
        case 11:
            cancelAppointment();
            break;
        case 12:
            addMedicalRecord();
            break;
        case 13:
            viewPatientMedicalHistory();
            break;
        case 0:
            exit(0);
        default:
            printf("Invalid choice.\n");
        }
    }
}
