#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "crud.h"

void clearStdin(void)
{
	int c;
	while ((c = getchar()) != '\n' && c != EOF)
		;
}

void getInput(char *buffer, int size)
{
	fgets(buffer, size, stdin);
	buffer[strcspn(buffer, "\n")] = 0;
}

// Centralized helper: get next ID from any file
static int getNextId(const char *filename, size_t struct_size)
{
	FILE *fp = fopen(filename, "rb");
	if (!fp)
		return 1;
	fseek(fp, 0, SEEK_END);
	long size = ftell(fp);
	fclose(fp);
	return (size / struct_size) + 1;
}

static int patientExists(int id)
{
	FILE *fp = fopen(PATIENT_FILE, "rb");
	if (!fp)
		return 0;
	Patient p;
	while (fread(&p, sizeof(Patient), 1, fp))
	{
		if (p.id == id)
		{
			fclose(fp);
			return 1;
		}
	}
	fclose(fp);
	return 0;
}

/*  REPLACED implementation:
static int doctorExists(int id)
{
	FILE *fp = fopen(DOCTOR_FILE, "rb");
	if (!fp)
		return 0;
	Doctor d;
	while (fread(&d, sizeof(Doctor), 1, fp))
	{
		if (d.id == id)
		{
			fclose(fp);
			return 1;
		}
	}
	fclose(fp);
	return 0;
}
*/

static int doctorExists(int id)
{
	/* 1) Check doctor profiles file first (normal case) */
	FILE *fp = fopen(DOCTOR_FILE, "rb");
	if (fp)
	{
		Doctor d;
		while (fread(&d, sizeof(Doctor), 1, fp))
		{
			if (d.id == id)
			{
				fclose(fp);
				return 1;
			}
		}
		fclose(fp);
	}

	/* 2) Fallback: check doctor credentials file in case credentials were
	   previously written to a different file. We can't include auth.h here
	   (circular include), so read the file using a local struct matching
	   the Credential layout. */
	const char *cred_path = "data/doctor_creds.dat"; /* must match auth.h's DOCTOR_CRED_FILE */
	FILE *cf = fopen(cred_path, "rb");
	if (cf)
	{
		struct
		{
			int id;
			char username[64];
			char password[64];
		} cred;
		while (fread(&cred, sizeof(cred), 1, cf))
		{
			if (cred.id == id)
			{
				fclose(cf);
				return 1;
			}
		}
		fclose(cf);
	}

	return 0;
}

static int appointmentExists(int id)
{
	FILE *fp = fopen(APPOINTMENT_FILE, "rb");
	if (!fp)
		return 0;
	Appointment a;
	while (fread(&a, sizeof(Appointment), 1, fp))
	{
		if (a.id == id)
		{
			fclose(fp);
			return 1;
		}
	}
	fclose(fp);
	return 0;
}

void addPatient()
{
	FILE *fp = fopen(PATIENT_FILE, "ab+");
	if (!fp)
	{
		perror("Error opening patient file");
		return;
	}

	Patient p;
	printf("Enter ID: ");
	scanf("%d", &p.id);
	clearStdin();
	printf("Name: ");
	getInput(p.name, sizeof(p.name));
	printf("Age: ");
	scanf("%d", &p.age);
	clearStdin();
	printf("Gender: ");
	getInput(p.gender, sizeof(p.gender));
	printf("Phone: ");
	getInput(p.phone, sizeof(p.phone));
	printf("Disease: ");
	getInput(p.disease, sizeof(p.disease));

	fwrite(&p, sizeof(Patient), 1, fp);
	fclose(fp);
}

void viewPatients()
{
	FILE *fp = fopen(PATIENT_FILE, "rb");
	if (!fp)
		return;

	Patient p;
	printf("\n%-5s %-15s %-5s %-10s %-15s %-20s\n",
		   "ID", "Name", "Age", "Gender", "Phone", "Disease");
	while (fread(&p, sizeof(Patient), 1, fp))
		printf("%-5d %-15s %-5d %-10s %-15s %-20s\n",
			   p.id, p.name, p.age, p.gender, p.phone, p.disease);

	fclose(fp);
}

