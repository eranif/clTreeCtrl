#include "clCaptionBar.hpp"
#include "clMenuBar.hpp"
#include "drawingutils.h"
#include <wx/dcbuffer.h>
#include <wx/dcgraph.h>
#include <wx/frame.h>
#include <wx/wupdlock.h>

namespace
{
constexpr int SPACER = 1;
}

#define check_ptr_return(ptr) \
    if(!ptr) {                \
        return;               \
    }

clCaptionBar::clCaptionBar(wxWindow* parent, wxTopLevelWindow* topLevelFrame)
    : wxWindow(parent, wxID_ANY)
    , m_topLevelWindow(topLevelFrame)
    , m_buttonMenu(this)
    , m_buttonClose(this)
    , m_buttonMinimize(this)
    , m_buttonMaximize(this)

{
    m_colours.InitDefaults();
    // initialise the callbacks
    m_leftDownCallbacks = {
        { wxCAPTION_HT_MENUBUTTON, { &m_buttonMenu, &clCaptionButton::LeftDown } },
        { wxCAPTION_HT_CLOSEBUTTON, { &m_buttonClose, &clCaptionButton::LeftDown } },
        { wxCAPTION_HT_MINIMIZEBUTTON, { &m_buttonMinimize, &clCaptionButton::LeftDown } },
        { wxCAPTION_HT_MAXMIZEBUTTON, { &m_buttonMaximize, &clCaptionButton::LeftDown } },
    };

    m_leftUpCallbacks = {
        { wxCAPTION_HT_MENUBUTTON, { &m_buttonMenu, &clCaptionButton::LeftUp } },
        { wxCAPTION_HT_CLOSEBUTTON, { &m_buttonClose, &clCaptionButton::LeftUp } },
        { wxCAPTION_HT_MINIMIZEBUTTON, { &m_buttonMinimize, &clCaptionButton::LeftUp } },
        { wxCAPTION_HT_MAXMIZEBUTTON, { &m_buttonMaximize, &clCaptionButton::LeftUp } },
    };

    Bind(wxEVT_PAINT, &clCaptionBar::OnPaint, this);
    Bind(wxEVT_ERASE_BACKGROUND, &clCaptionBar::OnEraseBg, this);
    Bind(wxEVT_LEFT_DOWN, &clCaptionBar::OnLeftDown, this);
    Bind(wxEVT_LEFT_UP, &clCaptionBar::OnLeftUp, this);
    Bind(wxEVT_MOTION, &clCaptionBar::OnMotion, this);
    Bind(wxEVT_ENTER_WINDOW, &clCaptionBar::OnEnterWindow, this);
    Bind(wxEVT_LEAVE_WINDOW, &clCaptionBar::OnLeaveWindow, this);
    Bind(wxEVT_SIZE, &clCaptionBar::OnSize, this);
    Bind(wxEVT_LEFT_DCLICK, &clCaptionBar::OnMouseDoubleClick, this);
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    DoSetBestSize();
    topLevelFrame->SetWindowStyle(wxRESIZE_BORDER);
    topLevelFrame->PostSizeEvent();
}

clCaptionBar::~clCaptionBar()
{
    Unbind(wxEVT_PAINT, &clCaptionBar::OnPaint, this);
    Unbind(wxEVT_ERASE_BACKGROUND, &clCaptionBar::OnEraseBg, this);
    Unbind(wxEVT_LEFT_DOWN, &clCaptionBar::OnLeftDown, this);
    Unbind(wxEVT_LEFT_UP, &clCaptionBar::OnLeftUp, this);
    Unbind(wxEVT_MOTION, &clCaptionBar::OnMotion, this);
    Unbind(wxEVT_ENTER_WINDOW, &clCaptionBar::OnEnterWindow, this);
    Unbind(wxEVT_LEAVE_WINDOW, &clCaptionBar::OnLeaveWindow, this);
    Unbind(wxEVT_SIZE, &clCaptionBar::OnSize, this);
    Unbind(wxEVT_LEFT_DCLICK, &clCaptionBar::OnMouseDoubleClick, this);
}

wxCaptionHitTest clCaptionBar::HitTest(const wxPoint& pt) const
{
    if(m_bitmapRect.Contains(pt)) {
        return wxCAPTION_HT_ICON;
    } else if(m_buttonClose.Contains(pt)) {
        return wxCAPTION_HT_CLOSEBUTTON;
    } else if(m_buttonMaximize.Contains(pt)) {
        return wxCAPTION_HT_MAXMIZEBUTTON;
    } else if(m_buttonMinimize.Contains(pt)) {
        return wxCAPTION_HT_MINIMIZEBUTTON;
    } else if(m_buttonMenu.Contains(pt)) {
        return wxCAPTION_HT_MENUBUTTON;
    }
    return wxCAPTION_HT_NOWHERE;
}

