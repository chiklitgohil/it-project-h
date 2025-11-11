Main Menu
    - Login
    - Signup
    - Exit

After login
    - Check user type → go to either Patient Menu or Doctor Menu.

Navigation conventions (applies to all menus)
    - Every submenu must include a "0. Back" option that returns the user to the previous screen.
    - Use numeric selections for menu choices. Use '0' to go back one level in the navigation stack.
    - Top-level menus (Main Menu) keep their original options (Exit remains available).
    - Input validation: invalid input redisplays the current menu with an error message and the same navigation options.

Patient Menu (top-level)
    1. Manage Appointments
        - 1. Book Appointment (collect medical history)
        - 2. View Appointments
        - 3. Update Appointment (reschedule, cancel)
        - 0. Back

    2. Reports
        - 1. View Reports
        - 0. Back

    3. Billing
        - 1. Make Payment
        - 2. View Bills
        - 0. Back

    4. Profile
        - 1. View Details
        - 2. Update Details
        - 0. Back

    5. Logout (returns to Main Menu)

Doctor Menu (top-level)
    1. Manage Appointments
        - 1. View Appointments
        - 2. Update Appointment (postpone, reassign)
        - 0. Back

    2. Profile
        - 1. View Details
        - 2. Update Details
        - 0. Back

    3. Logout (returns to Main Menu)

Submenu behavior and UX guidance
    - Use '0' to return to the immediately previous screen. Example: Main -> Patient Menu -> Manage Appointments -> Book Appointment: pressing '0' in Book Appointment returns to Manage Appointments; pressing '0' again returns to Patient Menu.
    - When presenting forms or prompts, allow the user to enter '0' at any point to cancel and return to the previous menu.
    - For destructive actions (cancel appointment, delete record), always ask a confirmation (y/n) before committing.
    - After a successful mutation (booking, payment, update), show a confirmation and remain in the current submenu so the user can continue or press '0' to go back.

Implementation notes
    - Keep a simple navigation stack (push when you enter a submenu, pop on '0') to guarantee correct back behavior.
    - For menus not yet implemented (e.g., billing), show a polite "Not implemented yet" message and allow the user to press '0' to go back.
    - Display a breadcrumb (e.g., "Patient > Manage Appointments > Book") to clarify where 'Back' will take the user.

Edge cases to handle
    - Empty lists: show "No items found" and offer '0. Back'.
    - Partial data entry: allow cancellation via '0' without saving partial data.
    - IO errors: show an error and keep the user on the same screen with the option to retry or go back.

Screen contract (compact)
    - Inputs: numeric menu selections and '0' for Back; text data for forms.
    - Outputs: menu display, confirmations, errors.
    - Error handling: redisplay the same menu with an error message on invalid input.


admin

3) View All Appointments and ability to assign doctor to patient
4) View Analytics
=== Analytics & Reports ===
1) Total Patients & Doctors
2) Total Appointments
3) View All Bills & Revenue
4) Doctor Appointment Count
5) Back to Menu



doctor
search by patient name