void updatePatient()
{
	FILE *fp = fopen(PATIENT_FILE, "rb+");
	if (!fp)
		return;

	int id, found = 0;
	printf("Enter Patient ID to update: ");
	scanf("%d", &id);
	clearStdin();

	Patient p;
	while (fread(&p, sizeof(Patient), 1, fp))
	{
		if (p.id == id)
		{
			char buf[128];
			printf("Enter new Name (press Enter to keep '%s'): ", p.name);
			getInput(buf, sizeof(buf));
			if (buf[0] != '\0')
				strncpy(p.name, buf, sizeof(p.name));

			printf("Enter new Age (press Enter to keep '%d'): ", p.age);
			getInput(buf, sizeof(buf));
			if (buf[0] != '\0')
			{
				int newAge;
				if (sscanf(buf, "%d", &newAge) == 1)
					p.age = newAge;
			}

			printf("Enter new Gender (press Enter to keep '%s'): ", p.gender);
			getInput(buf, sizeof(buf));
			if (buf[0] != '\0')
				strncpy(p.gender, buf, sizeof(p.gender));

			printf("Enter new Phone (press Enter to keep '%s'): ", p.phone);
			getInput(buf, sizeof(buf));
			if (buf[0] != '\0')
				strncpy(p.phone, buf, sizeof(p.phone));

			printf("Enter new Disease (press Enter to keep '%s'): ", p.disease);
			getInput(buf, sizeof(buf));
			if (buf[0] != '\0')
				strncpy(p.disease, buf, sizeof(p.disease));

			if (fseek(fp, -(long)sizeof(Patient), SEEK_CUR) != 0)
				perror("fseek");
			fwrite(&p, sizeof(Patient), 1, fp);
			found = 1;
			printf("Patient updated.\n");
			break;
		}
	}

	fclose(fp);
	if (!found)
		printf("Record not found.\n");
}

void deletePatient()
{
	FILE *fp = fopen(PATIENT_FILE, "rb");
	FILE *temp = fopen("data/temp.dat", "wb");
	if (!fp || !temp)
		return;

	int id, found = 0;
	printf("Enter Patient ID to delete: ");
	scanf("%d", &id);

	Patient p;
	while (fread(&p, sizeof(Patient), 1, fp))
	{
		if (p.id != id)
			fwrite(&p, sizeof(Patient), 1, temp);
		else
			found = 1;
	}

	fclose(fp);
	fclose(temp);
	remove(PATIENT_FILE);
	rename("data/temp.dat", PATIENT_FILE);

	if (!found)
		printf("Record not found.\n");
}

void addDoctor()
{
	Doctor d;
	printf("Enter ID: ");
	scanf("%d", &d.id);
	clearStdin();

	if (doctorExists(d.id))
	{
		printf("Doctor with ID %d already exists.\n", d.id);
		return;
	}

	printf("Name: ");
	getInput(d.name, sizeof(d.name));
	printf("Specialization: ");
	getInput(d.specialization, sizeof(d.specialization));
	printf("Phone: ");
	getInput(d.phone, sizeof(d.phone));

	FILE *fp = fopen(DOCTOR_FILE, "ab+");
	if (!fp)
		return;
	fwrite(&d, sizeof(Doctor), 1, fp);
	fclose(fp);
}

void viewDoctors()
{
	FILE *fp = fopen(DOCTOR_FILE, "rb");
	if (!fp)
		return;

	Doctor d;
	printf("\n%-5s %-15s %-20s %-15s\n",
		   "ID", "Name", "Specialization", "Phone");
	while (fread(&d, sizeof(Doctor), 1, fp))
		printf("%-5d %-15s %-20s %-15s\n",
			   d.id, d.name, d.specialization, d.phone);

	fclose(fp);
}

