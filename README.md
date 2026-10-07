# ClearDay Calendar Application

ClearDay is a desktop calendar application prototype developed in **C++ using wxWidgets**. The application provides an interactive interface for organizing events and viewing schedules across multiple calendar layouts.

## Features

* Create, edit, and delete events
* Daily, weekly, and monthly calendar views
* All-day and timed events
* Event descriptions
* Recurring events
* Multiple calendars
* Event overlap and conflict detection
* Keyword-based event search
* Calendar navigation
* Current-day highlighting
* Calendar customization and settings

## Technologies

* **C++**
* **wxWidgets**
* **Make**
* **Linux / WSL**
* **Git / GitLab**

## Project Structure

The application is organized into separate components for calendar data, date handling, and the graphical user interface.

Key components include:

* `Calendar` — manages calendar and event data
* `Date` — handles date-related functionality
* `CalendarView` — manages calendar visualization and navigation
* `main` — initializes and runs the application

## Building and Running

### Requirements

* C++ compiler
* wxWidgets
* Make
* Linux environment or WSL
* wxWidgets configured and installed

### Build

From the project directory:

```bash
make
```

### Run

```bash
./clearday
```

### Clean

To remove generated build files:

```bash
make clean
```

## Screenshots

Screenshots of the application will be added here.

## Development

ClearDay was developed as a collaborative software development project. The project focused on C++ application development, GUI design with wxWidgets, object-oriented programming, version control, and software engineering practices.

## Contributors

**CS3307 Group 02**

* Nikolas Kiroff
* Rosaline Liu
* Dinith Nawaratne
* Brandon Nguyen
* Uday Prashant
