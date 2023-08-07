#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>

#ifdef WINDOWS  // 윈도우즈에서 컴파일 할때는 아래를 포함
#include <GL/glew.h>
#else // MAC 에서 할때는 아래를 포함
#include <OpenGL/gl3.h>
#endif
#include <GLUT/glut.h>
#include <algorithm>
#include "glm.hpp"


using namespace std;
int screenWidth = 480;
int screenHeight = 480;
GLuint programID;
GLuint vertexBufferID[10];
GLuint VertexArrayID[10];
GLuint posLoc;
GLuint colLoc;
GLfloat move;
GLfloat sx, sy;

GLfloat Vertices[24];
GLuint Indices[36];
GLfloat matv[16];

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
    if(VertexShaderStream.is_open())
    {
        string Line = "";
        while(getline(VertexShaderStream, Line))
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
    
    if (InfoLogLength > 0)
    {
        vector<char> VertexShaderErrorMessage(InfoLogLength);
        glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
        fprintf(stdout, "%s\n", &VertexShaderErrorMessage[0]);
    }

    //Read the fragment shader code from the file
    string FragmentShaderCode;
    ifstream FragmentShaderStream(fragment_file_path, ios::in);
    
    if(FragmentShaderStream.is_open())
    {
        string Line = "";
        while(getline(FragmentShaderStream, Line))
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
    
    if (InfoLogLength > 0)
    {
        vector<char> FragmentShaderErrorMessage(InfoLogLength);
        glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
        fprintf(stdout, "%s\n", &FragmentShaderErrorMessage[0]);
    }
    
    //Link the program
    fprintf(stdout, "Linking program\n");
    GLuint ProgramID = glCreateProgram();
    glAttachShader(ProgramID, VertexShaderID);
    glAttachShader(ProgramID, FragmentShaderID);
    glLinkProgram(ProgramID);
 
    // Check the program
    glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
    glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    vector<char> ProgramErrorMessage( max(InfoLogLength, int(1)) );
    glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
    fprintf(stdout, "%s\n", &ProgramErrorMessage[0]);
 
    glDeleteShader(VertexShaderID);
    glDeleteShader(FragmentShaderID);
 
    return ProgramID;
}


void init()
{
    //select the background color
    // (1,1,1,1) <- R,G,B,A (즉, 흰색으로 설정)
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
    glEnable(GL_DEPTH_TEST);

    glGenVertexArrays(10, VertexArrayID);
    glGenBuffers(10, vertexBufferID);
    
    GLfloat Vertices[] = {
        -0.1f, -0.1f, -0.1f, 1.0f, 1.0f, 1.0f, //0
        0.1f, -0.1f, -0.1f, 0.0f, 1.0f, 1.0f, //1
        -0.1f, 0.1f, -0.1f, 1.0f, 0.0f, 1.0f, //2
        -0.1f, -0.1f, 0.1f, 1.0f, 1.0f, 0.0f, //3
        0.1f, 0.1f, -0.1f, 0.0f, 0.0f, 1.0f,   //4
        -0.1f, 0.1f, 0.1f, 1.0f, 0.0f, 0.0f,   //5
        0.1f, -0.1f, 0.1f, 0.0f, 1.0f, 0.0f,   //6
        0.1f, 0.1f, 0.1f, 0.0f, 0.0f, 0.0f   //7
    };
    
    GLuint Indices[] = {
        0, 1, 2,
        1, 2, 4,
        0, 2, 3,
        2, 3, 5,
        0, 1, 3,
        1, 3, 6,
        2, 4, 5,
        4, 5, 7,
        5, 7, 3,
        3, 6, 7,
        4, 7, 6,
        4, 6, 1
    };
    
    GLuint triangleElementBufferObject;
    glGenBuffers(1, &triangleElementBufferObject);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, triangleElementBufferObject);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);
    
    for(int i = 0; i < 2; i++)
    {
        glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID[i]); //버퍼 사용
        glBufferData(GL_ARRAY_BUFFER, 48 * sizeof(float),
                     Vertices, GL_STATIC_DRAW);
        posLoc = glGetAttribLocation(programID, "inPos");
        glVertexAttribPointer(posLoc, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)(0));
        glEnableVertexAttribArray(posLoc);

        colLoc = glGetAttribLocation(programID, "inCol");
        glVertexAttribPointer(colLoc, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)(3));
        glEnableVertexAttribArray(colLoc);
    }
}

