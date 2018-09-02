//////////////////////////////////////////////////////////////////////
// This file was auto-generated by codelite's wxCrafter Plugin
// wxCrafter project file: wxcrafter.wxcp
// Do not modify this file by hand!
//////////////////////////////////////////////////////////////////////

#include "wxcrafter.h"

// Declare the bitmap loading function
extern void wxC9ED9InitBitmapResources();

static bool bBitmapLoaded = false;

MainFrameBaseClass::MainFrameBaseClass(
    wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style)
    : wxFrame(parent, id, title, pos, size, style)
{
    if(!bBitmapLoaded) {
        // We need to initialise the default bitmap handler
        wxXmlResource::Get()->AddHandler(new wxBitmapXmlHandler);
        wxC9ED9InitBitmapResources();
        bBitmapLoaded = true;
    }

    wxBoxSizer* boxSizer1 = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(boxSizer1);

    m_mainPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), wxTAB_TRAVERSAL);

    boxSizer1->Add(m_mainPanel, 1, wxEXPAND, WXC_FROM_DIP(5));

    wxBoxSizer* boxSizer11 = new wxBoxSizer(wxVERTICAL);
    m_mainPanel->SetSizer(boxSizer11);

    m_stc15
        = new wxStyledTextCtrl(m_mainPanel, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(m_mainPanel, wxSize(-1, 150)), 0);
#ifdef __WXMSW__
    // To get the newer version of the font on MSW, we use font wxSYS_DEFAULT_GUI_FONT with family set to
    // wxFONTFAMILY_TELETYPE
    wxFont m_stc15Font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
    m_stc15Font.SetFamily(wxFONTFAMILY_TELETYPE);
#else
    wxFont m_stc15Font = wxSystemSettings::GetFont(wxSYS_ANSI_FIXED_FONT);
    m_stc15Font.SetFamily(wxFONTFAMILY_TELETYPE);
#endif
    m_stc15->SetFont(m_stc15Font);
    // Configure the fold margin
    m_stc15->SetMarginType(4, wxSTC_MARGIN_SYMBOL);
    m_stc15->SetMarginMask(4, wxSTC_MASK_FOLDERS);
    m_stc15->SetMarginSensitive(4, true);
    m_stc15->SetMarginWidth(4, 0);

    // Configure the tracker margin
    m_stc15->SetMarginWidth(1, 0);

    // Configure the symbol margin
    m_stc15->SetMarginType(2, wxSTC_MARGIN_SYMBOL);
    m_stc15->SetMarginMask(2, ~(wxSTC_MASK_FOLDERS));
    m_stc15->SetMarginWidth(2, 0);
    m_stc15->SetMarginSensitive(2, true);

    // Configure the line numbers margin
    m_stc15->SetMarginType(0, wxSTC_MARGIN_NUMBER);
    m_stc15->SetMarginWidth(0, 0);

    // Configure the line symbol margin
    m_stc15->SetMarginType(3, wxSTC_MARGIN_FORE);
    m_stc15->SetMarginMask(3, 0);
    m_stc15->SetMarginWidth(3, 0);
    // Select the lexer
    m_stc15->SetLexer(wxSTC_LEX_NULL);
    // Set default font / styles
    m_stc15->StyleClearAll();
    for(int i = 0; i < wxSTC_STYLE_MAX; ++i) { m_stc15->StyleSetFont(i, m_stc15Font); }
    m_stc15->SetWrapMode(0);
    m_stc15->SetIndentationGuides(0);
    m_stc15->SetKeyWords(0, wxT(""));
    m_stc15->SetKeyWords(1, wxT(""));
    m_stc15->SetKeyWords(2, wxT(""));
    m_stc15->SetKeyWords(3, wxT(""));
    m_stc15->SetKeyWords(4, wxT(""));

    boxSizer11->Add(m_stc15, 0, wxALL | wxEXPAND | wxALIGN_BOTTOM, WXC_FROM_DIP(5));

    m_menuBar = new wxMenuBar(0);
    this->SetMenuBar(m_menuBar);

    m_name6 = new wxMenu();
    m_menuBar->Append(m_name6, _("File"));

    m_menuItem13 = new wxMenuItem(m_name6, ID_OPEN_FOLDER, _("Open folder..."), wxT(""), wxITEM_NORMAL);
    m_name6->Append(m_menuItem13);

    m_name6->AppendSeparator();

    m_menuItemExpandAll = new wxMenuItem(m_name6, wxID_ANY, _("Expand All"), wxT(""), wxITEM_NORMAL);
    m_name6->Append(m_menuItemExpandAll);

    m_menuItemCollapseAll = new wxMenuItem(m_name6, wxID_ANY, _("Collapse All"), wxT(""), wxITEM_NORMAL);
    m_name6->Append(m_menuItemCollapseAll);

    m_menuItemFirstVisible = new wxMenuItem(m_name6, wxID_ANY, _("First Visible Item"), wxT(""), wxITEM_NORMAL);
    m_name6->Append(m_menuItemFirstVisible);

    m_menuItemNextVisibleItem = new wxMenuItem(m_name6, wxID_ANY, _("Next Visible Item"), wxT(""), wxITEM_NORMAL);
    m_name6->Append(m_menuItemNextVisibleItem);

    m_menuItemEnsureVisible = new wxMenuItem(m_name6, wxID_ANY, _("Ensure Item Visible"), wxT(""), wxITEM_NORMAL);
    m_name6->Append(m_menuItemEnsureVisible);

    m_name6->AppendSeparator();

    m_menuItem7 = new wxMenuItem(m_name6, wxID_EXIT, _("Exit\tAlt-X"), _("Quit"), wxITEM_NORMAL);
    m_name6->Append(m_menuItem7);

    m_name8 = new wxMenu();
    m_menuBar->Append(m_name8, _("Help"));

    m_menuItem9 = new wxMenuItem(m_name8, wxID_ABOUT, _("About..."), wxT(""), wxITEM_NORMAL);
    m_name8->Append(m_menuItem9);

    SetName(wxT("MainFrameBaseClass"));
    SetSize(wxDLG_UNIT(this, wxSize(-1, -1)));
    if(GetSizer()) { GetSizer()->Fit(this); }
    if(GetParent()) {
        CentreOnParent(wxBOTH);
    } else {
        CentreOnScreen(wxBOTH);
    }
