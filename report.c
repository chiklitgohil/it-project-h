#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "report.h"
#include "crud.h"

// Helper function to get the next available ID for a file
int getNextId(const char *filename, size_t struct_size)
{
    FILE *fp = fopen(filename, "rb");
    if (!fp)
    {
        return 1; // Start with ID 1 if file doesn't exist
    }
    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    fclose(fp);
    return (size / struct_size) + 1;
}

// Generates a bill for a completed appointment
void generateBill(int appointment_id)
{
    FILE *fp = fopen(BILL_FILE, "ab");
    if (!fp)
    {
        perror("Error opening bills file");
        return;
    }

    Bill b;
    b.id = getNextId(BILL_FILE, sizeof(Bill));
    b.appointment_id = appointment_id;
    b.amount = 500.00; // Example fixed consultation fee
    strcpy(b.status, "Unpaid");

    fwrite(&b, sizeof(Bill), 1, fp);
    fclose(fp);
    printf("Bill generated successfully for appointment ID %d.\n", appointment_id);
}

void addMedicalRecord()
{
    printf("\n--- Add Medical Record ---\n");

    int app_id;
    printf("Enter Appointment ID to add a record for: ");
    scanf("%d", &app_id);
    while (getchar() != '\n'); // Clear buffer

    // 1. Verify the appointment exists
    FILE *app_fp = fopen(APPOINTMENT_FILE, "rb+");
    if (!app_fp)
    {
        perror("Could not open appointments file");
        return;
    }

    Appointment app;
    int app_found = 0;
    while (fread(&app, sizeof(Appointment), 1, app_fp))
    {
        if (app.id == app_id)
        {
            app_found = 1;
            break;
        }
    }

    if (!app_found)
    {
        printf("Appointment with ID %d not found.\n", app_id);
        fclose(app_fp);
        return;
    }

    // 2. Get diagnosis and prescription details
    MedicalRecord rec;
    rec.id = getNextId(MEDICAL_RECORD_FILE, sizeof(MedicalRecord));
    rec.appointment_id = app_id;

    printf("Enter Diagnosis: ");
    getInput(rec.diagnosis, sizeof(rec.diagnosis));
    printf("Enter Prescription: ");
    getInput(rec.prescription, sizeof(rec.prescription));

    // 3. Save the new medical record
    FILE *rec_fp = fopen(MEDICAL_RECORD_FILE, "ab");
    if (!rec_fp)
    {
        perror("Could not open medical records file");
        fclose(app_fp);
        return;
    }
    fwrite(&rec, sizeof(MedicalRecord), 1, rec_fp);
    fclose(rec_fp);

    // 4. Update appointment status to "Completed"
    strcpy(app.status, "Completed");
    fseek(app_fp, -sizeof(Appointment), SEEK_CUR);
    fwrite(&app, sizeof(Appointment), 1, app_fp);
    fclose(app_fp);

    printf("Medical record added successfully.\n");

    // 5. Automatically generate a bill
    generateBill(app_id);
}

void viewPatientMedicalHistory()
{
    printf("\n--- View Patient Medical History ---\n");
    int patient_id;
    printf("Enter Patient ID: ");
    scanf("%d", &patient_id);
    while (getchar() != '\n');

    printf("\n--- Medical History for Patient ID: %d ---\n", patient_id);

    FILE *app_fp = fopen(APPOINTMENT_FILE, "rb");
    FILE *rec_fp = fopen(MEDICAL_RECORD_FILE, "rb");

    if (!app_fp || !rec_fp)
    {
        printf("Could not open data files.\n");
        if (app_fp) fclose(app_fp);
        if (rec_fp) fclose(rec_fp);
        return;
    }

    Appointment app;
    MedicalRecord rec;
    int records_found = 0;

    // Find all appointments for the patient
    while (fread(&app, sizeof(Appointment), 1, app_fp))
    {
        if (app.patient_id == patient_id)
        {
            // For each appointment, find the corresponding medical record
            rewind(rec_fp); // Search from the beginning of the records file
            while (fread(&rec, sizeof(MedicalRecord), 1, rec_fp))
            {
                if (rec.appointment_id == app.id)
                {
                    printf("\nAppointment ID: %d\n", app.id);
                    printf("Date: %s\n", app.appointment_date);
                    printf("Diagnosis: %s\n", rec.diagnosis);
                    printf("Prescription: %s\n", rec.prescription);
                    records_found++;
                    break; // Found the record for this appointment
                }
            }
        }
    }

    if (records_found == 0)
    {
        printf("No medical records found for this patient.\n");
    }

    fclose(app_fp);
    fclose(rec_fp);
}

void viewPatientBills()
{
    printf("Function not yet implemented.\n");
}