#ifndef AUTH_H
#define AUTH_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "crud.h"

/* Authentication flows */
int patientSignup(void);
int doctorSignup(void);
int adminSignup(void);
int patientLogin(void);
int doctorLogin(void);
int adminLogin(void);
void patientPortal(int patientId);
void doctorPortal(int doctorId);
void adminPortal(int adminId);

/* Migration helper: move old credential files to new names if present */
void migrateCredentialFiles(void);

/* Utility - clearStdin is declared in crud.h, just use it from there */
void getInputAuth(char *buf, int size);
int validatePassword(const char *pwd);
void maskInput(char *buf, int size);

#endif /* AUTH_H */