void clCaptionBar::OnPaint(wxPaintEvent& e)
{
    wxUnusedVar(e);
    wxAutoBufferedPaintDC abdc(this);
    wxGCDC dc(abdc);
    PrepareDC(dc);

    ClearRects();

    wxRect rect = GetClientRect();

    // define the area required for the buttons
    int right = rect.GetWidth() + rect.GetX();
    int total_buttons_width = 0;
    int button_width = rect.GetHeight();

    if(HasOption(wxCAPTION_CLOSE_BUTTON)) {
        m_buttonClose.SetRect({ right - button_width, 0, button_width, button_width });
        total_buttons_width += button_width;
        right = m_buttonClose.GetRect().GetLeft();
    }

    if(HasOption(wxCAPTION_MAXIMIZE_BUTTON)) {
        m_buttonMaximize.SetRect({ right - button_width, 0, button_width, button_width });
        total_buttons_width += button_width;
        right = m_buttonMaximize.GetRect().GetLeft();
    }

    if(HasOption(wxCAPTION_MINIMIZE_BUTTON)) {
        m_buttonMinimize.SetRect({ right - button_width, 0, button_width, button_width });
        total_buttons_width += button_width;
        right = m_buttonMinimize.GetRect().GetLeft();
    }

    rect.Inflate(2);
    dc.SetBrush(m_colours.GetBgColour());
    dc.SetPen(m_colours.GetBgColour());
    dc.DrawRectangle(rect);
    rect.Deflate(2);

    dc.SetTextForeground(m_colours.GetItemTextColour());
    auto font = DrawingUtils::GetDefaultGuiFont();
    if(HasOption(wxCAPTION_BOLD_FONT)) {
        font.SetWeight(wxFONTWEIGHT_BOLD);
    }
    dc.SetFont(font);

    // define the clipping region
    dc.SetClippingRegion(0, 0, rect.GetWidth() - total_buttons_width, rect.GetHeight());

    int xx = FromDIP(SPACER);

    // Check if we need to draw a menu button
    if(HasOption(wxCAPTION_MENU_BUTTON) && m_mainMenu) {
        m_buttonMenu.SetRect({ xx, 0, button_width, button_width });
        if(m_menu_is_up) {
            m_buttonMenu.SetState(wxCAPTION_BUTTON_PRESSED);
        }
        m_buttonMenu.Render(dc, wxCAPTION_HT_MENUBUTTON);
        xx += m_buttonMenu.GetRect().GetWidth() + FromDIP(SPACER);
    }
    // draw the bitmap
    if(GetBitmap().IsOk()) {
        int width = GetBitmap().GetScaledWidth();
        int height = GetBitmap().GetScaledHeight();
        wxRect bound_rect(xx, 0, width, height);
        bound_rect = bound_rect.CenterIn(rect, wxVERTICAL);
        dc.DrawBitmap(GetBitmap(), bound_rect.GetTopLeft());
        xx += bound_rect.GetWidth() + FromDIP(SPACER);
        m_bitmapRect = bound_rect;
    }

    // draw the text
    {
        wxSize textSize = dc.GetTextExtent(GetCaption());
        int width = textSize.GetWidth();
        int height = textSize.GetHeight();

        wxRect bound_rect(xx, 0, width, height);
        bound_rect = bound_rect.CenterIn(rect, wxVERTICAL);
        bound_rect.SetX(bound_rect.GetX());
        dc.DrawText(GetCaption(), bound_rect.GetTopLeft());
    }
    dc.DestroyClippingRegion();

    // draw buttons (if any)
    dc.SetPen(m_colours.GetItemTextColour());
    if(HasOption(wxCAPTION_CLOSE_BUTTON)) {
        m_buttonClose.Render(dc, wxCAPTION_HT_CLOSEBUTTON);
    }

    if(HasOption(wxCAPTION_MAXIMIZE_BUTTON)) {
        m_buttonMaximize.Render(dc, wxCAPTION_HT_MAXMIZEBUTTON);
    }

    if(HasOption(wxCAPTION_MINIMIZE_BUTTON)) {
        m_buttonMinimize.Render(dc, wxCAPTION_HT_MINIMIZEBUTTON);
    }
}

void clCaptionBar::OnEraseBg(wxEraseEvent& e) { wxUnusedVar(e); }

void clCaptionBar::OnLeftDown(wxMouseEvent& e)
{
    wxUnusedVar(e);
    check_ptr_return(m_topLevelWindow);

    auto ht = HitTest(e.GetPosition());
    if(ProcessCallback(m_leftDownCallbacks, ht)) {
        return;
    }

    CaptureMouse();
    wxPoint pos = m_topLevelWindow->ClientToScreen(e.GetPosition());
    wxPoint origin = m_topLevelWindow->GetPosition();
    int dx = pos.x - origin.x;
    int dy = pos.y - origin.y;
    m_delta = wxPoint(dx, dy);
}

