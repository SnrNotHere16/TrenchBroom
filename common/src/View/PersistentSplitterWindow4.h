/*
 Copyright (C) 2010-2016 Kristian Duske
 
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

#ifndef TrenchBroom_PersistentSplitterWindow4
#define TrenchBroom_PersistentSplitterWindow4

#include <wx/persist/toplevel.h>

namespace TrenchBroom {
    namespace View {
        class SplitterWindow4;
        
        class PersistentSplitterWindow4 : public wxPersistentWindow<SplitterWindow4> {
        private:
            static const double Scaling;
        public:
            PersistentSplitterWindow4(SplitterWindow4* obj);
            
            wxString GetKind() const;
            void Save() const;
            bool Restore();
        private:
            wxRealPoint getSplitRatios(const SplitterWindow4* window) const;
        };
    }
}

#endif /* defined(TrenchBroom_PersistentSplitterWindow4) */
