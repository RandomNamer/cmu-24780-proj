//
// Created by Zeyu Zhang on 11/12/24.
//

#include "Human.h"


Human::Human(Difficulty d, float g): difficulty(d), gravity(g) {
    // Initialize human with difficulty and gravity
    // With greater difficulty, greater the initial speed.
}

void Human::update(int frameTimeMs) {
    if(checkCollision()) {
        // Compute new speed;
    }
    updatePosition(frameTimeMs);

    updateSpeed(frameTimeMs);
}

void Human::draw(std::array<float, 2> offset) const {
    drawHumanOnCanvas({x + offset[0], y + offset[1]});
}

void Human::drawHumanOnCanvas(std::array<float, 2> origin) const {
    // Draw human on canvas with decoration
}

void Human::updatePosition(int duration) {
    x += vx * duration;
    y += vy * duration;
}

void Human::updateSpeed(int duration) {
    vy += gravity * duration;
    //Maybe there is friction or air resistance
    vx -= friction * duration;
    vy -= friction * duration;
}

