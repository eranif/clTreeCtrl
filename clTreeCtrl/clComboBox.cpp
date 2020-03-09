#include "clComboBox.hpp"
#include <wx/button.h>
#include <wx/sizer.h>

clComboBox::clComboBox(wxWindow* parent, wxWindowID id, const wxString& value, const wxPoint& pos, const wxSize& size,
                       size_t n, const wxString choices[], long style, const wxValidator& validator,
                       const wxString& name)
    : wxControl(parent, id, pos, size, style)
{
    wxUnusedVar(validator);
    wxUnusedVar(name);
    m_cbStyle = style & ~wxWINDOW_STYLE_MASK;
    m_choices.reserve(n);
    for(size_t i = 0; i < n; ++i) {
        m_choices.Add(choices[i]);
    }
    DoCreate(value);
}

clComboBox::clComboBox(wxWindow* parent, wxWindowID id, const wxString& value, const wxPoint& pos, const wxSize& size,
                       const wxArrayString& choices, long style, const wxValidator& validator, const wxString& name)
    : wxControl(parent, id, pos, size, style)
{
    wxUnusedVar(validator);
    wxUnusedVar(name);
    m_cbStyle = style & ~wxWINDOW_STYLE_MASK;
    m_choices.reserve(choices.size());
    m_choices = choices;
    DoCreate(value);
}

clComboBox::clComboBox() {}

clComboBox::~clComboBox() {}

bool clComboBox::Create(wxWindow* parent, wxWindowID id, const wxString& value, const wxPoint& pos, const wxSize& size,
                        size_t n, const wxString choices[], long style, const wxValidator& validator,
                        const wxString& name)
{
    wxUnusedVar(validator);
    wxUnusedVar(name);
    bool res = wxControl::Create(parent, id, pos, size, style);
    m_choices.reserve(n);
    for(size_t i = 0; i < n; ++i) {
        m_choices.push_back(choices[i]);
    }
    DoCreate(value);
    return res;
}

bool clComboBox::Create(wxWindow* parent, wxWindowID id, const wxString& value, const wxPoint& pos, const wxSize& size,
                        const wxArrayString& choices, long style, const wxValidator& validator, const wxString& name)
{
    wxUnusedVar(validator);
    wxUnusedVar(name);
    bool res = wxControl::Create(parent, id, pos, size, style);
    m_choices = choices;
    DoCreate(value);
    return res;
}

