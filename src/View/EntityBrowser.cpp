/*
 Copyright (C) 2010-2012 Kristian Duske
 
 This file is part of TrenchBroom.
 
 TrenchBroom is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 TrenchBroom is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with TrenchBroom. If not, see <http://www.gnu.org/licenses/>.
 */

#include "EntityBrowser.h"

#include "PreferenceManager.h"
#include "Preferences.h"
#include "Assets/EntityDefinitionManager.h"
#include "View/EntityBrowserView.h"
#include "View/LayoutConstants.h"
#include "View/MapDocument.h"

#include <wx/choice.h>
#include <wx/event.h>
#include <wx/tglbtn.h>
#include <wx/srchctrl.h>
#include <wx/sizer.h>

namespace TrenchBroom {
    namespace View {
        EntityBrowser::EntityBrowser(wxWindow* parent, const wxWindowID windowId, Renderer::RenderResources& resources, MapDocumentPtr document) :
        wxPanel(parent, windowId) {
            const wxString sortOrders[2] = { _T("Name"), _T("Usage") };
            m_sortOrderChoice = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 2, sortOrders);
            m_sortOrderChoice->SetSelection(0);
            
            m_groupButton = new wxToggleButton(this, wxID_ANY, wxT("Group"), wxDefaultPosition, wxDefaultSize, LayoutConstants::ToggleButtonStyle | wxBU_EXACTFIT);
            m_usedButton = new wxToggleButton(this, wxID_ANY, wxT("Used"), wxDefaultPosition, wxDefaultSize, LayoutConstants::ToggleButtonStyle | wxBU_EXACTFIT);
            
            m_filterBox = new wxSearchCtrl(this, wxID_ANY);
            m_filterBox->ShowCancelButton(true);

            m_sortOrderChoice->Bind(wxEVT_CHOICE, &EntityBrowser::OnSortOrderChanged, this);
            m_groupButton->Bind(wxEVT_TOGGLEBUTTON, &EntityBrowser::OnGroupButtonToggled, this);
            m_usedButton->Bind(wxEVT_TOGGLEBUTTON, &EntityBrowser::OnUsedButtonToggled, this);
            m_filterBox->Bind(wxEVT_TEXT, &EntityBrowser::OnFilterPatternChanged, this);
            
            wxSizer* controlSizer = new wxBoxSizer(wxHORIZONTAL);
            controlSizer->AddSpacer(LayoutConstants::ChoiceLeftMargin);
            controlSizer->Add(m_sortOrderChoice, 0, wxEXPAND);
            controlSizer->AddSpacer(LayoutConstants::BrowserControlsHorizontalMargin);
            controlSizer->Add(m_groupButton, 0, wxEXPAND);
            controlSizer->AddSpacer(LayoutConstants::BrowserControlsHorizontalMargin);
            controlSizer->Add(m_usedButton, 0, wxEXPAND);
            controlSizer->AddSpacer(LayoutConstants::BrowserControlsHorizontalMargin);
            controlSizer->Add(m_filterBox, 1, wxEXPAND);
            
            wxPanel* browserPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_SUNKEN);
            m_scrollBar = new wxScrollBar(browserPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSB_VERTICAL);
            m_view = new EntityBrowserView(browserPanel, wxID_ANY, m_scrollBar,
                                           resources,
                                           document->entityDefinitionManager(),
                                           document->entityModelManager());
            
            wxSizer* browserPanelSizer = new wxBoxSizer(wxHORIZONTAL);
            browserPanelSizer->Add(m_view, 1, wxEXPAND);
            browserPanelSizer->Add(m_scrollBar, 0, wxEXPAND);
            browserPanel->SetSizerAndFit(browserPanelSizer);
            
            wxSizer* outerSizer = new wxBoxSizer(wxVERTICAL);
            outerSizer->AddSpacer(LayoutConstants::ControlVerticalMargin);
            outerSizer->Add(controlSizer, 0, wxEXPAND);
            outerSizer->AddSpacer(LayoutConstants::ControlVerticalMargin);
            outerSizer->Add(browserPanel, 1, wxEXPAND);
            
            SetSizerAndFit(outerSizer);
            
            PreferenceManager& prefs = PreferenceManager::instance();
            prefs.preferenceDidChangeNotifier.addObserver(this, &EntityBrowser::preferenceDidChange);
        }
        
        EntityBrowser::~EntityBrowser() {
            PreferenceManager& prefs = PreferenceManager::instance();
            prefs.preferenceDidChangeNotifier.removeObserver(this, &EntityBrowser::preferenceDidChange);
        }
        
        void EntityBrowser::reload() {
            if (m_view != NULL) {
                m_view->clear();
                m_view->reload();
            }
        }
        
        void EntityBrowser::OnSortOrderChanged(wxCommandEvent& event) {
            const Assets::EntityDefinitionManager::SortOrder sortOrder = event.GetSelection() == 0 ? Assets::EntityDefinitionManager::Name : Assets::EntityDefinitionManager::Usage;
            m_view->setSortOrder(sortOrder);
        }
        
        void EntityBrowser::OnGroupButtonToggled(wxCommandEvent& event) {
            m_view->setGroup(m_groupButton->GetValue());
        }
        
        void EntityBrowser::OnUsedButtonToggled(wxCommandEvent& event) {
            m_view->setHideUnused(m_usedButton->GetValue());
        }
        
        void EntityBrowser::OnFilterPatternChanged(wxCommandEvent& event) {
            m_view->setFilterText(m_filterBox->GetValue().ToStdString());
        }

        void EntityBrowser::preferenceDidChange(const String& name) {
            if (name == Preferences::GamePaths.name())
                reload();
            else
                m_view->Refresh();
        }
    }
}