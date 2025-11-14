/*
 * analytics.c - Search, Sorting & Analytics
 * Handles searching, analytics, and reporting functionality
 */

#include "common.h"
#include "analytics.h"

void searchPatientByName(const char *name)
{
    FILE *fp = fopen(PATIENT_FILE, "rb");
    if (!fp)
    {
        printf("No patients found.\n");
        return;
    }

    Patient p;
    int found = 0;
    printf("\n=== Search Results for Patient: %s ===\n", name);
    printf("%-5s %-20s %-5s %-10s %-15s\n",
           "ID", "Name", "Age", "Gender", "Phone");

    while (fread(&p, sizeof(Patient), 1, fp))
    {
        if (strstr(p.name, name) != NULL)
        {
            printf("%-5d %-20s %-5d %-10s %-15s\n",
                   p.id, p.name, p.age, p.gender, p.phone);
            found = 1;
        }
    }
    fclose(fp);

    if (!found)
        printf("No patients found matching '%s'.\n", name);
}

void analyticsMenu()
{
    int choice;
    while (1)
    {
        printf("\n=== Analytics Menu ===\n");
        printf("1) Total Patients and Doctors\n");
        printf("2) Total Appointments\n");
        printf("3) Doctor Appointment Count\n");
        printf("0) Back\n");
        printf("Choose: ");
        scanf("%d", &choice);
        clearStdin();

        if (choice == 0)
            break;

        switch (choice)
        {
        case 1:
            totalPatientsAndDoctors();
            break;
        case 2:
            totalAppointments();
            break;
        case 3:
            doctorAppointmentCount();
            break;
        default:
            printf("Invalid choice.\n");
        }
    }
}

void totalPatientsAndDoctors()
{
    FILE *fp;
    int count = 0;

    /* Count patients */
    fp = fopen(PATIENT_FILE, "rb");
    if (fp)
    {
        Patient p;
        while (fread(&p, sizeof(Patient), 1, fp))
            count++;
        fclose(fp);
    }
    int patientCount = count;

    /* Count doctors */
    count = 0;
    fp = fopen(DOCTOR_FILE, "rb");
    if (fp)
    {
        Doctor d;
        while (fread(&d, sizeof(Doctor), 1, fp))
            count++;
        fclose(fp);
    }
    int doctorCount = count;

    printf("\n=== System Statistics ===\n");
    printf("Total Patients: %d\n", patientCount);
    printf("Total Doctors: %d\n", doctorCount);
}

void totalAppointments()
{
    FILE *fp = fopen(APPOINTMENT_FILE, "rb");
    if (!fp)
    {
        printf("No appointments found.\n");
        return;
    }

    int count = 0;
    Appointment a;
    while (fread(&a, sizeof(Appointment), 1, fp))
        count++;
    fclose(fp);

    printf("\n=== Appointment Statistics ===\n");
    printf("Total Appointments: %d\n", count);
}

void doctorAppointmentCount()
{
    int doctorId;
    printf("Enter Doctor ID: ");
    scanf("%d", &doctorId);
    clearStdin();

    FILE *fp = fopen(APPOINTMENT_FILE, "rb");
    if (!fp)
    {
        printf("No appointments found.\n");
        return;
    }

    int count = 0;
    Appointment a;
    while (fread(&a, sizeof(Appointment), 1, fp))
    {
        if (a.doctor_id == doctorId)
            count++;
    }
    fclose(fp);

    printf("\nDoctor %d has %d appointments.\n", doctorId, count);
}
