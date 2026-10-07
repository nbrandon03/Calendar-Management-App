/**
 * @file WeatherAPI.cpp
 * @brief Weather API implementation for forecast and geocoding features.
 * @details Implements network calls, payload parsing helpers, and icon mapping
 * used by the calendar UI weather panel.
 * @author Nikolas
 * @author Rosaline
 * @author Uday
 * @author Brandon
 * @author Dinith
 */

#include "WeatherAPI.h"

#include <wx/url.h>
#include <wx/sstream.h>
#include <wx/filename.h>
#include <wx/log.h>
#include <wx/image.h>
#include <wx/utils.h>

#include <iomanip>
#include <sstream>
#include <locale>
#include <cctype>
#include <cstdlib>

namespace {
/**
 * @brief ExtractArrayBody method of WeatherAPI.cpp.
 * @param source parameter for ExtractArrayBody.
 * @param key parameter for ExtractArrayBody.
 * @param body parameter for ExtractArrayBody.
 */
bool ExtractArrayBody(const std::string& source, const std::string& key, std::string& body) {
    size_t dailyPos = source.find("\"daily\"");
    if (dailyPos == std::string::npos) {
        return false;
    }

    std::string quotedKey = "\"" + key + "\"";
    size_t keyPos = source.find(quotedKey, dailyPos);
    if (keyPos == std::string::npos) {
        return false;
    }

    size_t openBracketPos = source.find('[', keyPos);
    if (openBracketPos == std::string::npos) {
        return false;
    }

    size_t closeBracketPos = source.find(']', openBracketPos + 1);
    if (closeBracketPos == std::string::npos || closeBracketPos <= openBracketPos) {
        return false;
    }

    body = source.substr(openBracketPos + 1, closeBracketPos - openBracketPos - 1);
    return true;
}

/**
 * @brief ExtractTopLevelArrayBody method of WeatherAPI.cpp.
 * @param source parameter for ExtractTopLevelArrayBody.
 * @param key parameter for ExtractTopLevelArrayBody.
 * @param body parameter for ExtractTopLevelArrayBody.
 */
bool ExtractTopLevelArrayBody(const std::string& source, const std::string& key, std::string& body) {
    std::string quotedKey = "\"" + key + "\"";
    size_t keyPos = source.find(quotedKey);
    if (keyPos == std::string::npos) {
        return false;
    }

    size_t openBracketPos = source.find('[', keyPos);
    if (openBracketPos == std::string::npos) {
        return false;
    }

    int depth = 1;
    size_t pos = openBracketPos + 1;
    while (pos < source.size() && depth > 0) {
        if (source[pos] == '[') {
            depth++;
        }
        else if (source[pos] == ']') {
            depth--;
        }
        pos++;
    }

    if (depth != 0 || pos <= openBracketPos + 1) {
        return false;
    }

    body = source.substr(openBracketPos + 1, pos - openBracketPos - 2);
    return true;
}

/**
 * @brief ParseObjectArray method of WeatherAPI.cpp.
 * @param body parameter for ParseObjectArray.
 */
std::vector<std::string> ParseObjectArray(const std::string& body) {
    std::vector<std::string> objects;
    size_t pos = 0;
    while (pos < body.size()) {
        size_t openBrace = body.find('{', pos);
        if (openBrace == std::string::npos) {
            break;
        }

        int depth = 1;
        size_t cursor = openBrace + 1;
        while (cursor < body.size() && depth > 0) {
            if (body[cursor] == '{') {
                depth++;
            }
            else if (body[cursor] == '}') {
                depth--;
            }
            cursor++;
        }

        if (depth != 0) {
            break;
        }

        objects.push_back(body.substr(openBrace, cursor - openBrace));
        pos = cursor;
    }

    return objects;
}

/**
 * @brief ParseStringArray method of WeatherAPI.cpp.
 * @param body parameter for ParseStringArray.
 */
std::vector<std::string> ParseStringArray(const std::string& body) {
    std::vector<std::string> values;
    size_t pos = 0;
    while (true) {
        size_t firstQuote = body.find('"', pos);
        if (firstQuote == std::string::npos) {
            break;
        }
        size_t secondQuote = body.find('"', firstQuote + 1);
        if (secondQuote == std::string::npos) {
            break;
        }

        values.push_back(body.substr(firstQuote + 1, secondQuote - firstQuote - 1));
        pos = secondQuote + 1;
    }
    return values;
}

/**
 * @brief ParseNumberArray method of WeatherAPI.cpp.
 * @param body parameter for ParseNumberArray.
 */
std::vector<double> ParseNumberArray(const std::string& body) {
    std::vector<double> values;
    std::stringstream ss(body);
    std::string token;
    while (std::getline(ss, token, ',')) {
        size_t start = 0;
        while (start < token.size() && std::isspace(static_cast<unsigned char>(token[start]))) {
            start++;
        }

        size_t end = token.size();
        while (end > start && std::isspace(static_cast<unsigned char>(token[end - 1]))) {
            end--;
        }

        if (end <= start) {
            continue;
        }

        values.push_back(std::stod(token.substr(start, end - start)));
    }
    return values;
}

/**
 * @brief ExtractJsonStringField method of WeatherAPI.cpp.
 * @param source parameter for ExtractJsonStringField.
 * @param key parameter for ExtractJsonStringField.
 * @param startPos parameter for ExtractJsonStringField.
 */
std::string ExtractJsonStringField(const std::string& source, const std::string& key, size_t startPos = 0) {
    std::string quotedKey = "\"" + key + "\"";
    size_t keyPos = source.find(quotedKey, startPos);
    if (keyPos == std::string::npos) {
        return "";
    }

    size_t colonPos = source.find(':', keyPos + quotedKey.size());
    if (colonPos == std::string::npos) {
        return "";
    }

    size_t quoteStart = source.find('"', colonPos + 1);
    if (quoteStart == std::string::npos) {
        return "";
    }

    size_t quoteEnd = quoteStart + 1;
    while (true) {
        quoteEnd = source.find('"', quoteEnd);
        if (quoteEnd == std::string::npos) {
            return "";
        }
        if (quoteEnd == quoteStart + 1 || source[quoteEnd - 1] != '\\') {
            break;
        }
        quoteEnd++;
    }

    return source.substr(quoteStart + 1, quoteEnd - quoteStart - 1);
}

/**
 * @brief ExtractJsonNumberField method of WeatherAPI.cpp.
 * @param source parameter for ExtractJsonNumberField.
 * @param key parameter for ExtractJsonNumberField.
 * @param value parameter for ExtractJsonNumberField.
 */
bool ExtractJsonNumberField(const std::string& source, const std::string& key, double& value) {
    std::string quotedKey = "\"" + key + "\"";
    size_t keyPos = source.find(quotedKey);
    if (keyPos == std::string::npos) {
        return false;
    }

    size_t colonPos = source.find(':', keyPos + quotedKey.size());
    if (colonPos == std::string::npos) {
        return false;
    }

    size_t start = colonPos + 1;
    while (start < source.size() && std::isspace(static_cast<unsigned char>(source[start]))) {
        start++;
    }

    size_t end = start;
    while (end < source.size()) {
        char c = source[end];
        if ((c >= '0' && c <= '9') || c == '-' || c == '+' || c == '.' || c == 'e' || c == 'E') {
            end++;
            continue;
        }
        break;
    }

    if (end <= start) {
        return false;
    }

    value = std::strtod(source.substr(start, end - start).c_str(), nullptr);
    return true;
}

/**
 * @brief UrlEncode method of WeatherAPI.cpp.
 * @param input parameter for UrlEncode.
 */
std::string UrlEncode(const std::string& input) {
    static const char* hex = "0123456789ABCDEF";
    std::string out;
    out.reserve(input.size() * 3);

    for (unsigned char c : input) {
        if (std::isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
            out.push_back(static_cast<char>(c));
        }
        else if (c == ' ') {
            out.push_back('+');
        }
        else {
            out.push_back('%');
            out.push_back(hex[(c >> 4) & 0x0F]);
            out.push_back(hex[c & 0x0F]);
        }
    }

    return out;
}

/**
 * @brief FormatCoordinateValue method of WeatherAPI.cpp.
 * @param value parameter for FormatCoordinateValue.
 * @param precision parameter for FormatCoordinateValue.
 */
wxString FormatCoordinateValue(double value, int precision) {
    std::ostringstream out;
    out.imbue(std::locale::classic());
    out << std::fixed << std::setprecision(precision) << value;
    return wxString(out.str());
}

/**
 * @brief FetchWithCurl method of WeatherAPI.cpp.
 * @param url parameter for FetchWithCurl.
 * @param response parameter for FetchWithCurl.
 */
bool FetchWithCurl(const wxString& url, wxString& response) {
    wxArrayString output;
    wxArrayString errors;
    wxString command = wxString::Format("curl -Ls --max-time 10 \"%s\"", url);
    long exitCode = wxExecute(command, output, errors, wxEXEC_SYNC);

    if (exitCode != 0 || output.IsEmpty()) {
        return false;
    }

    response = wxJoin(output, '\n');
    return !response.IsEmpty();
}

/**
 * @brief FetchWithWxUrl method of WeatherAPI.cpp.
 * @param urlString parameter for FetchWithWxUrl.
 * @param response parameter for FetchWithWxUrl.
 */
bool FetchWithWxUrl(const wxString& urlString, wxString& response) {
    wxURL url(urlString);
    if (url.GetError() != wxURL_NOERR) {
        wxLogDebug("WeatherAPI: Failed to create URL (%d): %s", static_cast<int>(url.GetError()), urlString);
        return false;
    }

    std::unique_ptr<wxInputStream> input(url.GetInputStream());
    if (!input || !input->IsOk()) {
        wxLogDebug("WeatherAPI: Failed to open input stream from Open-Meteo.");
        return false;
    }

    wxStringOutputStream outputStream;
    input->Read(outputStream);
    response = outputStream.GetString();
    if (response.IsEmpty()) {
        wxLogDebug("WeatherAPI: Empty response from Open-Meteo.");
        return false;
    }

    return true;
}
}