void updateDoctor()
{
	FILE *fp = fopen(DOCTOR_FILE, "rb+");
	if (!fp)
		return;

	int id, found = 0;
	printf("Enter Doctor ID to update: ");
	scanf("%d", &id);
	clearStdin();

	Doctor d;
	while (fread(&d, sizeof(Doctor), 1, fp))
	{
		if (d.id == id)
		{
			char buf[128];
			printf("Enter new Name (press Enter to keep '%s'): ", d.name);
			getInput(buf, sizeof(buf));
			if (buf[0] != '\0')
				strncpy(d.name, buf, sizeof(d.name));

			printf("Enter new Specialization (press Enter to keep '%s'): ", d.specialization);
			getInput(buf, sizeof(buf));
			if (buf[0] != '\0')
				strncpy(d.specialization, buf, sizeof(d.specialization));

			printf("Enter new Phone (press Enter to keep '%s'): ", d.phone);
			getInput(buf, sizeof(buf));
			if (buf[0] != '\0')
				strncpy(d.phone, buf, sizeof(d.phone));

			if (fseek(fp, -(long)sizeof(Doctor), SEEK_CUR) != 0)
				perror("fseek");
			fwrite(&d, sizeof(Doctor), 1, fp);
			found = 1;
			printf("Doctor updated.\n");
			break;
		}
	}

	fclose(fp);
	if (!found)
		printf("Record not found.\n");
}

void deleteDoctor()
{
	FILE *fp = fopen(DOCTOR_FILE, "rb");
	FILE *temp = fopen("data/temp.dat", "wb");
	if (!fp || !temp)
		return;

	int id, found = 0;
	printf("Enter Doctor ID to delete: ");
	scanf("%d", &id);
	clearStdin();

	Doctor d;
	while (fread(&d, sizeof(Doctor), 1, fp))
	{
		if (d.id != id)
			fwrite(&d, sizeof(Doctor), 1, temp);
		else
			found = 1;
	}

	fclose(fp);
	fclose(temp);
	remove(DOCTOR_FILE);
	rename("data/temp.dat", DOCTOR_FILE);

	if (!found)
		printf("Record not found.\n");
}

// --- Appointment Management ---
void scheduleAppointment()
{
	printf("\n--- Schedule Appointment ---\n");
	Appointment a;
	a.id = getNextId(APPOINTMENT_FILE, sizeof(Appointment));

	printf("Enter Patient ID: ");
	scanf("%d", &a.patient_id);
	clearStdin();
	if (!patientExists(a.patient_id))
	{
		printf("Patient not found.\n");
		return;
	}

	printf("Enter Doctor ID: ");
	scanf("%d", &a.doctor_id);
	clearStdin();
	if (!doctorExists(a.doctor_id))
	{
		printf("Doctor not found.\n");
		return;
	}

	printf("Enter appointment date/time (YYYY-MM-DD HH:MM): ");
	getInput(a.appointment_date, sizeof(a.appointment_date));
	printf("Enter reason: ");
	getInput(a.reason, sizeof(a.reason));
	strcpy(a.status, "Scheduled");

	FILE *fp = fopen(APPOINTMENT_FILE, "ab+");
	if (!fp)
	{
		perror("Error opening appointment file");
		return;
	}
	fwrite(&a, sizeof(Appointment), 1, fp);
	fclose(fp);
	printf("Appointment scheduled with ID %d\n", a.id);
}

void viewAppointments()
{
	printf("\n--- View Appointments ---\n");
	FILE *fp = fopen(APPOINTMENT_FILE, "rb");
	if (!fp)
	{
		printf("No appointments found.\n");
		return;
	}
	Appointment a;
	printf("\n%-5s %-10s %-10s %-20s %-25s %-10s\n",
		   "ID", "Patient", "Doctor", "Date/Time", "Reason", "Status");
	while (fread(&a, sizeof(Appointment), 1, fp))
		printf("%-5d %-10d %-10d %-20s %-25s %-10s\n",
			   a.id, a.patient_id, a.doctor_id, a.appointment_date, a.reason, a.status);
	fclose(fp);
}

void cancelAppointment()
{
	printf("\n--- Cancel Appointment ---\n");
	int id;
	printf("Enter Appointment ID to cancel: ");
	scanf("%d", &id);
	clearStdin();

	FILE *fp = fopen(APPOINTMENT_FILE, "rb+");
	if (!fp)
	{
		printf("No appointments found.\n");
		return;
	}
	Appointment a;
	int found = 0;
	while (fread(&a, sizeof(Appointment), 1, fp))
	{
		if (a.id == id)
		{
			strcpy(a.status, "Cancelled");
			if (fseek(fp, -(long)sizeof(Appointment), SEEK_CUR) != 0)
				perror("fseek");
			fwrite(&a, sizeof(Appointment), 1, fp);
			found = 1;
			printf("Appointment %d cancelled.\n", id);
			break;
		}
	}
	fclose(fp);
	if (!found)
		printf("Appointment not found.\n");
}

