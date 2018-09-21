// Local headers
#include "program.hpp"
#include "gloom/gloom.hpp"
#include "gloom/shader.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <cmath>
#include <glm/gtx/rotate_vector.hpp>

using namespace std;

void runProgram(GLFWwindow* window)
{
    // Enable depth (Z) buffer (accept "closest" fragment)
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Configure miscellaneous OpenGL settings
    glEnable(GL_CULL_FACE);

    // Set default colour after clearing the colour buffer
    glClearColor(0.3f, 0.1f, 0.8f, 1.0f);

    // Set up your scene here (create Vertex Array Objects, etc.)
    Gloom::Shader shader;
    shader.makeBasicShader("/home/anders/gloom/gloom/shaders/simple.vert","/home/anders/gloom/gloom/shaders/simple.frag");
    shader.activate();

    float PI = atan(1) *4;

    //Initialize vertices, indices and colors in a circular pattern with one shared vertex in the center. Color is determined by sine of angle.
    const int n = 20;
    const int nindices = 3*n;
    const int nvertices = 6*n+3;
    const int ncolors = 8*n+4;
    float z = -3.0f;

    float* vertices = new float[nvertices];
    float* colors = new float[ncolors];
    int* indices = new int[nindices];

    vertices[0] = 0.0f;
    vertices[1] = 0.0f;
    vertices[2] = z+0.3f;

    colors[0] = 0.0f;
    colors[1] = 1.0f;
    colors[2] = 0.0f;
    colors[3] = 1.0f;

    for (int i=0; i<n; i++){
        vertices[6*i+3] = sin(i*2*PI/n);
        vertices[6*i+4] = cos(i*2*PI/n);
        vertices[6*i+5] = z;
        vertices[6*i+6] = sin((2*i+1)*PI/n);
        vertices[6*i+7] = cos((2*i+1)*PI/n);
        vertices[6*i+8] = z;

        indices[3*i] = 0;
        indices[3*i+1] = 2*i+2;
        indices[3*i+2] = 2*i+1;

        colors[8*i+4] = abs(cos(i*2*PI/n));
        colors[8*i+5] = 0.0f;
        colors[8*i+6] = abs(sin(i*2*PI/n));
        colors[8*i+7] = 1.0f;
        colors[8*i+8] = abs(cos((i*2+i)*PI/n));
        colors[8*i+9] = 0.0f;
        colors[8*i+10] = abs(sin((2*i+1)*PI/n));
        colors[8*i+11] = 1.0f;
    }

/*  Initialize one single triangle
    const int nindices = 3;
    const int nvertices = 3*3;
    const int ncolors = 3*4;

    float z = -3.0f;

    float vertices[nvertices] = {-0.6f, 0.6f, z, 0.0f, 0.6f, z, 0.6f, -0.6f, z};
    int indices[nindices] = {0,2,1};
    float colors[ncolors] = {1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,1.0f, 0.0f, 0.0f, 1.0f};
*/

    unsigned int vaoid = CreateTriangleVAO(vertices,indices,nvertices,nindices, colors, ncolors);

    //Projection matrix and final matrix sent to shader
    glm::mat4x4 projection = glm::perspective(PI/4, 1.0f, 1.0f, 100.0f);
    glm::mat4x4 MVP;
    //Identity matrix for easy reference
    const glm::mat4x4 I = glm::mat4x4(1);

    //Camera translation and rotation matrices
    glm::mat4x4 translate;
    glm::mat4x4 rotate;
    //Rotation position and speed of camera
    float rotX = 0.0f;
    float rotY = 0.0f;
    glm::vec3 pos = glm::vec3(0.0f,0.0f,0.0f);
    float speed = 0.03f;

    // Rendering Loop
    while (!glfwWindowShouldClose(window))
    {
        //Do translation and rotation of camera
       translate = glm::translate(I,pos); 
       rotate = glm::rotate(I,rotY, glm::vec3(0.0f,1.0f,0.0f))*glm::rotate(I,rotX, glm::vec3(1.0f,0.0f,0.0f));

        //Calculate final matrix and send to shader. Camera matrixes are inversed to bring world into clipbox/projection.
        MVP = projection*glm::inverse(translate*rotate*I);
        glUniformMatrix4fv(3,1,GL_FALSE, &MVP[0][0]);

        // Clear colour and depth buffers
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Draw your scene here
        glDrawElements(GL_TRIANGLES, nindices,GL_UNSIGNED_INT, 0);

        // Handle other events
        glfwPollEvents();
        handleKeyboardInput(window, pos, rotX, rotY, speed);

        // Flip buffers
        glfwSwapBuffers(window);
    }
    shader.deactivate();
    shader.destroy();
}

//Creates VAO
unsigned int CreateTriangleVAO(float* vertices, int* indices, int nvertices, int nindices, float* colors, int ncolors){
    unsigned int a = 0;
    glGenVertexArrays(1,&a);
    glBindVertexArray(a);

    unsigned int b = 0;
    glGenBuffers(1,&b);
    glBindBuffer(GL_ARRAY_BUFFER,b);
    glBufferData(GL_ARRAY_BUFFER,sizeof(float)*nvertices, vertices,GL_STATIC_DRAW);

    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,0);
    glEnableVertexAttribArray(0);

    unsigned int cb = 0;
    glGenBuffers(1,&cb);
    glBindBuffer(GL_ARRAY_BUFFER, cb);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*ncolors, colors,GL_STATIC_DRAW);

    glVertexAttribPointer(1,4,GL_FLOAT,GL_FALSE,0,0);
    glEnableVertexAttribArray(1);

    unsigned int ib = 0;
    glGenBuffers(1,&ib);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,ib);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(int)*nindices,indices,GL_STATIC_DRAW);

    return a;
}


void handleKeyboardInput(GLFWwindow* window, glm::vec3 &pos, float &rotX, float &rotY, float speed)
{
    glm::vec3 dir;

    // Use escape key for terminating the GLFW window
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
    else if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
        dir = speed*glm::vec3(0.0f, 0.0f, 1.0f);
    }
    else if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
        dir = speed*glm::vec3(0.0f, 0.0f, -1.0f);
    }
    else if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
        dir = speed*glm::vec3(-1.0f, 0.0f, 0.0f);
    }
    else if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
        dir = speed*glm::vec3(1.0f, 0.0f, 0.0f);
    }
    else if(glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS){
        dir = speed*glm::vec3(0.0f, 1.0f, 0.0f);
    }
    else if(glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS){
        dir = speed*glm::vec3(0.0f, -1.0f, 0.0f);
    }
    else if(glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS){
        rotY -= speed;
    }
    else if(glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS){
        rotY += speed;
    }
    else if(glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS){
        rotX += speed;
    }
    else if(glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS){
        rotX -= speed;
    }
//Updates position vector 
pos += glm::rotateX(glm::rotateY(dir,rotY),rotX);
}
