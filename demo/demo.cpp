#include <vector>
#include <random>
#include <cmath>
#include <iostream>
#include "fssimplewindow.h"
#include "ysglfontdata.h"
#include "yssimplesound.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

//================ Color Definitions ================
struct Color3i
{
    int r, g, b;
};
enum difficultyLevel
{
    easy,
    medium,
    hard
};

const static Color3i BLACK = {0, 0, 0};
const static Color3i WHITE = {255, 255, 255};
const static Color3i RED = {255, 0, 0};
const static Color3i BLUE = {0, 0, 255};
const static Color3i YELLOW = {255, 255, 0};
const static Color3i GREEN = {0, 255, 0};

class Sound
{
private:
    YsSoundPlayer player;
    YsSoundPlayer::SoundData bgSoundSelect;
    YsSoundPlayer::SoundData bgSoundPlay;
    YsSoundPlayer::SoundData bounceHead;
    YsSoundPlayer::SoundData bounceFoot;
    YsSoundPlayer::SoundData end;
    YsSoundPlayer::SoundData switchColor;

    bool isBgSound1Playing = false;

public:
    Sound()
    {
        player.MakeCurrent();
        player.Start();
        FsChangeToProgramDir();
        if (YSOK != bgSoundSelect.LoadWav("bgSelect.wav"))
        {
            std::cerr << "Failed to load background sound 1." << std::endl;
        }
        if (YSOK != bgSoundPlay.LoadWav("bgPlay.wav"))
        {
            std::cerr << "Failed to load background sound 2." << std::endl;
        }
        if (YSOK != bounceHead.LoadWav("bounceHead.wav"))
        {
            std::cerr << "Failed to load sound 2." << std::endl;
        }
        if (YSOK != bounceFoot.LoadWav("bounceFoot.wav"))
        {
            std::cerr << "Failed to load sound 3." << std::endl;
        }
        if (YSOK != end.LoadWav("end.wav"))
        {
            std::cerr << "Failed to load sound 4." << std::endl;
        }
        if (YSOK != switchColor.LoadWav("switch.wav"))
        {
            std::cerr << "Failed to load sound 5." << std::endl;
        }
        
    }

    ~Sound()
    {
        player.End();
    }

    void PlayBgSoundSelect()
    {
        if (!isBgSound1Playing)
        {
            player.PlayBackground(bgSoundSelect);
            isBgSound1Playing = true;
        }
    }

    void StopBgSoundSelect()
    {
        if (isBgSound1Playing)
        {
            player.Stop(bgSoundSelect);
            isBgSound1Playing = false;
        }
    }
    void PlayBgSoundPlay()
    {
        if (!isBgSound1Playing)
        {
            player.PlayBackground(bgSoundPlay);
            isBgSound1Playing = true;
        }
    }
    void StopBgSoundPlay()
    {
        if (isBgSound1Playing)
        {
            player.Stop(bgSoundPlay);
            isBgSound1Playing = false;
        }
    }
    void PlaybounceHead()
    {
        player.PlayOneShot(bounceHead);
    }
    void PlaybounceFoot()
    {
        player.PlayOneShot(bounceFoot);
    }
    void PlaySoundEnd()
    {
        player.PlayOneShot(end);
    }
    void PlaySoundSwitch()
    {
        player.PlayOneShot(switchColor);
    }
};

//================ Render Context ================
class RenderContext
{
public:
    RenderContext(const std::array<float, 2> &origin, float scale, bool flipY = false) : scale(scale), flipY(flipY)
    {
        this->origin = {origin[0] / scale, origin[1] / scale};
    }

    void drawLine(const std::array<float, 2> &p1, const std::array<float, 2> &p2,
                  float lineWidth = 1.f, Color3i color = BLACK) const
    {
        glColor3ub(color.r, color.g, color.b);
        glLineWidth(lineWidth);
        glBegin(GL_LINES);
        auto p1t = coordinateTransform(p1);
        auto p2t = coordinateTransform(p2);
        glVertex2f(p1t[0], p1t[1]);
        glVertex2f(p2t[0], p2t[1]);
        glEnd();
    }

