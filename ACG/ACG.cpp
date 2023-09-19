#define _USE_MATH_DEFINES
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
GLuint Buffers[5];
GLuint ProgramID;
using namespace std;
int tessellation = 10;
int mouse_prev_x = 0, mouse_prev_y = 0;
float mouse_dx[] = { 0.0f, 0.0f,0.0f, };
float mouse_dy[] = { 0.0f, 0.0f,0.0f, };
int cnt = 0;
GLuint LoadShaders(const char* vertex_file_path, const char* fragment_file_path, const char* geometry_file_path)
{
    //create the shaders
    GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
    GLuint GeometryShaderID = glCreateShader(GL_GEOMETRY_SHADER);
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

    string GeometryShaderCode;
    ifstream GeometryShaderStream(geometry_file_path, ios::in);
    if (GeometryShaderStream.is_open())
    {
        string Line = "";
        while (getline(GeometryShaderStream, Line))
            GeometryShaderCode += "\n" + Line;
        GeometryShaderStream.close();
    }

    //Compile Fragment Shader
    printf("Compiling shader : %s\n", geometry_file_path);
    char const* GeometrySourcePointer = GeometryShaderCode.c_str();
    glShaderSource(GeometryShaderID, 1, &GeometrySourcePointer, NULL);
    glCompileShader(GeometryShaderID);

    //Check Fragment Shader
    glGetShaderiv(GeometryShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(GeometryShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength != 0) {
        vector<char> GeometryShaderErrorMessage(InfoLogLength);
        glGetShaderInfoLog(GeometryShaderID, InfoLogLength, NULL, &GeometryShaderErrorMessage[0]);
        fprintf(stdout, "%s\n", &GeometryShaderErrorMessage[0]);
    }




    //Link the program
    fprintf(stdout, "Linking program\n");
    ProgramID = glCreateProgram();
    glAttachShader(ProgramID, VertexShaderID);
    glAttachShader(ProgramID, FragmentShaderID);
    glAttachShader(ProgramID, GeometryShaderID);
    glLinkProgram(ProgramID);

    GLfloat vertices[] = {
       -0.5f,-0.5f,0.0f,
       -0.5f,0.5f,0.0f,
       0.5f,0.5f,0.0f,
       0.5f,-0.5f,0.0f,

    };
    GLuint positionAttribute = glGetAttribLocation(ProgramID, "vtxPosition");

    glGenBuffers(3, Buffers);
    glBindBuffer(GL_ARRAY_BUFFER, Buffers[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(positionAttribute, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(positionAttribute);


    // Check the program
    glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
    glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    vector<char> ProgramErrorMessage(max(InfoLogLength, int(1)));
    glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
    fprintf(stdout, "%s\n", &ProgramErrorMessage[0]);

    glDeleteShader(VertexShaderID);
    glDeleteShader(FragmentShaderID);
    glDeleteShader(GeometryShaderID);

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
    //Clear all pixels
    glClear(GL_COLOR_BUFFER_BIT);
    //Let's draw something heres
    
    glUniform1i(glGetUniformLocation(ProgramID, "tess"), tessellation);
    //define the size of point and draw a point.
    //glDrawArrays(GL_POINTS, 0, 1);
    glDrawArrays(GL_LINES_ADJACENCY, 0, 4);
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


int main(int argc, char** argv)
{
    //init GLUT and create Window
    //initialize the GLUT
    glutInit(&argc, argv);
    //GLUT_DOUBLE enables double buffering (drawing to a background buffer while the other buffer is displayed)

    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);

    //These two functions are used to define the position and size of the window. 
    glutInitWindowPosition(200, 200);
    glutInitWindowSize(480, 480);
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
    GLuint programID = LoadShaders("VertexShader.txt", "FragmentShader.txt", "GeometryShader.txt");
    glUseProgram(programID);

    glutDisplayFunc(renderScene);

    glutMotionFunc(myMouseDrag);
    glutMouseFunc(myMouseWheel);
    //glutTimerFunc(10, doTimer, 1);
    //enter GLUT event processing cycle
    glutMainLoop();
    glDeleteVertexArrays(1, &VertexArrayID);

    return 1;
}
