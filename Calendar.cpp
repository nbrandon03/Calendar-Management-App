/**
 * @file Calendar.cpp
 * @brief Source file for calendar application components.
 * @author Nikolas
 * @author Rosaline
 * @author Uday
 * @author Brandon
 * @author Dinith
 */

#include "Calendar.h"
#include <algorithm> // Ready to go functions to be used for vectors
#include <fstream>
#include <sstream>
#include <set>

// Parse "d/m/yyyy" into day, month, year integers, and returns false if fails
/**
 * @brief parseDMY method of Calendar.cpp.
 * @param s parameter for parseDMY.
 * @param day parameter for parseDMY.
 * @param month parameter for parseDMY.
 * @param year parameter for parseDMY.
 * @return Result of the operation.
 */
static bool parseDMY(const std::string& s, int& day, int& month, int& year) {
    size_t p1 = s.find('/');
    if (p1 == std::string::npos) return false;
    size_t p2 = s.find('/', p1 + 1);
    if (p2 == std::string::npos) return false;
    try {
        day   = std::stoi(s.substr(0, p1));
        month = std::stoi(s.substr(p1 + 1, p2 - p1 - 1));
        year  = std::stoi(s.substr(p2 + 1));
    } catch (...) { return false; }
    return true;
}

// Convert d/m/yyyy to a Julian Day Number for simple date arithmetic
/**
 * @brief toJulian method of Calendar.cpp.
 * @param d parameter for toJulian.
 * @param m parameter for toJulian.
 * @param y parameter for toJulian.
 * @return Result of the operation.
 */
static long toJulian(int d, int m, int y) {
    if (m <= 2) { y--; m += 12; }
    long A = y / 100;
    long B = 2 - A + A / 4;
    return (long)(365.25 * (y + 4716)) + (long)(30.6001 * (m + 1)) + d + B - 1524;
}

/**
 * @brief Calendar method of Calendar.cpp.
 */
Calendar::Calendar() {
    currentDate = Date();
    currentView = MONTH_VIEW;
    calendarList.push_back(CalendarMeta("Personal", "#0AAAFF", true));
}

/**
 * @brief getCurrentDate method of Calendar.cpp.
 * @return Result of the operation.
 */
Date Calendar::getCurrentDate() const {
    return currentDate;
}

/**
 * @brief getCurrentView method of Calendar.cpp.
 * @return Result of the operation.
 */
ViewMode Calendar::getCurrentView() const {
    return currentView;
}

/**
 * @brief setView method of Calendar.cpp.
 * @param view parameter for setView.
 */
void Calendar::setView(ViewMode view) {
    currentView = view;
}

/**
 * @brief goForward method of Calendar.cpp.
 */
void Calendar::goForward() {
    if (currentView == MONTH_VIEW) {
        int m = currentDate.getMonth();
        int y = currentDate.getYear();
        if (m == 12) { m = 1; y++; } else { m++; }
        currentDate = Date(1, m, y);
    } else if (currentView == WEEK_VIEW) {
        currentDate = currentDate.addDays(7);
    } else {
        currentDate = currentDate.addDays(1);
    }
}

/**
 * @brief goBackward method of Calendar.cpp.
 */
void Calendar::goBackward() {
    if (currentView == MONTH_VIEW) {
        int m = currentDate.getMonth();
        int y = currentDate.getYear();
        if (m == 1) { m = 12; y--; } else { m--; }
        currentDate = Date(1, m, y);
    } else if (currentView == WEEK_VIEW) {
        currentDate = currentDate.addDays(-7);
    } else {
        currentDate = currentDate.addDays(-1);
    }
}

/**
 * @brief addEvent method of Calendar.cpp.
 * @param e parameter for addEvent.
 */
void Calendar::addEvent(const Event& e) {
    events.push_back(e);
}

/**
 * @brief removeEvent method of Calendar.cpp.
 * @param title parameter for removeEvent.
 * @param calName parameter for removeEvent.
 */
