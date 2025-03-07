#include "App.hpp"
#include "Graphics.hpp"
#include "MeshData.hpp"
#include "Input.hpp"
#include "Utilities.hpp"
#include "Camera.hpp"

void MainLoop(App app, std::vector<Mesh3D> meshes) {

    // Locks Mouse Cursor to the Middle of the Screen
    SDL_WarpMouseInWindow(app.mGraphicsApplicationWindow, app.mScreenWidth/2, app.mScreenHeight/2);
    SDL_SetRelativeMouseMode(SDL_TRUE);

    while (!app.mQuit) {
        Input(&app);

        glViewport(0, 0, app.mScreenWidth, app.mScreenHeight);
        glClearColor(0.8f, 0.8f, 0.8f, 1.f);

        // OpenGL technical config:
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
    
        glDisable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glFrontFace(GL_CCW);
    
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
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
    Mesh3D mesh1, mesh2, mesh3;
    
    MeshDataVertexSpecification(&mesh1, MeshTemplates::Cube);
    MeshTraslate(&mesh1, 0.0f, 0.0f, -2.0f);
    MeshScale(&mesh1, 0.5f);

    MeshDataVertexSpecification(&mesh2, MeshTemplates::Cube);
    MeshTraslate(&mesh2, 0.5f, 0.25f, -2.0f);
    MeshScale(&mesh2, 0.3f);

    MeshDataVertexSpecification(&mesh3, MeshTemplates::Tetrahedron);
    MeshTraslate(&mesh3, -0.5f, -0.3f, -2.0f);
    MeshScale(&mesh3, 0.75f);

    // 3. Create Graphics Pipeline
    CreateGraphicsPipeline(&app);
    
    // 3.5 For each mesh, set them to the pipeline
    MeshSetPipeline(&mesh1, app.mGraphicsPipelineShaderProgram);
    MeshSetPipeline(&mesh2, app.mGraphicsPipelineShaderProgram);
    MeshSetPipeline(&mesh3, app.mGraphicsPipelineShaderProgram);

    // 4. Call the main application loop
    MainLoop(app, {mesh1, mesh2, mesh3});

    // 5. Cleanup
    CleanUp(app);

    return 0;
}
