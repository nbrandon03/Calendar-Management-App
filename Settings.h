/**
 * @file Settings.h
 * @brief Source file for calendar application components.
 * @author Nikolas
 * @author Rosaline
 * @author Uday
 * @author Brandon
 * @author Dinith
 */

#ifndef SETTINGS_H
#define SETTINGS_H

#include <string>

/**
 * @brief Class Settings.
 */
class Settings {
private:
    std::string timeZone;
    std::string layoutPref;
    std::string calendarView;
    bool showConflicts;
    std::string temperatureUnit;
    double weatherLatitude;
    double weatherLongitude;

public:
    
/**
 * @brief Settings method of Settings.h.
 */
    Settings();
    Settings(const std::string& timeZone,
             const std::string& layoutPref,
             const std::string& calendarView,
             bool showConflicts = false,
             const std::string& temperatureUnit = "Auto",
             double weatherLatitude = 43.5448,
             double weatherLongitude = -80.2482);

    
/**
 * @brief getTimeZone method of Settings.h.
 * @return Result of the operation.
 */
    std::string getTimeZone() const;
/**
 * @brief getLayoutPref method of Settings.h.
 * @return Result of the operation.
 */
    std::string getLayoutPref() const;
/**
 * @brief getCalendarView method of Settings.h.
 * @return Result of the operation.
 */
    std::string getCalendarView() const;
/**
 * @brief getShowConflicts method of Settings.h.
 * @return Result of the operation.
 */
    bool getShowConflicts() const;
/**
 * @brief getTemperatureUnit method of Settings.h.
 * @return Result of the operation.
 */
    std::string getTemperatureUnit() const;
/**
 * @brief getWeatherLatitude method of Settings.h.
 * @return Result of the operation.
 */
    double getWeatherLatitude() const;
/**
 * @brief getWeatherLongitude method of Settings.h.
 * @return Result of the operation.
 */
    double getWeatherLongitude() const;

    
/**
 * @brief setTimeZone method of Settings.h.
 * @param timeZone parameter for setTimeZone.
 */
    void setTimeZone(const std::string& timeZone);
/**
 * @brief setLayoutPref method of Settings.h.
 * @param layoutPref parameter for setLayoutPref.
 */
    void setLayoutPref(const std::string& layoutPref);
/**
 * @brief setCalendarView method of Settings.h.
 * @param calendarView parameter for setCalendarView.
 */
    void setCalendarView(const std::string& calendarView);
/**
 * @brief setShowConflicts method of Settings.h.
 * @param showConflicts parameter for setShowConflicts.
 */
    void setShowConflicts(bool showConflicts);
/**
 * @brief setTemperatureUnit method of Settings.h.
 * @param temperatureUnit parameter for setTemperatureUnit.
 */
    void setTemperatureUnit(const std::string& temperatureUnit);
/**
 * @brief setWeatherLatitude method of Settings.h.
 * @param latitude parameter for setWeatherLatitude.
 */
    void setWeatherLatitude(double latitude);
/**
 * @brief setWeatherLongitude method of Settings.h.
 * @param longitude parameter for setWeatherLongitude.
 */
    void setWeatherLongitude(double longitude);

    
/**
 * @brief saveSettings method of Settings.h.
 * @param filename parameter for saveSettings.
 * @return Result of the operation.
 */
    bool saveSettings(const std::string& filename = "settings.txt") const;
/**
 * @brief loadSettings method of Settings.h.
 * @param filename parameter for loadSettings.
 * @return Result of the operation.
 */
    bool loadSettings(const std::string& filename = "settings.txt");
/**
 * @brief resetToDefault method of Settings.h.
 */
    void resetToDefault();
};

#endif