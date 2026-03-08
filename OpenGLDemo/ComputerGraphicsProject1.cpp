#include <iostream>
#include <SDL3/SDL.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Camera.h"
#include "Shader.h"
#include "Model.h"

int main() {
    //───────────────────────────────────────────────────────────Initialize sdl and verify errors ───────────────────────────────────────────────────────────
    //SDL, GLAD and OpenGL initialization
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD)) {
        std::cout << "SDL_Init failed: " << SDL_GetError() << std::endl;
        return -1;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    //───────────────────────────────────────────────────────────Create Widnow───────────────────────────────────────────────────────────
    //Window Size
    int width = 1500, height = 800;
    SDL_Window* window = SDL_CreateWindow("OpenGL", width, height, SDL_WINDOW_OPENGL);
    if (window == nullptr) {
        std::cout << "Failed to create SDL Window: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return -1;
    }

    //For camera mouse
    SDL_SetWindowRelativeMouseMode(window, true);
    SDL_CaptureMouse(true);


    //───────────────────────────────────────────────────────────Initialize OpenGl and Glad and verify errors───────────────────────────────────────────────────────────
    SDL_GLContext context = SDL_GL_CreateContext(window);
    if (!context) {
        std::cout << "Failed to create OpenGL context: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        SDL_GL_DestroyContext(context);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -2;
    }

    glEnable(GL_DEPTH_TEST);

    //───────────────────────────────────────────────────────────Load Models and shaders───────────────────────────────────────────────────────────

    //Load files
    Model shrek, test;

    //The skin texture seems to have an issue other models tested did not have any issues
    shrek.LoadModel("Shrek.fbx");
    test.LoadModel("AvatarZZZ.fbx");

    //Cube Model
    //models.LoadModel("uvCube.fbx");


    //Creating shaders
    Shader shader("shaders/vertex_shader.glsl", "shaders/fragment_shader.glsl");

    //Setup matrices
    glm::mat4 model(1.0f);

    //Rotation test
    //shrek.setRotation(90,glm::vec3(0.0, 0.0, 1.0));


    Camera cameraRef;

    //───────────────────────────────────────────────────────────Loop───────────────────────────────────────────────────────────

    float previousTime = 0;

    SDL_Event windowEvent;
    bool running = true;
    while (running) {
        //Timer for movement
        Uint64 ticks = SDL_GetTicks();
        float time = ticks / 1000.0f;
        float deltaTime = time - previousTime;

        while (SDL_PollEvent(&windowEvent)) {
            if (windowEvent.type == SDL_EVENT_QUIT) {
                running = false;
            }
            cameraRef.processInput(windowEvent, deltaTime);
        }
        cameraRef.processKeyboard(deltaTime);

        //Clear window
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //Use shader
        shader.use();

        //Update matrices
        glm::mat4 view = cameraRef.getViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(cameraRef.getFOV()), (float)width / (float)height, 0.1f, 1000.0f );

        shader.setMat4("model", model);
        shader.setMat4("view", view);
        shader.setMat4("projection", projection);


        //───────────────────────────────────────────────────────────Draw Models───────────────────────────────────────────────────────────
        test.draw(shader);
        shrek.draw(shader);


        SDL_GL_SwapWindow(window);
        previousTime = time;
    }

    //Cleanup
    glDeleteProgram(shader.getID());
    SDL_GL_DestroyContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}