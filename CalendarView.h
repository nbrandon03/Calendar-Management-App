/**
 * @file CalendarView.h
 * @brief Source file for calendar application components.
 * @author Nikolas
 * @author Rosaline
 * @author Uday
 * @author Brandon
 * @author Dinith
 */

#ifndef CALENDARVIEW_H
#define CALENDARVIEW_H

#include <wx/wx.h>
#include <wx/scrolwin.h>
#include "Calendar.h"
#include "Settings.h"
#include "WeatherAPI.h"
#include "CheckList.h"

/**
 * @brief Class CalendarView.
 */
class CalendarView : public wxFrame {
private:
    Calendar calendar;
    Settings settings;

    wxPanel* mainPanel;
    wxStaticText* titleLabel;
    wxStaticText* dateLabel;
    wxStaticText* weatherLabel;
    wxPanel* contentPanel;
    wxBoxSizer* contentSizer;
    WeatherAPI weatherApi;
    bool weatherAvailable;
    wxString weatherLocationName;
    wxString weatherRequestKey;
    CheckList checkList;

/**
 * @brief updateView method of CalendarView.h.
 */
    void updateView();
/**
 * @brief clearContent method of CalendarView.h.
 */
    void clearContent();
/**
 * @brief refreshWeatherData method of CalendarView.h.
 */
    void refreshWeatherData();
/**
 * @brief toIsoDate method of CalendarView.h.
 * @param date parameter for toIsoDate.
 * @return Result of the operation.
 */
    wxString toIsoDate(const Date& date) const;
/**
 * @brief buildWeatherSummary method of CalendarView.h.
 * @param date parameter for buildWeatherSummary.
 * @return Result of the operation.
 */
    wxString buildWeatherSummary(const Date& date) const;
/**
 * @brief resolveWeatherTemperatureUnit method of CalendarView.h.
 * @return Result of the operation.
 */
    wxString resolveWeatherTemperatureUnit() const;
/**
 * @brief weatherUnitSymbol method of CalendarView.h.
 * @return Result of the operation.
 */
    wxString weatherUnitSymbol() const;
/**
 * @brief applyWeatherSettings method of CalendarView.h.
 */
    void applyWeatherSettings();

/**
 * @brief createCell method of CalendarView.h.
 * @param text parameter for createCell.
 * @param highlightToday parameter for createCell.
 * @param selectedDate parameter for createCell.
 * @return Result of the operation.
 */
    wxPanel* createCell(const wxString& text, bool highlightToday, bool selectedDate);

/**
 * @brief renderDayView method of CalendarView.h.
 */
    void renderDayView();
/**
 * @brief renderWeekView method of CalendarView.h.
 */
    void renderWeekView();
/**
 * @brief renderMonthView method of CalendarView.h.
 */
    void renderMonthView();

/**
 * @brief OnPrev method of CalendarView.h.
 * @param event parameter for OnPrev.
 */
    void OnPrev(wxCommandEvent& event);
/**
 * @brief OnNext method of CalendarView.h.
 * @param event parameter for OnNext.
 */
    void OnNext(wxCommandEvent& event);
/**
 * @brief OnDay method of CalendarView.h.
 * @param event parameter for OnDay.
 */
    void OnDay(wxCommandEvent& event);
/**
 * @brief OnWeek method of CalendarView.h.
 * @param event parameter for OnWeek.
 */
    void OnWeek(wxCommandEvent& event);
/**
 * @brief OnMonth method of CalendarView.h.
 * @param event parameter for OnMonth.
 */
    void OnMonth(wxCommandEvent& event);
/**
 * @brief OnSettings method of CalendarView.h.
 * @param event parameter for OnSettings.
 */
    void OnSettings(wxCommandEvent& event);
/**
 * @brief OnManageCalendars method of CalendarView.h.
 * @param event parameter for OnManageCalendars.
 */
    void OnManageCalendars(wxCommandEvent& event);
/**
 * @brief hexToColour method of CalendarView.h.
 * @param hex parameter for hexToColour.
 * @return Result of the operation.
 */
    wxColour hexToColour(const std::string& hex) const;
/**
 * @brief OnAddEvent method of CalendarView.h.
 * @param event parameter for OnAddEvent.
 */
    void OnAddEvent(wxCommandEvent& event);
/**
 * @brief ShowEventDetails method of CalendarView.h.
 * @param e parameter for ShowEventDetails.
 */
    void ShowEventDetails(const Event& e);
/**
 * @brief ShowEditEventDialog method of CalendarView.h.
 * @param oldEvent parameter for ShowEditEventDialog.
 */
    void ShowEditEventDialog(Event oldEvent);
/**
 * @brief OnClose method of CalendarView.h.
 * @param event parameter for OnClose.
 */
    void OnClose(wxCloseEvent& event);
/**
 * @brief OnChecklist method of CalendarView.h.
 * @param event parameter for OnChecklist.
 */
    void OnChecklist(wxCommandEvent& event);
/**
 * @brief OnViewEvents method of CalendarView.h.
 * @param event parameter for OnViewEvents.
 */
    void OnViewEvents(wxCommandEvent& event);
/**
 * @brief OnToday method of CalendarView.h.
 * @param event parameter for OnToday.
 */
    void OnToday(wxCommandEvent& event);

public:
/**
 * @brief CalendarView method of CalendarView.h.
 * @param title parameter for CalendarView.
 */
    CalendarView(const wxString& title);
    
};

#endif