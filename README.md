# ClearDay Calendar Application

ClearDay is a desktop calendar application prototype developed in **C++ using wxWidgets**. The application provides an interactive interface for managing events, viewing schedules, organizing tasks, and configuring calendar settings.

## Features

* Create, edit, and delete calendar events
* Daily, weekly, and monthly calendar views
* All-day and timed events
* Event descriptions
* Recurring events
* Multiple calendars
* Event overlap and conflict detection
* Keyword-based event search
* Calendar navigation
* Current-day highlighting
* Task and checklist management
* Calendar settings and customization
* Weather information integration

## Technologies

* **C++**
* **wxWidgets**
* **Make**
* **Linux / WSL**
* **Git / GitLab**

## Project Structure

The application is divided into several components responsible for calendar management, event handling, tasks, settings, and the graphical user interface.

| Component      | Description                                                   |
| -------------- | ------------------------------------------------------------- |
| `Calendar`     | Manages calendar data and calendar-related functionality      |
| `CalendarView` | Handles the visual calendar interface and calendar navigation |
| `Checklist`    | Provides checklist functionality for managing tasks           |
| `Date`         | Handles date-related functionality                            |
| `Event`        | Represents and manages calendar events                        |
| `Main`         | Initializes and runs the application                          |
| `Settings`     | Manages application and calendar settings                     |
| `Tasks`        | Handles task-related functionality                            |
| `WeatherAPI`   | Handles integration with weather information                  |

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

ClearDay was developed as a collaborative software development project for **CS3307**. The project involved C++ application development, GUI development using wxWidgets, object-oriented programming, software design, and collaborative version control.

## Contributors

**CS3307 Group 02**

* Nikolas Kiroff
* Rosaline Liu
* Dinith Nawaratne
* Brandon Nguyen
* Uday Prashant

## My Contributions

As a member of CS3307 Group 02, I contributed to the development of the ClearDay application with a focus on the graphical user interface and user-facing functionality.

* Designed and implemented the application's graphical user interface using wxWidgets
* Implemented calendar view highlighting to improve visual navigation and identify the selected/current day
* Implemented the weather API integration to display weather information within the application
* Contributed to the overall UI layout, usability, and visual presentation of the calendar
