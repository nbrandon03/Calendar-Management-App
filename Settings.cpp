/**
 * @file Settings.cpp
 * @brief Source file for calendar application components.
 * @author Nikolas
 * @author Rosaline
 * @author Uday
 * @author Brandon
 * @author Dinith
 */

#include "Settings.h"
#include <fstream>

/**
 * @brief Settings method of Settings.cpp.
 */
Settings::Settings()
    : timeZone("Local"),
      layoutPref("Default"),
      calendarView("Month"),
    showConflicts(false),
    temperatureUnit("Auto"),
    weatherLatitude(43.5448),
    weatherLongitude(-80.2482) {}

Settings::Settings(const std::string& timeZone,
                   const std::string& layoutPref,
                   const std::string& calendarView,
                                     bool showConflicts,
                                     const std::string& temperatureUnit,
                                     double weatherLatitude,
                                     double weatherLongitude)
    : timeZone(timeZone),
      layoutPref(layoutPref),
      calendarView(calendarView),
            showConflicts(showConflicts),
            temperatureUnit(temperatureUnit),
            weatherLatitude(weatherLatitude),
            weatherLongitude(weatherLongitude) {}


/**
 * @brief getTimeZone method of Settings.cpp.
 * @return Result of the operation.
 */
std::string Settings::getTimeZone() const {
    return timeZone;
}

/**
 * @brief getLayoutPref method of Settings.cpp.
 * @return Result of the operation.
 */
std::string Settings::getLayoutPref() const {
    return layoutPref;
}

/**
 * @brief getCalendarView method of Settings.cpp.
 * @return Result of the operation.
 */
std::string Settings::getCalendarView() const {
    return calendarView;
}

/**
 * @brief getShowConflicts method of Settings.cpp.
 * @return Result of the operation.
 */
bool Settings::getShowConflicts() const {
    return showConflicts;
}

/**
 * @brief getTemperatureUnit method of Settings.cpp.
 * @return Result of the operation.
 */
std::string Settings::getTemperatureUnit() const {
    return temperatureUnit;
}

/**
 * @brief getWeatherLatitude method of Settings.cpp.
 * @return Result of the operation.
 */
double Settings::getWeatherLatitude() const {
    return weatherLatitude;
}

/**
 * @brief getWeatherLongitude method of Settings.cpp.
 * @return Result of the operation.
 */
double Settings::getWeatherLongitude() const {
    return weatherLongitude;
}

// Setters
/**
 * @brief setTimeZone method of Settings.cpp.
 * @param timeZone parameter for setTimeZone.
 */
void Settings::setTimeZone(const std::string& timeZone) {
    this->timeZone = timeZone;
}

/**
 * @brief setLayoutPref method of Settings.cpp.
 * @param layoutPref parameter for setLayoutPref.
 */
void Settings::setLayoutPref(const std::string& layoutPref) {
    this->layoutPref = layoutPref;
}

/**
 * @brief setCalendarView method of Settings.cpp.
 * @param calendarView parameter for setCalendarView.
 */
void Settings::setCalendarView(const std::string& calendarView) {
    this->calendarView = calendarView;
}

/**
 * @brief setShowConflicts method of Settings.cpp.
 * @param showConflicts parameter for setShowConflicts.
 */
void Settings::setShowConflicts(bool showConflicts) {
    this->showConflicts = showConflicts;
}

/**
 * @brief setTemperatureUnit method of Settings.cpp.
 * @param temperatureUnit parameter for setTemperatureUnit.
 */
void Settings::setTemperatureUnit(const std::string& temperatureUnit) {
    this->temperatureUnit = temperatureUnit;
}

/**
 * @brief setWeatherLatitude method of Settings.cpp.
 * @param latitude parameter for setWeatherLatitude.
 */
void Settings::setWeatherLatitude(double latitude) {
    this->weatherLatitude = latitude;
}

/**
 * @brief setWeatherLongitude method of Settings.cpp.
 * @param longitude parameter for setWeatherLongitude.
 */
void Settings::setWeatherLongitude(double longitude) {
    this->weatherLongitude = longitude;
}

// Save settings to file
/**
 * @brief saveSettings method of Settings.cpp.
 * @param filename parameter for saveSettings.
 * @return Result of the operation.
 */
bool Settings::saveSettings(const std::string& filename) const {
    std::ofstream outFile(filename);
    if (!outFile.is_open()) {
        return false;
    }

    outFile << timeZone << '\n';
    outFile << layoutPref << '\n';
    outFile << calendarView << '\n';
    outFile << (showConflicts ? "true" : "false") << '\n';
    outFile << temperatureUnit << '\n';
    outFile << weatherLatitude << '\n';
    outFile << weatherLongitude << '\n';

    outFile.close();
    return true;
}

// Load settings from file
/**
 * @brief loadSettings method of Settings.cpp.
 * @param filename parameter for loadSettings.
 * @return Result of the operation.
 */
bool Settings::loadSettings(const std::string& filename) {
    std::ifstream inFile(filename);
    if (!inFile.is_open()) {
        return false;
    }

    std::getline(inFile, timeZone);
    std::getline(inFile, layoutPref);
    std::getline(inFile, calendarView);
    std::string conflictsStr;
    std::getline(inFile, conflictsStr);
    showConflicts = (conflictsStr == "true");

    if (!std::getline(inFile, temperatureUnit) || temperatureUnit.empty()) {
        temperatureUnit = "Auto";
    }

    std::string latitudeStr;
    std::string longitudeStr;
    if (std::getline(inFile, latitudeStr) && !latitudeStr.empty()) {
        try {
            weatherLatitude = std::stod(latitudeStr);
        }
        catch (...) {
            weatherLatitude = 43.5448;
        }
    }
    else {
        weatherLatitude = 43.5448;
    }

    if (std::getline(inFile, longitudeStr) && !longitudeStr.empty()) {
        try {
            weatherLongitude = std::stod(longitudeStr);
        }
        catch (...) {
            weatherLongitude = -80.2482;
        }
    }
    else {
        weatherLongitude = -80.2482;
    }

    inFile.close();
    return true;
}

/**
 * @brief resetToDefault method of Settings.cpp.
 */
void Settings::resetToDefault() {
    timeZone = "Local";
    layoutPref = "Default";
    calendarView = "Month";
    showConflicts = false;
    temperatureUnit = "Auto";
    weatherLatitude = 43.5448;
    weatherLongitude = -80.2482;
}