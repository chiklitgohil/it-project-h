# Hospital Management System - Project Report

## Cover Page
- **Project Title**: Hospital Management System
- **Group ID**: [Your Group ID]
- **Submitted To**: [Professor Name]
- **Submission Date**: [Date]
- **Course**: [Course Name and Code]
- **Institution**: [Your Institution Name]

---

## Executive Summary
[Brief overview of the project, objectives achieved, and key features implemented. 200-300 words]

---

## Table of Contents
1. Introduction
2. Project Objectives
3. Functional Requirements
4. System Architecture
5. Implementation Details
6. Design Patterns & Data Structures
7. Team Members & Contributions
8. Screenshots
9. Testing & Results
10. Conclusion
11. Future Enhancements

---

## 1. Introduction

### 1.1 Problem Statement
[Describe the problem your system solves. Explain why hospital management is needed and what challenges it addresses]

### 1.2 Project Scope
[Define what is included and excluded from this project]

### 1.3 Objectives
- Develop a role-based hospital management system
- Implement patient, doctor, and admin functionalities
- Create an appointment scheduling system with conflict detection
- Build a medical records and billing management module
- Provide analytics and reporting capabilities
- Ensure data persistence using file-based storage

---

## 2. Project Objectives

### 2.1 Primary Objectives
- ✓ Implement authentication system with role-based access
- ✓ Create CRUD operations for patients and doctors
- ✓ Build appointment scheduling with conflict detection
- ✓ Develop medical records management
- ✓ Implement billing system with payment tracking
- ✓ Add search and analytics features

### 2.2 Secondary Objectives
- ✓ Admin portal for system management
- ✓ Indian currency (₹) support
- ✓ UPI payment integration
- ✓ Data persistence in binary format
- ✓ Input validation and error handling

---

## 3. Functional Requirements

### 3.1 Authentication & Authorization
**FR1**: User login system
- Three roles: Patient, Doctor, Admin
- Separate credential management for each role
- Password validation (min 6 chars, 1 digit, 1 letter)
- Duplicate username prevention

**FR2**: User signup system
- Self-registration for patients and doctors
- Admin account creation by existing admins
- Profile validation before account creation

### 3.2 Patient Management
**FR3**: Patient CRUD Operations
- Create new patient records
- View/search patient profiles
- Update patient information (age, phone, gender)
- Delete patient records (admin only)
- Data validation for age (1-120), phone (7+ digits)

### 3.3 Doctor Management
**FR4**: Doctor CRUD Operations
- Create new doctor records
- View/search doctor profiles by specialty
- Update doctor information
- Delete doctor records (admin only)

### 3.4 Appointment Management
**FR5**: Appointment Scheduling
- Schedule appointments between patients and doctors
- Validate appointment dates (future dates only)
- Reschedule appointments with conflict checking
- Cancel appointments
- Assign doctors to appointments
- Prevent double-booking (conflict detection)

### 3.5 Medical Records & Billing
**FR6**: Medical Records Management
- Doctors add diagnosis and prescription
- Patients view medical history
- Doctors view patient reports

**FR7**: Billing Management
- Automatic bill generation after appointments
- Admin-generated bills
- Track bill payment status (Paid/Unpaid)
- Mark bills as paid
- View bills with payment instructions
- Support for Indian Rupees (₹)

### 3.6 Search & Analytics
**FR8**: Search Functionality
- Search patients by name
- Search doctors by specialty
- Search appointments

**FR9**: Analytics
- Total patients and doctors count
- Total appointments count
- Doctor-specific appointment count
- Sort patients/doctors by name
- Sort appointments by date

---

## 4. System Architecture

### 4.1 High-Level Architecture Diagram
```
┌─────────────────────────────────────────────────┐
│           Main Menu (Login/Signup)              │
├─────────────────────────────────────────────────┤
│  ┌──────────────┬──────────────┬──────────────┐ │
│  │   PATIENT    │    DOCTOR    │     ADMIN    │ │
│  │   PORTAL     │   PORTAL     │   PORTAL     │ │
│  └──────────────┴──────────────┴──────────────┘ │
├─────────────────────────────────────────────────┤
│                                                 │
│  ┌────────────────────────────────────────────┐ │
│  │  Core Modules (Modular Architecture)       │ │
│  │  ┌──────────────────────────────────────┐  │ │
│  │  │ Patients │ Doctors │ Appointments  │  │ │
│  │  │ Med Recs │ Billing │ Analytics     │  │ │
│  │  └──────────────────────────────────────┘  │ │
│  └────────────────────────────────────────────┘ │
├─────────────────────────────────────────────────┤
│           File-Based Data Storage               │
│  (patients.dat, doctors.dat, appointments.dat)  │
└─────────────────────────────────────────────────┘
```

