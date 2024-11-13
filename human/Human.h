//
// Created by Zeyu Zhang on 11/12/24.
//

#ifndef INC_24780_PROJECT_HUMAN_H
#define INC_24780_PROJECT_HUMAN_H

#include <array>
#include <vector>

typedef int Difficulty;

struct DrawContext {
    std::array<float, 2> offset;
};

class Human {
public:
    Human(Difficulty d, float g);

    void setVx(float vx) {
        this->vx = vx;
    }

    void setVy(float vy) {
        this->vy = vy;
    }

    std::array<float, 2> getVelocity() const {
        return {vx, vy};
    }

    std::array<float, 2> getPosition() const {
        return {x, y};
    }

    void update(int frameTimeMs);

    void draw(std::array<float, 2> offset) const;

private:
    float vx, vy;
    float x, y;
    float gravity;
    float friction = 0.f;
    Difficulty difficulty;

    void drawHumanOnCanvas(std::array<float, 2> origin) const;

    void updatePosition(int duration);

    void updateSpeed(int duration);
};


#endif //INC_24780_PROJECT_HUMAN_H