void Calendar::removeEvent(const string& title, const string& calName) {
    events.erase(
/**
 * @brief remove_if method of Calendar.cpp.
 * @param events.begin() parameter for remove_if.
 * @param events.end() parameter for remove_if.
 * @param e parameter for remove_if.
 * @return Result of the operation.
 */
        remove_if(events.begin(), events.end(), [&](const Event& e) {
            bool titleMatch = (e.getTitle() == title);
            bool calMatch   = calName.empty() || (e.getCalendarName() == calName);
            return titleMatch && calMatch;
        }),
        events.end());
}

/**
 * @brief recurringOccursOn method of Calendar.cpp.
 * @param e parameter for recurringOccursOn.
 * @param date parameter for recurringOccursOn.
 * @return Result of the operation.
 */
bool Calendar::recurringOccursOn(const Event& e, const std::string& date) const {
    if (!e.getIsRecurring()) return false;

    int bd, bm, by;
    if (!parseDMY(e.getDate(), bd, bm, by)) return false;

    int td, tm, ty;
    if (!parseDMY(date, td, tm, ty)) return false;

    long baseJ   = toJulian(bd, bm, by);
    long targetJ = toJulian(td, tm, ty);

    if (targetJ <= baseJ) return false; // target must be strictly after base

    std::string rtype = e.getRepeatType();
    int count = e.getRepeatCount(); // 0 = infinite

    if (rtype == "daily") {
        long diff = targetJ - baseJ;
        // diff is the occurrence number (1st repeat = diff 1, 2nd = diff 2...)
        if (count > 0 && diff > count) return false;
        return true;
    }

    if (rtype == "weekly") {
        long diff = targetJ - baseJ;
        if (diff % 7 != 0) return false;
        long occurrence = diff / 7; // 1st repeat = 1, 2nd = 2...
        if (count > 0 && occurrence > count) return false;
        return true;
    }

    if (rtype == "monthly") {
        if (td != bd) return false; // must fall on the same day-of-month
        int monthsElapsed = (ty - by) * 12 + (tm - bm);
        if (monthsElapsed <= 0) return false;
        // monthsElapsed is the occurrence number
        if (count > 0 && monthsElapsed > count) return false;
        return true;
    }

    return false;
}

/**
 * @brief getEventsForDate method of Calendar.cpp.
 * @param date parameter for getEventsForDate.
 * @return Result of the operation.
 */
vector<Event> Calendar::getEventsForDate(const string& date) const {
    // First, collect all cancelled occurrence markers for this date
    std::set<std::string> cancelledTitles;
    for (const Event& e : events) {
        if (e.getTitle().substr(0, 13) == "__CANCELLED__" && e.getDate() == date) {
            cancelledTitles.insert(e.getTitle().substr(13));
        }
    }

    vector<Event> result;
    for (const Event& e : events) {
        // Skip internal cancellation markers
        if (e.getTitle().substr(0, 13) == "__CANCELLED__") continue;

        // Skip events belonging to hidden calendars
        if (!isCalendarVisible(e.getCalendarName())) continue;

        if (e.getDate() == date) {
            // Skip if this occurrence was individually cancelled
            if (cancelledTitles.count(e.getTitle())) continue;
            result.push_back(e);
        } else if (e.getIsRecurring() && recurringOccursOn(e, date)) {
            // Skip if this occurrence was individually cancelled
            if (cancelledTitles.count(e.getTitle())) continue;
            // Make a virtual copy dated to this occurrence only
            Event copy = e;
            copy.editEvent(e.getTitle(), e.getLocation(), e.getDescription(),
                           date, date,
                           e.getStartTime(), e.getEndTime(),
                           e.getIsAllDay(), true,
                           e.getRepeatCount(), e.getRepeatType());
            copy.setCalendarName(e.getCalendarName());
            result.push_back(copy);
        }
    }
    return result;
}

bool Calendar::detectConflict(const string& date,
                               const string& startTime,
                               const string& endTime,
                               const string& excludeTitle) const {
    for (const Event& e : events) {
        if (!excludeTitle.empty() && e.getTitle() == excludeTitle) continue;
        if (!isCalendarVisible(e.getCalendarName())) continue;
        bool onDate = (e.getDate() == date) ||
                      (e.getIsRecurring() && recurringOccursOn(e, date));
        if (onDate && !e.getIsAllDay()) {
            if (!(endTime <= e.getStartTime() || startTime >= e.getEndTime()))
                return true;
        }
    }
    return false;
}

