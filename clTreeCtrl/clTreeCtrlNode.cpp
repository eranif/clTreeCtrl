#include "clTreeCtrl.h"
#include "clTreeCtrlNode.h"
#include <functional>
#include <wx/dc.h>
#include <wx/settings.h>

clTreeCtrlNode::clTreeCtrlNode(clTreeCtrl* tree)
    : m_tree(tree)
    , m_model(tree ? &tree->GetModel() : nullptr)
{
}

clTreeCtrlNode::clTreeCtrlNode(clTreeCtrl* tree, const wxString& label, int bitmapIndex, int bitmapSelectedIndex)
    : m_tree(tree)
    , m_model(tree ? &tree->GetModel() : nullptr)
    , m_label(label)
    , m_bitmapIndex(bitmapIndex)
    , m_bitmapSelectedIndex(bitmapSelectedIndex)
{
}

clTreeCtrlNode::~clTreeCtrlNode()
{
    // Delete all the node children
    DeleteAllChildren();
    wxDELETE(m_clientData);

    // Notify the model that a selection is being deleted
    if(m_model) { m_model->NodeDeleted(this); }
}

void clTreeCtrlNode::InsertBetween(clTreeCtrlNode* first, clTreeCtrlNode* second)
{
    if(first) { first->m_next = this; }
    this->m_prev = first;
    this->m_next = second;
    if(second) { second->m_prev = this; }
}

void clTreeCtrlNode::InsertChild(clTreeCtrlNode* child, clTreeCtrlNode* where)
{
    child->SetParent(this);
    child->SetIndentsCount(GetIndentsCount() + 1);

    // We need the last item of this subtree (where 'this' is the root)
    clTreeCtrlNode* lastChild = where;

    // Append the item

    // lastCHild is used later to determine the sequential location for the newly added item
    lastChild = where ? where : this;
    while(lastChild && lastChild->GetLastChild()) { lastChild = lastChild->GetLastChild(); }

    // Insert the item in the parent children list
    clTreeCtrlNode::Vec_t::iterator iter = m_children.end();
    iter = std::find_if(m_children.begin(), m_children.end(), [&](clTreeCtrlNode* c) { return c == where; });
    if(iter == m_children.end()) {
        m_children.push_back(child);
    } else {
        // Insert the item _after_ 'where'
        ++iter;
        m_children.insert(iter, child);
    }

    // Connect the list
    if(!lastChild) { lastChild = this; }
    child->InsertBetween(lastChild, lastChild->m_next);
}

void clTreeCtrlNode::AddChild(clTreeCtrlNode* child) { InsertChild(child, nullptr); }

void clTreeCtrlNode::SetParent(clTreeCtrlNode* parent)
{
    if(m_parent) { m_parent->DeleteChild(this); }
    m_parent = parent;
}

void clTreeCtrlNode::DeleteChild(clTreeCtrlNode* child)
{
    // first remove all of its children
    // do this in a while loop since 'child->RemoveChild(c);' will alter
    // the array and will invalidate all iterators
    while(!child->m_children.empty()) {
        clTreeCtrlNode* c = child->m_children[0];
        child->DeleteChild(c);
    }
    // Connect the list
    clTreeCtrlNode* prev = child->m_prev;
    clTreeCtrlNode* next = child->m_next;
    if(prev) { prev->m_next = next; }
    if(next) { next->m_prev = prev; }
    // Now disconnect this child from this node
    clTreeCtrlNode::Vec_t::iterator iter
        = std::find_if(m_children.begin(), m_children.end(), [&](clTreeCtrlNode* c) { return c == child; });
    if(iter != m_children.end()) { m_children.erase(iter); }
    wxDELETE(child);
}

int clTreeCtrlNode::GetExpandedLines() const
{
    clTreeCtrlNode* node = const_cast<clTreeCtrlNode*>(this);
    int counter = 0;
    while(node) {
        if(node->IsExpanded()) { ++counter; }
        node = node->m_next;
    }
    return counter;
}

void clTreeCtrlNode::GetNextItems(int count, clTreeCtrlNode::Vec_t& items)
{
    items.reserve(count);
    items.push_back(this);
    clTreeCtrlNode* next = GetNext();
    while(next) {
        if(next->IsVisible()) { items.push_back(next); }
        if((int)items.size() == count) { return; }
        next = next->GetNext();
    }
}

void clTreeCtrlNode::GetPrevItems(int count, clTreeCtrlNode::Vec_t& items)
{
    items.reserve(count);
    items.insert(items.begin(), this);
    clTreeCtrlNode* prev = GetPrev();
    while(prev) {
        if(prev->IsVisible()) { items.insert(items.begin(), prev); }
        if((int)items.size() == count) { return; }
        prev = prev->GetPrev();
    }
}

clTreeCtrlNode* clTreeCtrlNode::GetVisibleItem(int index)
{
    clTreeCtrlNode::Vec_t items;
    GetNextItems(index, items);
    if((int)items.size() != index) { return nullptr; }
    return items.back();
}

void clTreeCtrlNode::UnselectAll()
{
    clTreeCtrlNode* item = const_cast<clTreeCtrlNode*>(this);
    while(item) {
        item->SetSelected(false);
        item = item->GetNext();
    }
}

bool clTreeCtrlNode::SetExpanded(bool b)
{
    // Already expanded?
    if(!m_model) { return false; }
    if(b && IsExpanded()) { return true; }
    // Already collapsed?
    if(!b && !IsExpanded()) { return true; }
    if(!m_model->NodeExpanding(this, b)) { return false; }

    SetFlag(kExpanded, b);
    m_model->NodeExpanded(this, b);
    return true;
}

