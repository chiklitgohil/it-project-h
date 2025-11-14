#ifndef MEDICAL_RECORDS_H
#define MEDICAL_RECORDS_H

#include "common.h"

/* Medical Records */
void addMedicalRecord();
void viewPatientMedicalHistory();
void viewDoctorReports(int patientId);

/* Billing */
void generateBill(int appointmentId, int patientId, float amount);
void viewPatientBills(int patientId);
void viewAllBills(void);
void manageBilling(void);
void markBillAsPaid(int billId);

#endif
