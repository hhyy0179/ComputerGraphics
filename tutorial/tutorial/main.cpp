
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>

#ifdef WINDOWS  // 윈도우즈에서 컴파일 할때는 아래를 포함
#include <GL/glew.h>
#else // MAC 에서 할때는 아래를 포함
#include <OpenGL/gl3.h>
#endif
#include <GLUT/glut.h>

#include <algorithm>
using namespace std;
int screenWidth = 480;
int screenHeight = 480;
GLuint programID;
GLuint vertexBuffersID[2];
GLuint VertexArraysID[2];

vector <float> g_points;
vector <float> g_colors;

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

//마우스 입력을 받음.
void myMouseFn(int button, int state, int x, int y)
{
    float myX, myY;

    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        myX = (float)x / (float)screenWidth * 2 - 1;
        myY = -((float)y / (float)screenHeight * 2 - 1);

        g_points.push_back(myX);
        g_points.push_back(myY);
        g_points.push_back(0);

//        g_colors.push_back(rand() / (float)RAND_MAX);
//        g_colors.push_back(rand() / (float)RAND_MAX);
//        g_colors.push_back(rand() / (float)RAND_MAX);

       //~~~~~ init process ~~~~~
        
        //vertexshader에 할당한 변수를 가져옴.
        GLuint posLoc = glGetAttribLocation(programID, "inPos");
        
        //만들어진 버퍼를 사용하겠다.
        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffersID[0]);
        //버퍼에 데이터를 넣는 작업. 마지막은 데이터를 어떻게 관리할 것인지 (고정적으로 그리겠다.)
        glBufferData(GL_ARRAY_BUFFER, g_points.size() * sizeof(float),
            g_points.data(), GL_STATIC_DRAW);
        
        //buffer에서 하나씩 vertexShader 변수 index에 넣어서 사용하겠다.
        glVertexAttribPointer(posLoc, 3, GL_FLOAT, GL_FALSE, 0, (void*)(0));
        //쓸 준비 완료
        glEnableVertexAttribArray(posLoc);
        
        //~~~~~ When Update Rendering ~~~~~
        
        glBindVertexArray(VertexArraysID[0]);
        
        //입력받은 변수(posLoc)에 3개의 float 값을 할당. (myX,myY,0.0)
        //glVertexAttrib3f(posLoc, myX, myY, 0.0);
        

//        GLuint colLoc = glGetAttribLocation(programID, "inCol");
//        glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID[1]);
//        glBufferData(GL_ARRAY_BUFFER, g_colors.size() * sizeof(float),
//            g_colors.data(), GL_STATIC_DRAW);
//        glVertexAttribPointer(colLoc, 3, GL_FLOAT, GL_FALSE, 0, (void*)(0));
//        glEnableVertexAttribArray(colLoc);
//        glBindVertexArray(VertexArrayID[1]);
        
        
//        glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID[0]);
//        glVertexAttribPointer(colLoc, 3, GL_FLOAT, GL_FALSE, 0, (void*)(0));
//        glEnableVertexAttribArray(colLoc);
        
//        glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID[1]);
//        glVertexAttribPointer(posLoc, 3, GL_FLOAT, GL_FALSE, 0, (void*)(0));
//        glEnableVertexAttribArray(posLoc);

        glutPostRedisplay();
    }
}

void renderScene(void)
{
    //Clear all pixels
    //Clear color에 설정된 색깔로, frame buffer을 지워줌
    glClear(GL_COLOR_BUFFER_BIT);
    //Let's draw something here

    //define the size of point and draw a point.
    if (g_points.size() > 0)
    {
        GLuint posLoc = glGetAttribLocation(programID, "inPos");
        //만들어진 버퍼를 사용하겠다.
        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffersID[0]);
        glVertexAttribPointer(posLoc, 3, GL_FLOAT, GL_FALSE, 0, (void*)(0));
        
        GLuint colLoc = glGetAttribLocation(programID, "inCol");
        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffersID[1]);
        glVertexAttribPointer(colLoc, 3, GL_FLOAT, GL_FALSE, 0, (void*)(0));
        glBindVertexArray(VertexArraysID[1]);
        //그리는 함수: (어떤 형태로, 데이터 시작주소 , 그리는 개수);
        glDrawArrays(GL_LINES, 0, g_points.size()/3);
        glDrawArrays(GL_POINTS, 0, g_points.size() / 3);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, g_points.size() / 3);
    }
    
    //Double buffer
    //2개의 버퍼를 번갈아 가면서 사용함.
    glutSwapBuffers();
}


void init()
{
    //select the background color
    // (1,1,1,1) <- R,G,B,A (즉, 흰색으로 설정)
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
    // 2개의 버퍼를 int vertexBufferID[2] 이 배열에 만들겠다.
    glGenBuffers(2, vertexBuffersID);
}


int main(int argc, char **argv)
{
    //init GLUT and create Window
    //initialize the GLUT
    glutInit(&argc, argv);
    //GLUT_DOUBLE enables double buffering (drawing to a background buffer while the other buffer is displayed)
    //초기화 할 때, 스크린의 색깔이나, 프레임 버퍼를 어떻게 표현할 건지 정해줌!
    glutInitDisplayMode(GLUT_3_2_CORE_PROFILE | GLUT_DOUBLE | GLUT_RGBA);
    //These two functions are used to define the position and size of the window.
    //윈도우를 어느 포지션에 생성할 건지. 즉, nomalize position의 원점이다.
    glutInitWindowPosition(200, 200);
    //스크린 사이즈를 480 x 480 으로 설정한다.
    glutInitWindowSize(screenWidth, screenHeight);
    //This is used to define the name of the window.
    //윈도우를 만든다!(이름 설정)
    glutCreateWindow("Simple OpenGL Window");

  
    //1.
    //Generate VAO
    //vertex array를 생성함! 그리기 위한 데이터들의 id값을 생성하여 전달
    glGenVertexArrays(2, VertexArraysID);
    // 위에서 생성한 vertex array를 사용하겠다.
    // 데이터는 없지만, 데이터를 만들 수 있는 창구는 생성된 상태
    // 즉, 이것들은 만들 때 마다 새로 갱신할 필요가 없음.  -> 초기화 하기 전이나 main loop 들어가기 전에 해주는 것이 좋음.
    glBindVertexArray(VertexArraysID[0]);

    //call initization function
    //init 함수를 불러옴.
    init();
    glutMouseFunc(myMouseFn);

    //3.
    programID = LoadShaders("/Users/jadesuiii/ComputerGraphics/tutorial/tutorial/VertexShader.txt", "/Users/jadesuiii/ComputerGraphics/tutorial/tutorial/FragmentShader.txt");
    glUseProgram(programID);
    
    //loop 안에서 매번 불러온다.
    glutDisplayFunc(renderScene);

    //enter GLUT event processing cycle
    glutMainLoop();

    glDeleteVertexArrays(2, VertexArraysID);
    
    return 1;
}
