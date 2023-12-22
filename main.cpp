#define SFML_STATIC

#pragma once
#include <bits/stdc++.h>
#include <SFML/Graphics.hpp>

#include "solver.hpp"
#include "renderer.hpp"

using namespace std;
using namespace sf;

int main() {
    constexpr int windowWidth = 1000, windowHeight = 1000, frameRate = 60;

    sf::ContextSettings settings;
    settings.antialiasingLevel = 1;
    RenderWindow window(VideoMode(windowWidth, windowHeight), "2D Physics Simulator", Style::Default, settings);
    window.setFramerateLimit(frameRate);
    
    Solver solver;
    Renderer renderer(window);

    solver.setUpdateRate(frameRate);
    solver.setGravity(Vector2f(0, 300.0));
    solver.setBounds(Vector2f(500.0, 500.0), 2000.0);

    for(int i = 0; i < 15; i++) {
        solver.addObject(Vector2f(500.0 + i * 30.0, 500.0 ), 10.0, Color::White, !i);
        if(i) solver.addLink(i - 1, i);
    }
    // solver.addObject(Vector2f(680.0, 700.0), 20.0, Color::Red, 1);

    Clock clock;
    while(window.isOpen()) {
        Event e;
        while(window.pollEvent(e)) {
            if (e.type == Event::Closed)
                window.close();
        }

        Time elapsed = clock.getElapsedTime();
        /*if(elapsed.asMilliseconds() / 500 >= solver.getObjectCount() && solver.getObjectCount() < 100) {
            solver.addObject(Vector2f(700, 400), 10.0);
            if(solver.getObjectCount() >= 2) {
                solver.addLink(solver.getObjectCount() - 1, solver.getObjectCount() - 2, 60.0);
                // cout << "linked " << solver.getObjectCount() - 1 << solver.getObjectCount() - 2 <<"\n";
            }
            // cout << solver.getLinkCount() << "\n";
        } */

        // if(elapsed.asMilliseconds() >= 500) 
            solver.update(), clock.restart();
        window.clear(Color(150,150,150));
        renderer.render(solver);
        window.display();
    }
}