### 4.2 Module Dependencies
- **main.c** → Entry point
- **auth.c** → Authentication (depends on common.h)
- **patients.c** → Patient operations (depends on common.h)
- **doctors.c** → Doctor operations (depends on common.h)
- **appointments.c** → Appointments (depends on patients.h, doctors.h)
- **medical_records.c** → Medical records (depends on appointments.h)
- **analytics.c** → Analytics (depends on all modules)
- **common.c** → Utility functions

---

## 5. Implementation Details

### 5.1 Authentication Module (auth.c)

**Key Functions**:
- `int patientLogin()` - Patient login with credential verification
- `int patientSignup()` - New patient registration
- `int doctorLogin()` - Doctor login
- `int doctorSignup()` - Doctor registration
- `int adminLogin()` - Admin login
- `int adminSignup()` - Admin registration
- `int validatePassword()` - Password policy enforcement

**Implementation Details**:
[Explain how authentication works, how credentials are stored, verification process]

### 5.2 Patient Management Module (patients.c)

**Key Functions**:
- `void addPatient()` - Create new patient
- `void viewPatients()` - List all patients (sorted by name)
- `void updatePatient()` - Modify patient info
- `void deletePatient()` - Remove patient record
- `int patientExists()` - Verify patient existence

**Data Structure**:
```c
typedef struct {
    int id;
    char name[50];
    int age;
    char gender[10];
    char phone[15];
    char username[64];
    char password[64];
} Patient;
```

**Implementation Details**:
[Explain validation logic, sorting mechanism, file operations]

### 5.3 Doctor Management Module (doctors.c)

**Key Functions**:
- `void addDoctor()` - Create new doctor
- `void viewDoctors()` - List all doctors (sorted by name)
- `void updateDoctor()` - Modify doctor info
- `void deleteDoctor()` - Remove doctor record

**Data Structure**:
```c
typedef struct {
    int id;
    char name[50];
    char specialization[50];
    char phone[15];
    char username[64];
    char password[64];
} Doctor;
```

**Implementation Details**:
[Explain doctor-specific features, specialization handling]

### 5.4 Appointment Management Module (appointments.c)

**Key Functions**:
- `void scheduleAppointment()` - Book new appointment
- `void cancelAppointment()` - Remove appointment
- `void rescheduleAppointment()` - Change appointment date/time
- `void assignDoctorToPatient()` - Assign doctor to appointment
- `int hasConflict()` - Detect scheduling conflicts
- `int parseDateTime()` - Validate date/time format

**Data Structure**:
```c
typedef struct {
    int id;
    int patient_id;
    int doctor_id;
    char appointment_date[20];  // YYYY-MM-DD HH:MM
    char reason[100];
    char status[20];
} Appointment;
```

**Conflict Detection Algorithm**:
```
FOR EACH existing appointment:
    IF doctor_id matches AND appointment_date matches:
        RETURN conflict detected
    END IF
END FOR
RETURN no conflict
```

**Implementation Details**:
[Explain scheduling logic, conflict detection, date validation]

### 5.5 Medical Records & Billing Module (medical_records.c)

**Key Functions**:
- `void addMedicalRecord()` - Create diagnosis/prescription
- `void viewPatientMedicalHistory()` - View patient records
- `void generateBill()` - Create bill after appointment
- `void viewPatientBills()` - Display bills with payment instructions
- `void markBillAsPaid()` - Update bill status
- `void manageBilling()` - Billing menu interface

**Data Structures**:
```c
typedef struct {
    int id;
    int appointment_id;
    char diagnosis[100];
    char prescription[100];
} MedicalRecord;

typedef struct {
    int id;
    int appointment_id;
    int patient_id;
    float amount;
    char date[20];
    char status[20];  // Paid / Unpaid
} Bill;
```

**Implementation Details**:
[Explain medical record creation, bill generation, payment tracking]

### 5.6 Analytics Module (analytics.c)

**Key Functions**:
- `void searchPatientByName()` - Search patients
- `void analyticsMenu()` - Analytics interface
- `void totalPatientsAndDoctors()` - System statistics
- `void totalAppointments()` - Appointment count
- `void doctorAppointmentCount()` - Doctor-specific count

**Implementation Details**:
[Explain search algorithms, sorting, analytics calculations]

### 5.7 Utility Functions (common.c)

**Key Functions**:
- `void clearStdin()` - Clear input buffer
- `void getInput()` - Safe string input
- `int getNextId()` - Auto-generate unique IDs

**Implementation Details**:
[Explain how IDs are generated, buffer management]

---

## 6. Design Patterns & Data Structures

