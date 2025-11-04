
#include "booking.h"

#include <stdio.h>
#include <string.h>

/*
 * Simple implementation of appointment scheduling using fixed-size arrays.
 * This file intentionally keeps logic straightforward and includes many
 * comments so it's easy to follow and integrate into small projects.
 */

/* Internal storage for appointments. We keep a simple array and an
 * incremental id generator. No dynamic memory is used. */
static Appointment appointments[MAX_APPOINTMENTS];
static size_t appointments_count = 0; /* number of entries ever used (including inactive) */
static int next_appointment_id = 1;

/* Helper: find index in appointments[] by appointment id. Returns -1 if not found. */
static int findAppointmentIndexById(int appointment_id)
{
	if (appointment_id <= 0) return -1;
	for (size_t i = 0; i < MAX_APPOINTMENTS; ++i) {
		if (appointments[i].active && appointments[i].id == appointment_id) {
			return (int)i;
		}
	}
	return -1;
}

/* Helper: check whether given doctor has a conflicting appointment on date+slot.
 * If ignore_id >= 0, that appointment id will be ignored (useful for reschedule).
 */
static bool hasConflict(int doctor_id, const char *date, TimeSlot slot, int ignore_id)
{
	for (size_t i = 0; i < MAX_APPOINTMENTS; ++i) {
		if (!appointments[i].active) continue;
		if (appointments[i].id == ignore_id) continue;
		if (appointments[i].doctor_id == doctor_id &&
			strcmp(appointments[i].date, date) == 0 &&
			appointments[i].slot == slot) {
			return true;
		}
	}
	return false;
}

void initBookingSystem(void)
{
	/* Mark all appointments inactive and reset counters. Call before using API. */
	for (size_t i = 0; i < MAX_APPOINTMENTS; ++i) {
		appointments[i].active = false;
		appointments[i].id = 0;
	}
	appointments_count = 0;
	next_appointment_id = 1;
}

BookResult bookAppointment(int doctor_id, int patient_id, const char *date, TimeSlot slot, int *out_id)
{
	/* Basic parameter validation */
	if (doctor_id < 0 || doctor_id >= MAX_DOCTORS ||
		patient_id < 0 || patient_id >= MAX_PATIENTS ||
		date == NULL || slot < 0 || slot > 23 || out_id == NULL) {
		return BOOK_ERR_INVALID;
	}

	/* Check for conflict: no two active appointments for same doctor/date/slot */
	if (hasConflict(doctor_id, date, slot, -1)) {
		return BOOK_ERR_CONFLICT;
	}

	/* Find first free slot in internal array */
	int free_index = -1;
	for (size_t i = 0; i < MAX_APPOINTMENTS; ++i) {
		if (!appointments[i].active) {
			free_index = (int)i;
			break;
		}
	}
	if (free_index == -1) {
		/* no space left */
		return BOOK_ERR_FULL;
	}

	/* Fill appointment record */
	Appointment *a = &appointments[free_index];
	a->id = next_appointment_id++;
	a->doctor_id = doctor_id;
	a->patient_id = patient_id;
	/* Safe copy of date string; ensure null termination */
	strncpy(a->date, date, DATE_STR_LEN - 1);
	a->date[DATE_STR_LEN - 1] = '\0';
	a->slot = slot;
	a->active = true;

	if ((size_t)free_index >= appointments_count) appointments_count = free_index + 1;

	*out_id = a->id;
	return BOOK_OK;
}

BookResult cancelAppointment(int appointment_id)
{
	int idx = findAppointmentIndexById(appointment_id);
	if (idx < 0) return BOOK_ERR_NOT_FOUND;
	/* Mark inactive */
	appointments[idx].active = false;
	return BOOK_OK;
}

BookResult rescheduleAppointment(int appointment_id, const char *new_date, TimeSlot new_slot)
{
	if (new_date == NULL || new_slot < 0 || new_slot > 23) return BOOK_ERR_INVALID;
	int idx = findAppointmentIndexById(appointment_id);
	if (idx < 0) return BOOK_ERR_NOT_FOUND;

	Appointment *a = &appointments[idx];

	/* Check conflict for the same doctor, but ignore this appointment's id */
	if (hasConflict(a->doctor_id, new_date, new_slot, a->id)) {
		return BOOK_ERR_CONFLICT;
	}

	/* Update fields */
	strncpy(a->date, new_date, DATE_STR_LEN - 1);
	a->date[DATE_STR_LEN - 1] = '\0';
	a->slot = new_slot;
	return BOOK_OK;
}

bool isDoctorAvailable(int doctor_id, const char *date, TimeSlot slot)
{
	if (doctor_id < 0 || doctor_id >= MAX_DOCTORS || date == NULL || slot < 0 || slot > 23) {
		/* Invalid parameters -> treat as not available */
		return false;
	}
	return !hasConflict(doctor_id, date, slot, -1);
}

void printAppointments(void)
{
	puts("Appointments:");
	bool any = false;
	for (size_t i = 0; i < MAX_APPOINTMENTS; ++i) {
		if (!appointments[i].active) continue;
		any = true;
		printf("  id=%d doctor=%d patient=%d date=%s slot=%02d:00\n",
			   appointments[i].id,
			   appointments[i].doctor_id,
			   appointments[i].patient_id,
			   appointments[i].date,
			   appointments[i].slot);
	}
	if (!any) puts("  (no active appointments)");
}

const Appointment *getAppointments(size_t *out_count)
{
	if (out_count) *out_count = appointments_count;
	return appointments;
}

/*
 * Optional: small self-test when compiled with -DBOOKING_SELF_TEST
 * This does not run by default; it's for quick manual testing only.
 */
#ifdef BOOKING_SELF_TEST
#include <assert.h>
int main(void)
{
	initBookingSystem();
	int id1, id2;
	BookResult r;
	r = bookAppointment(0, 0, "2025-11-04", 9, &id1);
	assert(r == BOOK_OK);
	r = bookAppointment(0, 1, "2025-11-04", 9, &id2);
	assert(r == BOOK_ERR_CONFLICT);
	r = bookAppointment(0, 1, "2025-11-04", 10, &id2);
	assert(r == BOOK_OK);
	printAppointments();
	r = rescheduleAppointment(id2, "2025-11-04", 9);
	assert(r == BOOK_ERR_CONFLICT);
	r = cancelAppointment(id1);
	assert(r == BOOK_OK);
	r = rescheduleAppointment(id2, "2025-11-04", 9);
	assert(r == BOOK_OK);
	printAppointments();
	puts("Self-test passed");
	return 0;
}
#endif /* BOOKING_SELF_TEST */
