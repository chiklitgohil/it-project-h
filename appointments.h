#ifndef APPOINTMENTS_H
#define APPOINTMENTS_H

#include "common.h"

/* Appointment Management */
void scheduleAppointment();
void scheduleAppointmentWithConflictCheck(int patientId);
void viewAppointments();
void cancelAppointment();
void rescheduleAppointment();
void assignDoctorToPatient();
void searchPatientsByDoctor(int doctorId);

#endif
