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
* Current-day and selected-day highlighting
* Task and checklist management
* Calendar settings and customization
* Weather information through an external weather API

## Technologies

* **C++**
* **wxWidgets**
* **Make**
* **Linux / WSL**
* **Git / GitLab**
* **Weather API**

## Project Structure

The application is organized into separate components for calendar management, event handling, date processing, tasks, settings, and the graphical user interface.

| Component      | Description                                                                |
| -------------- | -------------------------------------------------------------------------- |
| `Calendar`     | Manages calendar data and calendar-related functionality                   |
| `CalendarView` | Handles the visual calendar interface, views, navigation, and highlighting |
| `Checklist`    | Provides checklist functionality for managing tasks                        |
| `Date`         | Handles date-related functionality and date processing                     |
| `Event`        | Represents and manages calendar events                                     |
| `Main`         | Initializes and runs the application                                       |
| `Settings`     | Manages application and calendar settings                                  |
| `Tasks`        | Handles task-related functionality                                         |
| `WeatherAPI`   | Handles integration with external weather information                      |

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

## My Contributions

As a member of **CS3307 Group 02**, I contributed to the development of ClearDay with a focus on the application's user interface, calendar visualization, date functionality, and weather integration.

### User Interface

* Designed and implemented the graphical user interface using **wxWidgets**
* Developed and organized UI components for the calendar application
* Contributed to the overall layout, usability, and visual presentation of the application

### Calendar Views

* Implemented calendar view highlighting to visually identify the selected and current day
* Contributed to the presentation and navigation of calendar views

### Date Handling

* Developed date handling functionality used throughout the calendar application
* Contributed to the application's handling and presentation of calendar dates

### Weather API

* Implemented integration with an external weather API
* Connected weather information to the calendar application
* Contributed to displaying weather information within the application's interface

## Development

ClearDay was developed as a collaborative software development project for **CS3307**. The project involved C++ application development, GUI development using wxWidgets, object-oriented programming, software design, API integration, and collaborative version control.

## Contributors

**CS3307 Group 02**

* Nikolas Kiroff
* Rosaline Liu
* Dinith Nawaratne
* Brandon Nguyen
* Uday Prashant