    void drawCircleFilled(const std::array<float, 2> &center, float radius,
                          Color3i color = BLACK) const
    {
        glColor3ub(color.r, color.g, color.b);
        glBegin(GL_TRIANGLE_FAN);
        auto p = coordinateTransform(center);
        glVertex2f(p[0], p[1]);
        for (int i = 0; i <= 100; ++i)
        {
            float angle = 2 * M_PI * i / 100;
            float x = center[0] + radius * cos(angle);
            float y = center[1] + radius * sin(angle);
            auto pt = coordinateTransform({x, y});
            glVertex2f(pt[0], pt[1]);
        }
        glEnd();
    }

private:
    std::array<float, 2> origin;
    float scale = 1.0f;
    bool flipY = false;

    std::array<float, 2> coordinateTransform(const std::array<float, 2> &p) const
    {
        return {(p[0] + origin[0]) * scale,
                (origin[1] + (flipY ? -p[1] : p[1])) * scale};
    }
};

//================ Human Class ================
class Human
{
public:
    struct DrawSpecs
    {
        float scale;
        float headSize;
        float bodyLength;
        float bodyWidth;
    };

    constexpr static const DrawSpecs DEFAULT = {1.f, 4, 20, 2};

    struct Config
    {
        int difficulty;
        float gravity;
        float initialAngle;
        std::array<float, 2> initialSpeed;
        std::array<float, 2> initialPosition;
        DrawSpecs drawSpecs;
    };

    Human(Config config) : difficulty(config.difficulty),
                           gravity(config.gravity),
                           renderContext({0, 0}, config.drawSpecs.scale)
    {
        x = config.initialPosition[0];
        y = config.initialPosition[1];
        vx = config.initialSpeed[0];
        vy = config.initialSpeed[1];
        angle = -0.5 * M_PI + config.initialAngle;
        drawSpecs = config.drawSpecs;
    }

    void update(int frameTimeMs)
    {
        updatePosition(static_cast<float>(frameTimeMs) / 1000.f);
        updateSpeed(static_cast<float>(frameTimeMs) / 1000.f);
    }

    void draw(std::array<float, 2> offset) const
    {
        std::array<float, 2> origin = {x + offset[0], y + offset[1]};
        renderContext.drawCircleFilled(origin, drawSpecs.headSize, color);

        std::array<float, 2> foot = {
            origin[0] - drawSpecs.bodyLength * cos(angle),
            origin[1] - drawSpecs.bodyLength * sin(angle)};

        renderContext.drawLine(origin, foot, drawSpecs.bodyWidth, color);
    }

    void input(int key)
    {
        switch (key)
        {
        case FSKEY_LEFT:
            angle += 0.3;
            break;
        case FSKEY_RIGHT:
            angle -= 0.3;
            break;
        default:
            break;
        }
    }

    // Getters and setters
    void setVx(float newVx) { vx = newVx; }
    void setVy(float newVy) { vy = newVy; }
    void setColor(Color3i c) { color = c; }
    float getVx() const { return vx; }
    float getVy() const { return vy; }
    float get_pos_x_head() const { return x; }
    float get_pos_y_head() const { return y; }
    float get_angle() const { return angle; }
    float get_pos_x_foot() const
    {
        return x - drawSpecs.bodyLength * cos(angle);
    }
    float get_pos_y_foot() const
    {
        return y - drawSpecs.bodyLength * sin(angle);
    }
    Color3i get_color() const { return color; }

private:
    float vx, vy;
    float x, y;
    float gravity;
    float angle = 0.f;
    Color3i color = BLACK;
    int difficulty;
    DrawSpecs drawSpecs;
    const RenderContext renderContext;

    struct Position
    {
        std::array<float, 2> head;
        std::array<float, 2> foot;
    };

    void updatePosition(float duration)
    {
        x += vx * duration;
        y += vy * duration;
    }

    void updateSpeed(float duration)
    {
        vy += gravity * duration;
    }
};
//================ Terrain Class ================
class Terrain
{
public:
    enum CollisionType
    {
        none,
        horizon_top,
        horizon_bottom,
        platform_left,
        platform_right,
        left_vertical,
        right_vertical
    };
    struct Platform
    {
        int x, y, width, thickness;
        bool is_moving;
        int velocity;
    };