#if wxVERSION_NUMBER >= 2900
    if(!wxPersistenceManager::Get().Find(this)) {
        wxPersistenceManager::Get().RegisterAndRestore(this);
    } else {
        wxPersistenceManager::Get().Restore(this);
    }
#endif
    // Connect events
    this->Connect(m_menuItem13->GetId(), wxEVT_COMMAND_MENU_SELECTED,
        wxCommandEventHandler(MainFrameBaseClass::OnOpenFolder), NULL, this);
    this->Connect(m_menuItemExpandAll->GetId(), wxEVT_COMMAND_MENU_SELECTED,
        wxCommandEventHandler(MainFrameBaseClass::OnExpandAll), NULL, this);
    this->Connect(m_menuItemCollapseAll->GetId(), wxEVT_COMMAND_MENU_SELECTED,
        wxCommandEventHandler(MainFrameBaseClass::OnCollapseAll), NULL, this);
    this->Connect(m_menuItemFirstVisible->GetId(), wxEVT_COMMAND_MENU_SELECTED,
        wxCommandEventHandler(MainFrameBaseClass::OnFirstVisible), NULL, this);
    this->Connect(m_menuItemNextVisibleItem->GetId(), wxEVT_COMMAND_MENU_SELECTED,
        wxCommandEventHandler(MainFrameBaseClass::OnNextVisible), NULL, this);
    this->Connect(m_menuItemEnsureVisible->GetId(), wxEVT_COMMAND_MENU_SELECTED,
        wxCommandEventHandler(MainFrameBaseClass::OnEnsureItemVisible), NULL, this);
    this->Connect(m_menuItem7->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrameBaseClass::OnExit),
        NULL, this);
    this->Connect(m_menuItem9->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrameBaseClass::OnAbout),
        NULL, this);
}