void clComboBox::DoCreate(const wxString& value)
{
    SetSizer(new wxBoxSizer(wxHORIZONTAL));
    m_textCtrl =
        new wxTextCtrl(this, wxID_ANY, value, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER | wxTE_RICH2);
    GetSizer()->Add(m_textCtrl, 1, wxEXPAND, 0);
    m_button = new clButton(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
    m_button->SetHasDropDownMenu(true);
    m_button->SetText(""); // this will force size calculation
    m_button->Bind(wxEVT_BUTTON, &clComboBox::OnButtonClicked, this);
    m_textCtrl->Bind(wxEVT_TEXT, &clComboBox::OnText, this);
    m_textCtrl->Bind(wxEVT_TEXT_ENTER, &clComboBox::OnTextEnter, this);
    m_textCtrl->Bind(wxEVT_CHAR, &clComboBox::OnNavigationKey, this);
    if(m_cbStyle & wxCB_READONLY) {
        m_textCtrl->SetEditable(false);
    }
    GetSizer()->Add(m_button);
    GetSizer()->Fit(this);
}

void clComboBox::OnButtonClicked(wxCommandEvent& event)
{
    wxUnusedVar(event);
    wxMenu menu;
    for(size_t i = 0; i < m_choices.size(); ++i) {
        const wxString& choice = m_choices.Item(i);
        auto item = menu.Append(wxID_ANY, choice, "", wxITEM_CHECK);
        item->Check(i == (size_t)m_selection);
        menu.Bind(wxEVT_MENU,
                  [this, choice, i](wxCommandEvent& e) {
                      wxUnusedVar(e);

                      // update the selected text
                      m_textCtrl->ChangeValue(choice);
                      m_selection = i;

                      // Notify about selection change
                      wxCommandEvent selectionChanged(wxEVT_COMBOBOX);
                      selectionChanged.SetEventObject(this);
                      selectionChanged.SetInt(i); // the new selection
                      GetEventHandler()->AddPendingEvent(selectionChanged);
                  },
                  item->GetId());
    }

#if 0
    wxPoint menuPos = GetClientRect().GetBottomLeft();
#ifdef __WXOSX__
    menuPos.y += 5;
#endif
    menuPos = m_button->ScreenToClient(ClientToScreen(menuPos));
#endif

    m_button->ShowMenu(menu, nullptr);
    m_textCtrl->CallAfter(&wxTextCtrl::SetFocus);
}

void clComboBox::SetHint(const wxString& hint) { wxUnusedVar(hint); }

void clComboBox::SetSelection(size_t sel)
{
    if(sel == INVALID_SIZE_T) {
        m_textCtrl->Clear();
        m_selection = INVALID_SIZE_T;
    } else {
        if(sel >= m_choices.GetCount()) {
            return;
        }
        m_selection = sel;
        // else, selection is a valid index
        SetValue(m_choices.Item(m_selection));
    }
}

void clComboBox::OnText(wxCommandEvent& event)
{
    wxUnusedVar(event);
    wxCommandEvent textEvent(wxEVT_TEXT);
    textEvent.SetEventObject(this);
    GetEventHandler()->AddPendingEvent(textEvent);
    m_selection = m_choices.Index(m_textCtrl->GetValue());
}

void clComboBox::OnTextEnter(wxCommandEvent& event)
{
    wxUnusedVar(event);
    wxCommandEvent textEvent(wxEVT_TEXT_ENTER);
    textEvent.SetEventObject(this);
    GetEventHandler()->AddPendingEvent(textEvent);
}

void clComboBox::SetString(size_t n, const wxString& text)
{
    if(n >= m_choices.size()) {
        return;
    }
    m_choices[n] = text;
    if(GetSelection() == (size_t)n) {
        SetValue(m_choices[n]);
    }
}

void clComboBox::SetValue(const wxString& text) { m_textCtrl->SetValue(text); }

wxString clComboBox::GetStringSelection() const { return m_textCtrl->GetValue(); }

void clComboBox::SetStringSelection(const wxString& text)
{
    for(size_t i = 0; i < m_choices.size(); ++i) {
        if(m_choices.Item(i).CmpNoCase(text) == 0) {
            m_textCtrl->ChangeValue(m_choices.Item(i));
            m_selection = i;
        }
    }
}

void clComboBox::SetFocus() { m_textCtrl->SetFocus(); }

void clComboBox::Append(const std::vector<wxString>& strings)
{
    if(strings.empty()) {
        return;
    }
    m_choices.reserve(strings.size() + m_choices.size());
    for(const auto& str : strings) {
        m_choices.Add(str);
    }
}

void clComboBox::Append(const wxArrayString& strings)
{
    if(strings.empty()) {
        return;
    }
    m_choices.reserve(strings.size() + m_choices.size());
    m_choices.insert(m_choices.end(), strings.begin(), strings.end());
}

size_t clComboBox::Append(const wxString& text)
{
    m_choices.Add(text);
    return m_choices.size() - 1;
}

void clComboBox::Clear()
{
    m_choices.Clear();
    m_selection = INVALID_SIZE_T;
    m_textCtrl->ChangeValue(wxEmptyString);
}

void clComboBox::Delete(size_t index)
{
    if(index >= m_choices.size()) {
        return;
    }
    if(index <= m_selection) {
        // the removed item is _before_ the selected item
        // invalidate the selection
        m_selection = INVALID_SIZE_T;
        m_textCtrl->ChangeValue(wxEmptyString);
    }
    m_choices.RemoveAt(index);
}

size_t clComboBox::FindString(const wxString& s, bool bCase) const { return m_choices.Index(s, bCase); }

wxString clComboBox::GetValue() const { return m_textCtrl->GetValue(); }

wxArrayString clComboBox::GetStrings() const
{
    wxArrayString strings;
    strings.reserve(m_choices.size() + 1);
    if(GetSelection() == INVALID_SIZE_T) {
        strings.Add(GetValue());
    }
    strings.insert(strings.end(), m_choices.begin(), m_choices.end());
    return strings;
}

void clComboBox::OnNavigationKey(wxKeyEvent& event)
{
    if(event.GetKeyCode() == WXK_TAB) {
        Navigate(event.ShiftDown() ? wxNavigationKeyEvent::IsBackward : wxNavigationKeyEvent::IsForward);
    } else {
        event.Skip();
    }
}
