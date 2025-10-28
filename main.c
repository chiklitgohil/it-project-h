#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "crud.h"
#include "report.h"

int main()
{
	int choice;
	while (1)
	{
		printf("\n--- Hospital Management System ---\n");
		printf("\n-- Patient Management --\n");
		printf("1. Add Patient\n2. View Patients\n3. Update Patient\n4. Delete Patient\n");
		printf("\n-- Doctor Management --\n");
		printf("5. Add Doctor\n6. View Doctors\n7. Update Doctor\n8. Delete Doctor\n");
		printf("\n-- Appointments & Records --\n");
		printf("9. Schedule Appointment\n10. View Appointments\n11. Cancel Appointment\n");
		printf("12. Add Medical Record\n13. View Patient Medical History\n");
		printf("14. View Patient Bills\n\n0. Exit\n");
		printf("Enter choice: ");
		scanf("%d", &choice);
		while (getchar() != '\n')
			; // Clear input buffer after reading number

		switch (choice)
		{
		case 1:
			addPatient();
			break;
		case 2:
			viewPatients();
			break;
		case 3:
			updatePatient();
			break;
		case 4:
			deletePatient();
			break;
		case 5:
			addDoctor();
			break;
		case 6:
			viewDoctors();
			break;
		case 7:
			updateDoctor();
			break;
		case 8:
			deleteDoctor();
			break;
		case 9:
			scheduleAppointment();
			break;
		case 10:
			viewAppointments();
			break;
		case 11:
			cancelAppointment();
			break;
		case 12:
			addMedicalRecord();
			break;
		case 13:
			viewPatientMedicalHistory();
			break;
		case 14:
			viewPatientBills();
			break;
		case 0:
			exit(0);
		default:
			printf("Invalid choice.\n");
		}
	}
}
