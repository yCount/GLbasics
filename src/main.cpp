#include "App.hpp"
#include "Graphics.hpp"
#include "Input.hpp"
#include "Utilities.hpp"
#include "Camera.hpp"

void MainLoop(App app, std::vector<Mesh3D> meshes) {

    // Locks Mouse Cursor to the Middle of the Screen
    SDL_WarpMouseInWindow(app.mGraphicsApplicationWindow, app.mScreenWidth/2, app.mScreenHeight/2);
    SDL_SetRelativeMouseMode(SDL_TRUE);

    while (!app.mQuit) {
        Input(&app);

        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);

        glViewport(0, 0, app.mScreenWidth, app.mScreenHeight);
        glClearColor(1.f, 1.f, 0.f, 1.f);

        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

        // Draw Meshes
        for (Mesh3D& mesh : meshes) {
            MeshDraw(&mesh, app);
        }

        // Update Screen
        SDL_GL_SwapWindow(app.mGraphicsApplicationWindow);
    }
}


int main() {
    App app;

    // 1. Initialize the graphics program
    InitializeProgram(&app);

    // Setup camera
    app.mCamera.SetProjectionMatrix(glm::radians(45.0f), 
                                    (float)app.mScreenWidth / app.mScreenHeight, 
                                    0.1f, 10.0f);

    // 2. Setup meshes (geometry)
    Mesh3D mesh1, mesh2;
    MeshDataVertexSpecification(&mesh1);
    MeshTraslate(&mesh1, 0.0f, 0.0f, -2.0f);
    MeshScale(&mesh1, 0.5f);

    MeshDataVertexSpecification(&mesh2);
    MeshTraslate(&mesh2, 0.25f, 0.25f, -2.0f);
    MeshScale(&mesh2, 0.5f);

    // 3. Create Graphics Pipeline
    CreateGraphicsPipeline(&app);
    
    // 3.5 For each mesh, set them to the pipeline
    MeshSetPipeline(&mesh1, app.mGraphicsPipelineShaderProgram);
    MeshSetPipeline(&mesh2, app.mGraphicsPipelineShaderProgram);

    // 4. Call the main application loop
    MainLoop(app, {mesh1, mesh2});

    // 5. Cleanup
    CleanUp(app);

    return 0;
}