// --- Reschedule Appointment ---
void rescheduleAppointment()
{
	printf("\n--- Reschedule Appointment ---\n");
	int id;
	printf("Enter Appointment ID to reschedule: ");
	scanf("%d", &id);
	clearStdin();

	FILE *fp = fopen(APPOINTMENT_FILE, "rb+");
	if (!fp)
	{
		printf("No appointments found.\n");
		return;
	}

	Appointment a;
	int found = 0;
	while (fread(&a, sizeof(Appointment), 1, fp))
	{
		if (a.id == id)
		{
			if (strcmp(a.status, "Cancelled") == 0)
			{
				printf("Cannot reschedule a cancelled appointment.\n");
				fclose(fp);
				return;
			}

			printf("Current date/time: %s\n", a.appointment_date);
			printf("Enter new date/time (YYYY-MM-DD HH:MM): ");
			getInput(a.appointment_date, sizeof(a.appointment_date));

			if (fseek(fp, -(long)sizeof(Appointment), SEEK_CUR) != 0)
				perror("fseek");
			fwrite(&a, sizeof(Appointment), 1, fp);
			found = 1;
			printf("Appointment %d rescheduled successfully.\n", id);
			break;
		}
	}

	fclose(fp);
	if (!found)
		printf("Appointment not found.\n");
}

// --- Conflict Detection ---
static int hasConflict(int doctorId, const char *dateTime, int excludeAppointmentId)
{
	FILE *fp = fopen(APPOINTMENT_FILE, "rb");
	if (!fp)
		return 0;

	Appointment a;
	while (fread(&a, sizeof(Appointment), 1, fp))
	{
		if (a.doctor_id == doctorId &&
			a.id != excludeAppointmentId &&
			strcmp(a.status, "Cancelled") != 0 &&
			strcmp(a.appointment_date, dateTime) == 0)
		{
			fclose(fp);
			return 1;
		}
	}
	fclose(fp);
	return 0;
}

// --- Enhanced Schedule Appointment with Conflict Detection ---
void scheduleAppointmentWithConflictCheck()
{
	printf("\n--- Schedule Appointment ---\n");
	Appointment a;
	a.id = getNextId(APPOINTMENT_FILE, sizeof(Appointment));

	printf("Enter Patient ID: ");
	scanf("%d", &a.patient_id);
	clearStdin();
	if (!patientExists(a.patient_id))
	{
		printf("Patient not found.\n");
		return;
	}

	printf("Enter Doctor ID: ");
	scanf("%d", &a.doctor_id);
	clearStdin();
	if (!doctorExists(a.doctor_id))
	{
		printf("Doctor not found.\n");
		return;
	}

	printf("Enter appointment date/time (YYYY-MM-DD HH:MM): ");
	getInput(a.appointment_date, sizeof(a.appointment_date));

	if (hasConflict(a.doctor_id, a.appointment_date, 0))
	{
		printf("ERROR: Doctor already has an appointment at that time.\n");
		return;
	}

	printf("Enter reason: ");
	getInput(a.reason, sizeof(a.reason));
	strcpy(a.status, "Scheduled");

	FILE *fp = fopen(APPOINTMENT_FILE, "ab+");
	if (!fp)
	{
		perror("Error opening appointment file");
		return;
	}
	fwrite(&a, sizeof(Appointment), 1, fp);
	fclose(fp);
	printf("Appointment scheduled with ID %d\n", a.id);
}

// --- Analytics Menu ---
void analyticsMenu()
{
	int choice = -1;
	while (1)
	{
		printf("\n=== Analytics & Reports ===\n");
		printf("1) Total Patients & Doctors\n");
		printf("2) Total Appointments\n");
		printf("3) View All Bills & Revenue\n");
		printf("4) Doctor Appointment Count\n");
		printf("5) Back to Menu\n");
		printf("Choose: ");

		if (scanf("%d", &choice) != 1)
		{
			clearStdin();
			choice = -1;
		}
		clearStdin();

		switch (choice)
		{
		case 1:
			totalPatientsAndDoctors();
			break;
		case 2:
			totalAppointments();
			break;
		case 3:
			viewAllBills();
			break;
		case 4:
			doctorAppointmentCount();
			break;
		case 5:
			return;
		default:
			printf("Invalid choice.\n");
			break;
		}
	}
}