    struct Door
    {
        int x, y, width, height;
        Color3i color;
        Platform *attached_platform;
    };

    struct Tri
    {
        int x, y;
        bool visible;
        Color3i color;
        int size;
        Platform *attached_platform;

        Tri(int x, int y, int size, Color3i color, Platform* platform)
        : x(x), y(y), visible(true), color(color), size(size), attached_platform(platform)
        {
        }
    };


    Terrain(int length, int top_width, int bottom_width, int thickness,
            int center_x, int difficulty_level, Sound& sound) : trapezoid_length(length),
                                                  trapezoid_top_width(top_width),
                                                  trapezoid_bottom_width(bottom_width),
                                                  edge_thickness(thickness),
                                                  screen_center_x(center_x),
                                                  difficulty(difficulty_level),
                                                  sound(sound)
    {
        initialize_platforms(15, 10);
        initializeTri();
    }

    CollisionType check_collision_head(const Human &human) const
    {
        CollisionType result = check_helper(human.get_pos_x_head(), human.get_pos_y_head(),
                                            human.getVx(), human.getVy());
        return result;
    }

    bool check_collision_foot(const Human &human) const
    {
        return (bool)check_helper(human.get_pos_x_foot(), human.get_pos_y_foot(),
                                  human.getVx(), human.getVy());
    }

    bool check_collision_tri(Human &human)
    {
        float humanX = human.get_pos_x_head();
        float humanY = human.get_pos_y_head();

        for (auto &tri : tris)
        {
            if (tri.visible &&
                humanX >= tri.x - tri.size / 2 &&
                humanX <= tri.x + tri.size / 2 &&
                humanY >= tri.y &&
                humanY <= tri.y + tri.size)
            {
                tri.visible = false;
                human.setColor(tri.color);
                sound.PlaySoundSwitch();
                return true;
            }
        }

        return false;
    }

    bool check_goal(const Human &human) const
    {
        float human_x = human.get_pos_x_head();
        float human_y = human.get_pos_y_head();

        return (human_x >= door_position.x &&
                human_x <= door_position.x + door_position.width &&
                human_y >= door_position.y &&
                human_y <= door_position.y + door_position.height);
    }
    
    bool check_goal_color(const Human &human) const{
        Color3i human_color = human.get_color();
        return door_position.color.r == human_color.r &&
        door_position.color.g == human_color.g &&
        door_position.color.b == human_color.b;
    }

    void draw(float y)
    {
        viewport_offset = y;
        drawTrapezoid(y);
        updateAndDrawPlatforms(y);
        updateAndDrawDoor(y);
        updateAndDrawTris(y);
        drawArrowAtTop();
    }
    
    void setViewportOffset(float offset)
    {
        viewport_offset = offset;
    }

private:
    int trapezoid_length;
    int trapezoid_top_width;
    int trapezoid_bottom_width;
    int edge_thickness;
    int screen_center_x;
    std::vector<Platform> platforms;
    Door door_position;
    int difficulty;
    float viewport_offset = 0.0f;
    Sound& sound;
    std::vector<Tri> tris;

    void initialize_platforms(int num_platforms, int platform_thickness)
    {
        platforms.clear();

        int previous_y = 0;
        for (int i = 0; i < num_platforms; ++i)
        {
            if (previous_y + 120 > trapezoid_length - 120)
                break;

            int y = random_range(previous_y + 120,
                                 std::min(previous_y + 170, trapezoid_length - 120));
            previous_y = y;

            double t = double(y) / trapezoid_length;
            int current_width = trapezoid_top_width +
                                t * (trapezoid_bottom_width - trapezoid_top_width);

            int width = random_range(current_width / 4, current_width / 2);
            int x = calculatePlatformX(i, current_width, width);

            bool is_moving = (difficulty == hard && random_range(0, 9) < 6);
            platforms.push_back({x, y, width, platform_thickness, is_moving,
                                 is_moving ? 1 : 0});
        }

        if (!platforms.empty())
        {
            auto &first_platform = platforms[0];
            const Color3i colors[] = {RED, BLUE, GREEN};
            const Color3i color = colors[random_range(0, 2)];
            door_position = {
                first_platform.x + first_platform.width / 2 - 10,
                first_platform.y - 30, 20, 30, color, &first_platform};
        }
    }

