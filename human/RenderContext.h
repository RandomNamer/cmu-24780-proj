//
// Created by Zeyu Zhang on 11/19/24.
//

#include "fssimplewindow.h"
#include <vector>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif


#define CIRCLE_SAMPLES 100

struct Point2i  {
    int x, y;
};

struct Point2f {
    float x, y;
};


struct Color3i {
    int r, g, b;
};

const static Color3i BLACK = {0, 0, 0};
const static Color3i WHITE = {255, 255, 255};
const static Color3i RED = {255, 0, 0};
const static Color3i GREEN = {0, 255, 0};
const static Color3i BLUE = {0, 0, 255};
const static Color3i YELLOW = {255, 255, 0};
const static Color3i CYAN = {0, 255, 255};
const static Color3i MAGENTA = {255, 0, 255};


class RenderContext {
public:
    RenderContext(const Point2i& origin, float scale, bool flipY = false): scale(scale), flipY(flipY) {
        this->origin = {origin.x / scale, origin.y / scale};
    }

    void drawLine(const Point2f& p1, const Point2f& p2, float lineWidth = 1.f, Color3i color = BLACK) const {
        glColor3ub(color.r, color.g, color.b);
        glLineWidth(lineWidth);
        glBegin(GL_LINES);
        auto p1t = coordinateTransform(p1);
        auto p2t = coordinateTransform(p2);
        glVertex2f(p1t.x, p1t.y);
        glVertex2f(p2t.x, p2t.y);
        glEnd();
    }

    void drawCircle(const Point2f& center, float radius, float lineWidth = 1.f, Color3i color = BLACK) const {
        glColor3ub(color.r, color.g, color.b);
        glLineWidth(lineWidth);
        glBegin(GL_LINE_LOOP);
        for (int i = 0; i < CIRCLE_SAMPLES; ++i) {
            float angle = 2 * M_PI * i / CIRCLE_SAMPLES;
            float x = center.x + radius * cos(angle);
            float y = center.y + radius * sin(angle);
            auto p = coordinateTransform({x, y});
            glVertex2f(p.x, p.y);
        }
        glEnd();
    }

    void drawCircleFilled(const Point2f& center, float radius, Color3i color = BLACK) const {
        glColor3ub(color.r, color.g, color.b);
        glBegin(GL_TRIANGLE_FAN);
        auto p = coordinateTransform(center);
        glVertex2f(p.x, p.y);
        for (int i = 0; i <= CIRCLE_SAMPLES; ++i) {
            float angle = 2 * M_PI * i / CIRCLE_SAMPLES;
            float x = center.x + radius * cos(angle);
            float y = center.y + radius * sin(angle);
            auto p = coordinateTransform({x, y});
            glVertex2f(p.x, p.y);
        }
        glEnd();
    }

    void drawLine(const Point2f& p1, float radian, float length, float lineWidth = 1.f, Color3i color = BLACK) const {
        Point2f p2 = {p1.x + length * cos(radian), p1.y + length * sin(radian)};
        drawLine(p1, p2, lineWidth, color);
    }

    void drawLineStrip(const std::vector<Point2f>& points, float lineWidth = 1.f, Color3i color = BLACK) const {
        glColor3ub(color.r, color.g, color.b);
        glLineWidth(lineWidth);
        glBegin(GL_LINE_STRIP);
        for (const auto& p : points) {
            auto pt = coordinateTransform(p);
            glVertex2f(pt.x, pt.y);
        }
        glEnd();
    }

private:
    Point2f origin;
    float scale = 1.0f;
    bool flipY = false;

    Point2f coordinateTransform(const Point2f& p) const {
        return {(p.x + origin.x) * scale, (origin.y + (flipY ? -p.y : p.y)) * scale};
    }
};