void totalPatientsAndDoctors()
{
	printf("\n--- System Statistics ---\n");

	FILE *pf = fopen(PATIENT_FILE, "rb");
	int patientCount = 0;
	if (pf)
	{
		fseek(pf, 0, SEEK_END);
		patientCount = ftell(pf) / sizeof(Patient);
		fclose(pf);
	}

	FILE *df = fopen(DOCTOR_FILE, "rb");
	int doctorCount = 0;
	if (df)
	{
		fseek(df, 0, SEEK_END);
		doctorCount = ftell(df) / sizeof(Doctor);
		fclose(df);
	}

	printf("Total Patients: %d\n", patientCount);
	printf("Total Doctors: %d\n", doctorCount);
}

void totalAppointments()
{
	printf("\n--- Appointment Statistics ---\n");

	FILE *ap = fopen(APPOINTMENT_FILE, "rb");
	if (!ap)
	{
		printf("No appointment data.\n");
		return;
	}

	Appointment a;
	int scheduled = 0, cancelled = 0, total = 0;

	while (fread(&a, sizeof(Appointment), 1, ap))
	{
		total++;
		if (strcmp(a.status, "Scheduled") == 0)
			scheduled++;
		else if (strcmp(a.status, "Cancelled") == 0)
			cancelled++;
	}

	fclose(ap);

	printf("Total Appointments: %d\n", total);
	printf("Scheduled: %d\n", scheduled);
	printf("Cancelled: %d\n", cancelled);
}

void doctorAppointmentCount()
{
	printf("\n--- Doctor Appointment Count ---\n");
	int doctorId;
	printf("Enter Doctor ID: ");
	scanf("%d", &doctorId);
	clearStdin();

	if (!doctorExists(doctorId))
	{
		printf("Doctor not found.\n");
		return;
	}

	FILE *ap = fopen(APPOINTMENT_FILE, "rb");
	if (!ap)
	{
		printf("No appointment data.\n");
		return;
	}

	Appointment a;
	int count = 0;

	while (fread(&a, sizeof(Appointment), 1, ap))
	{
		if (a.doctor_id == doctorId && strcmp(a.status, "Cancelled") != 0)
			count++;
	}

	fclose(ap);
	printf("Doctor ID %d has %d active appointments.\n", doctorId, count);
}

// --- Medical Records ---
void addMedicalRecord()
{
	printf("\n--- Add Medical Record ---\n");
	int app_id;
	printf("Enter Appointment ID: ");
	scanf("%d", &app_id);
	clearStdin();

	if (!appointmentExists(app_id))
	{
		printf("Appointment not found.\n");
		return;
	}

	MedicalRecord rec;
	rec.id = getNextId(MEDICAL_RECORD_FILE, sizeof(MedicalRecord));
	rec.appointment_id = app_id;

	printf("Enter Diagnosis: ");
	getInput(rec.diagnosis, sizeof(rec.diagnosis));
	printf("Enter Prescription: ");
	getInput(rec.prescription, sizeof(rec.prescription));

	FILE *fp = fopen(MEDICAL_RECORD_FILE, "ab");
	if (!fp)
	{
		perror("Error opening records file");
		return;
	}
	fwrite(&rec, sizeof(MedicalRecord), 1, fp);
	fclose(fp);

	printf("Medical record added successfully.\n");
}