    void initializeTri()
    {
        tris.clear();

        for (auto &platform : platforms)
        {
            if (random_range(0, 5) < 5)
            {
                int numTris = random_range(1, 2);
                for (int i = 0; i < numTris; ++i)
                {
                    int triSize = 30;
                    int triX = random_range(platform.x, platform.x + platform.width - triSize);
                    int triY = platform.y - triSize;

                    const Color3i colors[] = {RED, BLUE, GREEN};
                    const Color3i color = colors[random_range(0, 2)];

                    tris.emplace_back(triX, triY, triSize, color, &platform);
                }
            }
        }
    }

    int calculatePlatformX(int index, int current_width, int platform_width)
    {
        if (index % 3 == 0)
        {
            return random_range(0, 1) == 0 ? screen_center_x - current_width / 2 + edge_thickness : screen_center_x + current_width / 2 - edge_thickness - platform_width;
        }
        else
        {
            int left_bound = screen_center_x - current_width / 2 + edge_thickness;
            int right_bound = screen_center_x + current_width / 2 - edge_thickness - platform_width;
            return random_range(left_bound, right_bound);
        }
    }

    static int random_range(int min, int max)
    {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        std::uniform_int_distribution<> distr(min, max);
        return distr(gen);
    }

    void drawTrapezoid(int y)
    {
        glColor3ub(0, 0, 0);
        int top_y = y;
        int bottom_y = y + trapezoid_length;
        int left_top_x = screen_center_x - trapezoid_top_width / 2;
        int left_bottom_x = screen_center_x - trapezoid_bottom_width / 2;
        int right_top_x = screen_center_x + trapezoid_top_width / 2;
        int right_bottom_x = screen_center_x + trapezoid_bottom_width / 2;

        // Draw left edge
        for (int current_y = top_y; current_y <= bottom_y; ++current_y)
        {
            double t = double(current_y - top_y) / (bottom_y - top_y);
            int current_left_x = left_top_x + t * (left_bottom_x - left_top_x);
            draw_filled_rectangle(current_left_x, current_y,
                                  current_left_x + edge_thickness, current_y + 1);
        }

        // Draw right edge
        for (int current_y = top_y; current_y <= bottom_y; ++current_y)
        {
            double t = double(current_y - top_y) / (bottom_y - top_y);
            int current_right_x = right_top_x + t * (right_bottom_x - right_top_x);
            draw_filled_rectangle(current_right_x - edge_thickness, current_y,
                                  current_right_x, current_y + 1);
        }

        // Draw top and bottom edges
        draw_filled_rectangle(left_top_x + edge_thickness, top_y,
                              right_top_x - edge_thickness, top_y + edge_thickness);
        draw_filled_rectangle(left_bottom_x + edge_thickness - 5, bottom_y - edge_thickness,
                              right_bottom_x - edge_thickness + 5, bottom_y);
    }

    void updateAndDrawPlatforms(int y)
    {
        glColor3ub(0, 0, 0); // Set color to black
        for (auto &platform : platforms)
        {
            if (platform.is_moving)
            {
                updatePlatformPosition(platform);
            }
            draw_filled_rectangle(platform.x, platform.y + y,
                                  platform.x + platform.width,
                                  platform.y + y + platform.thickness);
        }
    }

    void updatePlatformPosition(Platform &platform)
    {
        platform.x += platform.velocity;
        double t = double(platform.y) / trapezoid_length;
        int current_width = trapezoid_top_width +
                            t * (trapezoid_bottom_width - trapezoid_top_width);
        int left_bound = screen_center_x - current_width / 2 + edge_thickness;
        int right_bound = screen_center_x + current_width / 2 - edge_thickness - platform.width;

        if (platform.x <= left_bound || platform.x >= right_bound)
        {
            platform.velocity = -platform.velocity;
        }
    }