/**
 * @brief Constructs a WeatherAPI instance.
 * @details Stores initial coordinates and defaults to celsius output.
 * @param latitude Initial latitude in decimal degrees.
 * @param longitude Initial longitude in decimal degrees.
 */
WeatherAPI::WeatherAPI(double latitude, double longitude)
    : latitude(latitude), longitude(longitude), temperatureUnit("celsius") {
}

/**
 * @brief Updates the location coordinates used by weather requests.
 * @details New coordinates are used for subsequent forecast and reverse
 * geocoding calls.
 * @param newLatitude Latitude in decimal degrees.
 * @param newLongitude Longitude in decimal degrees.
 */
void WeatherAPI::SetLocation(double newLatitude, double newLongitude) {
    latitude = newLatitude;
    longitude = newLongitude;
}

/**
 * @brief Builds the Open-Meteo forecast request URL.
 * @details Includes current coordinates, requested daily fields, temperature
 * unit, and automatic timezone conversion.
 * @return Fully assembled forecast endpoint URL.
 */
wxString WeatherAPI::BuildURL() const {
    // Open-Meteo daily forecast request:
    // - weather_code
    // - temperature_2m_max
    // - temperature_2m_min
    // - timezone=auto so returned dates/times line up nicely for the user's location
    std::ostringstream latStream;
    latStream.imbue(std::locale::classic());
    latStream << std::fixed << std::setprecision(6) << latitude;

    std::ostringstream lonStream;
    lonStream.imbue(std::locale::classic());
    lonStream << std::fixed << std::setprecision(6) << longitude;

    wxString url = "http://api.open-meteo.com/v1/forecast";
    url += "?latitude=" + wxString(latStream.str());
    url += "&longitude=" + wxString(lonStream.str());
    url += "&daily=weather_code,temperature_2m_max,temperature_2m_min";
    url += "&temperature_unit=" + temperatureUnit;
    url += "&timezone=auto";
    return url;
}