void clTreeCtrlNode::ClearRects()
{
    m_buttonRect = wxRect();
    m_itemRect = wxRect();
}

void clTreeCtrlNode::Render(wxDC& dc, const clTreeCtrlColours& c)
{
    wxRect itemRect = GetItemRect();
    clTreeCtrlColours colours = c;
    wxFont f = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
    if(GetFont().IsOk()) { f = GetFont(); }
    if(GetTextColour().IsOk()) { colours.itemTextColour = GetTextColour(); }
    if(GetBgColour().IsOk()) { colours.itemBgColour = GetBgColour(); }
    dc.SetFont(f);

    if(IsSelected() || IsHovered()) {
        dc.SetBrush(IsSelected() ? colours.selItemBgColour : colours.hoverBgColour);
        dc.SetPen(IsSelected() ? colours.selItemBgColour : colours.hoverBgColour);
        dc.DrawRoundedRectangle(itemRect, 1.5);
    } else if(colours.itemBgColour.IsOk()) {
        dc.SetBrush(colours.itemBgColour);
        dc.SetPen(colours.itemBgColour);
        dc.DrawRectangle(itemRect);
    }

    wxSize textSize = dc.GetTextExtent(GetLabel());
    int textY = itemRect.GetY() + (m_tree->GetLineHeight() - textSize.GetHeight()) / 2;
    // Draw the button
    int textXOffset = 0;
    if(HasChildren()) {
        wxPoint pts[3];
        wxRect buttonRect = GetButtonRect();
        textXOffset += buttonRect.GetWidth();
        buttonRect.Deflate((buttonRect.GetWidth() / 3), (buttonRect.GetHeight() / 3));
        if(IsExpanded()) {
            pts[0] = buttonRect.GetTopRight();
            pts[1] = buttonRect.GetBottomRight();
            pts[2] = buttonRect.GetBottomLeft();
            dc.SetBrush(colours.buttonColour);
            dc.SetPen(colours.buttonColour);
            dc.DrawPolygon(3, pts);
        } else {
            pts[0] = buttonRect.GetTopLeft();
            pts[1] = buttonRect.GetBottomLeft();
            pts[2].x = buttonRect.GetRight();
            pts[2].y = (buttonRect.GetY() + (buttonRect.GetHeight() / 2));
            dc.SetBrush(*wxTRANSPARENT_BRUSH);
            dc.SetPen(colours.buttonColour);
            dc.DrawPolygon(3, pts);
        }
    } else {
        textXOffset += itemRect.GetHeight();
    }

    int itemIndent = (GetIndentsCount() * m_tree->GetIndent());
    int bitmapIndex = GetBitmapIndex();
    if(IsExpanded() && HasChildren() && GetBitmapSelectedIndex() != wxNOT_FOUND) {
        bitmapIndex = GetBitmapSelectedIndex();
    }

    if(bitmapIndex != wxNOT_FOUND) {
        const wxBitmap& bmp = m_tree->GetBitmap(bitmapIndex);
        if(bmp.IsOk()) {
            textXOffset += X_SPACER;
            int bitmapY = itemRect.GetY() + ((itemRect.GetHeight() - bmp.GetScaledHeight()) / 2);
            dc.DrawBitmap(bmp, itemIndent + textXOffset, bitmapY);
            textXOffset += bmp.GetScaledWidth();
            textXOffset += X_SPACER;
        }
    }
    dc.SetTextForeground(IsSelected() ? colours.selItemTextColour : colours.itemTextColour);
    dc.DrawText(GetLabel(), itemIndent + textXOffset, textY);
}

size_t clTreeCtrlNode::GetChildrenCount(bool recurse) const
{
    if(!recurse) {
        return m_children.size();
    } else {
        size_t count = m_children.size();
        for(size_t i = 0; i < count; ++i) { count += m_children[i]->GetChildrenCount(recurse); }
        return count;
    }
}

bool clTreeCtrlNode::IsVisible() const
{
    clTreeCtrlNode* parent = GetParent();
    while(parent) {
        if(!parent->IsExpanded()) { return false; }
        parent = parent->GetParent();
    }
    return true;
}

void clTreeCtrlNode::DeleteAllChildren()
{
    while(!m_children.empty()) {
        clTreeCtrlNode* c = m_children[0];
        // DeleteChild will remove it from the array
        DeleteChild(c);
    }
}

clTreeCtrlNode* clTreeCtrlNode::GetLastChild() const
{
    if(m_children.empty()) { return nullptr; }
    return m_children.back();
}

void clTreeCtrlColours::InitDefaults()
{
    itemTextColour = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT);
    selItemTextColour = itemTextColour;
    selItemBgColour = wxColour("rgb(199,203,209)");
    buttonColour = wxSystemSettings::GetColour(wxSYS_COLOUR_3DDKSHADOW);
    hoverBgColour = wxColour("rgb(219,221,224)");
    bgColour = wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE);
    itemBgColour = bgColour;
}

void clTreeCtrlColours::InitDarkDefaults()
{
    bgColour = wxColour("#1c2833");
    itemTextColour = wxColour("#eaecee");
    selItemTextColour = *wxWHITE;
    selItemBgColour = wxColour("#566573");
    buttonColour = itemTextColour;
    hoverBgColour = wxColour("#2c3e50");
    itemBgColour = bgColour;
}
