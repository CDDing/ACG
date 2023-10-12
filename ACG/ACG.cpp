#define _USE_MATH_DEFINES
#define STB_IMAGE_IMPLEMENTATION
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <GL/glew.h>
#include <GL/glut.h>
#include <stb_image.h>
GLuint Buffers[5];
GLuint ProgramID,currentProgram;
GLuint programID, programID2, programID3, programID4,programID5;
GLuint texture;
int width, height;
using namespace std;
int tessellation = 10;
int mouse_prev_x = 0, mouse_prev_y = 0;
float mouse_dx[] = { 0.0f, 0.0f,0.0f, };
float mouse_dy[] = { 0.0f, 0.0f,0.0f, };
int cnt = 0;
unsigned char* image;
GLuint LoadShaders(const char* vertex_file_path, const char* fragment_file_path)
{
    //create the shaders
    GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
    GLint Result = GL_FALSE;
    int InfoLogLength;

    //Read the vertex shader code from the file
    string VertexShaderCode;
    ifstream VertexShaderStream(vertex_file_path, ios::in);
    if (VertexShaderStream.is_open())
    {
        string Line = "";
        while (getline(VertexShaderStream, Line))
            VertexShaderCode += "\n" + Line;
        VertexShaderStream.close();
    }

    //Compile Vertex Shader
    printf("Compiling shader : %s\n", vertex_file_path);
    char const* VertexSourcePointer = VertexShaderCode.c_str();
    glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
    glCompileShader(VertexShaderID);

    //Check Vertex Shader
    glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength != 0) {
        vector<char> VertexShaderErrorMessage(InfoLogLength);
        glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
        fprintf(stdout, "%s\n", &VertexShaderErrorMessage[0]);
    }
    //Read the fragment shader code from the file
    string FragmentShaderCode;
    ifstream FragmentShaderStream(fragment_file_path, ios::in);
    if (FragmentShaderStream.is_open())
    {
        string Line = "";
        while (getline(FragmentShaderStream, Line))
            FragmentShaderCode += "\n" + Line;
        FragmentShaderStream.close();
    }

    //Compile Fragment Shader
    printf("Compiling shader : %s\n", fragment_file_path);
    char const* FragmentSourcePointer = FragmentShaderCode.c_str();
    glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
    glCompileShader(FragmentShaderID);

    //Check Fragment Shader
    glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength != 0) {
        vector<char> FragmentShaderErrorMessage(InfoLogLength);
        glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
        fprintf(stdout, "%s\n", &FragmentShaderErrorMessage[0]);
    }




    //Link the program
    fprintf(stdout, "Linking program\n");
    ProgramID = glCreateProgram();
    glAttachShader(ProgramID, VertexShaderID);
    glAttachShader(ProgramID, FragmentShaderID);
    glLinkProgram(ProgramID);

    float vertices[] = {
        // positions          // colors           // texture coords
         1.0f,  1.0f, 0.0f,      1.0f, 1.0f, // top right
         1.0f, -1.0f, 0.0f,     1.0f, 0.0f, // bottom right
        -1.0f, -1.0f, 0.0f,     0.0f, 0.0f, // bottom left
        -1.0f,  1.0f, 0.0f,     0.0f, 1.0f  // top left 
    };
    unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };
    unsigned int VBO, EBO;
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);


    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    GLuint positionAttribute = glGetAttribLocation(ProgramID, "vtxPosition");
    GLuint textureAttribute = glGetAttribLocation(ProgramID, "Texcoords");
    // position attribute
    glVertexAttribPointer(positionAttribute, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(positionAttribute);
    // color attribute
    // texture coord attribute
    glVertexAttribPointer(textureAttribute, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(textureAttribute);



    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
    unsigned char* data = stbi_load("image.jpg", &width, &height, &nrChannels, 0);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    
    // Check the program
    glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
    glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    vector<char> ProgramErrorMessage(max(InfoLogLength, int(1)));
    glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
    fprintf(stdout, "%s\n", &ProgramErrorMessage[0]);

    glDeleteShader(VertexShaderID);
    glDeleteShader(FragmentShaderID);

    return ProgramID;
}
void myMouseDrag(int x, int y) {
}
void myMouseWheel(int button, int dir, int x, int y) {
    if (button == GLUT_LEFT_BUTTON) {
        if (dir == GLUT_UP) {
            cout << "마우스 뗌" << endl;
            mouse_dx[cnt] = float(x) / 240 - 1.0f;
            mouse_dy[cnt] = -(float(y) / 240 - 1.0f);
            cnt++;
            if (cnt == 2) {
                cnt = 0;
            }
            cout << x << " " << y;
        }
    }
    else if (button == GLUT_RIGHT_BUTTON) {
        if (dir == GLUT_UP) {
            cout << "우클릭됌" << endl;
            tessellation += 10;
        }
    }
}

void renderScene(void)
{
    // render
    // ------
    glClearColor(0.2f, 0.3f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // bind textures on corresponding texture units
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    //define the size of point and draw a point.
    glDrawArrays(GL_QUADS,0,4);
    //glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    //glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    //Double buffer
    glutSwapBuffers();
}

void doTimer(int i) {
    glutPostRedisplay();
    glutTimerFunc(10, doTimer, 1);
}
void init()
{
    //initilize the glew and check the errors.

    GLenum res = glewInit();
    if (res != GLEW_OK)
    {
        fprintf(stderr, "Error: '%s' \n", glewGetErrorString(res));
    }
    //select the background color
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);

}
void myKeyboard(unsigned char i, int x, int y) {
    cout << i << endl;
    if (i == '1') {
        currentProgram = programID;
    }
    else if (i == '2') {
        currentProgram = programID2;
    }
    else if (i == '3') {
        currentProgram = programID3;
    }
    else if (i == '4') {
        currentProgram = programID4;
    }
    else if (i == '5') {
        currentProgram = programID5;
    }
    glUseProgram(currentProgram);
    glutPostRedisplay();
}

int main(int argc, char** argv)
{
    auto n = 3;
    image= stbi_load("image.jpg", &width, &height, &n, 0);


    //init GLUT and create Window
    //initialize the GLUT
    glutInit(&argc, argv);
    //GLUT_DOUBLE enables double buffering (drawing to a background buffer while the other buffer is displayed)

    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);

    //These two functions are used to define the position and size of the window. 
    glutInitWindowPosition(200, 200);
    glutInitWindowSize(width, height);
    //This is used to define the name of the window.
    glutCreateWindow("Simple OpenGL Window");

    //call initization function
    init();

    //1.
    //Generate VAO
    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    //3. 
    programID = LoadShaders("VertexShader.txt", "FragmentShader.txt");
    programID2 = LoadShaders("VertexShader.txt", "FragmentShader_Negative.txt");
    programID3 = LoadShaders("VertexShader.txt", "FragmentShader_GaussianBlur.txt");
    programID4 = LoadShaders("VertexShader.txt", "FragmentShader_EdgeDetection.txt");
    programID5 = LoadShaders("VertexShader.txt", "FragmentShader_ToonRendering.txt");

    glUseProgram(programID);
    glutDisplayFunc(renderScene);

    glutKeyboardFunc(myKeyboard);
    glutMotionFunc(myMouseDrag);
    glutMouseFunc(myMouseWheel);
    glutMainLoop();
    glDeleteVertexArrays(1, &VertexArrayID);

    return 1;
}