MainFrameBaseClass::~MainFrameBaseClass()
{
    this->Disconnect(m_menuItem13->GetId(), wxEVT_COMMAND_MENU_SELECTED,
        wxCommandEventHandler(MainFrameBaseClass::OnOpenFolder), NULL, this);
    this->Disconnect(m_menuItemExpandAll->GetId(), wxEVT_COMMAND_MENU_SELECTED,
        wxCommandEventHandler(MainFrameBaseClass::OnExpandAll), NULL, this);
    this->Disconnect(m_menuItemCollapseAll->GetId(), wxEVT_COMMAND_MENU_SELECTED,
        wxCommandEventHandler(MainFrameBaseClass::OnCollapseAll), NULL, this);
    this->Disconnect(m_menuItemFirstVisible->GetId(), wxEVT_COMMAND_MENU_SELECTED,
        wxCommandEventHandler(MainFrameBaseClass::OnFirstVisible), NULL, this);
    this->Disconnect(m_menuItemNextVisibleItem->GetId(), wxEVT_COMMAND_MENU_SELECTED,
        wxCommandEventHandler(MainFrameBaseClass::OnNextVisible), NULL, this);
    this->Disconnect(m_menuItemEnsureVisible->GetId(), wxEVT_COMMAND_MENU_SELECTED,
        wxCommandEventHandler(MainFrameBaseClass::OnEnsureItemVisible), NULL, this);
    this->Disconnect(m_menuItem7->GetId(), wxEVT_COMMAND_MENU_SELECTED,
        wxCommandEventHandler(MainFrameBaseClass::OnExit), NULL, this);
    this->Disconnect(m_menuItem9->GetId(), wxEVT_COMMAND_MENU_SELECTED,
        wxCommandEventHandler(MainFrameBaseClass::OnAbout), NULL, this);
}

MyImages::MyImages()
    : wxImageList(16, 16, true)
    , m_imagesWidth(16)
    , m_imagesHeight(16)
{
    if(!bBitmapLoaded) {
        // We need to initialise the default bitmap handler
        wxXmlResource::Get()->AddHandler(new wxBitmapXmlHandler);
        wxC9ED9InitBitmapResources();
        bBitmapLoaded = true;
    }

    {
        wxBitmap bmp;
        wxIcon icn;
        bmp = wxXmlResource::Get()->LoadBitmap(wxT("folder"));
        if(bmp.IsOk()) {
            if((m_imagesWidth == bmp.GetWidth()) && (m_imagesHeight == bmp.GetHeight())) {
                icn.CopyFromBitmap(bmp);
                this->Add(icn);
            }
            m_bitmaps.insert(std::make_pair(wxT("folder"), bmp));
        }
    }
    {
        wxBitmap bmp;
        wxIcon icn;
        bmp = wxXmlResource::Get()->LoadBitmap(wxT("folder@2x"));
        if(bmp.IsOk()) {
            if((m_imagesWidth == bmp.GetWidth()) && (m_imagesHeight == bmp.GetHeight())) {
                icn.CopyFromBitmap(bmp);
                this->Add(icn);
            }
            m_bitmaps.insert(std::make_pair(wxT("folder@2x"), bmp));
        }
    }

    {
        wxBitmap bmp;
        wxIcon icn;
        bmp = wxXmlResource::Get()->LoadBitmap(wxT("folder_open"));
        if(bmp.IsOk()) {
            if((m_imagesWidth == bmp.GetWidth()) && (m_imagesHeight == bmp.GetHeight())) {
                icn.CopyFromBitmap(bmp);
                this->Add(icn);
            }
            m_bitmaps.insert(std::make_pair(wxT("folder_open"), bmp));
        }
    }
    {
        wxBitmap bmp;
        wxIcon icn;
        bmp = wxXmlResource::Get()->LoadBitmap(wxT("folder_open@2x"));
        if(bmp.IsOk()) {
            if((m_imagesWidth == bmp.GetWidth()) && (m_imagesHeight == bmp.GetHeight())) {
                icn.CopyFromBitmap(bmp);
                this->Add(icn);
            }
            m_bitmaps.insert(std::make_pair(wxT("folder_open@2x"), bmp));
        }
    }

    {
        wxBitmap bmp;
        wxIcon icn;
        bmp = wxXmlResource::Get()->LoadBitmap(wxT("file"));
        if(bmp.IsOk()) {
            if((m_imagesWidth == bmp.GetWidth()) && (m_imagesHeight == bmp.GetHeight())) {
                icn.CopyFromBitmap(bmp);
                this->Add(icn);
            }
            m_bitmaps.insert(std::make_pair(wxT("file"), bmp));
        }
    }
    {
        wxBitmap bmp;
        wxIcon icn;
        bmp = wxXmlResource::Get()->LoadBitmap(wxT("file@2x"));
        if(bmp.IsOk()) {
            if((m_imagesWidth == bmp.GetWidth()) && (m_imagesHeight == bmp.GetHeight())) {
                icn.CopyFromBitmap(bmp);
                this->Add(icn);
            }
            m_bitmaps.insert(std::make_pair(wxT("file@2x"), bmp));
        }
    }
}

MyImages::~MyImages() {}
