/**
 * @file Date.cpp
 * @brief Source file for calendar application components.
 * @author Nikolas
 * @author Rosaline
 * @author Uday
 * @author Brandon
 * @author Dinith
 */

#include "Date.h"
#include <ctime>
#include <sstream>

/**
 * @brief Date method of Date.cpp.
 */
Date::Date() {
    std::time_t t = std::time(nullptr);
    std::tm* now = std::localtime(&t);

    day = now->tm_mday;
    month = now->tm_mon + 1;
    year = now->tm_year + 1900;
}

/**
 * @brief Date method of Date.cpp.
 * @param d parameter for Date.
 * @param m parameter for Date.
 * @param y parameter for Date.
 */
Date::Date(int d, int m, int y) {
    day = d;
    month = m;
    year = y;
}

/**
 * @brief getDay method of Date.cpp.
 * @return Result of the operation.
 */
int Date::getDay() const {
    return day;
}

/**
 * @brief getMonth method of Date.cpp.
 * @return Result of the operation.
 */
int Date::getMonth() const {
    return month;
}

/**
 * @brief getYear method of Date.cpp.
 * @return Result of the operation.
 */
int Date::getYear() const {
    return year;
}

/**
 * @brief setDate method of Date.cpp.
 * @param d parameter for setDate.
 * @param m parameter for setDate.
 * @param y parameter for setDate.
 */
void Date::setDate(int d, int m, int y) {
    day = d;
    month = m;
    year = y;
}

/**
 * @brief isToday method of Date.cpp.
 * @return Result of the operation.
 */
bool Date::isToday() const {
    Date today;
    return (*this == today);
}

/**
 * @brief dayOfWeek method of Date.cpp.
 * @return Result of the operation.
 */
int Date::dayOfWeek() const {
    std::tm timeInfo = {};
    timeInfo.tm_mday = day;
    timeInfo.tm_mon = month - 1;
    timeInfo.tm_year = year - 1900;
    std::mktime(&timeInfo);
    return timeInfo.tm_wday;
}

/**
 * @brief addDays method of Date.cpp.
 * @param amount parameter for addDays.
 * @return Result of the operation.
 */
Date Date::addDays(int amount) const {
    std::tm timeInfo = {};
    timeInfo.tm_mday = day + amount;
    timeInfo.tm_mon = month - 1;
    timeInfo.tm_year = year - 1900;
    std::mktime(&timeInfo);

    return Date(timeInfo.tm_mday, timeInfo.tm_mon + 1, timeInfo.tm_year + 1900);
}

/**
 * @brief isLeapYear method of Date.cpp.
 * @param y parameter for isLeapYear.
 * @return Result of the operation.
 */
bool Date::isLeapYear(int y) {
    return (y % 400 == 0) || (y % 4 == 0 && y % 100 != 0);
}

/**
 * @brief daysInMonth method of Date.cpp.
 * @param m parameter for daysInMonth.
 * @param y parameter for daysInMonth.
 * @return Result of the operation.
 */
int Date::daysInMonth(int m, int y) {
    switch (m) {
        case 1: return 31;
        case 2: return isLeapYear(y) ? 29 : 28;
        case 3: return 31;
        case 4: return 30;
        case 5: return 31;
        case 6: return 30;
        case 7: return 31;
        case 8: return 31;
        case 9: return 30;
        case 10: return 31;
        case 11: return 30;
        case 12: return 31;
        default: return 30;
    }
}

/**
 * @brief toString method of Date.cpp.
 * @return Result of the operation.
 */
std::string Date::toString() const {
    std::ostringstream out;
    out << day << "/" << month << "/" << year;
    return out.str();
}

/**
 * @brief monthYearString method of Date.cpp.
 * @return Result of the operation.
 */
std::string Date::monthYearString() const {
    static const std::string months[] = {
        "January", "February", "March", "April", "May", "June",
        "July", "August", "September", "October", "November", "December"
    };

    std::ostringstream out;
    out << months[month - 1] << " " << year;
    return out.str();
}

bool Date::operator==(const Date& other) const {
    return day == other.day && month == other.month && year == other.year;
}