void clCaptionBar::OnLeftUp(wxMouseEvent& e)
{
    wxUnusedVar(e);
    if(HasCapture()) {
        ReleaseMouse();
    } else {
        auto ht = HitTest(e.GetPosition());
        if(ProcessCallback(m_leftUpCallbacks, ht)) {
            return;
        }
    }
}

void clCaptionBar::OnMotion(wxMouseEvent& e)
{
    wxUnusedVar(e);
    check_ptr_return(m_topLevelWindow);
    if(!HasCapture()) {
        bool refresh_needed = false;
        refresh_needed = refresh_needed || m_buttonClose.DoHover(e.GetPosition());
        refresh_needed = refresh_needed || m_buttonMaximize.DoHover(e.GetPosition());
        refresh_needed = refresh_needed || m_buttonMinimize.DoHover(e.GetPosition());
        refresh_needed = refresh_needed || m_buttonMenu.DoHover(e.GetPosition());

        if(refresh_needed) {
            Refresh();
        }
    } else {
        if(e.Dragging() && e.LeftIsDown()) {
            wxPoint pt = e.GetPosition();
            wxPoint pos = m_topLevelWindow->ClientToScreen(pt);
            m_topLevelWindow->Move(wxPoint(pos.x - m_delta.x, pos.y - m_delta.y));
        }
    }
}

void clCaptionBar::OnEnterWindow(wxMouseEvent& e) { wxUnusedVar(e); }

void clCaptionBar::OnLeaveWindow(wxMouseEvent& e)
{
    wxUnusedVar(e);
    m_buttonClose.ResetState();
    m_buttonMaximize.ResetState();
    m_buttonMenu.ResetState();
    m_buttonMinimize.ResetState();
    Refresh();
}

void clCaptionBar::SetColours(const clColours& colours)
{
    m_colours = colours;
    Refresh();
}

void clCaptionBar::DoSetBestSize()
{
    wxBitmap bmp(1, 1);
    wxMemoryDC memDC(bmp);
    wxGCDC dc(memDC);
    dc.SetFont(DrawingUtils::GetDefaultGuiFont());

    wxRect r(0, 0, 10, 16);
    int buttonHeight = 0;
    {
        wxSize textSize = dc.GetTextExtent("Tp");
        buttonHeight = wxMax(buttonHeight, textSize.GetHeight());
    }

    if(GetBitmap().IsOk()) {
        int bmpHeight = GetBitmap().GetScaledHeight();
        buttonHeight = wxMax(buttonHeight, bmpHeight);
    }
    r.SetHeight(buttonHeight);
    r.Inflate(FromDIP(SPACER));
    SetSizeHints(wxSize(-1, r.GetHeight()));
}

void clCaptionBar::OnSize(wxSizeEvent& e)
{
    e.Skip();
    Refresh();
}

void clCaptionBar::SetBitmap(const wxBitmap& bitmap)
{
    m_bitmap = bitmap;
    DoSetBestSize();
    Refresh();
}

void clCaptionBar::SetCaption(const wxString& caption)
{
    m_caption = caption;
    m_topLevelWindow->SetLabel(caption);
    Refresh();
}

void clCaptionBar::OnMouseDoubleClick(wxMouseEvent& e)
{
    wxUnusedVar(e);
    wxWindowUpdateLocker locker(m_topLevelWindow);
    check_ptr_return(m_topLevelWindow);
    if(m_topLevelWindow->IsMaximized()) {
        m_topLevelWindow->Restore();
    } else {
        m_topLevelWindow->Maximize();
    }
}

void clCaptionBar::SetOption(wxCaptionStyle option, bool enabled)
{
    if(enabled) {
        m_flags |= option;
    } else {
        m_flags &= ~option;
    }
    DoSetBestSize();
    Refresh();
}

void clCaptionBar::SetOptions(size_t options)
{
    m_flags = options;
    DoSetBestSize();
    Refresh();
}

void clCaptionBar::ClearRects()
{
    m_bitmapRect = {};
    m_buttonClose.Clear();
    m_buttonMaximize.Clear();
    m_buttonMenu.Clear();
    m_buttonMinimize.Clear();
}

bool clCaptionBar::ProcessCallback(const CallbackMap_t& map, wxCaptionHitTest where)
{
    if(map.count(where)) {
        const auto& p = map.find(where);
        auto This = p->second.first;
        auto cb = p->second.second;

        if(This && cb) {
            (This->*cb)(p->first);
        }
        return true;
    } else {
        return false;
    }
}

void clCaptionBar::SetMenuBar(clMenuBar* menuBar)
{
    if(menuBar) {
        m_mainMenu = menuBar->CreateSingleMenu();
    } else {
        m_mainMenu = nullptr;
    }

    SetOption(wxCAPTION_MENU_BUTTON, m_mainMenu ? true : false);
    DoSetBestSize();
    Refresh();
}