    void updateAndDrawDoor(int y)
    {
        glColor3ub(door_position.color.r, door_position.color.g, door_position.color.b); // Set color to blue for the door
        if (door_position.attached_platform)
        {
            door_position.x = door_position.attached_platform->x +
                              door_position.attached_platform->width / 2 -
                              door_position.width / 2;
            door_position.y = door_position.attached_platform->y - 30;
        }
        draw_filled_rectangle(door_position.x, door_position.y + y,
                              door_position.x + door_position.width,
                              door_position.y + door_position.height + y);
    }

    void updateAndDrawTris(float y)
    {
        for (auto &tri : tris)
        {
            if (tri.visible)
            {
                tri.x = tri.attached_platform->x +
                        tri.attached_platform->width / 2 -
                        tri.size / 2;
                glColor3ub(tri.color.r, tri.color.g, tri.color.b);
                glBegin(GL_TRIANGLES);
                glVertex2i(tri.x, tri.y + y);
                glVertex2i(tri.x - tri.size / 2, tri.y + y + tri.size);
                glVertex2i(tri.x + tri.size / 2, tri.y + y + tri.size);
                glEnd();
            }
        }
    }

    static void draw_filled_rectangle(int x1, int y1, int x2, int y2)
    {
        glBegin(GL_QUADS);
        glVertex2i(x1, y1);
        glVertex2i(x2, y1);
        glVertex2i(x2, y2);
        glVertex2i(x1, y2);
        glEnd();
    }
    
    void drawArrowAtTop() const
    {
        int arrowWidth = 20;
        int arrowHeight = 30;

        int arrowX = door_position.x + door_position.width / 2;
        const Color3i &arrowColor = door_position.color;

        glColor3ub(arrowColor.r, arrowColor.g, arrowColor.b);

        glBegin(GL_TRIANGLES);
        glVertex2i(arrowX, 0);
        glVertex2i(arrowX - arrowWidth / 2, arrowHeight);
        glVertex2i(arrowX + arrowWidth / 2, arrowHeight);
        glEnd();

        int rectWidth = 10;
        int rectHeight = 20;
        glBegin(GL_QUADS);
        glVertex2i(arrowX - rectWidth / 2, arrowHeight);
        glVertex2i(arrowX + rectWidth / 2, arrowHeight);
        glVertex2i(arrowX + rectWidth / 2, arrowHeight + rectHeight);
        glVertex2i(arrowX - rectWidth / 2, arrowHeight + rectHeight);
        glEnd();
    }

    CollisionType check_helper(float current_x, float current_y, float vx, float vy) const
    {
        float predicted_x = current_x + vx * 0.01;
        float predicted_y = current_y + vy * 0.01;

        float relative_y = predicted_y;
        double t = double(relative_y) / trapezoid_length;

        if (predicted_y < 10 || predicted_y > 990)
        {
            return horizon_top;
        }

        if (t < 0 || t > 1)
            return none;

        int current_width = trapezoid_top_width + t * (trapezoid_bottom_width - trapezoid_top_width);
        int left_x = screen_center_x - current_width / 2;
        int right_x = screen_center_x + current_width / 2;

        if (predicted_x < left_x + edge_thickness ||
            predicted_x > right_x - edge_thickness)
        {
            return predicted_x < left_x ? left_vertical : right_vertical;
        }

        for (const auto &platform : platforms)
        {
            float platform_screen_y = platform.y;

            if (predicted_y >= platform_screen_y &&
                predicted_y <= platform_screen_y + platform.thickness &&
                predicted_x >= platform.x &&
                predicted_x <= platform.x + platform.width)
            {

                float dist_to_top = std::abs(current_y - platform_screen_y);
                float dist_to_bottom = std::abs(current_y - (platform_screen_y + platform.thickness));
                float dist_to_left = std::abs(current_x - platform.x);
                float dist_to_right = std::abs(current_x - (platform.x + platform.width));

                float min_dist = std::min({dist_to_top, dist_to_bottom,
                                           dist_to_left, dist_to_right});

                if (min_dist == dist_to_top && vy > 0)
                {
                    return horizon_top;
                }
                if (min_dist == dist_to_bottom && vy < 0)
                {
                    return horizon_bottom;
                }
                if (min_dist == dist_to_left && vx > 0)
                {
                    return platform_left;
                }
                if (min_dist == dist_to_right && vx < 0)
                {
                    return platform_right;
                }

                if (std::abs(vy) > std::abs(vx))
                {
                    return vy > 0 ? horizon_top : horizon_bottom;
                }
                else
                {
                    return vx > 0 ? platform_left : platform_right;
                }
            }
        }

        return none;
    }
};
//================ Scoring Class ================
class Scoring
{
public:
    Scoring() : score(200)
    {
    }