void viewPatientMedicalHistory()
{
	printf("\n--- View Patient Medical History ---\n");
	int patient_id;
	printf("Enter Patient ID: ");
	scanf("%d", &patient_id);
	clearStdin();

	printf("\n--- Medical History for Patient ID: %d ---\n", patient_id);

	FILE *app_fp = fopen(APPOINTMENT_FILE, "rb");
	FILE *rec_fp = fopen(MEDICAL_RECORD_FILE, "rb");

	if (!app_fp || !rec_fp)
	{
		printf("Could not open data files.\n");
		if (app_fp)
			fclose(app_fp);
		if (rec_fp)
			fclose(rec_fp);
		return;
	}

	Appointment app;
	MedicalRecord rec;
	int records_found = 0;

	while (fread(&app, sizeof(Appointment), 1, app_fp))
	{
		if (app.patient_id == patient_id)
		{
			rewind(rec_fp);
			while (fread(&rec, sizeof(MedicalRecord), 1, rec_fp))
			{
				if (rec.appointment_id == app.id)
				{
					printf("\nAppointment ID: %d\n", app.id);
					printf("Date: %s\n", app.appointment_date);
					printf("Reason: %s\n", app.reason);
					printf("Diagnosis: %s\n", rec.diagnosis);
					printf("Prescription: %s\n", rec.prescription);
					records_found++;
					break;
				}
			}
		}
	}

	if (records_found == 0)
		printf("No medical records found for this patient.\n");

	fclose(app_fp);
	fclose(rec_fp);
}

// --- Billing ---
void generateBill(int appointmentId, int patientId, float amount)
{
	FILE *fp = fopen(BILL_FILE, "ab");
	if (!fp)
	{
		perror("Error opening bill file");
		return;
	}

	Bill b;
	b.id = getNextId(BILL_FILE, sizeof(Bill));
	b.appointment_id = appointmentId;
	b.patient_id = patientId;
	b.amount = amount;
	strcpy(b.status, "Unpaid");

	time_t now = time(NULL);
	struct tm *tm_info = localtime(&now);
	strftime(b.date, sizeof(b.date), "%Y-%m-%d", tm_info);

	fwrite(&b, sizeof(Bill), 1, fp);
	fclose(fp);
	printf("Bill generated: ID %d, Amount: $%.2f\n", b.id, amount);
}

void viewPatientBills(int patientId)
{
	FILE *fp = fopen(BILL_FILE, "rb");
	if (!fp)
	{
		printf("No bills found.\n");
		return;
	}

	Bill b;
	printf("\n%-5s %-15s %-10s %-12s %-10s\n",
		   "ID", "Appointment", "Amount", "Date", "Status");

	int found = 0;
	while (fread(&b, sizeof(Bill), 1, fp))
	{
		if (b.patient_id == patientId)
		{
			printf("%-5d %-15d $%-9.2f %-12s %-10s\n",
				   b.id, b.appointment_id, b.amount, b.date, b.status);
			found = 1;
		}
	}
	fclose(fp);

	if (!found)
		printf("No bills for this patient.\n");
}

void viewAllBills(void)
{
	FILE *fp = fopen(BILL_FILE, "rb");
	if (!fp)
	{
		printf("No bills found.\n");
		return;
	}

	Bill b;
	float totalRevenue = 0, totalUnpaid = 0;
	printf("\n%-5s %-10s %-10s %-10s %-12s %-10s\n",
		   "ID", "Patient", "Amount", "Appointment", "Date", "Status");

	while (fread(&b, sizeof(Bill), 1, fp))
	{
		printf("%-5d %-10d $%-9.2f %-10d %-12s %-10s\n",
			   b.id, b.patient_id, b.amount, b.appointment_id, b.date, b.status);
		totalRevenue += b.amount;
		if (strcmp(b.status, "Unpaid") == 0)
			totalUnpaid += b.amount;
	}
	fclose(fp);

	printf("\n--- Summary ---\n");
	printf("Total Revenue: $%.2f\n", totalRevenue);
	printf("Unpaid Amount: $%.2f\n", totalUnpaid);
}

// --- Search ---
void searchPatientByName(const char *name)
{
	FILE *fp = fopen(PATIENT_FILE, "rb");
	if (!fp)
	{
		printf("No patients found.\n");
		return;
	}

	Patient p;
	printf("\n%-5s %-15s %-5s %-10s %-15s %-20s\n",
		   "ID", "Name", "Age", "Gender", "Phone", "Disease");

	int found = 0;
	while (fread(&p, sizeof(Patient), 1, fp))
	{
		if (strstr(p.name, name) != NULL)
		{
			printf("%-5d %-15s %-5d %-10s %-15s %-20s\n",
				   p.id, p.name, p.age, p.gender, p.phone, p.disease);
			found = 1;
		}
	}
	fclose(fp);

	if (!found)
		printf("No patients found with name containing '%s'.\n", name);
}