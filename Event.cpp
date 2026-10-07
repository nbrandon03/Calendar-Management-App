/**
 * @file Event.cpp
 * @brief Source file for calendar application components.
 * @author Nikolas
 * @author Rosaline
 * @author Uday
 * @author Brandon
 * @author Dinith
 */

#include "Event.h"
#include <sstream>

using namespace std;

/**
 * @brief Event method of Event.cpp.
 */
Event::Event() {
    title = "";
    location = "";
    description = "";
    date = "";
    startTime = "";
    endTime = "";
    isAllDay = false;
    isRecurring = false;
    repeatCount = 0;
    repeatType = "";
    calendarName = "";
}

Event Event::createEvent(string title,
                         string location,
                         string description,
                         string date,
                         string endDate,
                         string startTime,
                         string endTime,
                         bool isAllDay,
                         bool isRecurring,
                         int repeatCount,
                         string repeatType,
                         string calendarName) {

    Event e;

    e.title = title;
    e.location = location;
    e.description = description;
    e.date = date;
    e.endDate = endDate;
    e.startTime = startTime;
    e.endTime = endTime;
    e.isAllDay = isAllDay;
    e.isRecurring = isRecurring;
    e.repeatCount = repeatCount;
    e.repeatType = repeatType;
    e.calendarName = calendarName;

    return e;
}

void Event::editEvent(string title, string location, string description, 
                    string date, string endDate, string startTime, // <-- Added string endDate
                    string endTime, bool isAllDay, bool isRecurring, 
                    int repeatCount, string repeatType) {
    this->title = title;
    this->location = location;
    this->description = description;
    this->date = date;
    this->endDate = endDate;
    this->startTime = startTime;
    this->endTime = endTime;
    this->isAllDay = isAllDay;
    this->isRecurring = isRecurring;
    this->repeatCount = repeatCount;
    this->repeatType = repeatType;
}

/**
 * @brief getDetails method of Event.cpp.
 * @return Result of the operation.
 */
string Event::getDetails() const {

    stringstream ss;

    ss << "Title: " << title << "\n";
    ss << "Location: " << location << "\n";
    ss << "Description: " << description << "\n";
    ss << "Date: " << date << "\n";
    ss << "Start Time: " << startTime << "\n";
    ss << "End Time: " << endTime << "\n";
    ss << "All Day: " << (isAllDay ? "Yes" : "No") << "\n";
    ss << "Recurring: " << (isRecurring ? "Yes" : "No") << "\n";
    ss << "Repeat Count: " << repeatCount << "\n";
    ss << "Repeat Type: " << repeatType << "\n";

    return ss.str();
}

/**
 * @brief getTitle method of Event.cpp.
 * @return Result of the operation.
 */
string Event::getTitle() const { return title; }
/**
 * @brief getLocation method of Event.cpp.
 * @return Result of the operation.
 */
string Event::getLocation() const { return location; }       // <-- ADDED
/**
 * @brief getDescription method of Event.cpp.
 * @return Result of the operation.
 */
string Event::getDescription() const { return description; } // <-- ADDED
/**
 * @brief getDate method of Event.cpp.
 * @return Result of the operation.
 */
string Event::getDate() const { return date; }
/**
 * @brief getEndDate method of Event.cpp.
 * @return Result of the operation.
 */
string Event::getEndDate() const { return endDate; }
/**
 * @brief getStartTime method of Event.cpp.
 * @return Result of the operation.
 */
string Event::getStartTime() const { return startTime; }
/**
 * @brief getEndTime method of Event.cpp.
 * @return Result of the operation.
 */
string Event::getEndTime() const { return endTime; }
/**
 * @brief getIsAllDay method of Event.cpp.
 * @return Result of the operation.
 */
bool Event::getIsAllDay() const { return isAllDay; }
/**
 * @brief getIsRecurring method of Event.cpp.
 * @return Result of the operation.
 */
bool Event::getIsRecurring() const { return isRecurring; }
/**
 * @brief getRepeatCount method of Event.cpp.
 * @return Result of the operation.
 */
int Event::getRepeatCount() const { return repeatCount; }
/**
 * @brief getRepeatType method of Event.cpp.
 * @return Result of the operation.
 */
string Event::getRepeatType() const { return repeatType; }
/**
 * @brief getCalendarName method of Event.cpp.
 * @return Result of the operation.
 */
string Event::getCalendarName() const { return calendarName; }

/**
 * @brief setCalendarName method of Event.cpp.
 * @param name parameter for setCalendarName.
 */
void Event::setCalendarName(const string& name) {
    calendarName = name;
}

/**
 * @brief setAllDay method of Event.cpp.
 * @param status parameter for setAllDay.
 */
void Event::setAllDay(bool status) {
    isAllDay = status;
}