void clCaptionBar::ShowMenuBar()
{
    if(!m_mainMenu || !m_mainMenu->GetMenu()) {
        return;
    }
    m_menu_is_up = true;
    PopupMenu(m_mainMenu->GetMenu(), GetClientRect().GetBottomLeft());
    m_menu_is_up = false;
    Refresh();
}

// Button methods
void clCaptionButton::LeftDown(wxCaptionHitTest where)
{
    wxUnusedVar(where);
    m_state = wxCAPTION_BUTTON_PRESSED;
    m_captionBar->Refresh();
}

void clCaptionButton::LeftUp(wxCaptionHitTest where)
{
    auto frame = m_captionBar->m_topLevelWindow;
    switch(where) {
    case wxCAPTION_HT_CLOSEBUTTON:
        frame->Close();
        break;
    case wxCAPTION_HT_MAXMIZEBUTTON:
        if(!frame->IsMaximized()) {
            frame->Maximize();
        } else {
            frame->Restore();
        }
        break;
    case wxCAPTION_HT_MINIMIZEBUTTON:
        frame->Iconize();
        break;
    case wxCAPTION_HT_MENUBUTTON:
        m_captionBar->ShowMenuBar();
        break;
    case wxCAPTION_HT_ICON:
        break;
    case wxCAPTION_HT_NOWHERE:
        break;
    }
    ResetState();
}

void clCaptionButton::Render(wxDC& dc, wxCaptionHitTest ht)
{
    // determine the colours
    wxColour bg_colour = m_captionBar->m_colours.GetBgColour();
    wxColour pen_colour = m_captionBar->m_colours.GetItemTextColour();

    bool draw_background = true;
    if(m_state == wxCAPTION_BUTTON_HOVER) {
        if(ht == wxCAPTION_HT_CLOSEBUTTON) {
            bg_colour = *wxRED;
            pen_colour = *wxWHITE;
        } else {
            bool is_dark = DrawingUtils::IsDark(bg_colour);
            bg_colour = bg_colour.ChangeLightness(is_dark ? 120 : 80);
        }
    } else if(m_state == wxCAPTION_BUTTON_PRESSED) {
        if(ht == wxCAPTION_HT_CLOSEBUTTON) {
            bg_colour = wxColour(*wxRED).ChangeLightness(60);
            pen_colour = wxColour(*wxWHITE).ChangeLightness(60);
        } else {
            bool is_dark = DrawingUtils::IsDark(bg_colour);
            bg_colour = bg_colour.ChangeLightness(is_dark ? 140 : 60);
        }
    } else {
        draw_background = false;
    }

#define PREPARE_BUTTON_DRAW()     \
    if(draw_background) {         \
        dc.SetPen(bg_colour);     \
        dc.SetBrush(bg_colour);   \
        dc.DrawRectangle(m_rect); \
    }                             \
    dc.SetPen(pen_colour);        \
    dc.SetBrush(bg_colour);

    // draw button per type
    switch(ht) {
    case wxCAPTION_HT_CLOSEBUTTON: {
        PREPARE_BUTTON_DRAW();
        dc.DrawLine(m_innerRect.GetTopLeft(), m_innerRect.GetBottomRight());
        dc.DrawLine(m_innerRect.GetTopRight(), m_innerRect.GetBottomLeft());
    } break;
    case wxCAPTION_HT_MAXMIZEBUTTON: {
        PREPARE_BUTTON_DRAW();
        dc.DrawRectangle(m_innerRect);
    } break;
    case wxCAPTION_HT_MENUBUTTON: {
        PREPARE_BUTTON_DRAW();
        // draw 3 lines
        wxPoint pt1, pt2;
        pt1 = m_innerRect.GetTopLeft();
        pt2 = m_innerRect.GetTopRight();

        pt1.y += m_innerRect.GetHeight() / 2;
        pt2.y += m_innerRect.GetHeight() / 2;

        dc.DrawLine(m_innerRect.GetTopLeft(), m_innerRect.GetTopRight());
        dc.DrawLine(pt1, pt2);
        dc.DrawLine(m_innerRect.GetBottomLeft(), m_innerRect.GetBottomRight());

    } break;
    case wxCAPTION_HT_MINIMIZEBUTTON: {
        PREPARE_BUTTON_DRAW();
        wxPoint p1 = m_innerRect.GetTopLeft();
        wxPoint p2 = m_innerRect.GetTopRight();

        p1.y += m_innerRect.GetHeight() / 2;
        p2.y += m_innerRect.GetHeight() / 2;
        dc.DrawLine(p1, p2);
    } break;
    default:
        break;
    }

#undef PREPARE_BUTTON_DRAW
}
