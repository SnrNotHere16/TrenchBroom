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

#include "MergeNodesIntoWorldVisitor.h"

#include "CollectionUtils.h"
#include "Model/Brush.h"
#include "Model/Entity.h"
#include "Model/EntityAttributes.h"
#include "Model/Group.h"
#include "Model/Layer.h"
#include "Model/World.h"

#include <cassert>

namespace TrenchBroom {
    namespace Model {
        MergeNodesIntoWorldVisitor::MergeNodesIntoWorldVisitor(World* world, Layer* layer) :
        m_world(world),
        m_layer(layer != NULL ? layer : m_world->defaultLayer()) {
            ensure(m_world != NULL, "world is null");
            assert(m_layer->isDescendantOf(m_world));
        }

        const ParentChildrenMap& MergeNodesIntoWorldVisitor::result() const {
            detachNodes();
            deleteNodes();
            return m_result;
        }

        void MergeNodesIntoWorldVisitor::doVisit(World* world) {
            world->iterate(*this);
            deleteNode(world);
        }
        
        void MergeNodesIntoWorldVisitor::doVisit(Layer* layer) {
            layer->iterate(*this);
            deleteNode(layer);
        }
        
        void MergeNodesIntoWorldVisitor::doVisit(Group* group) {
            addNode(group);
        }
        
        void MergeNodesIntoWorldVisitor::doVisit(Entity* entity) {
            if (isWorldspawn(entity->classname(), entity->attributes())) {
                entity->iterate(*this);
                deleteNode(entity);
            } else {
                addNode(entity);
            }
        }
        
        void MergeNodesIntoWorldVisitor::doVisit(Brush* brush) {
            addNode(brush);
        }

        void MergeNodesIntoWorldVisitor::addNode(Node* node) {
            m_result[m_layer].push_back(node);
            detachNode(node);
        }

        void MergeNodesIntoWorldVisitor::deleteNode(Node* node) {
            detachNode(node);
            m_nodesToDelete.push_back(node);
        }

        void MergeNodesIntoWorldVisitor::detachNode(Node* node) {
            Node* parent = node->parent();
            if (parent != NULL)
                m_nodesToDetach.push_back(node);
        }

        void MergeNodesIntoWorldVisitor::deleteNodes() const {
            VectorUtils::clearAndDelete(m_nodesToDelete);
        }
        
        void MergeNodesIntoWorldVisitor::detachNodes() const {
            for (Node* node : m_nodesToDetach) {
                Node* parent = node->parent();
                ensure(parent != NULL, "parent is null");
                parent->removeChild(node);
            }
            m_nodesToDetach.clear();
        }
    }
}
