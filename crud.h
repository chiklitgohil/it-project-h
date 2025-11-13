#ifndef CRUD_H
#define CRUD_H

#include <time.h>

#define PATIENT_FILE "data/patients.dat"
#define DOCTOR_FILE "data/doctors.dat"
#define ADMIN_FILE "data/admins.dat"
#define APPOINTMENT_FILE "data/appointments.dat"
#define MEDICAL_RECORD_FILE "data/medical_records.dat"
#define BILL_FILE "data/bills.dat"

typedef struct
{
    int id;
    char name[50];
    int age;
    char gender[10];
    char phone[15];
    char username[64];   /* moved credential here */
    char password[64];   /* plain text for simple project */
    /* disease removed — patient profile does not store disease by design */
} Patient;

typedef struct
{
    int id;
    char name[50];
    char specialization[50];
    char phone[15];
    char username[64];   /* moved credential here */
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
    char status[20]; /* Paid / Unpaid */
} Bill;

/* New Admin profile (credentials stored in admin file) */
typedef struct
{
    int id;
    char username[64];
    char password[64];
    char name[50];
} Admin;

/* Patient CRUD */
void addPatient();
void viewPatients();
void updatePatient();
void deletePatient();

/* Doctor CRUD */
void addDoctor();
void viewDoctors();
void updateDoctor();
void deleteDoctor();

/* Appointments */
void scheduleAppointmentWithConflictCheck(int patientId);
void scheduleAppointment();
void viewAppointments();
void cancelAppointment();
void rescheduleAppointment();
void assignDoctorToPatient();

/* Medical Records */
void addMedicalRecord();
void viewPatientMedicalHistory();
void viewDoctorReports(int patientId);

/* Billing */
void generateBill(int appointmentId, int patientId, float amount);
void viewPatientBills(int patientId);
void viewAllBills(void);
/* New billing management helpers */
void manageBilling(void);
void markBillAsPaid(int billId);

/* Search */
void searchPatientByName(const char *name);

/* Analytics */
void analyticsMenu();
void totalPatientsAndDoctors();
void totalAppointments();
void doctorAppointmentCount();

/* Utilities */
void getInput(char *buffer, int size);
void clearStdin(void);
int getNextId(const char *filename, size_t struct_size);

#endif
