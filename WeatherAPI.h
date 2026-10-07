/**
 * @file WeatherAPI.h
 * @brief Weather forecast service for the calendar application.
 * @details Declares data models and the WeatherAPI class used to fetch,
 * parse, and expose weather forecast information and location search results.
 * @author Nikolas
 * @author Rosaline
 * @author Uday
 * @author Brandon
 * @author Dinith
 */

#ifndef WEATHERAPI_H
#define WEATHERAPI_H

#include <wx/string.h>
#include <wx/bitmap.h>
#include <vector>

struct WeatherDay {
    wxString date;       // YYYY-MM-DD
    int weatherCode;
    double tempMax;
    double tempMin;
    wxString iconName;   // ex: "sun", "cloud", "rain"
};

struct WeatherLocationResult {
    wxString displayName;
    double latitude;
    double longitude;
};

/**
 * @brief Provides weather retrieval and parsing utilities.
 * @details This class calls external weather and geocoding endpoints, stores
 * a parsed weekly forecast, and exposes helper methods used by the UI.
 */
class WeatherAPI {
public:
/**
 * @brief Constructs a WeatherAPI instance for a location.
 * @details Initializes latitude/longitude and defaults temperature output to
 * celsius for Open-Meteo requests.
 * @param latitude Initial latitude in decimal degrees.
 * @param longitude Initial longitude in decimal degrees.
 */
    WeatherAPI(double latitude = 43.5448, double longitude = -80.2482);

/**
 * @brief Downloads and parses a weekly forecast.
 * @details Requests daily weather code and high/low temperatures, then updates
 * the internal forecast cache.
 * @return True if forecast data was successfully fetched and parsed.
 */
    bool FetchWeeklyForecast();
/**
 * @brief Returns the cached forecast entries.
 * @details This does not trigger a network call; it only exposes the last
 * successfully parsed in-memory forecast.
 * @return Const reference to the weekly forecast vector.
 */
    const std::vector<WeatherDay>& GetForecast() const;
/**
 * @brief Finds forecast data for a specific date.
 * @details Performs a linear search through cached forecast entries using the
 * API date format (YYYY-MM-DD).
 * @param date Date string in YYYY-MM-DD format.
 * @return Pointer to the matching day, or nullptr when no match exists.
 */
    const WeatherDay* GetWeatherForDate(const wxString& date) const;

    // Optional helper if you want the WeatherAPI class to also load the icon
/**
 * @brief Loads a weather icon bitmap from disk.
 * @details Builds a full PNG path from folder/name and returns an empty
 * bitmap when the file is missing or cannot be loaded.
 * @param iconFolder Directory containing weather icon files.
 * @param iconName Icon file stem without extension.
 * @return Loaded bitmap, or an empty bitmap on failure.
 */
    wxBitmap GetWeatherBitmap(const wxString& iconFolder, const wxString& iconName) const;

/**
 * @brief Updates the request location coordinates.
 * @details Subsequent fetches and reverse-geocode lookups use these values.
 * @param newLatitude Latitude in decimal degrees.
 * @param newLongitude Longitude in decimal degrees.
 */
    void SetLocation(double newLatitude, double newLongitude);
/**
 * @brief Sets the temperature unit used in API calls.
 * @details Accepts "fahrenheit" and defaults to "celsius" for all other
 * inputs.
 * @param newTemperatureUnit Preferred unit string.
 */
    void SetTemperatureUnit(const wxString& newTemperatureUnit);
/**
 * @brief Returns the active temperature unit.
 * @details The returned value is suitable for display and API query usage.
 * @return Current unit string ("celsius" or "fahrenheit").
 */
    wxString GetTemperatureUnit() const;
/**
 * @brief Resolves a human-friendly place label.
 * @details Reverse-geocodes the current coordinates and falls back to
 * formatted coordinates when no city/locality can be resolved.
 * @return Human-readable city/region label.
 */
    wxString GetFriendlyLocationName() const;
/**
 * @brief Searches for matching locations by name.
 * @details Calls Open-Meteo geocoding and returns display labels with
 * coordinates for up to maxResults matches.
 * @param query Partial or full location name.
 * @param maxResults Maximum number of matches to return.
 * @return List of matching location records.
 */
    std::vector<WeatherLocationResult> SearchLocations(const wxString& query, int maxResults = 5) const;

private:
    double latitude;
    double longitude;
    wxString temperatureUnit;
    std::vector<WeatherDay> forecast;

/**
 * @brief Builds the Open-Meteo forecast URL.
 * @details Encodes all required query parameters based on current object
 * state, including coordinates and temperature unit.
 * @return Fully formed forecast endpoint URL.
 */
    wxString BuildURL() const;
/**
 * @brief Maps a weather code to a local icon identifier.
 * @details Converts WMO weather interpretation codes into the small icon set
 * bundled with this project.
 * @param weatherCode Open-Meteo WMO weather code.
 * @return Icon stem such as "sun", "cloud", or "rain".
 */
    wxString WeatherCodeToIconName(int weatherCode) const;
};

#endif