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

#include "Camera.h"

#include <cassert>

namespace TrenchBroom {
    namespace Renderer {
        Camera::Viewport::Viewport() :
        x(0),
        y(0),
        width(0),
        height(0) {}
        
        Camera::Viewport::Viewport(const int i_x, const int i_y, const int i_width, const int i_height) :
        x(i_x),
        y(i_y),
        width(i_width),
        height(i_height) {}

        bool Camera::Viewport::operator== (const Viewport& other) const {
            return x == other.x && y == other.y && width == other.width && height == other.height;
        }

        const float Camera::DefaultPointDistance = 256.0f;
        
        Camera::~Camera() {}
        
        float Camera::nearPlane() const {
            return m_nearPlane;
        }
        
        float Camera::farPlane() const {
            return m_farPlane;
        }
        
        const Camera::Viewport& Camera::viewport() const {
            return m_viewport;
        }

        const Vec3f& Camera::position() const {
            return m_position;
        }
        
        const Vec3f& Camera::direction() const {
            return m_direction;
        }
        
        const Vec3f& Camera::up() const {
            return m_up;
        }
        
        const Vec3f& Camera::right() const {
            return m_right;
        }
        
        const Mat4x4f& Camera::projectionMatrix() const {
            if (!m_valid)
                validateMatrices();
            return m_projectionMatrix;
        }
        
        const Mat4x4f& Camera::viewMatrix() const {
            if (!m_valid)
                validateMatrices();
            return m_viewMatrix;
        }

        const Mat4x4f Camera::orthogonalBillboardMatrix() const {
            Vec3f bbLook, bbUp, bbRight;
            bbLook = -m_direction;
            bbUp = m_up;
            bbRight = crossed(bbUp, bbLook);
            
            return Mat4x4f(bbRight.x(),   bbUp.x(),   bbLook.x(), 0.0f,
                           bbRight.y(),   bbUp.y(),   bbLook.y(), 0.0f,
                           bbRight.z(),   bbUp.z(),   bbLook.z(), 0.0f,
                           0.0f,          0.0f,       0.0f,       1.0f);
        }
        
        const Mat4x4f Camera::verticalBillboardMatrix() const {
            Vec3f bbLook, bbUp, bbRight;
            bbLook = -m_direction;
            bbLook[2] = 0.0f;
            if (bbLook.null()) {
                bbLook = -m_up;
                bbLook[2] = 0.0f;
            }
            bbLook.normalize();
            bbUp = Vec3f::PosZ;
            bbRight = crossed(bbUp, bbLook);
            
            return Mat4x4f(bbRight.x(),   bbUp.x(),   bbLook.x(), 0.0f,
                           bbRight.y(),   bbUp.y(),   bbLook.y(), 0.0f,
                           bbRight.z(),   bbUp.z(),   bbLook.z(), 0.0f,
                           0.0f,          0.0f,       0.0f,       1.0f);
        }

        void Camera::frustumPlanes(Plane3f& top, Plane3f& right, Plane3f& bottom, Plane3f& left) const {
            doComputeFrustumPlanes(top, right, bottom, left);
        }

        Ray3f Camera::viewRay() const {
            return Ray3f(m_position, m_direction);
        }

        Ray3f Camera::pickRay(const int x, const int y) const {
            return doGetPickRay(x, y);
        }

        float Camera::distanceTo(const Vec3f& point) const {
            return (point - m_position).length();
        }
        
        float Camera::squaredDistanceTo(const Vec3f& point) const {
            return (point - m_position).squaredLength();
        }

        Vec3f Camera::defaultPoint() const {
            return m_position + DefaultPointDistance * direction();
        }
        
        Vec3f Camera::defaultPoint(const int x, const int y) const {
            const Ray3f ray = pickRay(x, y);
            return defaultPoint(ray);
        }

        Vec3f Camera::defaultPoint(const Ray3f& ray) const {
            return ray.pointAtDistance(DefaultPointDistance);
        }

        Vec3f Camera::project(const Vec3f& point) const {
            if (!m_valid)
                validateMatrices();
            
            Vec3f win = m_matrix * point;
            win[0] = m_viewport.x + m_viewport.width *(win.x() + 1.0f)/2.0f;
            win[1] = m_viewport.y + m_viewport.height*(win.y() + 1.0f)/2.0f;
            win[2] = (win.z() + 1.0f)/2.0f;
            return win;
        }

        Vec3f Camera::unproject(const float x, const float y, const float depth) const {
            if (!m_valid)
                validateMatrices();
            
            Vec3f normalized;
            normalized[0] = 2.0f*(x - m_viewport.x)/m_viewport.width - 1.0f;
            normalized[1] = 2.0f*(m_viewport.height - y - m_viewport.y)/m_viewport.height - 1.0f;
            normalized[2] = 2.0f*depth - 1.0f;
            
            return m_invertedMatrix * normalized;
        }
        