    int get_score() { return score; }

    void handle_foot_collision()
    {
        deduct_score(3);
    }

    void handle_head_collision()
    {
        deduct_score(10);
    }
    
    void handle_same_color_goal()
    {
        add_score(50);
    }

    void display_score() const
    {
        glColor3ub(255, 0, 0); // Set color to red for score
        glRasterPos2i(10, 20); // Position the text
        char scoreText[32];
        sprintf(scoreText, "Score: %d", score);
        YsGlDrawFontBitmap8x12(scoreText);
    }

    void draw_result(int windowWidth, int windowHeight) const
    {
        glColor3ub(255, 215, 0);

        glRasterPos2i(windowWidth / 2 - 50, windowHeight / 2 - 40);
        YsGlDrawFontBitmap16x24("Goal!");

        char finalScoreText[64];
        sprintf(finalScoreText, "Final Score: %d", score);
        glRasterPos2i(windowWidth / 2 - 80, windowHeight / 2 + 10);
        YsGlDrawFontBitmap16x24(finalScoreText);

        const char *evaluation;
        if (score >= 150)
        {
            evaluation = "Perfect!";
        }
        else if (score >= 100)
        {
            evaluation = "Great!";
        }
        else if (score >= 50)
        {
            evaluation = "Good!";
        }
        else
        {
            evaluation = "Try Again!";
        }

        glRasterPos2i(windowWidth / 2 - 60, windowHeight / 2 + 50);
        YsGlDrawFontBitmap16x24(evaluation);
    }

private:
    int score;

    void add_score(int points)
    {
        score += points;
    }

    void deduct_score(int points)
    {
        score -= points;
    }
};
//================ Bouncing Class ================
class Bouncing
{
public:
    Bouncing(Scoring &score, Sound &sound) : score(score), sound(sound)
    {
    }
    void bounce(Human &human, const Terrain &terrain)
    {
        float vx = human.getVx();
        float vy = human.getVy();

        Terrain::CollisionType result = terrain.check_collision_head(human);
        if (result)
        {
            sound.PlaybounceHead();
            if (result == Terrain::horizon_top || result == Terrain::horizon_bottom)
            {
                vx = vx;
                vy = -vy;
            }
            else if (result == Terrain::platform_left || result == Terrain::platform_right)
            {
                vx = -vx;
                vy = vy;
            }
            else
            {
                calculateReflection(vx, vy, 200, 1000, result == Terrain::left_vertical);
            }
            score.handle_head_collision();
        }
        else if (terrain.check_collision_foot(human))
        {
            sound.PlaybounceFoot();
            vx = 500 * cos(human.get_angle());
            vy = 500 * sin(human.get_angle());
            score.handle_foot_collision();
        }

        human.setVx(vx);
        human.setVy(vy);
    }

private:
    Scoring &score;
    Sound &sound;
    void calculateReflection(float &vx, float &vy, float dx, float dy, bool isLeft)
    {
        float length = std::sqrt(dx * dx + dy * dy);
        float nx = -dy / length;
        float ny = dx / length;

        float dotProduct = vx * nx + vy * ny;

        if (abs(vx) < 80)
        {
            vx = vx > 0 ? -80 : 80;
        }
        else
        {
            vx = -vx;
        }
        vy = vy - 2 * dotProduct * ny;
    }
};

class Difficulty
{
public:
    difficultyLevel Difflevel;

    void drawSelectionMenu() const
    {
        glColor3ub(0, 0, 255);

        glRasterPos2d(400, 250);
        YsGlDrawFontBitmap16x24("Select Difficulty Level");

        glRasterPos2d(400, 300);
        YsGlDrawFontBitmap12x16("Press 1: Easy");

        glRasterPos2d(400, 330);
        YsGlDrawFontBitmap12x16("Press 2: Medium");

        glRasterPos2d(400, 360);
        YsGlDrawFontBitmap12x16("Press 3: Hard");
    }

