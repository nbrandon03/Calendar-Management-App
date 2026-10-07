/**
 * @file Calendar.h
 * @brief Source file for calendar application components.
 * @author Nikolas
 * @author Rosaline
 * @author Uday
 * @author Brandon
 * @author Dinith
 */

#ifndef CALENDAR_H
#define CALENDAR_H

#include "Date.h"
#include "Event.h"
#include <string>
#include <vector>

using namespace std;

enum ViewMode {
    DAY_VIEW,
    WEEK_VIEW,
    MONTH_VIEW
};

struct CalendarMeta {
    string name;
    string colour;
    bool visible;

    CalendarMeta(const string& n = "Personal",
                 const string& c = "#0AAAFF",
                 bool v = true) : name(n), colour(c), visible(v) {}
};

/**
 * @brief Class Calendar.
 */
class Calendar {
private:
    Date currentDate;
    ViewMode currentView;

    vector<Event> events;
    vector<CalendarMeta> calendarList;

/**
 * @brief recurringOccursOn method of Calendar.h.
 * @param e parameter for recurringOccursOn.
 * @param date parameter for recurringOccursOn.
 * @return Result of the operation.
 */
    bool recurringOccursOn(const Event& e, const string& date) const;

public:
/**
 * @brief Calendar method of Calendar.h.
 */
    Calendar();

/**
 * @brief getCurrentDate method of Calendar.h.
 * @return Result of the operation.
 */
    Date getCurrentDate() const;
/**
 * @brief getCurrentView method of Calendar.h.
 * @return Result of the operation.
 */
    ViewMode getCurrentView() const;

/**
 * @brief setView method of Calendar.h.
 * @param view parameter for setView.
 */
    void setView(ViewMode view);

/**
 * @brief goForward method of Calendar.h.
 */
    void goForward();
/**
 * @brief goBackward method of Calendar.h.
 */
    void goBackward();

    // Event management
/**
 * @brief addEvent method of Calendar.h.
 * @param e parameter for addEvent.
 */
    void addEvent(const Event& e);
/**
 * @brief removeEvent method of Calendar.h.
 * @param title parameter for removeEvent.
 * @param calName parameter for removeEvent.
 */
    void removeEvent(const string& title, const string& calName = "");

/**
 * @brief getEventsForDate method of Calendar.h.
 * @param date parameter for getEventsForDate.
 * @return Result of the operation.
 */
    vector<Event> getEventsForDate(const string& date) const;

    bool detectConflict(const string& date,
                        const string& startTime,
                        const string& endTime,
                        const string& excludeTitle = "") const;

    vector<Event> getConflictingEvents(const string& date,
                                       const string& startTime,
                                       const string& endTime,
                                       const string& excludeTitle = "") const;

/**
 * @brief searchEvents method of Calendar.h.
 * @param keyword parameter for searchEvents.
 * @return Result of the operation.
 */
    vector<Event> searchEvents(const string& keyword) const;

/**
 * @brief clearSearch method of Calendar.h.
 */
    void clearSearch();

    // Calendar management for named calendars
/**
 * @brief addCalendar method of Calendar.h.
 * @param cal parameter for addCalendar.
 */
    void addCalendar(const CalendarMeta& cal);
/**
 * @brief removeCalendar method of Calendar.h.
 * @param name parameter for removeCalendar.
 */
    void removeCalendar(const std::string& name);
/**
 * @brief editCalendar method of Calendar.h.
 * @param oldName parameter for editCalendar.
 * @param updated parameter for editCalendar.
 */
    void editCalendar(const std::string& oldName, const CalendarMeta& updated);

/**
 * @brief getCalendars method of Calendar.h.
 * @return Result of the operation.
 */
    const vector<CalendarMeta>& getCalendars() const;
/**
 * @brief getColourForCalendar method of Calendar.h.
 * @param calName parameter for getColourForCalendar.
 * @return Result of the operation.
 */
    std::string getColourForCalendar(const std::string& calName) const;
/**
 * @brief setCalendarVisible method of Calendar.h.
 * @param name parameter for setCalendarVisible.
 * @param visible parameter for setCalendarVisible.
 */
    void setCalendarVisible(const std::string& name, bool visible);
/**
 * @brief isCalendarVisible method of Calendar.h.
 * @param name parameter for isCalendarVisible.
 * @return Result of the operation.
 */
    bool isCalendarVisible(const std::string& name) const;

/**
 * @brief saveCalendars method of Calendar.h.
 * @param filename parameter for saveCalendars.
 */
    void saveCalendars(const string& filename = "calendars.txt") const;
/**
 * @brief loadCalendars method of Calendar.h.
 * @param filename parameter for loadCalendars.
 */
    void loadCalendars(const string& filename = "calendars.txt");

/**
 * @brief saveEvents method of Calendar.h.
 * @param filename parameter for saveEvents.
 */
    void saveEvents(const string& filename = "events.txt") const;
/**
 * @brief loadEvents method of Calendar.h.
 * @param filename parameter for loadEvents.
 */
    void loadEvents(const string& filename = "events.txt");
/**
 * @brief setCurrentDate method of Calendar.h.
 * @param date parameter for setCurrentDate.
 */
    void setCurrentDate(const Date& date);

};

#endif