/**
 * @file CalendarView.cpp
 * @brief Source file for calendar application components.
 * @author Nikolas
 * @author Rosaline
 * @author Uday
 * @author Brandon
 * @author Dinith
 */

#include "CalendarView.h"
#include <wx/datectrl.h>
#include <wx/timectrl.h>
#include <wx/valnum.h>
#include <wx/spinctrl.h>
#include <sstream>
#include <iomanip>
#include <locale>
#include <cmath>
#include <memory>

/**
 * @brief hexToColour method of CalendarView.cpp.
 * @param hex parameter for hexToColour.
 * @return Result of the operation.
 */
wxColour CalendarView::hexToColour(const std::string& hex) const {
    std::string h = hex;
    if (!h.empty() && h[0] == '#') h = h.substr(1);
    if (h.size() < 6) return wxColour(10, 132, 255);
    unsigned long val = 0;
    try { val = std::stoul(h, nullptr, 16); } catch (...) { return wxColour(10, 132, 255); }
    return wxColour((val >> 16) & 0xFF, (val >> 8) & 0xFF, val & 0xFF);
}

/**
 * @brief parseDMY method of CalendarView.cpp.
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

/**
 * @brief toJulian method of CalendarView.cpp.
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

namespace {
const wxColour APP_BG(245, 246, 248);
const wxColour PANEL_BG(255, 255, 255);
const wxColour SOFT_BUTTON_BG(242, 242, 247);
const wxColour SOFT_BUTTON_BORDER(220, 223, 229);
const wxColour PRIMARY_ACCENT(10, 132, 255);
const wxColour HEADER_BG(245, 246, 248);
const wxColour CELL_BG(255, 255, 255);
const wxColour CELL_BORDER(230, 232, 237);
const wxColour TODAY_BG(255, 245, 196);
const wxColour SELECTED_BG(221, 235, 255);
const wxColour TITLE_COLOR(28, 28, 30);
const wxColour SUBTITLE_COLOR(99, 99, 102);
const wxColour GRID_LINE_COLOR(224, 224, 229);
const wxColour OUTSIDE_MONTH_TEXT(174, 174, 178);
const wxColour OUTSIDE_MONTH_BG(249, 249, 251);
const wxColour EVENT_CHIP_BG(219, 197, 216);
const wxColour TODAY_TEXT(255, 59, 48);
const wxColour WEEK_GRID_BG(224, 226, 231);
const wxColour WEEK_CELL_BG(255, 255, 255);
const wxColour WEEK_SELECTED_COL_BG(240, 246, 255);
const wxColour WEEK_ALLDAY_BG(248, 248, 250);
const wxColour WEEK_EVENT_BG(240, 224, 245);
const wxColour WEEK_EVENT_TEXT(96, 54, 131);
const wxColour WEEK_EVENT_ALT_BG(252, 228, 220);
const wxColour WEEK_EVENT_ALT_TEXT(168, 68, 38);
const wxString WEATHER_ICON_FOLDER = "icons";

/**
 * @brief ParseHour method of CalendarView.cpp.
 * @param timeValue parameter for ParseHour.
 */
int ParseHour(const std::string& timeValue) {
    if (timeValue.empty()) {
        return -1;
    }
    size_t colonPos = timeValue.find(':');
    std::string hourPart = (colonPos == std::string::npos) ? timeValue : timeValue.substr(0, colonPos);
    if (hourPart.empty()) {
        return -1;
    }
    return std::stoi(hourPart);
}

/**
 * @brief ParseMinute method of CalendarView.cpp.
 * @param timeValue parameter for ParseMinute.
 */
int ParseMinute(const std::string& timeValue) {
    size_t colonPos = timeValue.find(':');
    if (colonPos == std::string::npos || colonPos + 1 >= timeValue.size()) {
        return 0;
    }
    size_t endPos = timeValue.find(':', colonPos + 1);
    std::string minutePart = timeValue.substr(colonPos + 1, endPos - colonPos - 1);
    if (minutePart.empty()) {
        return 0;
    }
    return std::stoi(minutePart);
}

/**
 * @brief FormatHourLabel method of CalendarView.cpp.
 * @param hour24 parameter for FormatHourLabel.
 */
wxString FormatHourLabel(int hour24) {
    int hour12 = hour24 % 12;
    if (hour12 == 0) hour12 = 12;
    wxString suffix = (hour24 < 12) ? "AM" : "PM";
    return wxString::Format("%d %s", hour12, suffix);
}

/**
 * @brief FormatEventTimeLabel method of CalendarView.cpp.
 * @param timeValue parameter for FormatEventTimeLabel.
 */
wxString FormatEventTimeLabel(const std::string& timeValue) {
    int hour24 = ParseHour(timeValue);
    if (hour24 < 0) {
        return "";
    }

    int minute = ParseMinute(timeValue);
    int hour12 = hour24 % 12;
    if (hour12 == 0) hour12 = 12;
    wxString suffix = (hour24 < 12) ? "a" : "p";
    if (minute == 0) {
        return wxString::Format("%d%s", hour12, suffix);
    }
    return wxString::Format("%d:%02d%s", hour12, minute, suffix);
}

/**
 * @brief FormatLongDate method of CalendarView.cpp.
 * @param date parameter for FormatLongDate.
 */
wxString FormatLongDate(const Date& date) {
    static const char* months[] = {
        "January", "February", "March", "April", "May", "June",
        "July", "August", "September", "October", "November", "December"
    };
    return wxString::Format("%s %d, %d", months[date.getMonth() - 1], date.getDay(), date.getYear());
}

/**
 * @brief FormatWeekRange method of CalendarView.cpp.
 * @param current parameter for FormatWeekRange.
 */
wxString FormatWeekRange(const Date& current) {
    static const char* months[] = {
        "January", "February", "March", "April", "May", "June",
        "July", "August", "September", "October", "November", "December"
    };

    Date start = current.addDays(-current.dayOfWeek());
    Date end = start.addDays(6);
    if (start.getMonth() == end.getMonth() && start.getYear() == end.getYear()) {
        return wxString::Format("%s %d-%d, %d",
                                months[start.getMonth() - 1],
                                start.getDay(), end.getDay(), start.getYear());
    }

    if (start.getYear() == end.getYear()) {
        return wxString::Format("%s %d - %s %d, %d",
                                months[start.getMonth() - 1], start.getDay(),
                                months[end.getMonth() - 1], end.getDay(), start.getYear());
    }

    return wxString::Format("%s %d, %d - %s %d, %d",
                            months[start.getMonth() - 1], start.getDay(), start.getYear(),
                            months[end.getMonth() - 1], end.getDay(), end.getYear());
}

/**
 * @brief PlaceholderIconForWeather method of CalendarView.cpp.
 * @param iconName parameter for PlaceholderIconForWeather.
 */
wxString PlaceholderIconForWeather(const wxString& iconName) {
    if (iconName == "sun") return "[SUN]";
    if (iconName == "cloud") return "[CLD]";
    if (iconName == "fog") return "[FOG]";
    if (iconName == "drizzle") return "[DRZ]";
    if (iconName == "rain") return "[RAN]";
    if (iconName == "snow") return "[SNW]";
    if (iconName == "storm") return "[STM]";
    return "[WTH]";
}

/**
 * @brief FormatRoundedWhole method of CalendarView.cpp.
 * @param value parameter for FormatRoundedWhole.
 */
wxString FormatRoundedWhole(double value) {
    long rounded = std::lround(value);
    return wxString::Format("%ld", rounded);
}

/**
 * @brief DegreeSymbol method of CalendarView.cpp.
 */
wxString DegreeSymbol() {
    return wxString::FromUTF8("\xC2\xB0");
}

/**
 * @brief FormatCoordinateValue method of CalendarView.cpp.
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
 * @brief StyleButton method of CalendarView.cpp.
 * @param button parameter for StyleButton.
 * @param primary parameter for StyleButton.
 */
void StyleButton(wxButton* button, bool primary = false) {
    wxFont font = button->GetFont();
    font.SetPointSize(10);
    font.SetWeight(wxFONTWEIGHT_BOLD);
    button->SetFont(font);
    button->SetMinSize(wxSize(-1, 34));

    if (primary) {
        button->SetBackgroundColour(PRIMARY_ACCENT);
        button->SetForegroundColour(*wxWHITE);
    }
    else {
        button->SetBackgroundColour(SOFT_BUTTON_BG);
        button->SetForegroundColour(TITLE_COLOR);
    }
}

wxStaticText* CreateEventChip(wxWindow* parent, const wxString& text,
                              const wxColour& fg, const wxColour& bg,
                              int maxWidth) {
    wxStaticText* chip = new wxStaticText(
        parent, wxID_ANY, text, wxDefaultPosition, wxDefaultSize,
        wxST_ELLIPSIZE_END | wxST_NO_AUTORESIZE);
    wxFont chipFont = chip->GetFont();
    chipFont.SetPointSize(8);
    chip->SetFont(chipFont);
    chip->SetForegroundColour(fg);
    chip->SetBackgroundColour(bg);
    chip->SetMinSize(wxSize(1, -1));
    chip->SetMaxSize(wxSize(maxWidth, -1));
    return chip;
}

wxPanel* CreateTimedEventCard(wxWindow* parent, const Event& event,
                              const wxColour& fg, const wxColour& bg,
                              int maxWidth, int minHeight) {
    wxPanel* card = new wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
    card->SetBackgroundColour(bg);
    card->SetMinSize(wxSize(maxWidth, minHeight));
    card->SetMaxSize(wxSize(maxWidth, -1));

    wxBoxSizer* cardSizer = new wxBoxSizer(wxVERTICAL);

    wxString timeText = FormatEventTimeLabel(event.getStartTime());
    if (!timeText.empty()) {
        wxStaticText* timeLabel = new wxStaticText(card, wxID_ANY, timeText);
        wxFont timeFont = timeLabel->GetFont();
        timeFont.SetPointSize(7);
        timeFont.SetWeight(wxFONTWEIGHT_NORMAL);
        timeLabel->SetFont(timeFont);
        timeLabel->SetForegroundColour(fg);
        timeLabel->SetBackgroundColour(bg);
        cardSizer->Add(timeLabel, 0, wxLEFT | wxRIGHT | wxTOP, 4);
    }

    wxStaticText* titleLabel = new wxStaticText(card, wxID_ANY, wxString(event.getTitle()));
    wxFont titleFont = titleLabel->GetFont();
    titleFont.SetPointSize(9);
    titleFont.SetWeight(wxFONTWEIGHT_BOLD);
    titleLabel->SetFont(titleFont);
    titleLabel->SetForegroundColour(fg);
    titleLabel->SetBackgroundColour(bg);
    titleLabel->SetMinSize(wxSize(maxWidth - 8, -1));
    titleLabel->Wrap(maxWidth - 8);
    cardSizer->Add(titleLabel, 0, wxLEFT | wxRIGHT | wxBOTTOM, 4);

    card->SetSizer(cardSizer);
    card->Layout();
    return card;
}

void SyncScrollSection(wxPanel* outerPanel, wxPanel* fixedPanel,
                       wxPanel* bodyPanel, wxScrolledWindow* scrollWin,
                       wxScrollBar* vscroll) {
    if (!outerPanel || !fixedPanel || !bodyPanel || !scrollWin || !vscroll) {
        return;
    }

    wxSize virtualSize = scrollWin->GetVirtualSize();
    wxSize clientSize = scrollWin->GetClientSize();
    if (virtualSize.GetHeight() < clientSize.GetHeight()) {
        virtualSize.SetHeight(clientSize.GetHeight());
    }

    int x = 0;
    int y = 0;
    scrollWin->GetViewStart(&x, &y);
    vscroll->SetScrollbar(y * 10, clientSize.GetHeight(), virtualSize.GetHeight(), clientSize.GetHeight());
}
}

// wxWidgets is similar to JavaFX with how functions and structure is set up (ex. ImageView, buttons, etc)
/**
 * @brief CalendarView method of CalendarView.cpp.
 * @param title parameter for CalendarView.
 */
