//
// Created by Zeyu Zhang on 11/12/24.
//

#include<iostream>
#include "Human.h"
#include "fssimplewindow.h"


void Human::update(int frameTimeMs) {
    //Assume new speed is already been set
    updatePosition(static_cast<float>(frameTimeMs) / 1000.f);

    updateSpeed(static_cast<float>(frameTimeMs) / 1000.f);
}

void Human::draw(std::array<float, 2> offset) const {
    drawHumanOnCanvas({x + offset[0], y + offset[1]});
}

void Human::drawHumanOnCanvas(std::array<float, 2> origin) const {
    std::cout<<origin[0]<<" "<<origin[1]<<std::endl;
    renderContext.drawCircleFilled({origin[0], origin[1]}, drawSpecs.headSize, BLACK);
    auto [_, footPosition] = getDetailedPosition();
    renderContext.drawLine({origin[0], origin[1]}, {footPosition[0], footPosition[1]}, drawSpecs.bodyWidth,BLACK);
}

void Human::updatePosition(float duration) {
    x += vx * duration;
    y += vy * duration;
}

void Human::updateSpeed(float duration) {
    vy += gravity * duration;
    //Maybe there is friction or air resistance
    vx -= friction * duration;
    vy -= friction * duration;
}

Human::Human(Human::Config config): difficulty(config.difficulty), gravity(config.gravity), friction(config.friction), renderContext({0, 0}, config.drawSpecs.scale) {
    x = config.initialPosition[0];
    y = config.initialPosition[1];
    vx = config.initialSpeed[0];
    vy = config.initialSpeed[1];
    angle =  -0.5 * M_PI + config.initialAngle;
    drawSpecs = config.drawSpecs;
}

Human::Position Human::getDetailedPosition() const {
    // Calculate the position of body parts
    std::array<float,2> headPosition = {x, y};
    std::array<float,2> footPosition = {x - drawSpecs.bodyLength * cos(angle), y - drawSpecs.bodyLength * sin(angle)};
    return {headPosition, footPosition};
}

void Human::input(int key) {
    switch (key) {
        case FSKEY_LEFT:
            angle += 0.04;
            break;
        case FSKEY_RIGHT:
            angle -= 0.04;
            break;
        default:
            break;
    }
}

