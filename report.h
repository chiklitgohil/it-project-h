#ifndef REPORT_H
#define REPORT_H

#include "crud.h" // For struct definitions

#define MEDICAL_RECORD_FILE "data/records.dat"
#define BILL_FILE "data/bills.dat"

// Stores details of a patient's diagnosis and treatment for a specific visit.
typedef struct
{
    int id;
    int appointment_id; // Links this record to a specific appointment
    char diagnosis[100];
    char prescription[100];
} MedicalRecord;

typedef struct
{
    int id;
    int appointment_id;
    float amount;
    char status[20]; // e.g., "Paid", "Unpaid"
} Bill;

void addMedicalRecord();
void viewPatientMedicalHistory();
void viewPatientBills();

#endif // REPORT_H