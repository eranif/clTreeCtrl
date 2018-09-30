#include "clToolBarButtonBase.h"
#include "drawingutils.h"

// -----------------------------------------------
// Button base
// -----------------------------------------------
clToolBarButtonBase::clToolBarButtonBase(clToolBar* parent, wxWindowID id, const wxBitmap& bmp, const wxString& label,
                                         size_t flags)
    : m_toolbar(parent)
    , m_id(id)
    , m_bmp(bmp)
    , m_label(label)
    , m_flags(flags)
    , m_renderFlags(0)
    , m_menu(nullptr)
{
}

clToolBarButtonBase::~clToolBarButtonBase() { wxDELETE(m_menu); }

void clToolBarButtonBase::Render(wxDC& dc, const wxRect& rect)
{
    m_dropDownArrowRect = wxRect();
    m_buttonRect = rect;
    const clColours& colours = DrawingUtils::GetColours();
    wxColour textColour = colours.GetItemTextColour();
    wxColour penColour;
    wxColour buttonColour;
#if defined(__WXMSW__) || defined(__WXOSX__)
    wxColour bgHighlightColour("rgb(153, 209, 255)");
    penColour = bgHighlightColour;
#else
    wxColour bgHighlightColour(wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT));
    if(DrawingUtils::IsDark(bgHighlightColour)) { bgHighlightColour = bgHighlightColour.ChangeLightness(140); }
#endif

#ifdef __WXMSW__
    bool hasGrouping = false;
#else
    bool hasGrouping = !m_toolbar->HasFlag(clToolBar::kMiniToolBar);
#endif
    const wxRect clientRect = m_toolbar->GetClientRect();
    const wxColour bgColour = m_toolbar->HasFlag(clToolBar::kMiniToolBar)
                            ? colours.GetFillColour()
                            : wxSystemSettings::GetColour(wxSYS_COLOUR_MENUBAR);
    if(IsEnabled() && (IsPressed() || IsChecked())) {
        wxColour pressBgColour = bgColour.ChangeLightness(90);
        wxRect highlightRect = m_buttonRect;
        penColour = pressBgColour;

        // Adjust the highlight rect so it wont be drawn over the group border
        if(hasGrouping) {
            highlightRect.Deflate(1);
            highlightRect.CenterIn(clientRect, wxVERTICAL);
            highlightRect.SetX(highlightRect.GetX() + 1);
        }
        dc.SetBrush(pressBgColour);
        dc.SetPen(penColour);
        dc.DrawRectangle(highlightRect);
        textColour = colours.GetSelItemTextColour();
        buttonColour = colours.GetSelbuttonColour();

    } else if(!IsEnabled()) {
        // A disabled button
        textColour = wxSystemSettings::GetColour(wxSYS_COLOUR_GRAYTEXT);
        buttonColour = textColour;
    } else {
        // Default
        if(DrawingUtils::IsDark(bgColour)) {
            buttonColour = colours.GetSelbuttonColour();
        } else {
            buttonColour = colours.GetButtonColour();
        }
        textColour = colours.GetItemTextColour();
    }
    wxCoord xx = m_buttonRect.GetX();
    wxCoord yy = 0;
    xx += CL_TOOL_BAR_X_MARGIN;

    if(m_bmp.IsOk()) {
        wxBitmap bmp(m_bmp);
        if(!IsEnabled()) { bmp = DrawingUtils::CreateDisabledBitmap(m_bmp); }
        yy = (m_buttonRect.GetHeight() - bmp.GetScaledHeight()) / 2 + m_buttonRect.GetY();
        dc.DrawBitmap(bmp, wxPoint(xx, yy));
        xx += bmp.GetScaledWidth();
        xx += CL_TOOL_BAR_X_MARGIN;
    }

    if(!m_label.IsEmpty() && m_toolbar->IsShowLabels()) {
        dc.SetTextForeground(textColour);
        wxSize sz = dc.GetTextExtent(m_label);
        yy = (m_buttonRect.GetHeight() - sz.GetHeight()) / 2 + m_buttonRect.GetY();
        dc.DrawText(m_label, wxPoint(xx, yy));
        xx += sz.GetWidth();
        xx += CL_TOOL_BAR_X_MARGIN;
    }

    // Do we need to draw a drop down arrow?
    if(IsMenuButton()) {
        // draw a drop down menu
        m_dropDownArrowRect =
            wxRect(xx, m_buttonRect.GetY(), (2 * CL_TOOL_BAR_X_MARGIN) + CL_TOOL_BAR_DROPDOWN_ARROW_SIZE,
                   m_buttonRect.GetHeight());
        if((IsPressed() || IsHover()) && IsEnabled()) {
            dc.DrawLine(wxPoint(xx, m_buttonRect.GetY() + 2),
                        wxPoint(xx, m_buttonRect.GetY() + m_buttonRect.GetHeight() - 2));
        }
        xx += CL_TOOL_BAR_X_MARGIN;
        DrawingUtils::DrawDropDownArrow(nullptr, dc, m_dropDownArrowRect, buttonColour);
        xx += CL_TOOL_BAR_DROPDOWN_ARROW_SIZE;
        xx += CL_TOOL_BAR_X_MARGIN;
    }
}

void clToolBarButtonBase::SetMenu(wxMenu* menu)
{
    wxDELETE(m_menu);
    m_menu = menu;
}
