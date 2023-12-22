#pragma once
#include "solver.hpp"

class Renderer {
    public:
        explicit Renderer(RenderTarget& target) : m_target(target) {}

        void render(const Solver& solver) const {
            float rad = solver.getBoundsRadius();
            Vector2f cent = solver.getBoundsCenter();
            CircleShape border(rad);
            border.setOrigin(rad, rad);
            border.setPosition(cent);
            border.setFillColor(Color(0, 0, 0));
            m_target.draw(border);

            CircleShape circle(1.0);
            circle.setPointCount(32);
            circle.setOrigin(1.0, 1.0);
            const auto& objects = solver.getObjects();
            for(const auto& obj : objects) {
                circle.setPosition(obj.currentPosition);
                circle.setScale(obj.radius, obj.radius);
                circle.setFillColor(obj.color);
                m_target.draw(circle);
            }

            const auto& links = solver.getLinks();
            bool ini = 1;
            for(const auto &link : links) {
                auto object1 = objects[link.idx1], object2 = objects[link.idx2];
                VertexArray line(sf::Lines, 2);
                line[0].position = object1.currentPosition;
                line[1].position = object2.currentPosition;
                if(ini)
                    // cout << "(" << object1.currentPosition.x << " " << object1.currentPosition.x << ") (" << object2.currentPosition.x << " " << object2.currentPosition.y << ")\n";
                ini = 0;
                line[0].color = object1.color;
                line[1].color = object2.color;
                m_target.draw(line);
            }
        }

    private:
        RenderTarget& m_target;
};