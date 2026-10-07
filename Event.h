/**
 * @file Event.h
 * @brief Source file for calendar application components.
 * @author Nikolas
 * @author Rosaline
 * @author Uday
 * @author Brandon
 * @author Dinith
 */

#ifndef EVENT_H
#define EVENT_H

#include <string>
using namespace std;

/**
 * @brief Class Event.
 */
class Event {
private:
    string title;
    string location;
    string description;
    string date;
    string endDate;
    string startTime;
    string endTime;
    bool isAllDay;
    bool isRecurring;
    int repeatCount;
    string repeatType;
    string calendarName;

public:
/**
 * @brief Event method of Event.h.
 */
    Event();

    static Event createEvent(string title, string location, string description, 
        string date, string endDate, string startTime,
        string endTime, bool isAllDay, bool isRecurring, 
        int repeatCount, string repeatType, string calendarName = "");

    void editEvent(string title, string location, string description, 
            string date, string endDate, string startTime, // <-- ADDED endDate
            string endTime, bool isAllDay, bool isRecurring, 
            int repeatCount, string repeatType);

/**
 * @brief getDetails method of Event.h.
 * @return Result of the operation.
 */
    string getDetails() const;
/**
 * @brief getTitle method of Event.h.
 * @return Result of the operation.
 */
    string getTitle() const;
/**
 * @brief getLocation method of Event.h.
 * @return Result of the operation.
 */
    string getLocation() const;
/**
 * @brief getDescription method of Event.h.
 * @return Result of the operation.
 */
    string getDescription() const;
/**
 * @brief getDate method of Event.h.
 * @return Result of the operation.
 */
    string getDate() const;
/**
 * @brief getEndDate method of Event.h.
 * @return Result of the operation.
 */
    string getEndDate() const;
/**
 * @brief getStartTime method of Event.h.
 * @return Result of the operation.
 */
    string getStartTime() const;
/**
 * @brief getEndTime method of Event.h.
 * @return Result of the operation.
 */
    string getEndTime() const;
/**
 * @brief getIsAllDay method of Event.h.
 * @return Result of the operation.
 */
    bool getIsAllDay() const;
/**
 * @brief getIsRecurring method of Event.h.
 * @return Result of the operation.
 */
    bool getIsRecurring() const;
/**
 * @brief getRepeatCount method of Event.h.
 * @return Result of the operation.
 */
    int getRepeatCount() const;
/**
 * @brief getRepeatType method of Event.h.
 * @return Result of the operation.
 */
    string getRepeatType() const;
/**
 * @brief getCalendarName method of Event.h.
 * @return Result of the operation.
 */
    string getCalendarName() const;

/**
 * @brief setCalendarName method of Event.h.
 * @param name parameter for setCalendarName.
 */
    void setCalendarName(const string& name);

/**
 * @brief setAllDay method of Event.h.
 * @param status parameter for setAllDay.
 */
    void setAllDay(bool status);
};

#endif