vector<Event> Calendar::getConflictingEvents(const string& date,
                                              const string& startTime,
                                              const string& endTime,
                                              const string& excludeTitle) const {
    vector<Event> conflicts;
    for (const Event& e : events) {
        if (!excludeTitle.empty() && e.getTitle() == excludeTitle) continue;
        if (!isCalendarVisible(e.getCalendarName())) continue;
        bool onDate = (e.getDate() == date) ||
                      (e.getIsRecurring() && recurringOccursOn(e, date));
        if (onDate && !e.getIsAllDay()) {
            if (!(endTime <= e.getStartTime() || startTime >= e.getEndTime()))
                conflicts.push_back(e);
        }
    }
    return conflicts;
}

/**
 * @brief searchEvents method of Calendar.cpp.
 * @param keyword parameter for searchEvents.
 * @return Result of the operation.
 */
vector<Event> Calendar::searchEvents(const string& keyword) const {
    vector<Event> results;

    for (const Event& e : events) {
        if(e.getTitle().find(keyword) != string::npos) {
            results.push_back(e);
        }
    }

    return results;
}

/**
 * @brief clearSearch method of Calendar.cpp.
 */
void Calendar::clearSearch() {
    // No state to clear since search is stateless
}

// Replaces a substring within a string (helper for escaping)
/**
 * @brief replaceAll method of Calendar.cpp.
 * @param s parameter for replaceAll.
 * @param from parameter for replaceAll.
 * @param to parameter for replaceAll.
 * @return Result of the operation.
 */
static string replaceAll(string s, const string& from, const string& to) {
    size_t pos = 0;
    while ((pos = s.find(from, pos)) != string::npos) {
        s.replace(pos, from.size(), to);
        pos += to.size();
    }
    return s;
}

/**
 * @brief saveEvents method of Calendar.cpp.
 * @param filename parameter for saveEvents.
 */
void Calendar::saveEvents(const string& filename) const {
    ofstream file(filename);
    for (const Event& e : events) {
        // Escape pipe and newline characters in text fields
        auto esc = [](const string& s) {
            string r = replaceAll(s, "|", "<PIPE>");
            r = replaceAll(r, "\n", "<NL>");
            return r;
        };
        file << esc(e.getTitle())       << "|"
             << esc(e.getLocation())    << "|"
             << esc(e.getDescription()) << "|"
             << e.getDate()             << "|"
             << e.getEndDate()          << "|"
             << e.getStartTime()        << "|"
             << e.getEndTime()          << "|"
             << (e.getIsAllDay()    ? "1" : "0") << "|"
             << (e.getIsRecurring() ? "1" : "0") << "|"
             << e.getRepeatCount()      << "|"
             << esc(e.getRepeatType())  << "|"
             << esc(e.getCalendarName()) << "\n";
    }
}

/**
 * @brief loadEvents method of Calendar.cpp.
 * @param filename parameter for loadEvents.
 */
void Calendar::loadEvents(const string& filename) {
    events.clear();
    ifstream file(filename);
    if (!file.is_open()) return;

    auto unesc = [](const string& s) {
        string r = replaceAll(s, "<PIPE>", "|");
        r = replaceAll(r, "<NL>", "\n");
        return r;
    };

    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;
        vector<string> fields;
        stringstream ss(line);
        string field;
        while (getline(ss, field, '|'))
            fields.push_back(field);

        if (fields.size() < 8) continue;

        string title       = unesc(fields[0]);
        string location    = unesc(fields[1]);
        string description = unesc(fields[2]);
        string date        = fields[3];
        string endDate     = fields[4];
        string startTime   = fields[5];
        string endTime     = fields[6];
        bool isAllDay      = (fields[7] == "1");
        bool isRecurring   = (fields.size() > 8) ? (fields[8] == "1") : false;
        int repeatCount    = (fields.size() > 9 && !fields[9].empty()) ? stoi(fields[9]) : 0;
        string repeatType  = (fields.size() > 10) ? unesc(fields[10]) : "";
        string calName = (fields.size() > 11) ? unesc(fields[11]) : "Personal";

        events.push_back(Event::createEvent(
            title, location, description, date, endDate, 
            startTime, endTime,
            isAllDay, isRecurring, repeatCount, repeatType, calName));
    }
}