        void Camera::setNearPlane(const float nearPlane) {
            assert(nearPlane < m_farPlane);
            if (nearPlane == m_nearPlane)
                return;
            m_nearPlane = nearPlane;
            m_valid = false;
            cameraDidChangeNotifier(this);
        }
        
        void Camera::setFarPlane(const float farPlane) {
            assert(farPlane > m_nearPlane);
            if (farPlane == m_farPlane)
                return;
            m_farPlane = farPlane;
            m_valid = false;
            cameraDidChangeNotifier(this);
        }
        
        void Camera::setViewport(const Viewport& viewport) {
            if (viewport == m_viewport)
                return;
            m_viewport = viewport;
            m_valid = false;
        }

        void Camera::moveTo(const Vec3f& position) {
            if (position == m_position)
                return;
            m_position = position;
            m_valid = false;
            cameraDidChangeNotifier(this);
        }
        
        void Camera::moveBy(const Vec3f& delta) {
            if (delta.null())
                return;
            m_position += delta;
            m_valid = false;
            cameraDidChangeNotifier(this);
        }
        
        void Camera::lookAt(const Vec3f& point, const Vec3f& up) {
            setDirection((point - m_position).normalized(), up);
        }
        
        void Camera::setDirection(const Vec3f& direction, const Vec3f& up) {
            if (direction == m_direction && up == m_up)
                return;
            m_direction = direction;
            m_right = crossed(m_direction, up).normalized();
            m_up = crossed(m_right, m_direction);
            m_valid = false;
            cameraDidChangeNotifier(this);
        }
        
        void Camera::rotate(const float yaw, const float pitch) {
            if (yaw == 0.0f && pitch == 0.0f)
                return;
            
            const Quatf rotation = Quatf(Vec3f::PosZ, yaw) * Quatf(m_right, pitch);
            Vec3f newDirection = rotation * m_direction;
            Vec3f newUp = rotation * m_up;
            
            if (newUp[2] < 0.0f) {
                newUp[2] = 0.0f;
                newDirection[0] = 0.0f;
                newDirection[1] = 0.0f;
                
                newUp.normalize();
                newDirection.normalize();
            }
            
            setDirection(newDirection, newUp);
        }
        
        void Camera::orbit(const Vec3f& center, const float horizontal, const float vertical) {
            if (horizontal == 0.0f && vertical == 0.0f)
                return;
            
            Quatf rotation = Quatf(Vec3f::PosZ, horizontal) * Quatf(m_right, vertical);
            Vec3f newDirection = rotation * m_direction;
            Vec3f newUp = rotation * m_up;
            Vec3f offset = m_position - center;
            
            if (newUp[2] < 0.0f) {
                newUp[2] = 0.0f;
                newDirection[0] = 0.0f;
                newDirection[1] = 0.0f;
                
                newUp.normalize();
                newDirection.normalize();
                
                // correct rounding errors
                const float cos = (std::max)(-1.0f, (std::min)(1.0f, m_direction.dot(newDirection)));
                const float angle = acosf(cos);
                if (!Math::zero(angle)) {
                    const Vec3f axis = crossed(m_direction, newDirection).normalized();
                    rotation = Quatf(axis, angle);
                    offset = rotation * offset;
                    newUp = rotation * newUp;
                }
            } else {
                offset = rotation * offset;
            }
            
            setDirection(newDirection, newUp);
            moveTo(offset + center);
        }

        void Camera::renderFrustum(RenderContext& renderContext, Vbo& vbo, const float size, const Color& color) const {
            doRenderFrustum(renderContext, vbo, size, color);
        }

        float Camera::pickFrustum(const float size, const Ray3f& ray) const {
            return doPickFrustum(size, ray);
        }

        Camera::Camera() :
        m_nearPlane(1.0f),
        m_farPlane(8000.0f),
        m_viewport(Viewport(0, 0, 1024, 768)),
        m_position(Vec3f::Null),
        m_valid(false) {
            setDirection(Vec3f::PosX, Vec3f::PosZ);
        }
        
        Camera::Camera(const float nearPlane, const float farPlane, const Viewport& viewport, const Vec3f& position, const Vec3f& direction, const Vec3f& up) :
        m_nearPlane(nearPlane),
        m_farPlane(farPlane),
        m_viewport(viewport),
        m_position(position),
        m_valid(false) {
            assert(m_nearPlane >= 0.0f);
            assert(m_farPlane > m_nearPlane);
            assert(Math::eq(direction.length(), 1.0f));
            assert(Math::eq(up.length(), 1.0f));
            setDirection(direction, up);
        }
        
        void Camera::validateMatrices() const {
            doValidateMatrices(m_projectionMatrix, m_viewMatrix);
            m_matrix = m_projectionMatrix * m_viewMatrix;
            
            bool invertible = false;
            m_invertedMatrix = invertedMatrix(m_matrix, invertible);
            assert(invertible);
            m_valid = true;
        }
    }
}