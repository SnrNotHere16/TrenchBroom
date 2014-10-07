/*
 Copyright (C) 2010-2014 Kristian Duske
 
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

#ifndef __TrenchBroom__Entity__
#define __TrenchBroom__Entity__

#include "TrenchBroom.h"
#include "VecMath.h"
#include "Hit.h"
#include "Assets/AssetTypes.h"
#include "Model/Attributable.h"
#include "Model/EntityRotationPolicy.h"
#include "Model/Object.h"

namespace TrenchBroom {
    namespace Model {
        class Entity : public Attributable, public Object, private EntityRotationPolicy {
        public:
            static const Hit::HitType EntityHit;
        private:
            static const BBox3 DefaultBounds;
            mutable BBox3 m_bounds;
            mutable bool m_boundsValid;
            Assets::EntityModel* m_model;
        public:
            Entity();
            
            bool pointEntity() const;
            Vec3 origin() const;
            Quat3 rotation() const;
        private:
            void setOrigin(const Vec3& origin);
            void applyRotation(const Mat4x4& transformation);
        public: // entity model
            Assets::ModelSpecification modelSpecification() const;
            Assets::EntityModel* model() const;
            void setModel(Assets::EntityModel* model);
        private: // implement Node interface
            Node* doClone(const BBox3& worldBounds) const;
            NodeSnapshot* doTakeSnapshot();
            
            bool doCanAddChild(const Node* child) const;
            bool doCanRemoveChild(const Node* child) const;
            bool doRemoveIfEmpty() const;

            void doDescendantDidChange(Node* node);

            bool doSelectable() const;
            
            void doAccept(NodeVisitor& visitor);
            void doAccept(ConstNodeVisitor& visitor) const;
        private: // implement Attributable interface
            void doAttributesDidChange();
            bool doCanAddOrUpdateAttribute(const AttributeName& name, const AttributeValue& value) const;
            bool doCanRenameAttribute(const AttributeName& name, const AttributeName& newName) const;
            bool doCanRemoveAttribute(const AttributeName& name) const;
        private: // implement Object interface
            const BBox3& doGetBounds() const;
            void doPick(const Ray3& ray, Hits& hits) const;

            Node* doGetContainer() const;
            Layer* doGetLayer() const;
            Group* doGetGroup() const;
            
            void doTransform(const Mat4x4& transformation, bool lockTextures, const BBox3& worldBounds);
            bool doContains(const Node* node) const;
            bool doIntersects(const Node* node) const;
        private:
            void invalidateBounds();
            void validateBounds() const;
        private:
            Entity(const Entity&);
            Entity& operator=(const Entity&);
        };
    }
}

#endif /* defined(__TrenchBroom__Entity__) */