/**
 * @brief Sets the preferred temperature unit.
 * @details Only "fahrenheit" is accepted explicitly; all other values are
 * normalized to "celsius".
 * @param newTemperatureUnit Requested temperature unit string.
 */
void WeatherAPI::SetTemperatureUnit(const wxString& newTemperatureUnit) {
    wxString normalized = newTemperatureUnit.Lower();
    if (normalized == "fahrenheit") {
        temperatureUnit = "fahrenheit";
        return;
    }
    temperatureUnit = "celsius";
}

/**
 * @brief Returns the active temperature unit.
 * @details This value is also written into outgoing forecast requests.
 * @return "celsius" or "fahrenheit".
 */
wxString WeatherAPI::GetTemperatureUnit() const {
    return temperatureUnit;
}

/**
 * @brief Resolves a display-friendly location label.
 * @details Uses reverse geocoding for current coordinates and falls back to
 * formatted numeric coordinates when lookup data is unavailable.
 * @return City and region label, or coordinate fallback.
 */
wxString WeatherAPI::GetFriendlyLocationName() const {
    wxString latitudeText = FormatCoordinateValue(latitude, 6);
    wxString longitudeText = FormatCoordinateValue(longitude, 6);
    wxString coordinateFallback = FormatCoordinateValue(latitude, 4) + ", " + FormatCoordinateValue(longitude, 4);

    wxString url = "http://api.bigdatacloud.net/data/reverse-geocode-client";
    url += "?latitude=" + latitudeText;
    url += "&longitude=" + longitudeText;
    url += "&localityLanguage=en";

    wxString response;
    bool fetched = FetchWithWxUrl(url, response);
    if (!fetched) {
        fetched = FetchWithCurl(url, response);
    }
    if (!fetched || response.IsEmpty()) {
        return coordinateFallback;
    }

    wxScopedCharBuffer utf8 = response.utf8_str();
    std::string payload(utf8.data(), utf8.length());
    size_t resultsPos = payload.find("\"results\"");
    std::string city = ExtractJsonStringField(payload, "city");
    if (city.empty()) {
        city = ExtractJsonStringField(payload, "locality");
    }
    if (city.empty()) {
        return coordinateFallback;
    }

    std::string subdivisionCode = ExtractJsonStringField(payload, "principalSubdivisionCode");
    std::string subdivisionName = ExtractJsonStringField(payload, "principalSubdivision");
    std::string countryCode = ExtractJsonStringField(payload, "countryCode");

    wxString regionLabel;
    if (!subdivisionCode.empty()) {
        size_t dashPos = subdivisionCode.find('-');
        if (dashPos != std::string::npos && dashPos + 1 < subdivisionCode.size()) {
            regionLabel = wxString(subdivisionCode.substr(dashPos + 1));
        }
    }
    if (regionLabel.empty() && !subdivisionName.empty()) {
        regionLabel = wxString(subdivisionName);
    }
    if (regionLabel.empty() && !countryCode.empty()) {
        regionLabel = wxString(countryCode);
    }

    wxString label = wxString(city);
    if (!regionLabel.empty()) {
        label += ", " + regionLabel;
    }
    return label;
}

