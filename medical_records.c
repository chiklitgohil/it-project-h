/*
 * medical_records.c - Medical Records & Billing Management
 * Handles medical record creation, viewing, and bill management
 */

#include "common.h"
#include "medical_records.h"
#include "appointments.h"

void addMedicalRecord()
{
    int appointmentId, patientId;
    printf("Enter Appointment ID: ");
    scanf("%d", &appointmentId);
    clearStdin();

    printf("Enter Patient ID: ");
    scanf("%d", &patientId);
    clearStdin();

    FILE *fp = fopen(MEDICAL_RECORD_FILE, "ab+");
    if (!fp)
    {
        perror("Error opening medical record file");
        return;
    }

    MedicalRecord mr;
    mr.id = getNextId(MEDICAL_RECORD_FILE, sizeof(MedicalRecord));
    mr.appointment_id = appointmentId;

    printf("Enter diagnosis: ");
    fgets(mr.diagnosis, sizeof(mr.diagnosis), stdin);
    mr.diagnosis[strcspn(mr.diagnosis, "\n")] = 0;

    printf("Enter prescription: ");
    fgets(mr.prescription, sizeof(mr.prescription), stdin);
    mr.prescription[strcspn(mr.prescription, "\n")] = 0;

    fwrite(&mr, sizeof(MedicalRecord), 1, fp);
    fclose(fp);
    printf("Medical record added successfully.\n");
}

void viewPatientMedicalHistory()
{
    int patientId;
    printf("Enter Patient ID: ");
    scanf("%d", &patientId);
    clearStdin();

    FILE *fp = fopen(MEDICAL_RECORD_FILE, "rb");
    if (!fp)
    {
        printf("No medical records found.\n");
        return;
    }

    MedicalRecord mr;
    int found = 0;
    printf("\n=== Medical History for Patient %d ===\n", patientId);
    while (fread(&mr, sizeof(MedicalRecord), 1, fp))
    {
        if (mr.appointment_id > 0)
        {
            found = 1;
            printf("Appointment ID: %d\n", mr.appointment_id);
            printf("Diagnosis: %s\n", mr.diagnosis);
            printf("Prescription: %s\n", mr.prescription);
            printf("---\n");
        }
    }
    fclose(fp);

    if (!found)
        printf("No medical records found for patient %d.\n", patientId);
}

void viewDoctorReports(int patientId)
{
    FILE *fp = fopen(MEDICAL_RECORD_FILE, "rb");
    if (!fp)
        return;

    MedicalRecord mr;
    int found = 0;
    printf("\n=== Medical Reports ===\n");
    while (fread(&mr, sizeof(MedicalRecord), 1, fp))
    {
        found = 1;
        printf("Record ID: %d\n", mr.id);
        printf("Diagnosis: %s\n", mr.diagnosis);
        printf("Prescription: %s\n", mr.prescription);
        printf("---\n");
    }
    fclose(fp);

    if (!found)
        printf("No medical reports found.\n");
}

void generateBill(int appointmentId, int patientId, float amount)
{
    FILE *fp = fopen(BILL_FILE, "ab+");
    if (!fp)
    {
        perror("Error opening bill file");
        return;
    }

    Bill b;
    b.id = getNextId(BILL_FILE, sizeof(Bill));
    b.appointment_id = appointmentId;
    b.patient_id = patientId;
    b.amount = amount;

    time_t now = time(NULL);
    struct tm *timeinfo = localtime(&now);
    strftime(b.date, sizeof(b.date), "%Y-%m-%d", timeinfo);

    strcpy(b.status, "Unpaid");

    fwrite(&b, sizeof(Bill), 1, fp);
    fclose(fp);
    printf("Bill generated: ID %d, Amount: ₹%.2f\n", b.id, amount);
}

void viewPatientBills(int patientId)
{
    FILE *fp = fopen(BILL_FILE, "rb");
    if (!fp)
    {
        printf("No bills found.\n");
        return;
    }

    Bill b;
    int found = 0;
    printf("\n=== Bills for Patient %d ===\n", patientId);
    printf("%-5s %-10s %-10s %-10s %-15s\n",
           "ID", "Amount", "Date", "Status", "AppointmentID");

    while (fread(&b, sizeof(Bill), 1, fp))
    {
        if (b.patient_id == patientId)
        {
            printf("%-5d ₹%-9.2f %-10s %-10s %-15d\n",
                   b.id, b.amount, b.date, b.status, b.appointment_id);
            found = 1;
        }
    }
    fclose(fp);

    if (!found)
        printf("No bills found for patient %d.\n", patientId);
    else
    {
        printf("\n--- Payment Instructions ---\n");
        printf("UPI Payment: hospital.care@upi\n");
        printf("Or pay at the reception counter.\n");
    }
}

void viewAllBills(void)
{
    FILE *fp = fopen(BILL_FILE, "rb");
    if (!fp)
    {
        printf("No bills found.\n");
        return;
    }

    Bill b;
    printf("\n=== All Bills ===\n");
    printf("%-5s %-10s %-10s %-10s %-10s %-15s\n",
           "ID", "PatID", "Amount", "Date", "Status", "AppointmentID");

    while (fread(&b, sizeof(Bill), 1, fp))
    {
        printf("%-5d %-10d ₹%-9.2f %-10s %-10s %-15d\n",
               b.id, b.patient_id, b.amount, b.date, b.status, b.appointment_id);
    }
    fclose(fp);
}

void manageBilling(void)
{
    int choice;
    while (1)
    {
        printf("\n=== Billing Management ===\n");
        printf("1) View All Bills\n");
        printf("2) View Patient Bills\n");
        printf("3) View Medical Records\n");
        printf("4) Mark Bill as Paid\n");
        printf("0) Back\n");
        printf("Choose: ");
        scanf("%d", &choice);
        clearStdin();

        switch (choice)
        {
        case 1:
            viewAllBills();
            break;
        case 2:
        {
            int patientId;
            printf("Enter Patient ID: ");
            scanf("%d", &patientId);
            clearStdin();
            viewPatientBills(patientId);
            break;
        }
        case 3:
            viewPatientMedicalHistory();
            break;
        case 4:
        {
            int billId;
            printf("Enter Bill ID to mark as Paid: ");
            scanf("%d", &billId);
            clearStdin();
            markBillAsPaid(billId);
            break;
        }
        case 0:
            return;
        default:
            printf("Invalid choice.\n");
        }
    }
}

void markBillAsPaid(int billId)
{
    FILE *fp = fopen(BILL_FILE, "rb+");
    if (!fp)
    {
        printf("No bills found.\n");
        return;
    }

    Bill b;
    int found = 0;
    while (fread(&b, sizeof(Bill), 1, fp))
    {
        if (b.id == billId)
        {
            if (strcmp(b.status, "Paid") == 0)
            {
                printf("Bill %d is already marked as Paid.\n", billId);
                found = 1;
                break;
            }
            strncpy(b.status, "Paid", sizeof(b.status) - 1);
            if (fseek(fp, -(long)sizeof(Bill), SEEK_CUR) != 0)
                perror("fseek");
            if (fwrite(&b, sizeof(Bill), 1, fp) != 1)
                perror("fwrite");
            fflush(fp);
            printf("Bill %d marked as Paid.\n", billId);
            found = 1;
            break;
        }
    }
    if (!found)
        printf("Bill not found.\n");
    fclose(fp);
}
