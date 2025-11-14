#ifndef DOCTORS_H
#define DOCTORS_H

#include "common.h"

/* Doctor CRUD Operations */
void addDoctor();
void viewDoctors();
void updateDoctor();
void deleteDoctor();

/* Utility */
int doctorExists(int doctor_id);

#endif
