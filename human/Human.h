//
// Created by Zeyu Zhang on 11/12/24.
//

#ifndef INC_24780_PROJECT_HUMAN_H
#define INC_24780_PROJECT_HUMAN_H

#include <array>
#include <vector>
#include "RenderContext.h"

typedef int Difficulty;

struct DrawContext {
    std::array<float, 2> offset;
};
class Human;



class Human {
public:

    struct Position {
        std::array<float, 2> head;
        std::array<float, 2> foot;
    };

    struct DrawSpecs {
        float scale;
        float headSize;
        float bodyLength;
        float bodyWidth;
    };

    constexpr static const DrawSpecs DEFAULT = {1.f, 4, 20, 2};

    struct Config {
        Difficulty difficulty;
        float gravity;
        float friction;
        float initialAngle;
        std::array<float, 2> initialSpeed;
        std::array<float, 2> initialPosition;
        Human::DrawSpecs drawSpecs;
    };

    Human(Config config);

    void setVx(float vx) {
        this->vx = vx;
    }

    void setVy(float vy) {
        this->vy = vy;
    }

    std::array<float, 2> getVelocity() const {
        return {vx, vy};
    }

    // Return the position of the center
    std::array<float, 2> getPosition() const {
        return {x, y};
    }

    // Position of body parts
    Position getDetailedPosition() const;

    void input(int key);

    void update(int frameTimeMs);

    void draw(std::array<float, 2> offset) const;

private:
    float vx, vy;
    float x, y;
    float gravity;
    float friction = 0.f;
    float angle = 0.f;
    Difficulty difficulty;
    DrawSpecs drawSpecs;
    const RenderContext renderContext;

    void drawHumanOnCanvas(std::array<float, 2> origin) const;

    void updatePosition(float duration);

    void updateSpeed(float duration);
};


#endif //INC_24780_PROJECT_HUMAN_H
