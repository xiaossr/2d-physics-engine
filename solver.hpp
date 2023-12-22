#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <iostream>
using namespace std;
using namespace sf;

Vector2f operator*(float a, Vector2f pos) {
    return Vector2f(pos.x * a, pos.y * a);
}

Vector2f operator^(Vector2f pos, int p) {
    return Vector2f(pow(pos.x, p), pow(pos.y, p));
}

struct VerletObject {
    Vector2f currentPosition = Vector2f(0, 0), previousPosition = Vector2f(0, 0), acceleration = Vector2f(0, 0);
    float radius = 10.0;
    Color color = Color(255, 255, 255);
    bool stationary = 0;

    VerletObject() {}
    VerletObject(Vector2f position, float r, Color c = Color::White, bool fix = 0) : currentPosition(position), previousPosition(position), radius(r), color(c), stationary(fix) {}

    void update(float dt) {
        if(stationary) return;
        Vector2f nextPosition = 2 * currentPosition - previousPosition + acceleration * dt * dt;
        previousPosition = currentPosition;
        currentPosition = nextPosition;
        acceleration = {0, 0};
    }

    void accelerate(Vector2f add_accel) {
        acceleration += add_accel;
    }
};


class Solver {
    public:
        Solver() {};

        struct Link {
            int idx1, idx2;
            float link_length = 60.0;
            
            Link(int a, int b, float d = 60.0) {
                idx1 = a, idx2 = b;
                link_length = d;
            }
        };

        VerletObject& addObject(Vector2f position, float radius, Color c = Color::White, bool fix = 0) {
            ++m_num_objects;
            return m_objects.emplace_back(VerletObject(position, radius, c, fix));
        }

        Link& addLink(int idx1, int idx2, float dist = -1.0) {
            ++m_num_links;
            if(dist == -1.0) {
                Vector2f vdist =  m_objects[idx1].currentPosition - m_objects[idx2].currentPosition;
                dist = sqrt(vdist.x * vdist.x + vdist.y * vdist.y);
            }
            return m_links.emplace_back(Link(idx1, idx2, dist));
        }

        void update() {
            float step_dt = m_dt / m_substeps;
            for(int i = 0; i < m_substeps; i++) {
                applyGravity();
                resolveCollisions();
                updateLinks();
                applyBounds();
                updateObjects(step_dt);
            }
        }

        void setUpdateRate(float rate) {
            m_dt = 1.0 / (float) rate;
            // cout << m_dt << "\n";
        }

        void setBounds(Vector2f center, float radius) {
            m_bound_center = center;
            m_bound_radius = radius;
        }

        void setGravity(Vector2f newgrav) {
            m_gravity = newgrav;
        }

        [[nodiscard]]
        const vector<VerletObject>& getObjects() const {
            return m_objects;
        }

        [[nodiscard]]
        const Vector2f& getBoundsCenter() const {
            return m_bound_center;
        }

        [[nodiscard]]
        const float& getBoundsRadius() const {
            return m_bound_radius;
        }

        [[nodiscard]]
        const int& getObjectCount() const {
            return m_num_objects;
        }

        [[nodiscard]]
        const int& getLinkCount() const {
            return m_num_links;
        }

        [[nodiscard]]
        const vector<Link>& getLinks() const {
            return m_links;
        }

    private:
        int m_substeps = 8;
        float m_dt = 0.0;
        
        Vector2f m_bound_center = Vector2f(500.0, 500.0);
        float m_bound_radius = 300.0;

        Vector2f m_gravity = Vector2f(0.0, 10.0);

        int m_num_objects = 0, m_num_links = 0;
        vector<VerletObject> m_objects;
        vector<Link> m_links;

        void applyGravity() {
            for(auto &obj : m_objects)
                obj.accelerate(m_gravity);//, cout << obj.acceleration.x << " " << obj.acceleration.y << "\n";
        }

        void applyBounds() {
            for(auto &obj : m_objects) {
                Vector2f dist =  m_bound_center - obj.currentPosition;
                float absdist = sqrt(dist.x * dist.x + dist.y * dist.y);
                if(absdist > m_bound_radius - obj.radius) {
                    auto rat = dist / absdist;
                    // cout << "bounds applied\n";
                    obj.currentPosition = m_bound_center - rat * (m_bound_radius - obj.radius);
                }
            }
        }

        void updateObjects(float dt) {
            for(auto &obj : m_objects) 
                obj.update(dt);//, cout << obj.currentPosition.x << " " << obj.currentPosition.y << "\n";;
        }

        void resolveCollisions() {
            for(int i = 0; i < m_num_objects; i++) {
                for(int j = i + 1; j < m_num_objects; j++) {
                    Vector2f dist = m_objects[i].currentPosition - m_objects[j].currentPosition;
                    float absdist = sqrt(dist.x * dist.x + dist.y * dist.y);
                    if(absdist < m_objects[i].radius + m_objects[j].radius) {
                        auto rat = dist / absdist;
                        float overlap = m_objects[i].radius + m_objects[j].radius - absdist;
                        if(!m_objects[i].stationary&& !m_objects[j].stationary) {
                            m_objects[i].currentPosition += (0.5 * m_objects[i].radius / (m_objects[i].radius + m_objects[j].radius)) * overlap * rat;
                            m_objects[j].currentPosition -= (0.5 * m_objects[j].radius / (m_objects[i].radius + m_objects[j].radius)) * overlap * rat;
                        } else if(m_objects[i].stationary) {
                            m_objects[j].currentPosition -= overlap * rat;
                        } else if(m_objects[j].stationary) {
                            m_objects[i].currentPosition += overlap * rat;
                        }
                    }
                }
            }
        }

        void updateLink(Link link) {
            auto& object1 = m_objects[link.idx1], &object2 = m_objects[link.idx2];
            Vector2f dist =  object1.currentPosition - object2.currentPosition;
            float absdist = sqrt(dist.x * dist.x + dist.y * dist.y);
            auto rat = dist / absdist;
            // cout << "link applied\n";
            float dif = link.link_length - absdist;
            // cout << absdist << " " << dif << "\n";
            if(!object1.stationary && !object2.stationary) {
                object1.currentPosition += (0.5 * dif) * rat;
                object2.currentPosition -= (0.5 * dif) * rat;
            } 
            else if(object1.stationary) {
                object2.currentPosition = object1.currentPosition - rat * (link.link_length - object2.radius);
            } else {
                object1.currentPosition = object2.currentPosition - link.link_length * rat;
            }
        }

        void updateLinks() {
            for(int i = 0; i < m_num_links; i++) {
                updateLink(m_links[i]);
            }
        }
};