/**
 * @brief setCurrentDate method of Calendar.cpp.
 * @param date parameter for setCurrentDate.
 */
void Calendar::setCurrentDate(const Date& date) {
    currentDate = date;
}

// Functions for managing calendar metadata (name, colour, visibility)

/**
 * @brief addCalendar method of Calendar.cpp.
 * @param cal parameter for addCalendar.
 */
void Calendar::addCalendar(const CalendarMeta& cal) {
    for (auto& c : calendarList)
        if (c.name == cal.name) return; // no duplicates
    calendarList.push_back(cal);
}

/**
 * @brief removeCalendar method of Calendar.cpp.
 * @param name parameter for removeCalendar.
 */
void Calendar::removeCalendar(const std::string& name) {
    if (calendarList.size() <= 1) return; // always keep at least one
    calendarList.erase(
        remove_if(calendarList.begin(), calendarList.end(),
/**
 * @brief [&] method of Calendar.cpp.
 * @param c.name parameter for [&].
 * @return Result of the operation.
 */
                  [&](const CalendarMeta& c) { return c.name == name; }),
        calendarList.end());
    // Re-assign orphaned events to the first remaining calendar
    for (Event& e : events)
        if (e.getCalendarName() == name)
            e.setCalendarName(calendarList.front().name);
}

/**
 * @brief editCalendar method of Calendar.cpp.
 * @param oldName parameter for editCalendar.
 * @param updated parameter for editCalendar.
 */
void Calendar::editCalendar(const std::string& oldName, const CalendarMeta& updated) {
    for (auto& c : calendarList) {
        if (c.name == oldName) {
            if (oldName != updated.name)
                for (Event& e : events)
                    if (e.getCalendarName() == oldName)
                        e.setCalendarName(updated.name);
            c = updated;
            return;
        }
    }
}

/**
 * @brief getCalendars method of Calendar.cpp.
 * @return Result of the operation.
 */
const vector<CalendarMeta>& Calendar::getCalendars() const { return calendarList; }

/**
 * @brief getColourForCalendar method of Calendar.cpp.
 * @param calName parameter for getColourForCalendar.
 * @return Result of the operation.
 */
std::string Calendar::getColourForCalendar(const std::string& calName) const {
    for (const auto& c : calendarList)
        if (c.name == calName) return c.colour;
    return "#0A84FF";
}

/**
 * @brief setCalendarVisible method of Calendar.cpp.
 * @param name parameter for setCalendarVisible.
 * @param visible parameter for setCalendarVisible.
 */
void Calendar::setCalendarVisible(const std::string& name, bool visible) {
    for (auto& c : calendarList)
        if (c.name == name) { c.visible = visible; return; }
}

/**
 * @brief isCalendarVisible method of Calendar.cpp.
 * @param calName parameter for isCalendarVisible.
 * @return Result of the operation.
 */
bool Calendar::isCalendarVisible(const std::string& calName) const {
    if (calName.empty()) return true;
    for (const auto& c : calendarList)
        if (c.name == calName) return c.visible;
    return true;
}

/**
 * @brief saveCalendars method of Calendar.cpp.
 * @param filename parameter for saveCalendars.
 */
void Calendar::saveCalendars(const string& filename) const {
    ofstream file(filename);
    for (const auto& c : calendarList)
        file << replaceAll(c.name,   "|", "<PIPE>") << "|"
             << replaceAll(c.colour, "|", "<PIPE>") << "|"
             << (c.visible ? "1" : "0") << "\n";
}

/**
 * @brief loadCalendars method of Calendar.cpp.
 * @param filename parameter for loadCalendars.
 */
void Calendar::loadCalendars(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) return;
    calendarList.clear();
    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;
        vector<string> fields;
        stringstream ss(line);
        string field;
        while (getline(ss, field, '|')) fields.push_back(field);
        if (fields.size() < 3) continue;
        std::string name   = replaceAll(fields[0], "<PIPE>", "|");
        std::string colour = replaceAll(fields[1], "<PIPE>", "|");
        bool visible = (fields[2] == "1");
        calendarList.push_back(CalendarMeta(name, colour, visible));
    }
    if (calendarList.empty())
        calendarList.push_back(CalendarMeta("Personal", "#0A84FF", true));
}