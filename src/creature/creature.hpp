#ifndef CREATURE_HPP
#define CREATURE_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <random>

#include "../libs/stb_image.h"

#include "../shader/shader.hpp"

#define MAX_MOVE_TICKS 500

const static int vertices[] = {
        -1, -1,
        -1, 1,
        1, -1,
        1, 1,
};

class Creature {
public:
    explicit Creature(const std::string &folderPath) {
        int channels;
        stbi_set_flip_vertically_on_load(true);
        unsigned char *data = stbi_load((folderPath + "/shime1.png").c_str(), &_width, &_height, &channels, 0);
        if (!data) {
            std::cout << "Failed to load texture" << stbi_failure_reason() << std::endl;
            stbi_image_free(data);
            throw std::exception();
        }

        // Check for image format before importing it
        unsigned int format;
        switch (channels) {
            case 1:
                format = GL_RED;
                break;
            case 2:
                format = GL_RG;
                break;
            case 3:
                format = GL_RGB;
                break;
            case 4:
            default:
                format = GL_RGBA;
                break;
        }

        // Create the window
        _window = glfwCreateWindow(_width, _height, "Dorimeji", nullptr, nullptr);
        if (_window == nullptr) {
            std::cout << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
            throw std::exception();
        }
        glfwMakeContextCurrent(_window);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            std::cout << "Failed to initialize GLAD" << std::endl;
            throw std::exception();
        }

        glViewport(0, 0, _width, _height);
        glfwSetWindowPos(_window, _x, _y);

        // (https://learnopengl.com/Getting-started/Textures)
        glGenTextures(1, &_texture);
        glBindTexture(GL_TEXTURE_2D, _texture);

        // Wrapping (We use border clamping with a transparent border) (Not really necessary at all)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        const static float borderColor[] = {0.0f, 0.0f, 0.0f, 0.0f};
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
        // Filtering
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexImage2D(GL_TEXTURE_2D, 0, (int)format, _width, _height, 0, format, GL_UNSIGNED_BYTE, data);
        // glGenerateMipmap(GL_TEXTURE_2D); //Not really necessary for this use case (I believe)
        stbi_image_free(data); // Free the image data

        glGenVertexArrays(1, &_VAO);
        glGenBuffers(1, &_VBO);

        glBindVertexArray(_VAO);

        glBindBuffer(GL_ARRAY_BUFFER, _VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        // Vertex positions
        glVertexAttribPointer(0, 2, GL_INT, GL_FALSE, 2 * sizeof(float), (void*)nullptr);
        glEnableVertexAttribArray(0);

        // Enable transparency for the background (optional)
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Create and setup shader
        _shader = Shader("../assets/shaders/vertex.glsl", "../assets/shaders/fragment.glsl");
        _shader.setInt("texture1", 0);
    }
    ~Creature() {
        glfwDestroyWindow(_window);

        glDeleteVertexArrays(1, &_VAO);
        glDeleteBuffers(1, &_VBO);
        glDeleteTextures(1, &_texture);
    }

    void render() {
        while(!glfwWindowShouldClose(_window)) {
            // Process inputs
            if(glfwGetKey(_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
                glfwSetWindowShouldClose(_window, true);

            applyGravity();
            if (_walking) {
                moveTo(_moveToX, 5);
            } else if (!_falling) {
                if (_ticks - _lastMoveTick >= _nextMoveTicks) {
                    int width;
                    glfwGetMonitorWorkarea(_getCurrentMonitor(), nullptr, nullptr, &width, nullptr);

                    std::random_device rd;
                    std::mt19937 mt(rd());
                    std::uniform_real_distribution<float> distMove(0.0f, (float)(width - _width));
                    _moveToX = (int)distMove(mt);
                    moveTo(_moveToX, 5);

                    std::uniform_real_distribution<float> distTicks(0.0f, (float)MAX_MOVE_TICKS);
                    _nextMoveTicks = (unsigned int)distTicks(mt);
                    _lastMoveTick = _ticks;
                }
            }

            // Actually rendering
            glClearColor(0.0f, 0.0f, 0.0f, 0.2f);
            glClear(GL_COLOR_BUFFER_BIT);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, _texture);

            _shader.use();
            glBindVertexArray(_VAO);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
            glBindVertexArray(0);

            glfwSwapBuffers(_window);
            glfwPollEvents();

            _ticks++;
        }
    }

    void applyGravity() {
        int ypos, height;
        glfwGetMonitorWorkarea(_getCurrentMonitor(), nullptr, &ypos, nullptr, &height);
        if (_y >= (height - _height + ypos - 1)) {
            _y = height - _height + ypos - 1;
            if (_falling) _falling = false;
            return;
        }
        _falling = true;
        _y += 15;
        glfwSetWindowPos(_window, _x, _y);
    }

    void moveTo(int x, int speed) {
        if (_falling) return;
        _walking = true;
        int xpos, width;
        glfwGetMonitorWorkarea(_getCurrentMonitor(), &xpos, nullptr, &width, nullptr);
        if ((_x >= (x - speed + xpos) && _x <= (x + speed + xpos)) ||
            _x >= (width - _width + xpos) || _x <= xpos) {
            _walking = false;
            return;
        }
        if (x + xpos < _x) speed = -speed;
        glfwSetWindowPos(_window, _x += speed, _y);
    }

private:
    GLFWwindow* _window;
    Shader _shader;
    int _width = 0, _height = 0, _x = 2300, _y = 100, _moveToX = 100;
    unsigned int _VBO = 0, _VAO = 0, _texture = 0;
    unsigned int _ticks = 0, _lastMoveTick = 0, _nextMoveTicks = 50;
    bool _falling = false, _walking = true;

    // Modified from https://github.com/glfw/glfw/issues/1699
    GLFWmonitor *_getCurrentMonitor() const {
        int monitors_size = 0;
        GLFWmonitor** monitors = glfwGetMonitors(&monitors_size);

        GLFWmonitor* closest_monitor = nullptr;
        int max_overlap_area = 0;

        for (int i = 0; i < monitors_size; i++) {
            int mpx, mpy;
            glfwGetMonitorPos(monitors[i], &mpx, &mpy);
            const GLFWvidmode* mvm = glfwGetVideoMode(monitors[i]);

            if ((mpx > (_x + _width)) || (_x > (mpx + mvm->width)) || (mpy > (_y + _height)) || (_y > (mpy + mvm->height))) continue;
            int ir[4] = {0};

            // x, width
            if (_x < mpx) {
                ir[0] = mpx;

                if ((_x + _width) < (mpx + mvm->width)) ir[2] = (_x + _width) - ir[0];
                else ir[2] = mvm->width;
            } else {
                ir[0] = _x;

                if ((mpx + mvm->width) < (_x + _width)) ir[2] = (mpx + mvm->width) - ir[0];
                else ir[2] = _width;
            }

            // y, height
            if (_y < mpy) {
                ir[1] = mpy;

                if ((_y + _height) < (mpy + mvm->height)) ir[3] = (_y + _height) - ir[1];
                else ir[3] = mvm->height;
            } else {
                ir[1] = _y;

                if ((mpy + mvm->height) < (_y + _height)) ir[3] = (mpy + mvm->height) - ir[1];
                else ir[3] = _height;
            }

            int overlap_area = ir[2] * ir[3];
            if (overlap_area > max_overlap_area) {
                closest_monitor = monitors[i];
                max_overlap_area = overlap_area;
            }
        }

        if (closest_monitor) return closest_monitor;
        return nullptr;
    }
};


#endif