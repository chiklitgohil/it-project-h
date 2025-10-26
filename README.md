# it-project-h

Project Overview
Objective
To develop a file-based Hospital Management System (HMS) in C that provides secure user authentication, efficient patient data management, appointment booking, and analytical reporting.
Description
The Hospital Management System is a terminal-based application that allows administrators and staff to manage hospital operations digitally.
Key operations include registering new patients, updating or removing records, booking doctor appointments, and generating reports on hospital usage trends.
The system will have two main user roles: Doctor (for viewing appointments, reports, and global settings) and Patient/User (for performing bookings, fetching their reports and bills).
All data will be stored persistently in structured text files to simulate a simple database. 3. Functional Components

1. Authentication & Role-Based Access
   • Patient & Doctor login systems with file-based credential storage (users.dat, doctors.dat)
   • Separate portals after login (role-specific menus & permissions)
   • Password validation, basic masking, and duplication checks on signup
2. Data Management (Patient, Doctor & Records CRUD)
   • Create, view, update, and delete patient/doctor data stored in .dat files
   • Validate fields (age, gender, phone, etc.) before saving
   • Assign unique IDs & maintain consistent file format for all modules
3. Appointment Scheduling System
   • Book, cancel, or reschedule appointments between patients and doctors
   • Conflict detection (no double-booking same doctor/time)
   • Availability checking and time-slot logic
4. Report & Billing Management
   • Doctors can add/update medical reports for patients
   • Patients can view reports & bills
   • Bills generated automatically after appointments, stored & retrievable
5. Search, Sorting & Analytics
   • Search records by ID, name, or doctor
   • Sort appointments or patients (by date, name, etc.)
   • Generate simple analytics (total patients, daily appointments, revenue summaries)