### 6.1 Design Patterns Used
- **MVC-like Pattern**: Separation of data models (structs) from business logic
- **Role-Based Access Control**: Different portals for different user roles
- **File I/O Pattern**: Efficient binary file reading/writing
- **Conflict Detection Pattern**: Algorithm to prevent data conflicts

### 6.2 Data Structures
- **Binary Structs**: Patient, Doctor, Appointment, MedicalRecord, Bill
- **Dynamic Arrays**: For sorting patients, doctors, appointments
- **File Pointers**: For persistent data storage
- **String Buffers**: For input validation and user interaction

### 6.3 Sorting & Search
- **Sorting Algorithm**: qsort() with custom comparators
- **Search Method**: Linear search for simplicity, optimized with early exit
- **Complexity**: O(n) for search, O(n log n) for sorting

---

## 7. Team Members & Contributions

### Team Member 1: [Name - Roll No: ____]
**Role**: [e.g., Backend Developer / Frontend Developer / Database Designer]

**Contributions**:
- [Describe specific modules/features implemented in your own words]
- [Explain the challenges faced and how you solved them]
- [Mention testing and debugging efforts]

### Team Member 2: [Name - Roll No: ____]
**Role**: [e.g., Project Manager / Documentation / Testing]

**Contributions**:
- [Describe specific modules/features implemented in your own words]
- [Explain the challenges faced and how you solved them]
- [Mention testing and debugging efforts]

### Team Member 3: [Name - Roll No: ____]
**Role**: [e.g., Architect / Systems Designer]

**Contributions**:
- [Describe specific modules/features implemented in your own words]
- [Explain the challenges faced and how you solved them]
- [Mention testing and debugging efforts]

---

## 8. Screenshots

### 8.1 Main Menu
[Insert screenshot of main login/signup screen]

### 8.2 Patient Portal
[Insert screenshots of patient features]
- Login screen
- Appointment booking
- View bills

### 8.3 Doctor Portal
[Insert screenshots of doctor features]
- Appointments
- Medical records

### 8.4 Admin Portal
[Insert screenshots of admin features]
- Patient management
- Bill generation
- Analytics

---

## 9. Testing & Results

### 9.1 Test Cases

| Test ID | Feature | Test Case | Expected Result | Actual Result | Status |
|---------|---------|-----------|-----------------|---------------|--------|
| TC-1 | Login | Valid credentials | Login successful | Login successful | ✓ PASS |
| TC-2 | Login | Invalid credentials | Login failed | Login failed | ✓ PASS |
| TC-3 | Signup | Valid data | Account created | Account created | ✓ PASS |
| TC-4 | Signup | Duplicate username | Signup rejected | Signup rejected | ✓ PASS |
| TC-5 | Appointment | Schedule valid | Appointment created | Appointment created | ✓ PASS |
| TC-6 | Appointment | Conflict detection | Booking rejected | Booking rejected | ✓ PASS |
| TC-7 | Billing | Generate bill | Bill created | Bill created | ✓ PASS |
| TC-8 | Analytics | Total count | Count displayed | Count displayed | ✓ PASS |

### 9.2 Results Summary
[Explain overall test results, success rate, any issues found and resolved]

---

## 10. Conclusion

### 10.1 Achievements
- Successfully implemented all required functional components
- Achieved role-based access control for three user types
- Implemented conflict detection in appointment scheduling
- Created comprehensive file-based data persistence
- Added localization for Indian currency and payment methods

### 10.2 Learning Outcomes
[Discuss what you learned from this project, technical skills developed, challenges overcome]

### 10.3 Challenges & Solutions
| Challenge | Solution |
|-----------|----------|
| File-based data management | Implemented binary struct serialization |
| Conflict detection | Created hasConflict() algorithm |
| Input validation | Built comprehensive validation functions |
| Modular architecture | Separated concerns into independent modules |

---

## 11. Future Enhancements

- [ ] Database integration (MySQL/PostgreSQL)
- [ ] Web-based UI using HTML/CSS/JavaScript
- [ ] Email notifications for appointments
- [ ] SMS reminders for patients
- [ ] Online payment gateway integration
- [ ] Prescription printing functionality
- [ ] Doctor availability calendar
- [ ] Patient history analytics
- [ ] Appointment history and statistics
- [ ] Multi-location support
- [ ] Role-based audit logging
- [ ] Data backup and recovery mechanisms

---

## References
[List any external resources, libraries, or documentation used]

---

## Appendix A: File Compilation Instructions
```
gcc *.c -o hospital_management_system.exe
```

## Appendix B: File Structure
[Provide detailed file organization as shown in README]

## Appendix C: Data File Format
[Explain binary format of .dat files if needed]

---

**End of Report**
