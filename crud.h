#ifndef CRUD_H
#define CRUD_H

#define PATIENT_FILE "data/patients.dat"
#define DOCTOR_FILE  "data/doctors.dat"
#define APPOINTMENT_FILE "data/appointments.dat"

typedef struct {
    int id;
    char name[50];
    int age;
    char gender[10];
    char phone[15];
    char disease[50];
} Patient;

typedef struct {
    int id;
    char name[50];
    char specialization[50];
    char phone[15];
} Doctor;

// --- New Feature: Appointments ---
// Links a patient to a doctor for a scheduled visit.
typedef struct {
    int id;
    int patient_id; // Foreign key to Patient
    int doctor_id;  // Foreign key to Doctor
    char appointment_date[20]; // Format: YYYY-MM-DD HH:MM
    char reason[100];
    char status[20]; // e.g., "Scheduled", "Completed", "Cancelled"
} Appointment;

// --- New Feature: Medical Records ---
// Stores details of a patient's diagnosis and treatment for a specific visit.
typedef struct {
    int id;
    int appointment_id; // Links this record to a specific appointment
    char diagnosis[100];
    char prescription[100];
} MedicalRecord;

void addPatient();
void viewPatients();
void updatePatient();
void deletePatient();

void addDoctor();
void viewDoctors();
void updateDoctor();
void deleteDoctor();

// --- Appointment Management Functions ---
void scheduleAppointment();
void viewAppointments(); // Could be filtered by doctor or patient
void cancelAppointment();

// --- Medical Record Management Functions ---
void addMedicalRecord();
void viewPatientMedicalHistory();

#endif
