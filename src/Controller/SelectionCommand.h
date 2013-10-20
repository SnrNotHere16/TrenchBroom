/*
 Copyright (C) 2010-2013 Kristian Duske
 
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

#ifndef __TrenchBroom__SelectionCommand__
#define __TrenchBroom__SelectionCommand__

#include "SharedPointer.h"
#include "StringUtils.h"
#include "Controller/Command.h"
#include "Model/ModelTypes.h"
#include "Model/SelectionResult.h"
#include "View/ViewTypes.h"

namespace TrenchBroom {
    namespace Model {
        class Map;
    }
    
    namespace Controller {
        class SelectionCommand : public Command {
        public:
            static const CommandType Type;
            typedef std::tr1::shared_ptr<SelectionCommand> Ptr;

            typedef enum {
                SCSelectObjects,
                SCSelectFaces,
                SCSelectAllObjects,
                SCSelectAllFaces,
                SCDeselectObjects,
                SCDeselectFaces,
                SCDeselectAll
            } SelectCommand;
        private:
            View::MapDocumentPtr m_document;
            SelectCommand m_command;
            
            Model::ObjectList m_objects;
            Model::BrushFaceList m_faces;

            Model::ObjectList m_previouslySelectedObjects;
            Model::BrushFaceList m_previouslySelectedFaces;
            
            Model::SelectionResult m_lastResult;
        public:
            static Ptr select(View::MapDocumentPtr document, const Model::ObjectList& objects);
            static Ptr select(View::MapDocumentPtr document, const Model::BrushFaceList& faces);
            static Ptr selectAllObjects(View::MapDocumentPtr document);
            static Ptr selectAllFaces(View::MapDocumentPtr document);
            
            static Ptr deselect(View::MapDocumentPtr document, const Model::ObjectList& objects);
            static Ptr deselect(View::MapDocumentPtr document, const Model::BrushFaceList& faces);
            static Ptr deselectAll(View::MapDocumentPtr document);
            
            View::MapDocumentPtr document() const;
            const Model::SelectionResult& lastResult() const;
        private:
            SelectionCommand(View::MapDocumentPtr document, const SelectCommand command, const Model::ObjectList& objects, const Model::BrushFaceList& faces);

            static String makeName(const SelectCommand command, const Model::ObjectList& objects, const Model::BrushFaceList& faces);
            bool doPerformDo();
            bool doPerformUndo();
        };
    }
}

#endif /* defined(__TrenchBroom__SelectionCommand__) */