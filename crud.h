#ifndef CRUD_H
#define CRUD_H

#define PATIENT_FILE "data/patients.dat"
#define DOCTOR_FILE  "data/doctors.dat"

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

void addPatient();
void viewPatients();
void updatePatient();
void deletePatient();

void addDoctor();
void viewDoctors();
void updateDoctor();
void deleteDoctor();

#endif