void renderScene(void)
{
    //Clear all pixels
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //Let's draw something here
    GLfloat MoveX = -1.0;
    GLfloat MoveY = 0.3;
    for(int i = 0; i < 2; i++)
    {
        glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID[i]);
        posLoc = glGetAttribLocation(programID, "inPos");
        glVertexAttribPointer(posLoc, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)(0));
        glEnableVertexAttribArray(posLoc);
        
        colLoc = glGetAttribLocation(programID, "inCol");
        glVertexAttribPointer(colLoc, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)(12));
        glEnableVertexAttribArray(colLoc);
        
//        if(i == 5)
//        {
//            MoveY = -0.3;
//            MoveX = -1.0;
//        }
        
        MoveX += 0.3;
        GLuint matTrans = glGetUniformLocation(programID, "trans");
            GLfloat matv[] = {
            0.866, -0.25, 0.0, MoveX,
            0.354, 0.866, -0.354, MoveY,
            0.0, 0.433, 0.901, 0.0,
            0.0, 0.0, 0.0, 1.0
        };
        glUniformMatrix4fv(matTrans, 1, GL_TRUE, matv);
        
        //그리는 함수: (어떤 형태로, 데이터 시작주소 , 그리는 개수);
        //glDrawArrays(GL_LINES, 0, 2);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        glDrawArrays(GL_POINTS, 0, 8);
    }

    glutSwapBuffers();
}

void myKeyFunc(unsigned char key, int x, int y)
{
    if(key == 'w')
    {
        sy += 0.1;
    }
    if(key == 'a')
    {
        sx -= 0.1;
    }
    if(key == 's')
    {
        sy -= 0.1;
    }
    if(key == 'd')
    {
        sx += 0.1;
    }
    
    GLuint diffLoc = glGetUniformLocation(programID, "diff");
    glUniform3f(diffLoc, sx, sy, 0);
    glutPostRedisplay();
    
}


int main(int argc, char **argv)
{
    //init GLUT and create Window
    //initialize the GLUT
    glutInit(&argc, argv);
    //GLUT_DOUBLE enables double buffering (drawing to a background buffer while the other buffer is displayed)
    //초기화 할 때, 스크린의 색깔이나, 프레임 버퍼를 어떻게 표현할 건지 정해줌!
    glutInitDisplayMode(GLUT_3_2_CORE_PROFILE | GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    //These two functions are used to define the position and size of the window.
    //윈도우를 어느 포지션에 생성할 건지. 즉, nomalize position의 원점이다.
    glutInitWindowPosition(200, 200);
    //스크린 사이즈를 480 x 480 으로 설정한다.
    glutInitWindowSize(screenWidth, screenHeight);
    //This is used to define the name of the window.
    //윈도우를 만든다!(이름 설정)
    glutCreateWindow("201914172 김현수 Homework2");

    //1.
    //Generate VAO
    glGenVertexArrays(10, VertexArrayID);
    
    for(int i = 0; i < 10; i++)
    {
        glBindVertexArray(VertexArrayID[i]);
    }
    
    //call initization function
    init();

    //3.
    programID = LoadShaders("/Users/jadesuiii/ComputerGraphics/DrawCube/DrawCube/VertexShader.txt", "/Users/jadesuiii/ComputerGraphics/DrawCube/DrawCube/FragmentShader.txt");
    glUseProgram(programID);

    glutDisplayFunc(renderScene);
    glutKeyboardFunc(myKeyFunc);

    //enter GLUT event processing cycle
    glutMainLoop();

    glDeleteVertexArrays(10, VertexArrayID);
    
    return 1;
}
