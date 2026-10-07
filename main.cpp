/**
 * @file main.cpp
 * @brief Source file for calendar application components.
 * @author Nikolas
 * @author Rosaline
 * @author Uday
 * @author Brandon
 * @author Dinith
 */

#include <wx/wx.h>
#include "CalendarView.h"

/**
 * @brief Class MyApp.
 */
class MyApp : public wxApp {
public:
/**
 * @brief OnInit method of main.cpp.
 */
    virtual bool OnInit() {
        wxInitAllImageHandlers();
        CalendarView* window = new CalendarView("ClearDay Prototype");
        window->Show(true);
        return true;
    }
};

wxIMPLEMENT_APP(MyApp);