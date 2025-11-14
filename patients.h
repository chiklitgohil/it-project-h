#ifndef PATIENTS_H
#define PATIENTS_H

#include "common.h"

/* Patient CRUD Operations */
void addPatient();
void viewPatients();
void updatePatient();
void deletePatient();

/* Utility */
int patientExists(int patient_id);

#endif