/**
 * @brief Searches geocoding results by a text query.
 * @details Performs a remote lookup and converts each result into display
 * labels plus latitude/longitude coordinates.
 * @param query User-entered location query.
 * @param maxResults Maximum number of records to return.
 * @return Vector of matching locations.
 */
std::vector<WeatherLocationResult> WeatherAPI::SearchLocations(const wxString& query, int maxResults) const {
    std::vector<WeatherLocationResult> matches;
    wxString trimmed = query;
    trimmed.Trim(true);
    trimmed.Trim(false);
    if (trimmed.IsEmpty()) {
        return matches;
    }

    if (maxResults < 1) {
        maxResults = 1;
    }
    if (maxResults > 20) {
        maxResults = 20;
    }

    wxScopedCharBuffer queryUtf8 = trimmed.utf8_str();
    std::string encodedQuery = UrlEncode(std::string(queryUtf8.data(), queryUtf8.length()));

    wxString url = "https://geocoding-api.open-meteo.com/v1/search";
    url += "?name=" + wxString(encodedQuery);
    url += "&count=" + wxString::Format("%d", maxResults);
    url += "&language=en&format=json";

    wxString response;
    bool fetched = FetchWithWxUrl(url, response);
    if (!fetched) {
        fetched = FetchWithCurl(url, response);
    }
    if (!fetched || response.IsEmpty()) {
        return matches;
    }

    wxScopedCharBuffer utf8 = response.utf8_str();
    std::string payload(utf8.data(), utf8.length());
    std::string resultsBody;
    if (!ExtractTopLevelArrayBody(payload, "results", resultsBody)) {
        return matches;
    }

    std::vector<std::string> objects = ParseObjectArray(resultsBody);
    for (const std::string& objectText : objects) {
        std::string name = ExtractJsonStringField(objectText, "name");
        if (name.empty()) {
            continue;
        }

        double resultLatitude = 0.0;
        double resultLongitude = 0.0;
        if (!ExtractJsonNumberField(objectText, "latitude", resultLatitude) ||
            !ExtractJsonNumberField(objectText, "longitude", resultLongitude)) {
            continue;
        }

        std::string admin1 = ExtractJsonStringField(objectText, "admin1");
        std::string countryCode = ExtractJsonStringField(objectText, "country_code");

        wxString label = wxString(name);
        if (!admin1.empty()) {
            label += ", " + wxString(admin1);
        }
        if (!countryCode.empty()) {
            label += " (" + wxString(countryCode) + ")";
        }

        WeatherLocationResult result;
        result.displayName = label;
        result.latitude = resultLatitude;
        result.longitude = resultLongitude;
        matches.push_back(result);
    }

    return matches;
}

