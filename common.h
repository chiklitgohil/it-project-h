#ifndef COMMON_H
#define COMMON_H

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* ==================== FILE PATHS ==================== */
#define PATIENT_FILE "data/patients.dat"
#define DOCTOR_FILE "data/doctors.dat"
#define ADMIN_FILE "data/admins.dat"
#define APPOINTMENT_FILE "data/appointments.dat"
#define MEDICAL_RECORD_FILE "data/medical_records.dat"
#define BILL_FILE "data/bills.dat"

/* ==================== DATA STRUCTURES ==================== */

typedef struct
{
    int id;
    char name[50];
    int age;
    char gender[10];
    char phone[15];
    char username[64];
    char password[64];
} Patient;

typedef struct
{
    int id;
    char name[50];
    char specialization[50];
    char phone[15];
    char username[64];
    char password[64];
} Doctor;

typedef struct
{
    int id;
    int patient_id;
    int doctor_id;
    char appointment_date[20];
    char reason[100];
    char status[20];
} Appointment;

typedef struct
{
    int id;
    int appointment_id;
    char diagnosis[100];
    char prescription[100];
} MedicalRecord;

typedef struct
{
    int id;
    int appointment_id;
    int patient_id;
    float amount;
    char date[20];
    char status[20];
} Bill;

typedef struct
{
    int id;
    char username[64];
    char password[64];
    char name[50];
} Admin;

/* ==================== UTILITY FUNCTIONS ==================== */

void getInput(char *buffer, int size);
void clearStdin(void);
int getNextId(const char *filename, size_t struct_size);

#endif
