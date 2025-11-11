#ifndef AUTH_H
#define AUTH_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "crud.h"

/* Credential files */
#define USER_CRED_FILE "data/users.dat"
#define DOCTOR_CRED_FILE "data/doctor_creds.dat" /* changed: separate file for doctor credentials */

typedef struct Credential
{
    int id; /* patient or doctor id */
    char username[64];
    char password[64]; /* stored in plain text for this simple project */
} Credential;

/* Authentication flows */
int patientSignup(void);
int doctorSignup(void);
int patientLogin(void);
int doctorLogin(void);
void patientPortal(int patientId);
void doctorPortal(int doctorId);

/* Utility - clearStdin is declared in crud.h, just use it from there */
void getInputAuth(char *buf, int size);
int validatePassword(const char *pwd);
void maskInput(char *buf, int size);

#endif /* AUTH_H */