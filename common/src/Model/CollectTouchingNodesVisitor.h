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

#ifndef TrenchBroom_CollectTouchingNodesVisitor
#define TrenchBroom_CollectTouchingNodesVisitor

#include "Model/CollectMatchingNodesVisitor.h"
#include "Model/MatchSelectableNodes.h"
#include "Model/ModelTypes.h"
#include "Model/NodePredicates.h"

namespace TrenchBroom {
    namespace Model {
        class EditorContext;
        
        template <typename I>
        class MatchTouchingNodes {
        private:
            const I m_begin;
            const I m_end;
        public:
            MatchTouchingNodes(I begin, I end) :
            m_begin(begin),
            m_end(end) {}
            
            bool operator()(const Node* node) const {
                I cur = m_begin;
                while (cur != m_end) {
                    if (*cur != node && (*cur)->intersects(node))
                        return true;
                    ++cur;
                }
                return false;
            }
        };
        
        template <typename I>
        class CollectTouchingNodesVisitor : public CollectMatchingNodesVisitor<NodePredicates::And<MatchSelectableNodes, MatchTouchingNodes<I> >, UniqueNodeCollectionStrategy> {
        public:
                CollectTouchingNodesVisitor(I begin, I end, const Model::EditorContext& editorContext) :
                CollectMatchingNodesVisitor<NodePredicates::And<MatchSelectableNodes, MatchTouchingNodes<I> >, UniqueNodeCollectionStrategy>(NodePredicates::And<MatchSelectableNodes, MatchTouchingNodes<I> >(MatchSelectableNodes(editorContext), MatchTouchingNodes<I>(begin, end))) {}
        };
    }
}

#endif /* defined(TrenchBroom_CollectTouchingObjectsVisitor) */