    void setDifficulty(int Inputlevel)
    {
        switch (Inputlevel)
        {
        case 1:
            Difflevel = easy;
            break;
        case 2:
            Difflevel = medium;
            break;
        case 3:
            Difflevel = hard;
            break;
        }
    }

    int get_difficulty() const
    {
        return Difflevel;
    }

    void draw()
    {
        const char *text = nullptr;
        switch (Difflevel)
        {
        case easy:
            text = "easy";
            break;
        case medium:
            text = "medium";
            break;
        case hard:
            text = "hard";
            break;
        }

        glColor3ub(0, 0, 255);
        glRasterPos2d(10, 40);
        YsGlDrawFontBitmap8x12("Difficulty: ");

        glRasterPos2d(160, 40);
        YsGlDrawFontBitmap8x12(text);
    }
};

int main()
{
    const int WINDOW_WIDTH = 1000;
    const int WINDOW_HEIGHT = 600;
    const float CENTER_Y = WINDOW_HEIGHT / 2.0f;

    FsOpenWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 1);

    bool gameRunning = true;
    Sound sound;
    while (gameRunning)
    {
        Difficulty difficulty;
        bool difficultySelected = false;
        sound.PlayBgSoundSelect();
        while (!difficultySelected && FSKEY_ESC != FsInkey())
        {
            FsPollDevice();
            int key = FsInkey();

            if (key == FSKEY_1 || key == FSKEY_2 || key == FSKEY_3)
            {
                difficulty.setDifficulty(key - FSKEY_0);
                difficultySelected = true;
            }

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            difficulty.drawSelectionMenu();
            FsSwapBuffers();
            FsSleep(10);
        }
        sound.StopBgSoundSelect();
        if (!difficultySelected)
        {
            gameRunning = false;
            break;
        }

        float gravity;
        switch (difficulty.get_difficulty())
        {
        case easy:
            gravity = 400.0f;
            break;
        case medium:
            gravity = 500.0f;
            break;
        case hard:
            gravity = 600.0f;
            break;
        default:
            gravity = 400.0f;
        }

        Human human({difficulty.get_difficulty(),
                     gravity,
                     0.0f,
                     {0.0f, -500.0f},
                     {500.0f, 970.0f},
                     Human::DEFAULT});

        Terrain terrain(1000, 800, 400, 10, 500, difficulty.get_difficulty(), sound);
        Scoring scoring;
        Bouncing bouncing(scoring, sound);

        bool levelComplete = false;
        sound.PlayBgSoundPlay();
        FsPassedTime();
        while (!levelComplete && FSKEY_ESC != FsInkey())
        {
            FsPollDevice();

            int key = FsInkey();
            if (key != FSKEY_NULL)
            {
                human.input(key);
            }

            int frameTime = FsPassedTime();

            human.update(frameTime / 2);

            float viewOffsetY = CENTER_Y - human.get_pos_y_head();
            terrain.setViewportOffset(viewOffsetY);

            if (terrain.check_goal(human))
            {
                if(terrain.check_goal_color(human))
                {
                    scoring.handle_same_color_goal();
                }
                sound.PlaySoundEnd();
                bool waitingForKey = true;
                while (waitingForKey && FSKEY_ESC != FsInkey())
                {
                    FsPollDevice();
                    if (FsInkey() != FSKEY_NULL)
                    {
                        waitingForKey = false;
                    }

                    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                    terrain.draw(viewOffsetY);
                    human.draw({0, viewOffsetY});
                    scoring.draw_result(WINDOW_WIDTH, WINDOW_HEIGHT);
                    FsSwapBuffers();
                    FsSleep(10);
                }
                levelComplete = true;
                continue;
            }

            bouncing.bounce(human, terrain);
            terrain.check_collision_tri(human);

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            terrain.draw(viewOffsetY);
            human.draw({0, viewOffsetY});
            scoring.display_score();
            difficulty.draw();

            FsSwapBuffers();
            FsSleep(10);
        }
        sound.StopBgSoundPlay();

        if (FSKEY_ESC == FsInkey())
        {
            gameRunning = false;
        }
    }

    return 0;
}

