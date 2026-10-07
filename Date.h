/**
 * @file Date.h
 * @brief Source file for calendar application components.
 * @author Nikolas
 * @author Rosaline
 * @author Uday
 * @author Brandon
 * @author Dinith
 */

#ifndef DATE_H
#define DATE_H

#include <string>

/**
 * @brief Class Date.
 */
class Date {
private:
    int day;
    int month;
    int year;

public:
/**
 * @brief Date method of Date.h.
 */
    Date();
/**
 * @brief Date method of Date.h.
 * @param d parameter for Date.
 * @param m parameter for Date.
 * @param y parameter for Date.
 */
    Date(int d, int m, int y);

/**
 * @brief getDay method of Date.h.
 * @return Result of the operation.
 */
    int getDay() const;
/**
 * @brief getMonth method of Date.h.
 * @return Result of the operation.
 */
    int getMonth() const;
/**
 * @brief getYear method of Date.h.
 * @return Result of the operation.
 */
    int getYear() const;

/**
 * @brief setDate method of Date.h.
 * @param d parameter for setDate.
 * @param m parameter for setDate.
 * @param y parameter for setDate.
 */
    void setDate(int d, int m, int y);

/**
 * @brief isToday method of Date.h.
 * @return Result of the operation.
 */
    bool isToday() const;
    int dayOfWeek() const; // 0 = Sunday, 6 = Saturday

/**
 * @brief addDays method of Date.h.
 * @param amount parameter for addDays.
 * @return Result of the operation.
 */
    Date addDays(int amount) const;

/**
 * @brief isLeapYear method of Date.h.
 * @param year parameter for isLeapYear.
 * @return Result of the operation.
 */
    static bool isLeapYear(int year);
/**
 * @brief daysInMonth method of Date.h.
 * @param month parameter for daysInMonth.
 * @param year parameter for daysInMonth.
 * @return Result of the operation.
 */
    static int daysInMonth(int month, int year);

/**
 * @brief toString method of Date.h.
 * @return Result of the operation.
 */
    std::string toString() const;
/**
 * @brief monthYearString method of Date.h.
 * @return Result of the operation.
 */
    std::string monthYearString() const;

    bool operator==(const Date& other) const;
};

#endif