/**
 * @brief Fetches and caches a weekly weather forecast.
 * @details Attempts wxURL first, then curl as a fallback. On success, parses
 * daily arrays and replaces cached forecast entries.
 * @return True when forecast data is successfully fetched and parsed.
 */
bool WeatherAPI::FetchWeeklyForecast() {
    forecast.clear();

    wxString urlString = BuildURL();
    auto parseAndStore = [this](const wxString& responseText) -> bool {
        try {
            wxScopedCharBuffer utf8 = responseText.utf8_str();
            std::string payload(utf8.data(), utf8.length());
            std::string timesBody;
            std::string codesBody;
            std::string maxBody;
            std::string minBody;

            if (!ExtractArrayBody(payload, "time", timesBody) ||
                !ExtractArrayBody(payload, "weather_code", codesBody) ||
                !ExtractArrayBody(payload, "temperature_2m_max", maxBody) ||
                !ExtractArrayBody(payload, "temperature_2m_min", minBody)) {
                wxLogDebug("WeatherAPI: Missing one or more expected daily arrays in response.");
                wxString preview = responseText;
                preview.Replace("\n", " ");
                if (preview.Length() > 220) {
                    preview = preview.Left(220);
                }
                wxLogDebug("WeatherAPI: Payload preview: %s", preview);
                return false;
            }

            std::vector<std::string> times = ParseStringArray(timesBody);
            std::vector<double> codes = ParseNumberArray(codesBody);
            std::vector<double> maxTemps = ParseNumberArray(maxBody);
            std::vector<double> minTemps = ParseNumberArray(minBody);

            size_t count = times.size();
            if (count == 0) {
                wxLogDebug("WeatherAPI: No forecast entries found.");
                return false;
            }

            if (codes.size() != count || maxTemps.size() != count || minTemps.size() != count) {
                wxLogDebug("WeatherAPI: Daily arrays are not the same size.");
                return false;
            }

            forecast.clear();
            for (size_t i = 0; i < count; i++) {
                WeatherDay day;
                day.date = wxString(times[i]);
                day.weatherCode = static_cast<int>(codes[i]);
                day.tempMax = maxTemps[i];
                day.tempMin = minTemps[i];
                day.iconName = WeatherCodeToIconName(day.weatherCode);
                forecast.push_back(day);
            }

            return true;
        }
        catch (const std::exception& e) {
            wxLogDebug("WeatherAPI: Failed to parse response: %s", e.what());
            return false;
        }
    };

    wxString response;
    if (FetchWithWxUrl(urlString, response) && parseAndStore(response)) {
        return true;
    }

    if (FetchWithCurl(urlString, response) && parseAndStore(response)) {
        return true;
    }

    return false;
}