CalendarView::CalendarView(const wxString& title)
        : wxFrame(nullptr, wxID_ANY, title, wxDefaultPosition, wxSize(980, 650)),
            weatherApi(),
            weatherAvailable(false) {

    settings.loadSettings();
    calendar.loadCalendars();
    calendar.loadEvents();
    applyWeatherSettings();

    // Sync calendar view with settings
    if (settings.getCalendarView() == "Day") calendar.setView(DAY_VIEW);
    else if (settings.getCalendarView() == "Week") calendar.setView(WEEK_VIEW);
    else calendar.setView(MONTH_VIEW);

    SetBackgroundColour(APP_BG);

    // Main container for everything shown in the frame.
    mainPanel = new wxPanel(this);
    mainPanel->SetBackgroundColour(APP_BG);
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    titleLabel = new wxStaticText(mainPanel, wxID_ANY, "ClearDay");
    titleLabel->SetForegroundColour(TITLE_COLOR);
    wxFont titleFont(22, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
    titleLabel->SetFont(titleFont);
    titleLabel->SetLabel("ClearDay");
    titleLabel->InvalidateBestSize();

    dateLabel = new wxStaticText(mainPanel, wxID_ANY, "");
    dateLabel->SetForegroundColour(SUBTITLE_COLOR);
    wxFont dateFont(14, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
    dateLabel->SetFont(dateFont);

    weatherLabel = new wxStaticText(mainPanel, wxID_ANY, "Weather: loading...");
    weatherLabel->SetForegroundColour(SUBTITLE_COLOR);
    wxFont weatherFont(11, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
    weatherLabel->SetFont(weatherFont);

    // Top navigation row: date movement + view mode buttons.
    wxBoxSizer* navSizer = new wxBoxSizer(wxHORIZONTAL);
    wxButton* todayBtn = new wxButton(mainPanel, wxID_ANY, "Today");
    wxButton* prevBtn = new wxButton(mainPanel, wxID_ANY, "Previous");
    wxButton* nextBtn = new wxButton(mainPanel, wxID_ANY, "Next");
    wxButton* dayBtn = new wxButton(mainPanel, wxID_ANY, "Day");
    wxButton* weekBtn = new wxButton(mainPanel, wxID_ANY, "Week");
    wxButton* monthBtn = new wxButton(mainPanel, wxID_ANY, "Month");
    wxButton* settingsBtn = new wxButton(mainPanel, wxID_ANY, "Settings");
    wxButton* viewEventsBtn = new wxButton(mainPanel, wxID_ANY, "View Events");
    wxButton* manageCalBtn = new wxButton(mainPanel, wxID_ANY, "Calendars");
    wxButton* addEventBtn = new wxButton(mainPanel, wxID_ANY, "Add Event");
    wxButton* checklistBtn = new wxButton(mainPanel, wxID_ANY, "Checklist");

    StyleButton(todayBtn);
    StyleButton(prevBtn);
    StyleButton(nextBtn);
    StyleButton(dayBtn);
    StyleButton(weekBtn);
    StyleButton(monthBtn);
    StyleButton(settingsBtn);
    StyleButton(viewEventsBtn);
    StyleButton(manageCalBtn);
    StyleButton(addEventBtn, true);
    StyleButton(checklistBtn);

    // Spacing and order for buttons
    navSizer->Add(todayBtn, 0, wxALL, 5);
    navSizer->Add(prevBtn, 0, wxALL, 5);
    navSizer->Add(nextBtn, 0, wxALL, 5);
    navSizer->AddSpacer(12);
    navSizer->Add(dayBtn, 0, wxALL, 5);
    navSizer->Add(weekBtn, 0, wxALL, 5);
    navSizer->Add(monthBtn, 0, wxALL, 5);
    navSizer->AddSpacer(12);
    navSizer->Add(settingsBtn, 0, wxALL, 5);
    navSizer->Add(viewEventsBtn, 0, wxALL, 5);
    navSizer->Add(manageCalBtn, 0, wxALL, 5);
    navSizer->Add(addEventBtn, 0, wxALL, 5);
    navSizer->Add(checklistBtn, 0, wxALL, 5);

    todayBtn->Bind(wxEVT_BUTTON, &CalendarView::OnToday, this);
    viewEventsBtn->Bind(wxEVT_BUTTON, &CalendarView::OnViewEvents, this);

    // Content area where the calendar cells will be drawn.
    contentPanel = new wxPanel(mainPanel);
    contentPanel->SetBackgroundColour(PANEL_BG);
    contentSizer = new wxBoxSizer(wxVERTICAL);
    contentPanel->SetSizer(contentSizer);

    // Main Layout
    mainSizer->Add(titleLabel, 0, wxTOP | wxLEFT | wxRIGHT | wxALIGN_CENTER_HORIZONTAL, 16);
    mainSizer->Add(dateLabel, 0, wxTOP | wxBOTTOM | wxALIGN_CENTER_HORIZONTAL, 8);
    mainSizer->Add(weatherLabel, 0, wxBOTTOM | wxALIGN_CENTER_HORIZONTAL, 8);
    mainSizer->Add(navSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxBOTTOM, 14);
    mainSizer->Add(contentPanel, 1, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 14);

    mainPanel->SetSizer(mainSizer);

    // Button bindings to handle interactions, similar to event handlers in JavaFX.
    prevBtn->Bind(wxEVT_BUTTON, &CalendarView::OnPrev, this);
    nextBtn->Bind(wxEVT_BUTTON, &CalendarView::OnNext, this);
    dayBtn->Bind(wxEVT_BUTTON, &CalendarView::OnDay, this);
    weekBtn->Bind(wxEVT_BUTTON, &CalendarView::OnWeek, this);
    monthBtn->Bind(wxEVT_BUTTON, &CalendarView::OnMonth, this);
    settingsBtn->Bind(wxEVT_BUTTON, &CalendarView::OnSettings, this);
    manageCalBtn->Bind(wxEVT_BUTTON, &CalendarView::OnManageCalendars, this);
    addEventBtn->Bind(wxEVT_BUTTON, &CalendarView::OnAddEvent, this);
    checklistBtn->Bind(wxEVT_BUTTON, &CalendarView::OnChecklist, this);
    Bind(wxEVT_CLOSE_WINDOW, &CalendarView::OnClose, this);

    // Initial draw based on the calendar's default date/view state.
    updateView();

    CallAfter([this]() {
        if (titleLabel) {
            titleLabel->InvalidateBestSize();
            titleLabel->SetSize(titleLabel->GetBestSize());
        }
        if (mainPanel) {
            mainPanel->Layout();
        }
        Layout();
        Refresh();
    });
}

/**
 * @brief toIsoDate method of CalendarView.cpp.
 * @param date parameter for toIsoDate.
 * @return Result of the operation.
 */
wxString CalendarView::toIsoDate(const Date& date) const {
    return wxString::Format("%04d-%02d-%02d", date.getYear(), date.getMonth(), date.getDay());
}

/**
 * @brief resolveWeatherTemperatureUnit method of CalendarView.cpp.
 * @return Result of the operation.
 */
wxString CalendarView::resolveWeatherTemperatureUnit() const {
    wxString settingUnit = wxString(settings.getTemperatureUnit()).Lower();
    if (settingUnit == "fahrenheit") {
        return "fahrenheit";
    }
    if (settingUnit == "celsius") {
        return "celsius";
    }

    double latitude = settings.getWeatherLatitude();
    double longitude = settings.getWeatherLongitude();
    bool inUnitedStates = (latitude >= 24.0 && latitude <= 49.5 && longitude >= -125.0 && longitude <= -66.0);
    return inUnitedStates ? "fahrenheit" : "celsius";
}

/**
 * @brief weatherUnitSymbol method of CalendarView.cpp.
 * @return Result of the operation.
 */
wxString CalendarView::weatherUnitSymbol() const {
    return (weatherApi.GetTemperatureUnit() == "fahrenheit") ? "F" : "C";
}

/**
 * @brief applyWeatherSettings method of CalendarView.cpp.
 */
void CalendarView::applyWeatherSettings() {
    weatherApi.SetLocation(settings.getWeatherLatitude(), settings.getWeatherLongitude());
    weatherApi.SetTemperatureUnit(resolveWeatherTemperatureUnit());
}

/**
 * @brief refreshWeatherData method of CalendarView.cpp.
 */
void CalendarView::refreshWeatherData() {
    wxString requestedUnit = resolveWeatherTemperatureUnit();
    wxString requestKey =
        FormatCoordinateValue(settings.getWeatherLatitude(), 6) + "," +
        FormatCoordinateValue(settings.getWeatherLongitude(), 6) + "," +
        requestedUnit;

    bool settingsChanged = (weatherRequestKey != requestKey);
    if (settingsChanged) {
        weatherRequestKey = requestKey;
        applyWeatherSettings();
        weatherLocationName = weatherApi.GetFriendlyLocationName();
    }

    wxString currentDateIso = toIsoDate(calendar.getCurrentDate());
    bool needsForecast = settingsChanged || !weatherAvailable || (weatherApi.GetWeatherForDate(currentDateIso) == nullptr);
    if (needsForecast) {
        weatherAvailable = weatherApi.FetchWeeklyForecast();
    }
}

/**
 * @brief buildWeatherSummary method of CalendarView.cpp.
 * @param date parameter for buildWeatherSummary.
 * @return Result of the operation.
 */
wxString CalendarView::buildWeatherSummary(const Date& date) const {
    wxString locationText = weatherLocationName;
    if (locationText.IsEmpty()) {
        locationText = FormatCoordinateValue(settings.getWeatherLatitude(), 4) + ", "
                     + FormatCoordinateValue(settings.getWeatherLongitude(), 4);
    }
    return locationText;
}

/**
 * @brief clearContent method of CalendarView.cpp.
 */
void CalendarView::clearContent() {
    if (contentSizer) {
        // Destroy previous controls so each redraw starts clean.
        contentSizer->Clear(true);
    }
}

// Method to create cell for current date, week, or month. Highlights today and selected date.
/**
 * @brief createCell method of CalendarView.cpp.
 * @param text parameter for createCell.
 * @param highlightToday parameter for createCell.
 * @param selectedDate parameter for createCell.
 * @return Result of the operation.
 */
wxPanel* CalendarView::createCell(const wxString& text, bool highlightToday, bool selectedDate) {
    wxPanel* panel = new wxPanel(contentPanel, wxID_ANY, wxDefaultPosition, wxSize(100, 70), wxBORDER_SIMPLE);
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

    wxStaticText* label = new wxStaticText(panel, wxID_ANY, text);
    wxFont labelFont = label->GetFont();
    labelFont.SetPointSize(11);
    label->SetFont(labelFont);
    label->SetForegroundColour(TITLE_COLOR);
    sizer->Add(label, 1, wxALIGN_LEFT | wxALL, 8);
    panel->SetSizer(sizer);

    // Color priority: today takes precedence over selected date.
    if (highlightToday) {
        panel->SetBackgroundColour(TODAY_BG);
    }
/**
 * @brief if method of CalendarView.cpp.
 * @param selectedDate parameter for if.
 * @return Result of the operation.
 */
    else if (selectedDate) {
        panel->SetBackgroundColour(SELECTED_BG);
    }
    else {
        panel->SetBackgroundColour(CELL_BG);
    }

    panel->SetForegroundColour(TITLE_COLOR);

    panel->SetMinSize(wxSize(100, 76));
    return panel;
}

/**
 * @brief renderDayView method of CalendarView.cpp.
 */
void CalendarView::renderDayView() {
    Date d = calendar.getCurrentDate();
    const int hourCount = 24;
    const int rowH = 48;
    int contentWidth = contentPanel->GetClientSize().GetWidth();
    if (contentWidth <= 0) contentWidth = GetClientSize().GetWidth();
    const int timeColWidth = 76;
    const int scrollbarWidth = 18;
    const int gridGap = 1;
    int dayColWidth = contentWidth - timeColWidth - scrollbarWidth - gridGap - 4;
    if (dayColWidth < 160) dayColWidth = 160;
    int chipMaxWidth = dayColWidth - 12;
    if (chipMaxWidth < 120) chipMaxWidth = 120;

    static const char* dayNames[] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
    wxString dayNameText = wxString(dayNames[d.dayOfWeek()]);
    int dayNumber = d.getDay();
    bool isToday = d.isToday();

    wxPanel* outerPanel = new wxPanel(contentPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
    outerPanel->SetBackgroundColour(WEEK_GRID_BG);
    wxBoxSizer* outerSizer = new wxBoxSizer(wxHORIZONTAL);

    // ── Fixed header ──
    wxPanel* fixedPanel = new wxPanel(outerPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
    fixedPanel->SetBackgroundColour(WEEK_GRID_BG);
    fixedPanel->SetDoubleBuffered(true);

    wxFlexGridSizer* headerGrid = new wxFlexGridSizer(2, 2, 1, 1);
    headerGrid->AddGrowableCol(1, 1);

    // Corner
    wxPanel* corner = new wxPanel(fixedPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
    corner->SetBackgroundColour(HEADER_BG);
    corner->SetMinSize(wxSize(76, 60));
    headerGrid->Add(corner, 1, wxEXPAND);

/**
 * @brief cell method of CalendarView.cpp.
 * @param custom-painted parameter for cell.
 * @return Result of the operation.
 */
    // Day header cell (custom-painted)
    wxPanel* headerCell = new wxPanel(fixedPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
    headerCell->SetBackgroundColour(HEADER_BG);
    headerCell->SetMinSize(wxSize(dayColWidth, 60));
    headerCell->SetMaxSize(wxSize(dayColWidth, -1));
    headerCell->SetDoubleBuffered(true);
/**
 * @brief headerCell->Bind method of CalendarView.cpp.
 * @param wxEVT_PAINT parameter for headerCell->Bind.
 * @param dayNameText parameter for headerCell->Bind.
 * @param dayNumber parameter for headerCell->Bind.
 * @param isToday](wxPaintEvent parameter for headerCell->Bind.
 * @return Result of the operation.
 */
    headerCell->Bind(wxEVT_PAINT, [headerCell, dayNameText, dayNumber, isToday](wxPaintEvent&) {
        wxPaintDC dc(headerCell);
        dc.SetBackground(wxBrush(HEADER_BG));
        dc.Clear();

        wxFont nameFont = headerCell->GetFont();
        nameFont.SetPointSize(9);
        nameFont.SetWeight(wxFONTWEIGHT_BOLD);
        dc.SetFont(nameFont);
        dc.SetTextForeground(SUBTITLE_COLOR);
        int nameW = 0, nameH = 0;
        dc.GetTextExtent(dayNameText, &nameW, &nameH);
        int cellW = headerCell->GetClientSize().GetWidth();
        dc.DrawText(dayNameText, (cellW - nameW) / 2, 4);

        wxFont numFont = headerCell->GetFont();
        numFont.SetPointSize(12);
        numFont.SetWeight(isToday ? wxFONTWEIGHT_BOLD : wxFONTWEIGHT_NORMAL);
        dc.SetFont(numFont);
        dc.SetTextForeground(isToday ? TODAY_TEXT : TITLE_COLOR);
        wxString dayNumberText = wxString::Format("%d", dayNumber);
        int numW = 0, numH = 0;
        dc.GetTextExtent(dayNumberText, &numW, &numH);
        dc.DrawText(dayNumberText, (cellW - numW) / 2, 22);
    });
    headerGrid->Add(headerCell, 1, wxEXPAND);

    // All-day label
    wxPanel* allDayLabelCell = new wxPanel(fixedPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
    allDayLabelCell->SetBackgroundColour(WEEK_ALLDAY_BG);
    wxBoxSizer* allDayLabelSizer = new wxBoxSizer(wxVERTICAL);
    wxStaticText* allDayLabel = new wxStaticText(allDayLabelCell, wxID_ANY, "all-day");
    wxFont allDayFont = allDayLabel->GetFont();
    allDayFont.SetPointSize(9);
    allDayLabel->SetFont(allDayFont);
    allDayLabel->SetForegroundColour(SUBTITLE_COLOR);
    allDayLabelSizer->Add(allDayLabel, 0, wxALIGN_RIGHT | wxTOP | wxRIGHT, 6);
    allDayLabelCell->SetSizer(allDayLabelSizer);
    allDayLabelCell->SetMinSize(wxSize(timeColWidth, 34));
    allDayLabelCell->SetMaxSize(wxSize(timeColWidth, -1));
    headerGrid->Add(allDayLabelCell, 1, wxEXPAND);

    // All-day events cell
    std::vector<Event> allEvents = calendar.getEventsForDate(d.toString());
    wxPanel* allDayCell = new wxPanel(fixedPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
    allDayCell->SetBackgroundColour(isToday ? WEEK_SELECTED_COL_BG : WEEK_ALLDAY_BG);
    allDayCell->SetMinSize(wxSize(dayColWidth, 34));
    allDayCell->SetMaxSize(wxSize(dayColWidth, -1));
    wxBoxSizer* allDaySizer = new wxBoxSizer(wxVERTICAL);
    int timedEventCardHeight = 42;
    int timedEventGap = 2;
    int timedEventsInHour[24] = {0};
    int allDayCount = 0;
    for (const Event& e : allEvents) {
        if (e.getIsAllDay()) {
            allDayCount++;
            continue;
        }
        int eventHour = ParseHour(e.getStartTime());
        if (eventHour >= 0 && eventHour < hourCount) {
            timedEventsInHour[eventHour]++;
        }
    }
    int allDayHeight = 34;
    if (allDayCount > 0) {
        allDayHeight = 8 + (allDayCount * 20);
        if (allDayHeight < 34) allDayHeight = 34;
        allDayCell->SetMinSize(wxSize(dayColWidth, allDayHeight));
    }
    for (const Event& e : allEvents) {
        if (!e.getIsAllDay()) continue;
        wxString chipText = wxString(e.getTitle());
        if (chipText.Length() > 16) chipText = chipText.SubString(0, 15) + "…";
        wxStaticText* chip = CreateEventChip(
            allDayCell, chipText, WEEK_EVENT_ALT_TEXT, WEEK_EVENT_ALT_BG, chipMaxWidth);
        allDaySizer->Add(chip, 0, wxLEFT | wxRIGHT | wxTOP, 3);
    }
    allDayCell->SetSizer(allDaySizer);
    headerGrid->Add(allDayCell, 1, wxEXPAND);

    fixedPanel->SetSizer(headerGrid);

    // ── Scrollable 24-hour body ──
    wxScrolledWindow* scrollWin = new wxScrolledWindow(
        outerPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize,
        wxBORDER_NONE);
    scrollWin->SetScrollRate(0, 10);
    scrollWin->SetBackgroundColour(WEEK_GRID_BG);
    scrollWin->SetDoubleBuffered(true);
    scrollWin->ShowScrollbars(wxSHOW_SB_NEVER, wxSHOW_SB_NEVER);

    wxPanel* bodyPanel = new wxPanel(scrollWin, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
    bodyPanel->SetBackgroundColour(WEEK_GRID_BG);
    std::vector<wxWindow*> hourLabelCells;
    std::vector<wxWindow*> timeCells;

    wxFlexGridSizer* bodyGrid = new wxFlexGridSizer(hourCount, 2, 1, 1);
    bodyGrid->AddGrowableCol(1, 1);

    for (int hour = 0; hour < hourCount; hour++) {
        int timeRowHeight = rowH;
        if (timedEventsInHour[hour] > 0) {
            timeRowHeight = 4 + (timedEventsInHour[hour] * (timedEventCardHeight + timedEventGap));
            if (timeRowHeight < rowH) timeRowHeight = rowH;
        }
        wxPanel* hourLabelCell = new wxPanel(bodyPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
        hourLabelCell->SetBackgroundColour(WEEK_ALLDAY_BG);
        wxBoxSizer* hourLabelSizer = new wxBoxSizer(wxVERTICAL);
        wxStaticText* hourLabel = new wxStaticText(hourLabelCell, wxID_ANY, FormatHourLabel(hour));
        wxFont hourFont = hourLabel->GetFont();
        hourFont.SetPointSize(8);
        hourLabel->SetFont(hourFont);
        hourLabel->SetForegroundColour(SUBTITLE_COLOR);
        hourLabelSizer->Add(hourLabel, 0, wxALIGN_RIGHT | wxTOP | wxRIGHT, 6);
        hourLabelCell->SetSizer(hourLabelSizer);
        hourLabelCell->SetMinSize(wxSize(timeColWidth, timeRowHeight));
        hourLabelCell->SetMaxSize(wxSize(timeColWidth, -1));
        hourLabelCells.push_back(hourLabelCell);
        bodyGrid->Add(hourLabelCell, 1, wxEXPAND);

        wxPanel* timeCell = new wxPanel(bodyPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
        timeCell->SetBackgroundColour(isToday ? WEEK_SELECTED_COL_BG : WEEK_CELL_BG);
        timeCell->SetMinSize(wxSize(dayColWidth, timeRowHeight));
        timeCell->SetMaxSize(wxSize(dayColWidth, -1));
        timeCells.push_back(timeCell);
        wxBoxSizer* timeCellSizer = new wxBoxSizer(wxVERTICAL);

        for (const Event& e : allEvents) {
            if (e.getIsAllDay()) continue;
            if (ParseHour(e.getStartTime()) != hour) continue;
            Event capturedEvent = e; // capture by value for click handler
            if ((timeCellSizer->GetItemCount() % 2) == 0) {
                wxPanel* card = CreateTimedEventCard(
                    timeCell, e, WEEK_EVENT_TEXT, WEEK_EVENT_BG, chipMaxWidth, timedEventCardHeight);
                // Story: View Event Details — clicking an event card opens its details
                auto clickHandler = [this, capturedEvent](wxMouseEvent&) { ShowEventDetails(capturedEvent); };
                card->SetCursor(wxCursor(wxCURSOR_HAND));
                card->Bind(wxEVT_LEFT_UP, clickHandler);
                for (wxWindow* child : card->GetChildren()) { child->Bind(wxEVT_LEFT_UP, clickHandler); child->SetCursor(wxCursor(wxCURSOR_HAND)); }
                timeCellSizer->Add(card, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 2);
            } else {
                wxPanel* card = CreateTimedEventCard(
                    timeCell, e, WEEK_EVENT_ALT_TEXT, WEEK_EVENT_ALT_BG, chipMaxWidth, timedEventCardHeight);
                auto clickHandler = [this, capturedEvent](wxMouseEvent&) { ShowEventDetails(capturedEvent); };
                card->SetCursor(wxCursor(wxCURSOR_HAND));
                card->Bind(wxEVT_LEFT_UP, clickHandler);
                for (wxWindow* child : card->GetChildren()) { child->Bind(wxEVT_LEFT_UP, clickHandler); child->SetCursor(wxCursor(wxCURSOR_HAND)); }
                timeCellSizer->Add(card, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 2);
            }
        }
        timeCell->SetSizer(timeCellSizer);
        bodyGrid->Add(timeCell, 1, wxEXPAND);
    }

    bodyPanel->SetSizer(bodyGrid);

    wxBoxSizer* scrollContentSizer = new wxBoxSizer(wxVERTICAL);
    scrollContentSizer->Add(bodyPanel, 0, wxEXPAND);
    scrollWin->SetSizer(scrollContentSizer);
    scrollWin->FitInside();
    int dayPanelWidth = timeColWidth + gridGap + dayColWidth;
    fixedPanel->SetMinSize(wxSize(dayPanelWidth, -1));
    bodyPanel->SetMinSize(wxSize(dayPanelWidth, -1));

    wxScrollBar* vscroll = new wxScrollBar(outerPanel, wxID_ANY,
        wxDefaultPosition, wxDefaultSize, wxSB_VERTICAL);

    wxBoxSizer* leftCol = new wxBoxSizer(wxVERTICAL);
    leftCol->Add(fixedPanel, 0, wxEXPAND | wxBOTTOM, 1);
    leftCol->Add(scrollWin, 1, wxEXPAND);
    outerSizer->Add(leftCol, 1, wxEXPAND);
    outerSizer->Add(vscroll, 0, wxEXPAND);
    outerPanel->SetSizer(outerSizer);

    scrollWin->CallAfter([outerPanel, fixedPanel, bodyPanel, scrollWin, vscroll, rowH]() {
        int initPos = 7 * (rowH + 1);
        scrollWin->Scroll(-1, initPos / 10);
        SyncScrollSection(outerPanel, fixedPanel, bodyPanel, scrollWin, vscroll);
    });

    auto onVScrollDay = [scrollWin](wxScrollEvent& e) {
        scrollWin->Scroll(-1, e.GetPosition() / 10);
        e.Skip();
    };
    vscroll->Bind(wxEVT_SCROLL_TOP,          onVScrollDay);
    vscroll->Bind(wxEVT_SCROLL_BOTTOM,       onVScrollDay);
    vscroll->Bind(wxEVT_SCROLL_LINEUP,       onVScrollDay);
    vscroll->Bind(wxEVT_SCROLL_LINEDOWN,     onVScrollDay);
    vscroll->Bind(wxEVT_SCROLL_PAGEUP,       onVScrollDay);
    vscroll->Bind(wxEVT_SCROLL_PAGEDOWN,     onVScrollDay);
    vscroll->Bind(wxEVT_SCROLL_THUMBTRACK,   onVScrollDay);
    vscroll->Bind(wxEVT_SCROLL_THUMBRELEASE, onVScrollDay);
    vscroll->Bind(wxEVT_SCROLL_CHANGED,      onVScrollDay);

/**
 * @brief scrollWin->Bind method of CalendarView.cpp.
 * @param wxEVT_MOUSEWHEEL parameter for scrollWin->Bind.
 * @param fixedPanel parameter for scrollWin->Bind.
 * @param bodyPanel parameter for scrollWin->Bind.
 * @param scrollWin parameter for scrollWin->Bind.
 * @param e parameter for scrollWin->Bind.
 * @return Result of the operation.
 */
    scrollWin->Bind(wxEVT_MOUSEWHEEL, [outerPanel, fixedPanel, bodyPanel, scrollWin, vscroll](wxMouseEvent& e) {
        e.Skip();
        scrollWin->CallAfter([outerPanel, fixedPanel, bodyPanel, scrollWin, vscroll]() {
            SyncScrollSection(outerPanel, fixedPanel, bodyPanel, scrollWin, vscroll);
        });
    });

/**
 * @brief scrollWin->Bind method of CalendarView.cpp.
 * @param wxEVT_SIZE parameter for scrollWin->Bind.
 * @param fixedPanel parameter for scrollWin->Bind.
 * @param bodyPanel parameter for scrollWin->Bind.
 * @param scrollWin parameter for scrollWin->Bind.
 * @param e parameter for scrollWin->Bind.
 * @return Result of the operation.
 */
    scrollWin->Bind(wxEVT_SIZE, [outerPanel, fixedPanel, bodyPanel, scrollWin, vscroll](wxSizeEvent& e) {
        e.Skip();
        scrollWin->CallAfter([outerPanel, fixedPanel, bodyPanel, scrollWin, vscroll]() {
            SyncScrollSection(outerPanel, fixedPanel, bodyPanel, scrollWin, vscroll);
        });
    });

    contentSizer->Add(outerPanel, 1, wxEXPAND | wxALL, 1);
}

/**
 * @brief renderWeekView method of CalendarView.cpp.
 */
void CalendarView::renderWeekView() {
    Date current = calendar.getCurrentDate();
    int weekday = current.dayOfWeek();
    Date start = current.addDays(-weekday);

    static const char* names[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
    const int hourCount = 24;
    const int rowH = 48; // pixels per hour row
    int contentWidth = contentPanel->GetClientSize().GetWidth();
    if (contentWidth <= 0) contentWidth = GetClientSize().GetWidth();
    const int timeColWidth = 76;
    const int scrollbarWidth = 18;
    const int gridGap = 1;
    int availableDayWidth = contentWidth - timeColWidth - scrollbarWidth - (7 * gridGap) - 4;
    if (availableDayWidth < 7 * 72) availableDayWidth = 7 * 72;
    std::vector<int> dayColWidths(7, availableDayWidth / 7);
    for (int i = 0; i < availableDayWidth % 7; i++) {
        dayColWidths[i]++;
    }
    int chipMaxWidth = (availableDayWidth / 7) - 12;
    if (chipMaxWidth < 56) chipMaxWidth = 56;
    int timedEventCardHeight = 42;
    int timedEventGap = 2;

    // Outer panel: fixed header on top, scrollable body below
    wxPanel* outerPanel = new wxPanel(contentPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
    outerPanel->SetBackgroundColour(WEEK_GRID_BG);
    wxBoxSizer* outerSizer = new wxBoxSizer(wxHORIZONTAL);

    // ── Fixed header section (day name + number cells, all-day row) ──
    wxPanel* fixedPanel = new wxPanel(outerPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
    fixedPanel->SetBackgroundColour(WEEK_GRID_BG);
    fixedPanel->SetDoubleBuffered(true);

    wxFlexGridSizer* headerGrid = new wxFlexGridSizer(2, 8, 1, 1);
    for (int col = 1; col <= 7; col++) {
        headerGrid->AddGrowableCol(col, 1);
    }

    // Corner cell
    wxPanel* corner = new wxPanel(fixedPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
    corner->SetBackgroundColour(HEADER_BG);
    corner->SetMinSize(wxSize(76, 60));
    headerGrid->Add(corner, 1, wxEXPAND);

/**
 * @brief cells method of CalendarView.cpp.
 * @param custom-painted parameter for cells.
 * @return Result of the operation.
 */
    // Day header cells (custom-painted)
    std::vector<wxWindow*> headerCells;
    for (int i = 0; i < 7; i++) {
        Date d = start.addDays(i);
        wxPanel* headerCell = new wxPanel(fixedPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
        headerCell->SetBackgroundColour(HEADER_BG);
        headerCell->SetMinSize(wxSize(dayColWidths[i], 60));
        headerCell->SetMaxSize(wxSize(dayColWidths[i], -1));
        headerCell->SetDoubleBuffered(true);

        wxString dayNameText = wxString(names[i]);
        int dayNumber = d.getDay();
        bool isToday = d.isToday();

/**
 * @brief headerCell->Bind method of CalendarView.cpp.
 * @param wxEVT_PAINT parameter for headerCell->Bind.
 * @param dayNameText parameter for headerCell->Bind.
 * @param dayNumber parameter for headerCell->Bind.
 * @param isToday](wxPaintEvent parameter for headerCell->Bind.
 * @return Result of the operation.
 */
        headerCell->Bind(wxEVT_PAINT, [headerCell, dayNameText, dayNumber, isToday](wxPaintEvent&) {
            wxPaintDC dc(headerCell);
            dc.SetBackground(wxBrush(HEADER_BG));
            dc.Clear();

            wxFont nameFont = headerCell->GetFont();
            nameFont.SetPointSize(9);
            nameFont.SetWeight(wxFONTWEIGHT_BOLD);
            dc.SetFont(nameFont);
            dc.SetTextForeground(SUBTITLE_COLOR);
            int nameW = 0, nameH = 0;
            dc.GetTextExtent(dayNameText, &nameW, &nameH);
            int cellW = headerCell->GetClientSize().GetWidth();
            dc.DrawText(dayNameText, (cellW - nameW) / 2, 4);

            wxFont numFont = headerCell->GetFont();
            numFont.SetPointSize(12);
            numFont.SetWeight(isToday ? wxFONTWEIGHT_BOLD : wxFONTWEIGHT_NORMAL);
            dc.SetFont(numFont);
            dc.SetTextForeground(isToday ? TODAY_TEXT : TITLE_COLOR);
            wxString dayNumberText = wxString::Format("%d", dayNumber);
            int numW = 0, numH = 0;
            dc.GetTextExtent(dayNumberText, &numW, &numH);
            dc.DrawText(dayNumberText, (cellW - numW) / 2, 22);
        });

        headerGrid->Add(headerCell, 1, wxEXPAND);
        headerCells.push_back(headerCell);
    }

    // All-day label cell
    wxPanel* allDayLabelCell = new wxPanel(fixedPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
    allDayLabelCell->SetBackgroundColour(WEEK_ALLDAY_BG);
    wxBoxSizer* allDayLabelSizer = new wxBoxSizer(wxVERTICAL);
    wxStaticText* allDayLabel = new wxStaticText(allDayLabelCell, wxID_ANY, "all-day");
    wxFont allDayFont = allDayLabel->GetFont();
    allDayFont.SetPointSize(9);
    allDayLabel->SetFont(allDayFont);
    allDayLabel->SetForegroundColour(SUBTITLE_COLOR);
    allDayLabelSizer->Add(allDayLabel, 0, wxALIGN_RIGHT | wxTOP | wxRIGHT, 6);
    allDayLabelCell->SetSizer(allDayLabelSizer);
    allDayLabelCell->SetMinSize(wxSize(timeColWidth, 34));
    allDayLabelCell->SetMaxSize(wxSize(timeColWidth, -1));
    headerGrid->Add(allDayLabelCell, 1, wxEXPAND);

    // All-day event cells
    std::vector<wxWindow*> allDayCells;
    int maxAllDayCount = 0;
    std::vector<std::vector<Event>> weekEventsByDay;
    for (int i = 0; i < 7; i++) {
        Date d = start.addDays(i);
        std::vector<Event> events = calendar.getEventsForDate(d.toString());
        weekEventsByDay.push_back(events);
        int dayAllDayCount = 0;
        for (const Event& event : events) {
            if (event.getIsAllDay()) dayAllDayCount++;
        }
        if (dayAllDayCount > maxAllDayCount) maxAllDayCount = dayAllDayCount;

        wxPanel* allDayCell = new wxPanel(fixedPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
        allDayCell->SetBackgroundColour((d == current) ? WEEK_SELECTED_COL_BG : WEEK_ALLDAY_BG);
        int allDayHeight = 34;
        if (maxAllDayCount > 0) {
            allDayHeight = 8 + (maxAllDayCount * 20);
            if (allDayHeight < 34) allDayHeight = 34;
        }
        allDayCell->SetMinSize(wxSize(dayColWidths[i], allDayHeight));
        allDayCell->SetMaxSize(wxSize(dayColWidths[i], -1));
        wxBoxSizer* allDaySizer = new wxBoxSizer(wxVERTICAL);

        for (const Event& e : events) {
            if (!e.getIsAllDay()) {
                continue;
            }
            wxString chipText = wxString(e.getTitle());
            if (chipText.Length() > 16) {
                chipText = chipText.SubString(0, 15) + "…";
            }
            wxStaticText* chip = CreateEventChip(
                allDayCell, chipText, WEEK_EVENT_ALT_TEXT, WEEK_EVENT_ALT_BG, chipMaxWidth);
            allDaySizer->Add(chip, 0, wxLEFT | wxRIGHT | wxTOP, 3);
        }
        allDayCell->SetSizer(allDaySizer);
        headerGrid->Add(allDayCell, 1, wxEXPAND);
        allDayCells.push_back(allDayCell);
    }

    fixedPanel->SetSizer(headerGrid);

    // ── Scrollable body (24 hour rows) ──
    wxScrolledWindow* scrollWin = new wxScrolledWindow(
        outerPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize,
        wxBORDER_NONE);
    scrollWin->SetScrollRate(0, 10);
    scrollWin->SetBackgroundColour(WEEK_GRID_BG);
    scrollWin->SetDoubleBuffered(true);
    scrollWin->ShowScrollbars(wxSHOW_SB_NEVER, wxSHOW_SB_NEVER);

    wxPanel* bodyPanel = new wxPanel(scrollWin, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
    bodyPanel->SetBackgroundColour(WEEK_GRID_BG);
    std::vector<wxWindow*> hourLabelCells;
    std::vector<std::vector<wxWindow*>> timeRows;

    wxFlexGridSizer* bodyGrid = new wxFlexGridSizer(hourCount, 8, 1, 1);
    for (int col = 1; col <= 7; col++) {
        bodyGrid->AddGrowableCol(col, 1);
    }
    // No AddGrowableRow — fixed-height rows so content overflows for scrolling

    for (int hour = 0; hour < hourCount; hour++) {
        int maxTimedEventsThisHour = 0;
        for (int dayIndex = 0; dayIndex < 7; dayIndex++) {
            int count = 0;
            for (const Event& event : weekEventsByDay[dayIndex]) {
                if (event.getIsAllDay()) continue;
                if (ParseHour(event.getStartTime()) == hour) count++;
            }
            if (count > maxTimedEventsThisHour) maxTimedEventsThisHour = count;
        }
        int timeRowHeight = rowH;
        if (maxTimedEventsThisHour > 0) {
            timeRowHeight = 4 + (maxTimedEventsThisHour * (timedEventCardHeight + timedEventGap));
            if (timeRowHeight < rowH) timeRowHeight = rowH;
        }
        wxPanel* hourLabelCell = new wxPanel(bodyPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
        hourLabelCell->SetBackgroundColour(WEEK_ALLDAY_BG);
        wxBoxSizer* hourLabelSizer = new wxBoxSizer(wxVERTICAL);
        wxStaticText* hourLabel = new wxStaticText(hourLabelCell, wxID_ANY, FormatHourLabel(hour));
        wxFont hourFont = hourLabel->GetFont();
        hourFont.SetPointSize(8);
        hourLabel->SetFont(hourFont);
        hourLabel->SetForegroundColour(SUBTITLE_COLOR);
        hourLabelSizer->Add(hourLabel, 0, wxALIGN_RIGHT | wxTOP | wxRIGHT, 6);
        hourLabelCell->SetSizer(hourLabelSizer);
        hourLabelCell->SetMinSize(wxSize(timeColWidth, timeRowHeight));
        hourLabelCell->SetMaxSize(wxSize(timeColWidth, -1));
        hourLabelCells.push_back(hourLabelCell);
        bodyGrid->Add(hourLabelCell, 1, wxEXPAND);

        std::vector<wxWindow*> rowCells;

        for (int dayIndex = 0; dayIndex < 7; dayIndex++) {
            Date d = start.addDays(dayIndex);
            const std::vector<Event>& events = weekEventsByDay[dayIndex];

            wxPanel* timeCell = new wxPanel(bodyPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
            timeCell->SetBackgroundColour((d == current) ? WEEK_SELECTED_COL_BG : WEEK_CELL_BG);
            timeCell->SetMinSize(wxSize(dayColWidths[dayIndex], timeRowHeight));
            timeCell->SetMaxSize(wxSize(dayColWidths[dayIndex], -1));
            rowCells.push_back(timeCell);
            wxBoxSizer* timeCellSizer = new wxBoxSizer(wxVERTICAL);

            for (const Event& e : events) {
                if (e.getIsAllDay()) {
                    continue;
                }
                int eventHour = ParseHour(e.getStartTime());
                if (eventHour != hour) {
                    continue;
                }
                Event capturedEvent = e; // capture by value for click handler
                if ((timeCellSizer->GetItemCount() % 2) == 0) {
                    wxPanel* card = CreateTimedEventCard(
                        timeCell, e, WEEK_EVENT_TEXT, WEEK_EVENT_BG, chipMaxWidth, timedEventCardHeight);
                    auto clickHandler = [this, capturedEvent](wxMouseEvent&) { ShowEventDetails(capturedEvent); };
                    card->SetCursor(wxCursor(wxCURSOR_HAND));
                    card->Bind(wxEVT_LEFT_UP, clickHandler);
                    for (wxWindow* child : card->GetChildren()) { child->Bind(wxEVT_LEFT_UP, clickHandler); child->SetCursor(wxCursor(wxCURSOR_HAND)); }
                    timeCellSizer->Add(card, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 2);
                }
                else {
                    wxPanel* card = CreateTimedEventCard(
                        timeCell, e, WEEK_EVENT_ALT_TEXT, WEEK_EVENT_ALT_BG, chipMaxWidth, timedEventCardHeight);
                    auto clickHandler = [this, capturedEvent](wxMouseEvent&) { ShowEventDetails(capturedEvent); };
                    card->SetCursor(wxCursor(wxCURSOR_HAND));
                    card->Bind(wxEVT_LEFT_UP, clickHandler);
                    for (wxWindow* child : card->GetChildren()) { child->Bind(wxEVT_LEFT_UP, clickHandler); child->SetCursor(wxCursor(wxCURSOR_HAND)); }
                    timeCellSizer->Add(card, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 2);
                }
            }

            timeCell->SetSizer(timeCellSizer);
            bodyGrid->Add(timeCell, 1, wxEXPAND);
        }

        timeRows.push_back(rowCells);
    }

    bodyPanel->SetSizer(bodyGrid);

    wxBoxSizer* scrollContentSizer = new wxBoxSizer(wxVERTICAL);
    scrollContentSizer->Add(bodyPanel, 0, wxEXPAND);
    scrollWin->SetSizer(scrollContentSizer);
    scrollWin->FitInside();
    int weekPanelWidth = timeColWidth + (7 * gridGap);
    for (int width : dayColWidths) {
        weekPanelWidth += width;
    }
    fixedPanel->SetMinSize(wxSize(weekPanelWidth, -1));
    bodyPanel->SetMinSize(wxSize(weekPanelWidth, -1));

    wxScrollBar* vscroll = new wxScrollBar(outerPanel, wxID_ANY,
        wxDefaultPosition, wxDefaultSize, wxSB_VERTICAL);

    wxBoxSizer* leftCol = new wxBoxSizer(wxVERTICAL);
    leftCol->Add(fixedPanel, 0, wxEXPAND | wxBOTTOM, 1);
    leftCol->Add(scrollWin, 1, wxEXPAND);
    outerSizer->Add(leftCol, 1, wxEXPAND);
    outerSizer->Add(vscroll, 0, wxEXPAND);
    outerPanel->SetSizer(outerSizer);

    scrollWin->CallAfter([outerPanel, fixedPanel, bodyPanel, scrollWin, vscroll, rowH]() {
        int initPos = 7 * (rowH + 1);
        scrollWin->Scroll(-1, initPos / 10);
        SyncScrollSection(outerPanel, fixedPanel, bodyPanel, scrollWin, vscroll);
    });

    auto onVScrollWeek = [scrollWin](wxScrollEvent& e) {
        scrollWin->Scroll(-1, e.GetPosition() / 10);
        e.Skip();
    };
    vscroll->Bind(wxEVT_SCROLL_TOP,          onVScrollWeek);
    vscroll->Bind(wxEVT_SCROLL_BOTTOM,       onVScrollWeek);
    vscroll->Bind(wxEVT_SCROLL_LINEUP,       onVScrollWeek);
    vscroll->Bind(wxEVT_SCROLL_LINEDOWN,     onVScrollWeek);
    vscroll->Bind(wxEVT_SCROLL_PAGEUP,       onVScrollWeek);
    vscroll->Bind(wxEVT_SCROLL_PAGEDOWN,     onVScrollWeek);
    vscroll->Bind(wxEVT_SCROLL_THUMBTRACK,   onVScrollWeek);
    vscroll->Bind(wxEVT_SCROLL_THUMBRELEASE, onVScrollWeek);
    vscroll->Bind(wxEVT_SCROLL_CHANGED,      onVScrollWeek);

/**
 * @brief scrollWin->Bind method of CalendarView.cpp.
 * @param wxEVT_MOUSEWHEEL parameter for scrollWin->Bind.
 * @param fixedPanel parameter for scrollWin->Bind.
 * @param bodyPanel parameter for scrollWin->Bind.
 * @param scrollWin parameter for scrollWin->Bind.
 * @param e parameter for scrollWin->Bind.
 * @return Result of the operation.
 */
    scrollWin->Bind(wxEVT_MOUSEWHEEL, [outerPanel, fixedPanel, bodyPanel, scrollWin, vscroll](wxMouseEvent& e) {
        e.Skip();
        scrollWin->CallAfter([outerPanel, fixedPanel, bodyPanel, scrollWin, vscroll]() {
            SyncScrollSection(outerPanel, fixedPanel, bodyPanel, scrollWin, vscroll);
        });
    });

/**
 * @brief scrollWin->Bind method of CalendarView.cpp.
 * @param wxEVT_SIZE parameter for scrollWin->Bind.
 * @param fixedPanel parameter for scrollWin->Bind.
 * @param bodyPanel parameter for scrollWin->Bind.
 * @param scrollWin parameter for scrollWin->Bind.
 * @param e parameter for scrollWin->Bind.
 * @return Result of the operation.
 */
    scrollWin->Bind(wxEVT_SIZE, [outerPanel, fixedPanel, bodyPanel, scrollWin, vscroll](wxSizeEvent& e) {
        e.Skip();
        scrollWin->CallAfter([outerPanel, fixedPanel, bodyPanel, scrollWin, vscroll]() {
            SyncScrollSection(outerPanel, fixedPanel, bodyPanel, scrollWin, vscroll);
        });
    });

    contentSizer->Add(outerPanel, 1, wxEXPAND | wxALL, 1);
}

/**
 * @brief renderMonthView method of CalendarView.cpp.
 */
void CalendarView::renderMonthView() {
    Date current = calendar.getCurrentDate();
    Date firstOfMonth(1, current.getMonth(), current.getYear());

    int firstWeekday = firstOfMonth.dayOfWeek();
    Date gridStart = firstOfMonth.addDays(-firstWeekday);

    static const char* names[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

    // Header row with compact weekday labels.
    wxPanel* headerPanel = new wxPanel(contentPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
    headerPanel->SetBackgroundColour(PANEL_BG);
    wxGridSizer* header = new wxGridSizer(1, 7, 1, 1);
    for (int i = 0; i < 7; i++) {
        wxPanel* p = new wxPanel(headerPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
        p->SetBackgroundColour(HEADER_BG);
        wxBoxSizer* s = new wxBoxSizer(wxVERTICAL);
        wxStaticText* dayName = new wxStaticText(p, wxID_ANY, names[i]);
        wxFont headerFont = dayName->GetFont();
        headerFont.SetPointSize(9);
        headerFont.SetWeight(wxFONTWEIGHT_NORMAL);
        dayName->SetFont(headerFont);
        dayName->SetForegroundColour(SUBTITLE_COLOR);
        s->Add(dayName, 1, wxALIGN_CENTER | wxALL, 4);
        p->SetSizer(s);
        header->Add(p, 1, wxEXPAND);
    }
    headerPanel->SetSizer(header);

    wxPanel* gridPanel = new wxPanel(contentPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
    gridPanel->SetBackgroundColour(GRID_LINE_COLOR);
    wxGridSizer* grid = new wxGridSizer(6, 7, 1, 1);

    for (int i = 0; i < 42; i++) {
        Date d = gridStart.addDays(i);
        bool inCurrentMonth = (d.getMonth() == current.getMonth() && d.getYear() == current.getYear());
        bool isSelected = (d == current);
        bool isToday = d.isToday();
        const WeatherDay* weather = weatherApi.GetWeatherForDate(toIsoDate(d));

        wxPanel* cell = new wxPanel(gridPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
        if (isSelected) {
            cell->SetBackgroundColour(SELECTED_BG);
        }
        else if (inCurrentMonth) {
            cell->SetBackgroundColour(CELL_BG);
        }
        else {
            cell->SetBackgroundColour(OUTSIDE_MONTH_BG);
        }

        wxBoxSizer* cellSizer = new wxBoxSizer(wxVERTICAL);

        wxStaticText* dayLabel = new wxStaticText(cell, wxID_ANY, wxString::Format("%d", d.getDay()));
        wxFont dayFont = dayLabel->GetFont();
        dayFont.SetPointSize(11);
        dayFont.SetWeight(wxFONTWEIGHT_NORMAL);
        if (isToday) {
            dayFont.SetWeight(wxFONTWEIGHT_BOLD);
        }
        dayLabel->SetFont(dayFont);

        if (isToday) {
            dayLabel->SetForegroundColour(TODAY_TEXT);
        }
        else if (!inCurrentMonth) {
            dayLabel->SetForegroundColour(OUTSIDE_MONTH_TEXT);
        }
        else {
            dayLabel->SetForegroundColour(TITLE_COLOR);
        }

        wxBoxSizer* topRowSizer = new wxBoxSizer(wxHORIZONTAL);
        wxBoxSizer* dateAndIconSizer = new wxBoxSizer(wxHORIZONTAL);
        dateAndIconSizer->Add(dayLabel, 0, wxTOP | wxLEFT | wxRIGHT, 6);

        if (weather != nullptr) {
            wxBitmap iconBitmap = weatherApi.GetWeatherBitmap(WEATHER_ICON_FOLDER, weather->iconName);
            if (iconBitmap.IsOk()) {
                wxImage iconImage = iconBitmap.ConvertToImage();
                iconImage.Rescale(14, 14, wxIMAGE_QUALITY_HIGH);
                wxStaticBitmap* weatherIcon = new wxStaticBitmap(cell, wxID_ANY, wxBitmap(iconImage));
                dateAndIconSizer->Add(weatherIcon, 0, wxTOP | wxLEFT, 6);
            }
            else {
                wxStaticText* weatherIcon = new wxStaticText(cell, wxID_ANY, PlaceholderIconForWeather(weather->iconName));
                wxFont iconFont = weatherIcon->GetFont();
                iconFont.SetPointSize(7);
                iconFont.SetWeight(wxFONTWEIGHT_BOLD);
                weatherIcon->SetFont(iconFont);
                weatherIcon->SetForegroundColour(SUBTITLE_COLOR);
                dateAndIconSizer->Add(weatherIcon, 0, wxTOP | wxLEFT, 6);
            }
        }

        topRowSizer->Add(dateAndIconSizer, 0, wxALIGN_TOP | wxLEFT);
        topRowSizer->AddStretchSpacer(1);

        if (weather != nullptr) {
            wxString highText = "H " + FormatRoundedWhole(weather->tempMax) + DegreeSymbol();
            wxStaticText* highLabel = new wxStaticText(cell, wxID_ANY, highText);
            wxFont highFont = highLabel->GetFont();
            highFont.SetPointSize(8);
            highFont.SetWeight(wxFONTWEIGHT_BOLD);
            highLabel->SetFont(highFont);
            highLabel->SetForegroundColour(SUBTITLE_COLOR);
            topRowSizer->Add(highLabel, 0, wxTOP | wxRIGHT, 6);
        }

        cellSizer->Add(topRowSizer, 0, wxEXPAND);
        cellSizer->AddStretchSpacer(1);

        if (weather != nullptr) {
            wxString lowText = "L " + FormatRoundedWhole(weather->tempMin) + DegreeSymbol();
            wxStaticText* lowLabel = new wxStaticText(cell, wxID_ANY, lowText);
            wxFont lowFont = lowLabel->GetFont();
            lowFont.SetPointSize(8);
            lowFont.SetWeight(wxFONTWEIGHT_NORMAL);
            lowLabel->SetFont(lowFont);
            lowLabel->SetForegroundColour(SUBTITLE_COLOR);
            cellSizer->Add(lowLabel, 0, wxRIGHT | wxBOTTOM | wxALIGN_RIGHT, 3);
        }

        std::vector<Event> events = calendar.getEventsForDate(d.toString());
        if (!events.empty()) {
            wxString chipText = wxString(events[0].getTitle());
            if (chipText.Length() > 14) {
                chipText = chipText.SubString(0, 13) + "…";
            }
            wxStaticText* eventChip = new wxStaticText(cell, wxID_ANY, chipText);
            wxFont chipFont = eventChip->GetFont();
            chipFont.SetPointSize(8);
            eventChip->SetFont(chipFont);
            eventChip->SetForegroundColour(TITLE_COLOR);
            wxColour chipColour = hexToColour(calendar.getColourForCalendar(events[0].getCalendarName()));
            eventChip->SetBackgroundColour(chipColour);
            eventChip->SetForegroundColour(*wxWHITE);
            eventChip->SetCursor(wxCursor(wxCURSOR_HAND));

            // Story: View Event Details — clicking the chip opens the detail view
            Event capturedEvent = events[0];
            auto clickHandler = [this, capturedEvent](wxMouseEvent& e) {
                ShowEventDetails(capturedEvent);
                e.Skip();
            };
            
            eventChip->Bind(wxEVT_LEFT_UP, clickHandler);
            
            // Mac Fallback: Make the whole calendar day cell clickable too
            cell->SetCursor(wxCursor(wxCURSOR_HAND)); 
            cell->Bind(wxEVT_LEFT_UP, clickHandler);

            cellSizer->Add(eventChip, 0, wxLEFT | wxRIGHT | wxBOTTOM, 4);
        }

        Date capturedDate = d;
        auto cellClickHandler = [this, capturedDate](wxMouseEvent&) {
            // Set the calendar's current date to the clicked day
            calendar.setCurrentDate(capturedDate);
            updateView();
        };

        cell->SetCursor(wxCursor(wxCURSOR_HAND));
        cell->Bind(wxEVT_LEFT_UP, cellClickHandler);

        // Also bind all child widgets so clicking anywhere in the cell works
        for (wxWindow* child : cell->GetChildren()) {
            child->SetCursor(wxCursor(wxCURSOR_HAND));
            child->Bind(wxEVT_LEFT_UP, cellClickHandler);
        }

        cell->SetMinSize(wxSize(-1, 78));
        cell->SetSizer(cellSizer);
        grid->Add(cell, 1, wxEXPAND);
    }
    gridPanel->SetSizer(grid);

    contentSizer->Add(headerPanel, 0, wxEXPAND | wxBOTTOM, 1);
    contentSizer->Add(gridPanel, 1, wxEXPAND);
}

/**
 * @brief updateView method of CalendarView.cpp.
 */
void CalendarView::updateView() {
    // Rebuild the content area whenever date or mode changes.
    clearContent();

    Date d = calendar.getCurrentDate();
    ViewMode mode = calendar.getCurrentView();

    refreshWeatherData();
    weatherLabel->SetLabel(buildWeatherSummary(d));

    if (mode == DAY_VIEW) {
        dateLabel->SetLabel("Day View - " + FormatLongDate(d));
        renderDayView();
    }
    else if (mode == WEEK_VIEW) {
        dateLabel->SetLabel("Week View - " + FormatWeekRange(d));
        renderWeekView();
    }
    else {
        dateLabel->SetLabel(wxString(d.monthYearString()));
        renderMonthView();
    }

    contentPanel->Layout();
    mainPanel->Layout();
}

/**
 * @brief OnPrev method of CalendarView.cpp.
 * @param event parameter for OnPrev.
 */
void CalendarView::OnPrev(wxCommandEvent& event) {
    calendar.goBackward();
    updateView();
}

/**
 * @brief OnNext method of CalendarView.cpp.
 * @param event parameter for OnNext.
 */
void CalendarView::OnNext(wxCommandEvent& event) {
    calendar.goForward();
    updateView();
}

/**
 * @brief OnDay method of CalendarView.cpp.
 * @param event parameter for OnDay.
 */
void CalendarView::OnDay(wxCommandEvent& event) {
    calendar.setView(DAY_VIEW);
    updateView();
}

/**
 * @brief OnWeek method of CalendarView.cpp.
 * @param event parameter for OnWeek.
 */
void CalendarView::OnWeek(wxCommandEvent& event) {
    calendar.setView(WEEK_VIEW);
    updateView();
}

/**
 * @brief OnMonth method of CalendarView.cpp.
 * @param event parameter for OnMonth.
 */
void CalendarView::OnMonth(wxCommandEvent& event) {
    calendar.setView(MONTH_VIEW);
    updateView();
}

/**
 * @brief OnSettings method of CalendarView.cpp.
 * @param event parameter for OnSettings.
 */
void CalendarView::OnSettings(wxCommandEvent& event) {
    wxDialog* settingsDialog = new wxDialog(this, wxID_ANY, "Settings", wxDefaultPosition, wxSize(460, 420));

    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    // Time Zone
    wxBoxSizer* tzSizer = new wxBoxSizer(wxHORIZONTAL);
    tzSizer->Add(new wxStaticText(settingsDialog, wxID_ANY, "Time Zone:"), 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
    wxTextCtrl* tzCtrl = new wxTextCtrl(settingsDialog, wxID_ANY, settings.getTimeZone());
    tzSizer->Add(tzCtrl, 1, wxALL, 5);
    mainSizer->Add(tzSizer, 0, wxEXPAND);

    // Layout Preference
    wxBoxSizer* layoutSizer = new wxBoxSizer(wxHORIZONTAL);
    layoutSizer->Add(new wxStaticText(settingsDialog, wxID_ANY, "Layout Pref:"), 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
    wxTextCtrl* layoutCtrl = new wxTextCtrl(settingsDialog, wxID_ANY, settings.getLayoutPref());
    layoutSizer->Add(layoutCtrl, 1, wxALL, 5);
    mainSizer->Add(layoutSizer, 0, wxEXPAND);

    // Calendar View
    wxBoxSizer* viewSizer = new wxBoxSizer(wxHORIZONTAL);
    viewSizer->Add(new wxStaticText(settingsDialog, wxID_ANY, "Calendar View:"), 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
    wxChoice* viewChoice = new wxChoice(settingsDialog, wxID_ANY);
    viewChoice->Append("Day");
    viewChoice->Append("Week");
    viewChoice->Append("Month");
    if (settings.getCalendarView() == "Day") viewChoice->SetSelection(0);
    else if (settings.getCalendarView() == "Week") viewChoice->SetSelection(1);
    else viewChoice->SetSelection(2);
    viewSizer->Add(viewChoice, 1, wxALL, 5);
    mainSizer->Add(viewSizer, 0, wxEXPAND);

    wxBoxSizer* unitSizer = new wxBoxSizer(wxHORIZONTAL);
    unitSizer->Add(new wxStaticText(settingsDialog, wxID_ANY, "Temp Unit:"), 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
    wxChoice* unitChoice = new wxChoice(settingsDialog, wxID_ANY);
    unitChoice->Append("Auto (from location)");
    unitChoice->Append("Celsius");
    unitChoice->Append("Fahrenheit");
    std::string unitPref = settings.getTemperatureUnit();
    if (unitPref == "Celsius") unitChoice->SetSelection(1);
    else if (unitPref == "Fahrenheit") unitChoice->SetSelection(2);
    else unitChoice->SetSelection(0);
    unitSizer->Add(unitChoice, 1, wxALL, 5);
    mainSizer->Add(unitSizer, 0, wxEXPAND);

    wxBoxSizer* locationSearchSizer = new wxBoxSizer(wxHORIZONTAL);
    locationSearchSizer->Add(new wxStaticText(settingsDialog, wxID_ANY, "Find Location:"), 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
    wxTextCtrl* locationCtrl = new wxTextCtrl(settingsDialog, wxID_ANY, "");
    wxButton* searchLocationBtn = new wxButton(settingsDialog, wxID_ANY, "Search");
    locationSearchSizer->Add(locationCtrl, 1, wxALL, 5);
    locationSearchSizer->Add(searchLocationBtn, 0, wxALL, 5);
    mainSizer->Add(locationSearchSizer, 0, wxEXPAND);

    wxBoxSizer* searchResultSizer = new wxBoxSizer(wxHORIZONTAL);
    searchResultSizer->Add(new wxStaticText(settingsDialog, wxID_ANY, "Matches:"), 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
    wxChoice* locationChoice = new wxChoice(settingsDialog, wxID_ANY);
    searchResultSizer->Add(locationChoice, 1, wxALL, 5);
    mainSizer->Add(searchResultSizer, 0, wxEXPAND);

    wxFloatingPointValidator<double> coordValidator(6, nullptr, wxNUM_VAL_DEFAULT);
    coordValidator.SetRange(-180.0, 180.0);

    wxBoxSizer* latSizer = new wxBoxSizer(wxHORIZONTAL);
    latSizer->Add(new wxStaticText(settingsDialog, wxID_ANY, "Latitude:"), 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
    wxTextCtrl* latCtrl = new wxTextCtrl(settingsDialog, wxID_ANY,
        FormatCoordinateValue(settings.getWeatherLatitude(), 6),
        wxDefaultPosition, wxDefaultSize, 0, coordValidator);
    latSizer->Add(latCtrl, 1, wxALL, 5);
    mainSizer->Add(latSizer, 0, wxEXPAND);

    wxBoxSizer* lonSizer = new wxBoxSizer(wxHORIZONTAL);
    lonSizer->Add(new wxStaticText(settingsDialog, wxID_ANY, "Longitude:"), 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
    wxTextCtrl* lonCtrl = new wxTextCtrl(settingsDialog, wxID_ANY,
        FormatCoordinateValue(settings.getWeatherLongitude(), 6),
        wxDefaultPosition, wxDefaultSize, 0, coordValidator);
    lonSizer->Add(lonCtrl, 1, wxALL, 5);
    mainSizer->Add(lonSizer, 0, wxEXPAND);

    std::shared_ptr<std::vector<WeatherLocationResult>> foundLocations =
        std::make_shared<std::vector<WeatherLocationResult>>();

    searchLocationBtn->Bind(wxEVT_BUTTON,
/**
 * @brief foundLocations] method of CalendarView.cpp.
 * @param wxCommandEvent parameter for foundLocations].
 * @return Result of the operation.
 */
        [this, settingsDialog, locationCtrl, locationChoice, latCtrl, lonCtrl, foundLocations](wxCommandEvent&) {
            wxString searchText = locationCtrl->GetValue();
            searchText.Trim(true);
            searchText.Trim(false);
            if (searchText.IsEmpty()) {
                wxMessageBox("Enter a city or town name (for example: London, ON).", "Location Search", wxOK | wxICON_INFORMATION, settingsDialog);
                return;
            }

            *foundLocations = weatherApi.SearchLocations(searchText, 8);
            locationChoice->Clear();

            if (foundLocations->empty()) {
                wxMessageBox("No matching locations found.", "Location Search", wxOK | wxICON_INFORMATION, settingsDialog);
                return;
            }

            for (const WeatherLocationResult& location : *foundLocations) {
                locationChoice->Append(location.displayName);
            }

            locationChoice->SetSelection(0);
            const WeatherLocationResult& chosen = (*foundLocations)[0];
            latCtrl->SetValue(FormatCoordinateValue(chosen.latitude, 6));
            lonCtrl->SetValue(FormatCoordinateValue(chosen.longitude, 6));
        });

    locationChoice->Bind(wxEVT_CHOICE,
/**
 * @brief foundLocations] method of CalendarView.cpp.
 * @param wxCommandEvent parameter for foundLocations].
 * @return Result of the operation.
 */
        [locationChoice, latCtrl, lonCtrl, foundLocations](wxCommandEvent&) {
            int selected = locationChoice->GetSelection();
            if (selected == wxNOT_FOUND || selected >= static_cast<int>(foundLocations->size())) {
                return;
            }

            const WeatherLocationResult& chosen = (*foundLocations)[selected];
            latCtrl->SetValue(FormatCoordinateValue(chosen.latitude, 6));
            lonCtrl->SetValue(FormatCoordinateValue(chosen.longitude, 6));
        });

    // Show Conflicts
    wxCheckBox* conflictsCheck = new wxCheckBox(settingsDialog, wxID_ANY, "Show Conflicts");
    conflictsCheck->SetValue(settings.getShowConflicts());
    mainSizer->Add(conflictsCheck, 0, wxALL, 5);

    // Buttons
    wxBoxSizer* buttonSizer = new wxBoxSizer(wxHORIZONTAL);
    wxButton* okBtn = new wxButton(settingsDialog, wxID_OK, "OK");
    wxButton* cancelBtn = new wxButton(settingsDialog, wxID_CANCEL, "Cancel");
    wxButton* resetBtn = new wxButton(settingsDialog, wxID_ANY, "Reset to Default");
    buttonSizer->Add(okBtn, 0, wxALL, 5);
    buttonSizer->Add(cancelBtn, 0, wxALL, 5);
    buttonSizer->Add(resetBtn, 0, wxALL, 5);
    mainSizer->Add(buttonSizer, 0, wxALIGN_CENTER);

    settingsDialog->SetSizer(mainSizer);
    mainSizer->Fit(settingsDialog);

    // Handle Reset button
/**
 * @brief resetBtn->Bind method of CalendarView.cpp.
 * @param wxEVT_BUTTON parameter for resetBtn->Bind.
 * @param tzCtrl parameter for resetBtn->Bind.
 * @param layoutCtrl parameter for resetBtn->Bind.
 * @param viewChoice parameter for resetBtn->Bind.
 * @param unitChoice parameter for resetBtn->Bind.
 * @param latCtrl parameter for resetBtn->Bind.
 * @param lonCtrl parameter for resetBtn->Bind.
 * @param conflictsCheck](wxCommandEvent parameter for resetBtn->Bind.
 * @return Result of the operation.
 */
    resetBtn->Bind(wxEVT_BUTTON, [this, tzCtrl, layoutCtrl, viewChoice, unitChoice, latCtrl, lonCtrl, conflictsCheck](wxCommandEvent&) {
        settings.resetToDefault();
        tzCtrl->SetValue(settings.getTimeZone());
        layoutCtrl->SetValue(settings.getLayoutPref());
        if (settings.getCalendarView() == "Day") viewChoice->SetSelection(0);
        else if (settings.getCalendarView() == "Week") viewChoice->SetSelection(1);
        else viewChoice->SetSelection(2);
        if (settings.getTemperatureUnit() == "Celsius") unitChoice->SetSelection(1);
        else if (settings.getTemperatureUnit() == "Fahrenheit") unitChoice->SetSelection(2);
        else unitChoice->SetSelection(0);
        latCtrl->SetValue(FormatCoordinateValue(settings.getWeatherLatitude(), 6));
        lonCtrl->SetValue(FormatCoordinateValue(settings.getWeatherLongitude(), 6));
        conflictsCheck->SetValue(settings.getShowConflicts());
    });

    if (settingsDialog->ShowModal() == wxID_OK) {
        settings.setTimeZone(tzCtrl->GetValue().ToStdString());
        settings.setLayoutPref(layoutCtrl->GetValue().ToStdString());

        int unitSel = unitChoice->GetSelection();
        std::string unitStr = (unitSel == 1) ? "Celsius" : (unitSel == 2) ? "Fahrenheit" : "Auto";
        settings.setTemperatureUnit(unitStr);

        double latitude = settings.getWeatherLatitude();
        double longitude = settings.getWeatherLongitude();
        latCtrl->GetValue().ToDouble(&latitude);
        lonCtrl->GetValue().ToDouble(&longitude);
        if (latitude < -90.0 || latitude > 90.0) {
            wxMessageBox("Latitude must be between -90 and 90.", "Validation Error", wxOK | wxICON_ERROR);
            settingsDialog->Destroy();
            return;
        }
        if (longitude < -180.0 || longitude > 180.0) {
            wxMessageBox("Longitude must be between -180 and 180.", "Validation Error", wxOK | wxICON_ERROR);
            settingsDialog->Destroy();
            return;
        }
        settings.setWeatherLatitude(latitude);
        settings.setWeatherLongitude(longitude);

        int sel = viewChoice->GetSelection();
        std::string viewStr = (sel == 0) ? "Day" : (sel == 1) ? "Week" : "Month";
        settings.setCalendarView(viewStr);
        settings.setShowConflicts(conflictsCheck->GetValue());

        // Sync calendar view if changed
        if (viewStr == "Day") calendar.setView(DAY_VIEW);
        else if (viewStr == "Week") calendar.setView(WEEK_VIEW);
        else calendar.setView(MONTH_VIEW);

        applyWeatherSettings();
        updateView();
        settings.saveSettings();
    }

    settingsDialog->Destroy();
}

/**
 * @brief OnAddEvent method of CalendarView.cpp.
 * @param event parameter for OnAddEvent.
 */
void CalendarView::OnAddEvent(wxCommandEvent& event) {
    wxDialog* addDialog = new wxDialog(this, wxID_ANY, "Add Event", wxDefaultPosition, wxSize(450, 450));
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    // Title (Required)
    wxBoxSizer* titleSizer = new wxBoxSizer(wxHORIZONTAL);
    titleSizer->Add(new wxStaticText(addDialog, wxID_ANY, "Title (Required):"), 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
    wxTextCtrl* titleCtrl = new wxTextCtrl(addDialog, wxID_ANY);
    titleSizer->Add(titleCtrl, 1, wxALL, 5);
    mainSizer->Add(titleSizer, 0, wxEXPAND);

    // Location (Optional)
    wxBoxSizer* locSizer = new wxBoxSizer(wxHORIZONTAL);
    locSizer->Add(new wxStaticText(addDialog, wxID_ANY, "Location:"), 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
    wxTextCtrl* locCtrl = new wxTextCtrl(addDialog, wxID_ANY);
    locSizer->Add(locCtrl, 1, wxALL, 5);
    mainSizer->Add(locSizer, 0, wxEXPAND);

    // Description (Optional)
    wxBoxSizer* descSizer = new wxBoxSizer(wxHORIZONTAL);
    descSizer->Add(new wxStaticText(addDialog, wxID_ANY, "Description:"), 0, wxALL, 5);
    // wxTE_MULTILINE makes this a larger box instead of a single line
    wxTextCtrl* descCtrl = new wxTextCtrl(addDialog, wxID_ANY, "", wxDefaultPosition, wxSize(-1, 60), wxTE_MULTILINE);
    descSizer->Add(descCtrl, 1, wxALL, 5);
    mainSizer->Add(descSizer, 0, wxEXPAND);

/**
 * @brief Picker method of CalendarView.cpp.
 * @param Calendar/Arrows parameter for Picker.
 */
    // Date Picker (Built-in Calendar/Arrows)
    wxBoxSizer* dateSizer = new wxBoxSizer(wxHORIZONTAL);
    dateSizer->Add(new wxStaticText(addDialog, wxID_ANY, "Date:"), 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
    Date selectedDate = calendar.getCurrentDate();
    wxDateTime prefilledDate(selectedDate.getDay(),
                            wxDateTime::Month(selectedDate.getMonth() - 1), 
                            selectedDate.getYear());
    wxDatePickerCtrl* dateCtrl = new wxDatePickerCtrl(addDialog, wxID_ANY, prefilledDate);    dateSizer->Add(dateCtrl, 1, wxALL, 5);
    mainSizer->Add(dateSizer, 0, wxEXPAND);

    // --- End Date Picker ---
    wxBoxSizer* endDateSizer = new wxBoxSizer(wxHORIZONTAL);
    endDateSizer->Add(new wxStaticText(addDialog, wxID_ANY, "End Date:"), 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
    wxDatePickerCtrl* endDateCtrl = new wxDatePickerCtrl(addDialog, wxID_ANY, wxDefaultDateTime);
    endDateSizer->Add(endDateCtrl, 1, wxALL, 5);
    mainSizer->Add(endDateSizer, 0, wxEXPAND);

    // Start Time Picker (Built-in Arrows)
    wxBoxSizer* startSizer = new wxBoxSizer(wxHORIZONTAL);
    startSizer->Add(new wxStaticText(addDialog, wxID_ANY, "Start Time:"), 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
    wxTimePickerCtrl* startCtrl = new wxTimePickerCtrl(addDialog, wxID_ANY, wxDefaultDateTime);
    startSizer->Add(startCtrl, 1, wxALL, 5);
    mainSizer->Add(startSizer, 0, wxEXPAND);

    // End Time Picker (Built-in Arrows)
    wxBoxSizer* endSizer = new wxBoxSizer(wxHORIZONTAL);
    endSizer->Add(new wxStaticText(addDialog, wxID_ANY, "End Time:"), 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
    wxTimePickerCtrl* endCtrl = new wxTimePickerCtrl(addDialog, wxID_ANY, wxDefaultDateTime);
    endSizer->Add(endCtrl, 1, wxALL, 5);
    mainSizer->Add(endSizer, 0, wxEXPAND);

    // All Day Toggle
    wxCheckBox* allDayCheck = new wxCheckBox(addDialog, wxID_ANY, "All Day Event");
    mainSizer->Add(allDayCheck, 0, wxALL, 5);

    // --- BINDING: Greys out the time pickers when checked ---
/**
 * @brief allDayCheck->Bind method of CalendarView.cpp.
 * @param wxEVT_CHECKBOX parameter for allDayCheck->Bind.
 * @param e parameter for allDayCheck->Bind.
 * @return Result of the operation.
 */
    allDayCheck->Bind(wxEVT_CHECKBOX, [startCtrl, endCtrl](wxCommandEvent& e) {
        bool isChecked = e.IsChecked();
        startCtrl->Enable(!isChecked); // Disables if checked
        endCtrl->Enable(!isChecked);
    });

    // Calendar picker
    wxBoxSizer* calSizer = new wxBoxSizer(wxHORIZONTAL);
    calSizer->Add(new wxStaticText(addDialog, wxID_ANY, "Calendar:"), 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
    wxChoice* calChoice = new wxChoice(addDialog, wxID_ANY);
    for (const CalendarMeta& cm : calendar.getCalendars())
        calChoice->Append(wxString(cm.name));
    calChoice->SetSelection(0);
    calSizer->Add(calChoice, 1, wxALL, 5);
    mainSizer->Add(calSizer, 0, wxEXPAND);

    // Recurring toggle
    wxCheckBox* recurCheck = new wxCheckBox(addDialog, wxID_ANY, "Recurring Event");
    mainSizer->Add(recurCheck, 0, wxALL, 5);

    wxBoxSizer* recurOptionsSizer = new wxBoxSizer(wxHORIZONTAL);
    wxChoice* repeatTypeChoice = new wxChoice(addDialog, wxID_ANY);
    repeatTypeChoice->Append("daily");
    repeatTypeChoice->Append("weekly");
    repeatTypeChoice->Append("monthly");
    repeatTypeChoice->SetSelection(0);
    repeatTypeChoice->Enable(false);

    wxSpinCtrl* repeatCountSpin = new wxSpinCtrl(addDialog, wxID_ANY, "0", wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 365, 0);
    repeatCountSpin->Enable(false);

    recurOptionsSizer->Add(new wxStaticText(addDialog, wxID_ANY, "Repeat:"), 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
    recurOptionsSizer->Add(repeatTypeChoice, 1, wxALL, 5);
    recurOptionsSizer->Add(new wxStaticText(addDialog, wxID_ANY, "Times (0=∞):"), 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
    recurOptionsSizer->Add(repeatCountSpin, 0, wxALL, 5);
    mainSizer->Add(recurOptionsSizer, 0, wxEXPAND);

    // Enable/disable recurring options based on checkbox
/**
 * @brief recurCheck->Bind method of CalendarView.cpp.
 * @param wxEVT_CHECKBOX parameter for recurCheck->Bind.
 * @param e parameter for recurCheck->Bind.
 * @return Result of the operation.
 */
    recurCheck->Bind(wxEVT_CHECKBOX, [repeatTypeChoice, repeatCountSpin](wxCommandEvent& e) {
        bool checked = e.IsChecked();
        repeatTypeChoice->Enable(checked);
        repeatCountSpin->Enable(checked);
    });

    // Buttons
    wxBoxSizer* buttonSizer = new wxBoxSizer(wxHORIZONTAL);
    wxButton* okBtn = new wxButton(addDialog, wxID_OK, "Add");
    wxButton* cancelBtn = new wxButton(addDialog, wxID_CANCEL, "Cancel");
    buttonSizer->Add(okBtn, 0, wxALL, 5);
    buttonSizer->Add(cancelBtn, 0, wxALL, 5);
    mainSizer->Add(buttonSizer, 0, wxALIGN_CENTER);

    addDialog->SetSizer(mainSizer);
    mainSizer->Fit(addDialog);

    // --- VALIDATION & SAVE LOGIC ---
    bool saved = false;
    while (!saved) {
        if (addDialog->ShowModal() == wxID_OK) {
            std::string title = titleCtrl->GetValue().ToStdString();
            
            // Acceptance Test Enforcement: Title cannot be blank
            if (title.empty()) {
                wxMessageBox("Title is required to create an event.", "Validation Error", wxOK | wxICON_ERROR);
                continue; // Loops back to the dialog without deleting it
            }

            std::string location = locCtrl->GetValue().ToStdString();
            std::string description = descCtrl->GetValue().ToStdString();
            
            // Format the Date Picker to exactly match your custom Date class output
            wxDateTime dt = dateCtrl->GetValue();
            std::string dateStr = std::to_string(dt.GetDay()) + "/" + 
                                  std::to_string(dt.GetMonth() + 1) + "/" + 
                                  std::to_string(dt.GetYear());

            wxDateTime endDt = endDateCtrl->GetValue();
            std::string endDateStr = std::to_string(endDt.GetDay()) + "/" + 
                                     std::to_string(endDt.GetMonth() + 1) + "/" + 
                                     std::to_string(endDt.GetYear());

            std::string startTime = startCtrl->GetValue().FormatISOTime().ToStdString();
            std::string endTime = endCtrl->GetValue().FormatISOTime().ToStdString();
            bool isAllDay = allDayCheck->GetValue();

            // --- TIME & DATE VALIDATION ---
            wxDateTime startDateVal = dateCtrl->GetValue();
            wxDateTime endDateVal = endDateCtrl->GetValue();

            if (!isAllDay) {
                wxDateTime startTimeVal = startCtrl->GetValue();
                wxDateTime endTimeVal = endCtrl->GetValue();
                
                // Merge times into the date objects
                startDateVal.SetHour(startTimeVal.GetHour());
                startDateVal.SetMinute(startTimeVal.GetMinute());
                endDateVal.SetHour(endTimeVal.GetHour());
                endDateVal.SetMinute(endTimeVal.GetMinute());
            } else {
                // All-day events compare purely by date
                startDateVal.SetHour(0); startDateVal.SetMinute(0);
                endDateVal.SetHour(23); endDateVal.SetMinute(59);
            }

            if (endDateVal.IsEarlierThan(startDateVal)) {
                wxMessageBox("The event cannot end before it starts.", "Validation Error", wxOK | wxICON_ERROR);
                continue; // Loop back to the dialog
            }
            // ------------------------------

            // Conflict Check
            if (!isAllDay && !startTime.empty() && !endTime.empty()) {
                if (calendar.detectConflict(dateStr, startTime, endTime)) {
                    wxMessageBox("Warning: This event conflicts with existing events on the same date.", "Event Conflict", wxOK | wxICON_WARNING);
                }
            }

            bool isRecurring = recurCheck->GetValue();
            std::string repeatType = isRecurring ? repeatTypeChoice->GetStringSelection().ToStdString() : "";
            int repeatCount = isRecurring ? repeatCountSpin->GetValue() : 0;
            std::string calName = calChoice->GetStringSelection().ToStdString();

            Event e = Event::createEvent(title, location, description, dateStr, endDateStr,
            startTime, endTime, isAllDay,
            isRecurring, repeatCount, repeatType, calName);
            calendar.addEvent(e);
            calendar.saveEvents(); // Persist new event
            updateView();
            saved = true; // Breaks the loop
        } else {
            break; // User clicked Cancel
        }
    }

    addDialog->Destroy();
}

/**
 * @brief ShowEventDetails method of CalendarView.cpp.
 * @param e parameter for ShowEventDetails.
 */
void CalendarView::ShowEventDetails(const Event& e) {
    wxDialog* dlg = new wxDialog(this, wxID_ANY, "Event Details",
                                 wxDefaultPosition, wxSize(450, 370));
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    // Helper: add a bold label + value row
    auto addRow = [&](const wxString& label, const wxString& value, bool multiline = false) {
        wxBoxSizer* row = new wxBoxSizer(wxHORIZONTAL);

        wxStaticText* lbl = new wxStaticText(dlg, wxID_ANY, label,
            wxDefaultPosition, wxSize(90, -1), wxALIGN_RIGHT);
        wxFont boldFont = lbl->GetFont();
        boldFont.SetWeight(wxFONTWEIGHT_BOLD);
        lbl->SetFont(boldFont);
        row->Add(lbl, 0, wxALIGN_TOP | wxALL, 6);

        if (multiline) {
            // wxTE_WORDWRAP ensures long descriptions wrap inside the box
            wxTextCtrl* val = new wxTextCtrl(dlg, wxID_ANY, value,
                wxDefaultPosition, wxSize(-1, 90),
                wxTE_MULTILINE | wxTE_READONLY | wxTE_WORDWRAP | wxBORDER_SIMPLE);
            val->SetBackgroundColour(dlg->GetBackgroundColour());
            row->Add(val, 1, wxEXPAND | wxALL, 6);
        } else {
            wxStaticText* val = new wxStaticText(dlg, wxID_ANY, value);
            val->Wrap(280);
            row->Add(val, 1, wxALIGN_CENTER_VERTICAL | wxALL, 6);
        }
        mainSizer->Add(row, 0, wxEXPAND | wxLEFT | wxRIGHT, 8);
    };

    wxString timeStr;
    if (e.getIsAllDay()) {
        timeStr = "All Day";
    } else {
        timeStr = wxString(e.getStartTime()) + " \u2013 " + wxString(e.getEndTime());
    }

    wxString dateDisplayStr;
    if (e.getDate() == e.getEndDate() || e.getEndDate().empty()) {
        dateDisplayStr = wxString(e.getDate());
    } else {
        dateDisplayStr = wxString(e.getDate()) + " to " + wxString(e.getEndDate());
    }

    addRow("Title:",       wxString(e.getTitle()));
    addRow("Date:",        dateDisplayStr);
    addRow("Time:",        timeStr);
    addRow("Location:",    wxString(e.getLocation()));
    addRow("Description:", wxString(e.getDescription()), /*multiline=*/true);

    wxBoxSizer* actionSizer = new wxBoxSizer(wxHORIZONTAL);
    wxButton* editBtn = new wxButton(dlg, wxID_ANY, "Edit");
    wxButton* deleteBtn = new wxButton(dlg, wxID_ANY, "Delete");
    wxButton* closeBtn = new wxButton(dlg, wxID_CANCEL, "Close");

    StyleButton(editBtn);
    StyleButton(deleteBtn);
    StyleButton(closeBtn);

    actionSizer->Add(editBtn, 0, wxALL, 5);
    actionSizer->Add(deleteBtn, 0, wxALL, 5);
    actionSizer->Add(closeBtn, 0, wxALL, 5);
    mainSizer->Add(actionSizer, 0, wxALIGN_CENTER | wxALL, 10);

    // Capture the event safely for the lambdas
    Event capturedEvent = e;

    // Bind Edit Button
/**
 * @brief editBtn->Bind method of CalendarView.cpp.
 * @param wxEVT_BUTTON parameter for editBtn->Bind.
 * @param capturedEvent parameter for editBtn->Bind.
 * @param dlg](wxCommandEvent parameter for editBtn->Bind.
 * @return Result of the operation.
 */
    editBtn->Bind(wxEVT_BUTTON, [this, capturedEvent, dlg](wxCommandEvent&) {
        dlg->EndModal(wxID_OK); // Close details view
        ShowEditEventDialog(capturedEvent); // Open edit view
    });

    // Bind Delete Button
/**
 * @brief deleteBtn->Bind method of CalendarView.cpp.
 * @param wxEVT_BUTTON parameter for deleteBtn->Bind.
 * @param capturedEvent parameter for deleteBtn->Bind.
 * @param dlg](wxCommandEvent parameter for deleteBtn->Bind.
 * @return Result of the operation.
 */
    deleteBtn->Bind(wxEVT_BUTTON, [this, capturedEvent, dlg](wxCommandEvent&) {
        int confirm = wxMessageBox("Are you sure you want to delete \"" + wxString(capturedEvent.getTitle()) + "\"?", 
                                   "Confirm Delete", wxYES_NO | wxNO_DEFAULT | wxICON_QUESTION, dlg);
        if (confirm == wxYES) {
            calendar.removeEvent(capturedEvent.getTitle(), "");
            calendar.saveEvents();
            updateView();
            dlg->EndModal(wxID_OK); // Close details view
        }
    });

    dlg->SetSizer(mainSizer);
    mainSizer->Fit(dlg);
    dlg->ShowModal();
    dlg->Destroy();
}

/**
 * @brief OnToday method of CalendarView.cpp.
 * @param event parameter for OnToday.
 */
void CalendarView::OnToday(wxCommandEvent& event) {
    calendar.setCurrentDate(Date()); // Resets to the system's current local date
    updateView();
}

/**
 * @brief OnViewEvents method of CalendarView.cpp.
 * @param event parameter for OnViewEvents.
 */
void CalendarView::OnViewEvents(wxCommandEvent& event) {
    Date current = calendar.getCurrentDate();
    std::string dateStr = current.toString();
    
    // Get all events for the date currently being viewed on the calendar
    std::vector<Event> events = calendar.getEventsForDate(dateStr);

    if (events.empty()) {
        wxMessageBox("No events scheduled for " + wxString(dateStr) + ".", "View Events", wxOK | wxICON_INFORMATION);
        return;
    }

    wxDialog* viewDialog = new wxDialog(this, wxID_ANY, "Events for " + wxString(dateStr), wxDefaultPosition, wxSize(450, 400));
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    wxStaticText* instruction = new wxStaticText(viewDialog, wxID_ANY, "Select an event below to view its details:");
    mainSizer->Add(instruction, 0, wxALL, 10);

    // ListBox to hold the event names
    wxListBox* eventList = new wxListBox(viewDialog, wxID_ANY);
    for (const Event& e : events) {
        wxString displayText = wxString(e.getTitle());
        if (e.getIsAllDay()) {
            displayText += " (All Day)";
        } else {
            displayText += " (" + wxString(e.getStartTime()) + ")";
        }
        eventList->Append(displayText);
    }
    mainSizer->Add(eventList, 1, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 10);

    wxBoxSizer* btnSizer = new wxBoxSizer(wxHORIZONTAL);
    wxButton* detailsBtn = new wxButton(viewDialog, wxID_ANY, "View Details");
    wxButton* editBtn = new wxButton(viewDialog, wxID_ANY, "Edit Event");
    wxButton* deleteBtn = new wxButton(viewDialog, wxID_ANY, "Delete");
    wxButton* closeBtn = new wxButton(viewDialog, wxID_CANCEL, "Close");
    
    btnSizer->Add(detailsBtn, 0, wxALL, 5);
    btnSizer->Add(editBtn, 0, wxALL, 5);
    btnSizer->Add(deleteBtn, 0, wxALL, 5);
    btnSizer->Add(closeBtn, 0, wxALL, 5);

/**
 * @brief deleteBtn->Bind method of CalendarView.cpp.
 * @param wxEVT_BUTTON parameter for deleteBtn->Bind.
 * @param eventList parameter for deleteBtn->Bind.
 * @param events parameter for deleteBtn->Bind.
 * @param viewDialog](wxCommandEvent parameter for deleteBtn->Bind.
 * @return Result of the operation.
 */
    deleteBtn->Bind(wxEVT_BUTTON, [this, eventList, events, viewDialog](wxCommandEvent&) {
        int sel = eventList->GetSelection();
        if (sel != wxNOT_FOUND) {
            int confirm = wxMessageBox("Are you sure you want to delete \"" + wxString(events[sel].getTitle()) + "\"?", 
                                       "Confirm Delete", wxYES_NO | wxNO_DEFAULT | wxICON_QUESTION, viewDialog);
            if (confirm == wxYES) {
                calendar.removeEvent(events[sel].getTitle(), "");
                calendar.saveEvents();
                updateView();
                viewDialog->EndModal(wxID_OK);
            }
        } else {
            wxMessageBox("Please select an event from the list first.", "Notice", wxOK | wxICON_INFORMATION);
        }
    });

/**
 * @brief editBtn->Bind method of CalendarView.cpp.
 * @param wxEVT_BUTTON parameter for editBtn->Bind.
 * @param eventList parameter for editBtn->Bind.
 * @param events parameter for editBtn->Bind.
 * @param viewDialog](wxCommandEvent parameter for editBtn->Bind.
 * @return Result of the operation.
 */
    editBtn->Bind(wxEVT_BUTTON, [this, eventList, events, viewDialog](wxCommandEvent&) {
        int sel = eventList->GetSelection();
        if (sel != wxNOT_FOUND) {
            viewDialog->EndModal(wxID_OK); // Close the list view first
            ShowEditEventDialog(events[sel]); // Open the Edit window
        } else {
            wxMessageBox("Please select an event from the list first.", "Notice", wxOK | wxICON_INFORMATION);
        }
    });
    // --------------------------------------------
    mainSizer->Add(btnSizer, 0, wxALIGN_CENTER | wxBOTTOM, 10);

    // Bind the Details button
/**
 * @brief detailsBtn->Bind method of CalendarView.cpp.
 * @param wxEVT_BUTTON parameter for detailsBtn->Bind.
 * @param eventList parameter for detailsBtn->Bind.
 * @param events parameter for detailsBtn->Bind.
 * @param viewDialog](wxCommandEvent parameter for detailsBtn->Bind.
 * @return Result of the operation.
 */
    detailsBtn->Bind(wxEVT_BUTTON, [this, eventList, events, viewDialog](wxCommandEvent&) {
        int sel = eventList->GetSelection();
        if (sel != wxNOT_FOUND) {
            viewDialog->EndModal(wxID_OK); // <-- CLOSES THE LIST FIRST
            ShowEventDetails(events[sel]); // Opens the Details
        } else {
            wxMessageBox("Please select an event from the list first.", "Notice", wxOK | wxICON_INFORMATION);
        }
    });

    // Bind Double-Click
/**
 * @brief eventList->Bind method of CalendarView.cpp.
 * @param wxEVT_LISTBOX_DCLICK parameter for eventList->Bind.
 * @param eventList parameter for eventList->Bind.
 * @param events parameter for eventList->Bind.
 * @param viewDialog](wxCommandEvent parameter for eventList->Bind.
 * @return Result of the operation.
 */
    eventList->Bind(wxEVT_LISTBOX_DCLICK, [this, eventList, events, viewDialog](wxCommandEvent&) {
        int sel = eventList->GetSelection();
        if (sel != wxNOT_FOUND) {
            viewDialog->EndModal(wxID_OK); // <-- CLOSES THE LIST FIRST
            ShowEventDetails(events[sel]); // Opens the Details
        }
    });

    viewDialog->SetSizer(mainSizer);
    viewDialog->ShowModal();
    viewDialog->Destroy();
}

/**
 * @brief OnClose method of CalendarView.cpp.
 * @param event parameter for OnClose.
 */
void CalendarView::OnClose(wxCloseEvent& event) {
    calendar.saveCalendars();
    calendar.saveEvents();
    settings.saveSettings();
    event.Skip();
}

/**
 * @brief OnManageCalendars method of CalendarView.cpp.
 * @param wxCommandEvent parameter for OnManageCalendars.
 */
void CalendarView::OnManageCalendars(wxCommandEvent&) {
    wxDialog* dlg = new wxDialog(this, wxID_ANY, "Manage Calendars",
                                 wxDefaultPosition, wxSize(420, 400));
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    // List showing current calendars
    wxListBox* listBox = new wxListBox(dlg, wxID_ANY);
    auto refreshList = [&]() {
        listBox->Clear();
        for (const CalendarMeta& cm : calendar.getCalendars())
            listBox->Append(wxString(cm.name + " (" + cm.colour + ")"));
    };
    refreshList();
    mainSizer->Add(listBox, 1, wxEXPAND | wxALL, 8);

    // Visibility toggle
    wxCheckBox* visCheck = new wxCheckBox(dlg, wxID_ANY, "Visible");
    visCheck->SetValue(true);
    mainSizer->Add(visCheck, 0, wxLEFT, 10);

    // Update visibility checkbox when selection changes
/**
 * @brief listBox->Bind method of CalendarView.cpp.
 * @param wxEVT_LISTBOX parameter for listBox->Bind.
 * @param listBox parameter for listBox->Bind.
 * @param visCheck](wxCommandEvent parameter for listBox->Bind.
 * @return Result of the operation.
 */
    listBox->Bind(wxEVT_LISTBOX, [this, listBox, visCheck](wxCommandEvent&) {
        int sel = listBox->GetSelection();
        if (sel == wxNOT_FOUND) return;
        const auto& cals = calendar.getCalendars();
        if (sel < (int)cals.size())
            visCheck->SetValue(cals[sel].visible);
    });

/**
 * @brief visCheck->Bind method of CalendarView.cpp.
 * @param wxEVT_CHECKBOX parameter for visCheck->Bind.
 * @param listBox parameter for visCheck->Bind.
 * @param visCheck parameter for visCheck->Bind.
 * @param refreshList](wxCommandEvent parameter for visCheck->Bind.
 * @return Result of the operation.
 */
    visCheck->Bind(wxEVT_CHECKBOX, [this, listBox, visCheck, &refreshList](wxCommandEvent&) {
        int sel = listBox->GetSelection();
        if (sel == wxNOT_FOUND) return;
        const auto& cals = calendar.getCalendars();
        if (sel < (int)cals.size()) {
            calendar.setCalendarVisible(cals[sel].name, visCheck->GetValue());
            calendar.saveCalendars(); // ADD THIS
        }
    });

    // Buttons: Add, Edit, Remove
    wxBoxSizer* btnSizer = new wxBoxSizer(wxHORIZONTAL);

    wxButton* addBtn = new wxButton(dlg, wxID_ANY, "Add");
    wxButton* editBtn = new wxButton(dlg, wxID_ANY, "Edit");
    wxButton* removeBtn = new wxButton(dlg, wxID_ANY, "Remove");
    wxButton* closeBtn = new wxButton(dlg, wxID_OK, "Close");

    btnSizer->Add(addBtn,    0, wxALL, 5);
    btnSizer->Add(editBtn,   0, wxALL, 5);
    btnSizer->Add(removeBtn, 0, wxALL, 5);
    btnSizer->Add(closeBtn,  0, wxALL, 5);
    mainSizer->Add(btnSizer, 0, wxALIGN_CENTER | wxBOTTOM, 8);

    // ── Add ──
/**
 * @brief addBtn->Bind method of CalendarView.cpp.
 * @param wxEVT_BUTTON parameter for addBtn->Bind.
 * @param listBox parameter for addBtn->Bind.
 * @param refreshList](wxCommandEvent parameter for addBtn->Bind.
 * @return Result of the operation.
 */
    addBtn->Bind(wxEVT_BUTTON, [this, listBox, &refreshList](wxCommandEvent&) {
        wxTextEntryDialog nameDlg(nullptr, "Calendar name:", "Add Calendar");
        if (nameDlg.ShowModal() != wxID_OK) return;
        std::string name = nameDlg.GetValue().ToStdString();
        if (name.empty()) return;

        wxTextEntryDialog colDlg(nullptr, "Colour (hex, e.g. #FF5733):", "Add Calendar", "#0A84FF");
        if (colDlg.ShowModal() != wxID_OK) return;
        std::string colour = colDlg.GetValue().ToStdString();
        if (colour.empty()) colour = "#0A84FF";

        calendar.addCalendar(CalendarMeta(name, colour, true));
        calendar.saveCalendars();
        refreshList();
    });

    // ── Edit ──
/**
 * @brief editBtn->Bind method of CalendarView.cpp.
 * @param wxEVT_BUTTON parameter for editBtn->Bind.
 * @param listBox parameter for editBtn->Bind.
 * @param refreshList](wxCommandEvent parameter for editBtn->Bind.
 * @return Result of the operation.
 */
    editBtn->Bind(wxEVT_BUTTON, [this, listBox, &refreshList](wxCommandEvent&) {
        int sel = listBox->GetSelection();
        if (sel == wxNOT_FOUND) { wxMessageBox("Select a calendar first."); return; }
        const CalendarMeta old = calendar.getCalendars()[sel];

        wxTextEntryDialog nameDlg(nullptr, "New name:", "Edit Calendar", wxString(old.name));
        if (nameDlg.ShowModal() != wxID_OK) return;
        std::string newName = nameDlg.GetValue().ToStdString();
        if (newName.empty()) return;

        wxTextEntryDialog colDlg(nullptr, "Colour (hex):", "Edit Calendar", wxString(old.colour));
        if (colDlg.ShowModal() != wxID_OK) return;
        std::string newColour = colDlg.GetValue().ToStdString();
        if (newColour.empty()) newColour = old.colour;

        calendar.editCalendar(old.name, CalendarMeta(newName, newColour, old.visible));
        calendar.saveCalendars();
        calendar.saveEvents(); // re-save so event calendarName fields are updated
        refreshList();
    });

    // ── Remove ──
/**
 * @brief removeBtn->Bind method of CalendarView.cpp.
 * @param wxEVT_BUTTON parameter for removeBtn->Bind.
 * @param listBox parameter for removeBtn->Bind.
 * @param dlg parameter for removeBtn->Bind.
 * @param refreshList](wxCommandEvent parameter for removeBtn->Bind.
 * @return Result of the operation.
 */
    removeBtn->Bind(wxEVT_BUTTON, [this, listBox, dlg, &refreshList](wxCommandEvent&) {
        int sel = listBox->GetSelection();
        if (sel == wxNOT_FOUND) { wxMessageBox("Select a calendar first."); return; }
        const std::string name = calendar.getCalendars()[sel].name;
        int confirm = wxMessageBox(
            wxString::Format("Remove calendar \"%s\"? Its events will move to the first calendar.", wxString(name)),
            "Confirm", wxYES_NO | wxNO_DEFAULT | wxICON_QUESTION, dlg);
        if (confirm != wxYES) return;
        calendar.removeCalendar(name);
        calendar.saveCalendars();
        calendar.saveEvents();
        refreshList();
    });

    dlg->SetSizer(mainSizer);
    mainSizer->Fit(dlg);
    dlg->ShowModal();
    dlg->Destroy();

    updateView(); // refresh so visibility changes take effect immediately   
}

/**
 * @brief OnChecklist method of CalendarView.cpp.
 * @param event parameter for OnChecklist.
 */
void CalendarView::OnChecklist(wxCommandEvent& event) {
    wxDialog* dialog = new wxDialog(this, wxID_ANY, "Checklist", wxDefaultPosition, wxSize(300, 400));

    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    // Scrollable area for checkboxes
    wxScrolledWindow* scrollWin = new wxScrolledWindow(dialog, wxID_ANY);
    scrollWin->SetScrollRate(0, 10);

    wxBoxSizer* scrollSizer = new wxBoxSizer(wxVERTICAL);
    scrollWin->SetSizer(scrollSizer);

    mainSizer->Add(scrollWin, 1, wxEXPAND | wxALL, 5);

    // Load and display tasks
    checkList.loadTasks();

    for (size_t i = 0; i < checkList.getTasks().size(); i++) {
        Task& task = checkList.getTasks()[i];

        wxCheckBox* cb = new wxCheckBox(scrollWin, wxID_ANY, task.getTaskDesc());
        cb->SetValue(task.isTaskComplete());

/**
 * @brief cb->Bind method of CalendarView.cpp.
 * @param wxEVT_CHECKBOX parameter for cb->Bind.
 * @param e parameter for cb->Bind.
 * @return Result of the operation.
 */
        cb->Bind(wxEVT_CHECKBOX, [this, i](wxCommandEvent& e) {
            checkList.updateCheck(i, e.IsChecked());
        });

        scrollSizer->Add(cb, 0, wxALL, 5);
    }

    // Push buttons to bottom
    mainSizer->AddStretchSpacer(0);

    // Create buttons
    wxButton* addBtn    = new wxButton(dialog, wxID_ANY,    "Add");
    wxButton* deleteBtn = new wxButton(dialog, wxID_ANY,    "Delete");
    wxButton* saveBtn   = new wxButton(dialog, wxID_OK,     "Save");

    // Add button — only adds the NEW task, not all tasks
/**
 * @brief addBtn->Bind method of CalendarView.cpp.
 * @param wxEVT_BUTTON parameter for addBtn->Bind.
 * @param dialog parameter for addBtn->Bind.
 * @param scrollWin parameter for addBtn->Bind.
 * @param scrollSizer](wxCommandEvent parameter for addBtn->Bind.
 * @return Result of the operation.
 */
    addBtn->Bind(wxEVT_BUTTON, [this, dialog, scrollWin, scrollSizer](wxCommandEvent&) {
        wxTextEntryDialog inputDialog(dialog, "Enter task:", "New Task");

        if (inputDialog.ShowModal() == wxID_OK) {
            wxString taskText = inputDialog.GetValue();
            if (taskText.Trim().IsEmpty()) {
                wxMessageBox("Task name cannot be empty!", "Invalid Input", wxOK | wxICON_WARNING, dialog);
                return;
            }

            checkList.addTask(taskText.ToStdString());

            // Only add the newest task checkbox
            size_t i = checkList.getTasks().size() - 1;
            Task& task = checkList.getTasks()[i];

            wxCheckBox* cb = new wxCheckBox(scrollWin, wxID_ANY, task.getTaskDesc());
            cb->SetValue(task.isTaskComplete());

/**
 * @brief cb->Bind method of CalendarView.cpp.
 * @param wxEVT_CHECKBOX parameter for cb->Bind.
 * @param e parameter for cb->Bind.
 * @return Result of the operation.
 */
            cb->Bind(wxEVT_CHECKBOX, [this, i](wxCommandEvent& e) {
                checkList.updateCheck(i, e.IsChecked());
            });

            scrollSizer->Add(cb, 0, wxALL, 5);
            scrollSizer->Layout();
            scrollWin->FitInside();  // tells scroll window to recalculate scroll area
        }
    });

    // Delete button — removes the last checked task
/**
 * @brief deleteBtn->Bind method of CalendarView.cpp.
 * @param wxEVT_BUTTON parameter for deleteBtn->Bind.
 * @param dialog parameter for deleteBtn->Bind.
 * @param scrollWin parameter for deleteBtn->Bind.
 * @param scrollSizer](wxCommandEvent parameter for deleteBtn->Bind.
 * @return Result of the operation.
 */
    deleteBtn->Bind(wxEVT_BUTTON, [this, dialog, scrollWin, scrollSizer](wxCommandEvent&) {
    wxTextEntryDialog indexDialog(dialog, "Enter task number to delete:", "Delete Task");

    if (indexDialog.ShowModal() == wxID_OK) {
        wxString input = indexDialog.GetValue();

        long index;
        if (!input.ToLong(&index)) {
            wxMessageBox("Please enter a valid number!", "Invalid Input", wxOK | wxICON_WARNING, dialog);
            return;
        }

        int taskCount = (int)checkList.getTasks().size();

        if (index < 1 || index > taskCount) {
            wxMessageBox("Task number out of range!", "Invalid Input", wxOK | wxICON_WARNING, dialog);
            return;
        }

        // Convert to 0-based index
        int realIndex = (int)index - 1;

        checkList.removeTask(realIndex);
        checkList.saveTasks();

        // Remove the matching checkbox from UI
        wxWindowList& children = scrollWin->GetChildren();
        wxCheckBox* cb = dynamic_cast<wxCheckBox*>(children[realIndex]);
        if (cb) {
            scrollSizer->Detach(cb);
            cb->Destroy();
            scrollSizer->Layout();
            scrollWin->FitInside();
        }
    }
    });

    // Button row
    wxBoxSizer* btnSizer = new wxBoxSizer(wxHORIZONTAL);
    btnSizer->Add(addBtn,    0, wxALL, 5);
    btnSizer->Add(deleteBtn, 0, wxALL, 5);
    btnSizer->Add(saveBtn,   0, wxALL, 5);

    mainSizer->Add(btnSizer, 0, wxALIGN_CENTER, 0);

    dialog->SetSizer(mainSizer);

/**
 * @brief dialog->Bind method of CalendarView.cpp.
 * @param wxEVT_CLOSE_WINDOW parameter for dialog->Bind.
 * @param e parameter for dialog->Bind.
 * @return Result of the operation.
 */
    dialog->Bind(wxEVT_CLOSE_WINDOW, [dialog](wxCloseEvent& e) {
    dialog->EndModal(wxID_CANCEL);
    });

    if (dialog->ShowModal() == wxID_OK) {
        checkList.saveTasks();
    }

    dialog->Destroy();
}

/**
 * @brief ShowEditEventDialog method of CalendarView.cpp.
 * @param oldEvent parameter for ShowEditEventDialog.
 */
void CalendarView::ShowEditEventDialog(Event oldEvent) {
    wxDialog* editDialog = new wxDialog(this, wxID_ANY, "Edit Event", wxDefaultPosition, wxSize(450, 450));
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    // Title
    wxBoxSizer* titleSizer = new wxBoxSizer(wxHORIZONTAL);
    titleSizer->Add(new wxStaticText(editDialog, wxID_ANY, "Title (Required):"), 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
    wxTextCtrl* titleCtrl = new wxTextCtrl(editDialog, wxID_ANY, oldEvent.getTitle());
    titleSizer->Add(titleCtrl, 1, wxALL, 5);
    mainSizer->Add(titleSizer, 0, wxEXPAND);

    // Location
    wxBoxSizer* locSizer = new wxBoxSizer(wxHORIZONTAL);
    locSizer->Add(new wxStaticText(editDialog, wxID_ANY, "Location:"), 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
    wxTextCtrl* locCtrl = new wxTextCtrl(editDialog, wxID_ANY, oldEvent.getLocation());
    locSizer->Add(locCtrl, 1, wxALL, 5);
    mainSizer->Add(locSizer, 0, wxEXPAND);

    // Description
    wxBoxSizer* descSizer = new wxBoxSizer(wxHORIZONTAL);
    descSizer->Add(new wxStaticText(editDialog, wxID_ANY, "Description:"), 0, wxALL, 5);
    wxTextCtrl* descCtrl = new wxTextCtrl(editDialog, wxID_ANY, oldEvent.getDescription(), wxDefaultPosition, wxSize(-1, 60), wxTE_MULTILINE);
    descSizer->Add(descCtrl, 1, wxALL, 5);
    mainSizer->Add(descSizer, 0, wxEXPAND);

/**
 * @brief Times method of CalendarView.cpp.
 * @param dates parameter for Times.
 */
    // Dates & Times (We use the internal parseDMY function to convert strings back to UI dates)
    int d, m, y;
    
    wxBoxSizer* dateSizer = new wxBoxSizer(wxHORIZONTAL);
    dateSizer->Add(new wxStaticText(editDialog, wxID_ANY, "Start Date:"), 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
    wxDatePickerCtrl* dateCtrl = new wxDatePickerCtrl(editDialog, wxID_ANY, wxDefaultDateTime);
    if (parseDMY(oldEvent.getDate(), d, m, y)) { dateCtrl->SetValue(wxDateTime(d, wxDateTime::Month(m - 1), y)); }
    dateSizer->Add(dateCtrl, 1, wxALL, 5);
    mainSizer->Add(dateSizer, 0, wxEXPAND);

    wxBoxSizer* endDateSizer = new wxBoxSizer(wxHORIZONTAL);
    endDateSizer->Add(new wxStaticText(editDialog, wxID_ANY, "End Date:"), 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
    wxDatePickerCtrl* endDateCtrl = new wxDatePickerCtrl(editDialog, wxID_ANY, wxDefaultDateTime);
    if (parseDMY(oldEvent.getEndDate(), d, m, y)) { endDateCtrl->SetValue(wxDateTime(d, wxDateTime::Month(m - 1), y)); }
/**
 * @brief if method of CalendarView.cpp.
 * @param parseDMY(oldEvent.getDate() parameter for if.
 * @param d parameter for if.
 * @param m parameter for if.
 * @param endDateCtrl->SetValue(wxDateTime(d parameter for if.
 * @param y) parameter for if.
 * @return Result of the operation.
 */
    else if (parseDMY(oldEvent.getDate(), d, m, y)) { endDateCtrl->SetValue(wxDateTime(d, wxDateTime::Month(m - 1), y)); }
    endDateSizer->Add(endDateCtrl, 1, wxALL, 5);
    mainSizer->Add(endDateSizer, 0, wxEXPAND);

    wxBoxSizer* startSizer = new wxBoxSizer(wxHORIZONTAL);
    startSizer->Add(new wxStaticText(editDialog, wxID_ANY, "Start Time:"), 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
    wxTimePickerCtrl* startCtrl = new wxTimePickerCtrl(editDialog, wxID_ANY, wxDefaultDateTime);
    wxDateTime st; if (st.ParseISOTime(oldEvent.getStartTime())) startCtrl->SetValue(st);
    startSizer->Add(startCtrl, 1, wxALL, 5);
    mainSizer->Add(startSizer, 0, wxEXPAND);

    wxBoxSizer* endSizer = new wxBoxSizer(wxHORIZONTAL);
    endSizer->Add(new wxStaticText(editDialog, wxID_ANY, "End Time:"), 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
    wxTimePickerCtrl* endCtrl = new wxTimePickerCtrl(editDialog, wxID_ANY, wxDefaultDateTime);
    wxDateTime et; if (et.ParseISOTime(oldEvent.getEndTime())) endCtrl->SetValue(et);
    endSizer->Add(endCtrl, 1, wxALL, 5);
    mainSizer->Add(endSizer, 0, wxEXPAND);

    // All Day Toggle
    wxCheckBox* allDayCheck = new wxCheckBox(editDialog, wxID_ANY, "All Day Event");
    allDayCheck->SetValue(oldEvent.getIsAllDay());
    if (oldEvent.getIsAllDay()) { startCtrl->Enable(false); endCtrl->Enable(false); }
    mainSizer->Add(allDayCheck, 0, wxALL, 5);

/**
 * @brief allDayCheck->Bind method of CalendarView.cpp.
 * @param wxEVT_CHECKBOX parameter for allDayCheck->Bind.
 * @param e parameter for allDayCheck->Bind.
 * @return Result of the operation.
 */
    allDayCheck->Bind(wxEVT_CHECKBOX, [startCtrl, endCtrl](wxCommandEvent& e) {
        startCtrl->Enable(!e.IsChecked()); endCtrl->Enable(!e.IsChecked());
    });

    // Buttons
    wxBoxSizer* buttonSizer = new wxBoxSizer(wxHORIZONTAL);
    wxButton* saveBtn = new wxButton(editDialog, wxID_OK, "Save Changes");
    wxButton* cancelBtn = new wxButton(editDialog, wxID_CANCEL, "Cancel");
    buttonSizer->Add(saveBtn, 0, wxALL, 5);
    buttonSizer->Add(cancelBtn, 0, wxALL, 5);
    mainSizer->Add(buttonSizer, 0, wxALIGN_CENTER);

    editDialog->SetSizer(mainSizer);
    mainSizer->Fit(editDialog);

    // Save Logic
    bool saved = false;
    while (!saved) {
        if (editDialog->ShowModal() == wxID_OK) {
            std::string title = titleCtrl->GetValue().ToStdString();
            if (title.empty()) {
                wxMessageBox("Title is required to save an event.", "Validation Error", wxOK | wxICON_ERROR);
                continue;
            }

            wxDateTime startDateVal = dateCtrl->GetValue();
            wxDateTime endDateVal = endDateCtrl->GetValue();
            bool isAllDay = allDayCheck->GetValue();

            if (!isAllDay) {
                wxDateTime startTimeVal = startCtrl->GetValue();
                wxDateTime endTimeVal = endCtrl->GetValue();
                startDateVal.SetHour(startTimeVal.GetHour()); startDateVal.SetMinute(startTimeVal.GetMinute());
                endDateVal.SetHour(endTimeVal.GetHour()); endDateVal.SetMinute(endTimeVal.GetMinute());
            } else {
                startDateVal.SetHour(0); startDateVal.SetMinute(0);
                endDateVal.SetHour(23); endDateVal.SetMinute(59);
            }

            if (endDateVal.IsEarlierThan(startDateVal)) {
                wxMessageBox("The event cannot end before it starts.", "Validation Error", wxOK | wxICON_ERROR);
                continue;
            }

            std::string dateStr = std::to_string(dateCtrl->GetValue().GetDay()) + "/" + std::to_string(dateCtrl->GetValue().GetMonth() + 1) + "/" + std::to_string(dateCtrl->GetValue().GetYear());
            std::string endDateStr = std::to_string(endDateCtrl->GetValue().GetDay()) + "/" + std::to_string(endDateCtrl->GetValue().GetMonth() + 1) + "/" + std::to_string(endDateCtrl->GetValue().GetYear());
            std::string startTimeStr = startCtrl->GetValue().FormatISOTime().ToStdString();
            std::string endTimeStr = endCtrl->GetValue().FormatISOTime().ToStdString();

            // Apply modifications: Remove the old event, insert the new one
            calendar.removeEvent(oldEvent.getTitle(), ""); // Empty string respects teammates calendarName logic
            Event newEvent = Event::createEvent(title, locCtrl->GetValue().ToStdString(), descCtrl->GetValue().ToStdString(), 
                                                dateStr, endDateStr, startTimeStr, endTimeStr, 
                                                isAllDay, oldEvent.getIsRecurring(), oldEvent.getRepeatCount(), oldEvent.getRepeatType(), "");
            
            calendar.addEvent(newEvent);
            calendar.saveEvents();
            updateView();
            saved = true;
        } else { break; }
    }
    editDialog->Destroy();
}