/**
 * @brief Returns the cached forecast data.
 * @details This accessor is non-owning and does not modify object state.
 * @return Const reference to the forecast vector.
 */
const std::vector<WeatherDay>& WeatherAPI::GetForecast() const {
    return forecast;
}

/**
 * @brief Finds a forecast entry for a specific date.
 * @details Performs a sequential lookup over cached entries.
 * @param date Date string formatted as YYYY-MM-DD.
 * @return Pointer to matching weather data or nullptr when absent.
 */
const WeatherDay* WeatherAPI::GetWeatherForDate(const wxString& date) const {
    for (const auto& day : forecast) {
        if (day.date == date) {
            return &day;
        }
    }
    return nullptr;
}

/**
 * @brief Maps WMO weather codes to icon names.
 * @details Converts Open-Meteo weather interpretation codes to the local icon
 * set used by the UI.
 * @param weatherCode WMO weather code.
 * @return Icon stem name used for PNG loading.
 */
wxString WeatherAPI::WeatherCodeToIconName(int weatherCode) const {
    // Open-Meteo uses WMO weather interpretation codes.
    // These are grouped here into a small custom icon set.
    // Suggested files:
    // sun.png, cloud.png, fog.png, drizzle.png, rain.png, snow.png, storm.png

    if (weatherCode == 0) {
        return "sun";
    }

    if (weatherCode == 1 || weatherCode == 2 || weatherCode == 3) {
        return "cloud";
    }

    if (weatherCode == 45 || weatherCode == 48) {
        return "fog";
    }

    if (weatherCode == 51 || weatherCode == 53 || weatherCode == 55 ||
        weatherCode == 56 || weatherCode == 57) {
        return "drizzle";
    }

    if (weatherCode == 61 || weatherCode == 63 || weatherCode == 65 ||
        weatherCode == 66 || weatherCode == 67 ||
        weatherCode == 80 || weatherCode == 81 || weatherCode == 82) {
        return "rain";
    }

    if (weatherCode == 71 || weatherCode == 73 || weatherCode == 75 ||
        weatherCode == 77 || weatherCode == 85 || weatherCode == 86) {
        return "snow";
    }

    if (weatherCode == 95 || weatherCode == 96 || weatherCode == 99) {
        return "storm";
    }

    return "cloud";
}

/**
 * @brief Loads a weather icon bitmap from disk.
 * @details Expects PNG files in the provided directory and logs warnings when
 * files are missing or invalid.
 * @param iconFolder Directory path containing icon files.
 * @param iconName File stem of the icon (without extension).
 * @return Loaded bitmap, or an empty bitmap on failure.
 */
wxBitmap WeatherAPI::GetWeatherBitmap(const wxString& iconFolder, const wxString& iconName) const {
    wxString fullPath = wxFileName(iconFolder, iconName + ".png").GetFullPath();

    if (!wxFileExists(fullPath)) {
        wxLogWarning("WeatherAPI: Icon file not found: %s", fullPath);
        return wxBitmap();
    }

    wxBitmap bmp(fullPath, wxBITMAP_TYPE_PNG);

    if (!bmp.IsOk()) {
        wxLogWarning("WeatherAPI: Failed to load bitmap: %s", fullPath);
        return wxBitmap();
    }

    return bmp;
}