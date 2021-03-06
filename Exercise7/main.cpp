#include <GL/glew.h>
#include <GL/glut.h>
#include <GL/freeglut.h>  
#include <iostream>
#include <cstdio>  
#include <cstdlib>  
#include <cstring>  

#pragma comment(lib,"glew32.lib")    

using namespace std;

GLfloat l_dir[3] = { 1.0f, 1.0f, 1.0f };
GLfloat l_ambient[4] = { 0.2f , 0.15f , 0.1f , 1.0f };
GLfloat l_diffuse[4] = { 0.8f,0.6f,0.4f,1.0f };
GLfloat l_specular[4] = { 1.0f,1.0f,1.0f,1.0f };
GLfloat l_shininess = 30;
GLfloat m_diffuse[4] = { 1.000000, 0.829000, 0.829000, 0.922000 };
GLfloat m_ambient[4] = { 0.250000, 0.207250, 0.207250, 0.922000 };
GLfloat m_specular[4] = { 0.296648, 0.296648, 0.296648, 0.922000 };
GLfloat m_shininess = 11.264000;
GLfloat objectSize = 5.0;
GLuint programHandle;
GLuint vShader, fShader;

// Camera Position
float camX = 0, camY = 10, camZ = 30;


// Camera Spherical Coordinates
float alpha = 39.0f, beta = 51.0f;
float r = 30.0f;

int rx1, ry1;
int g_mouseX, g_mouseY;
bool g_isTracking = false;

//读入字符流  
char* textFileRead(const char* fn)
{
	FILE* fp;
	char* content = NULL;
	int count = 0;
	if (fn != NULL)
	{
		fp = fopen(fn, "rt");
		if (fp != NULL)
		{
			fseek(fp, 0, SEEK_END);
			count = ftell(fp);
			rewind(fp);
			if (count > 0)
			{
				content = (char*)malloc(sizeof(char) * (count + 1));
				count = fread(content, sizeof(char), count, fp);
				content[count] = '\0';
			}
			fclose(fp);
		}
	}
	return content;
}

void initShader(const char* VShaderFile, const char* FShaderFile)
{
	//1、查看显卡、GLSL和OpenGL的信息    
	const GLubyte* vendor = glGetString(GL_VENDOR);
	const GLubyte* renderer = glGetString(GL_RENDERER);
	const GLubyte* version = glGetString(GL_VERSION);
	const GLubyte* glslVersion = glGetString(GL_SHADING_LANGUAGE_VERSION);
	cout << "显卡供应商   : " << vendor << endl;
	cout << "显卡型号     : " << renderer << endl;
	cout << "OpenGL版本   : " << version << endl;
	cout << "GLSL版本     : " << glslVersion << endl;
	//2、编译着色器    
	//创建着色器对象：顶点着色器    
	vShader = glCreateShader(GL_VERTEX_SHADER);
	//错误检测    
	if (0 == vShader)
	{
		cerr << "ERROR : Create vertex shader failed" << endl;
		exit(1);
	}
	//把着色器源代码和着色器对象相关联    
	const GLchar* vShaderCode = textFileRead(VShaderFile);
	const GLchar* vCodeArray[1] = { vShaderCode };

	//将字符数组绑定到对应的着色器对象上  
	glShaderSource(vShader, 1, vCodeArray, NULL);

	//编译着色器对象    
	glCompileShader(vShader);

	//检查编译是否成功    
	GLint compileResult;
	glGetShaderiv(vShader, GL_COMPILE_STATUS, &compileResult);
	if (GL_FALSE == compileResult)
	{
		GLint logLen;
		//得到编译日志长度    
		glGetShaderiv(vShader, GL_INFO_LOG_LENGTH, &logLen);
		if (logLen > 0)
		{
			char* log = (char*)malloc(logLen);
			GLsizei written;
			//得到日志信息并输出    
			glGetShaderInfoLog(vShader, logLen, &written, log);
			cerr << "vertex shader compile log : " << endl;
			cerr << log << endl;
			free(log);//释放空间    
		}
	}

	//创建着色器对象：片断着色器    
	fShader = glCreateShader(GL_FRAGMENT_SHADER);
	//错误检测    
	if (0 == fShader)
	{
		cerr << "ERROR : Create fragment shader failed" << endl;
		exit(1);
	}

	//把着色器源代码和着色器对象相关联    
	const GLchar* fShaderCode = textFileRead(FShaderFile);
	const GLchar* fCodeArray[1] = { fShaderCode };
	glShaderSource(fShader, 1, fCodeArray, NULL);

	//编译着色器对象    
	glCompileShader(fShader);

	//检查编译是否成功    
	glGetShaderiv(fShader, GL_COMPILE_STATUS, &compileResult);
	if (GL_FALSE == compileResult)
	{
		GLint logLen;
		//得到编译日志长度    
		glGetShaderiv(fShader, GL_INFO_LOG_LENGTH, &logLen);
		if (logLen > 0)
		{
			char* log = (char*)malloc(logLen);
			GLsizei written;
			//得到日志信息并输出    
			glGetShaderInfoLog(fShader, logLen, &written, log);
			cerr << "fragment shader compile log : " << endl;
			cerr << log << endl;
			free(log);//释放空间    
		}
	}
	//3、链接着色器对象    
	//创建着色器程序    
	programHandle = glCreateProgram();
	if (!programHandle)
	{
		cerr << "ERROR : create program failed" << endl;
		exit(1);
	}
	//将着色器程序链接到所创建的程序中    
	glAttachShader(programHandle, vShader);
	glAttachShader(programHandle, fShader);
	//将这些对象链接成一个可执行程序    
	glLinkProgram(programHandle);
	//查询链接的结果    
	GLint linkStatus;
	glGetProgramiv(programHandle, GL_LINK_STATUS, &linkStatus);
	if (GL_FALSE == linkStatus)
	{
		cerr << "ERROR : link shader program failed" << endl;
		GLint logLen;
		glGetProgramiv(programHandle, GL_INFO_LOG_LENGTH,
			&logLen);
		if (logLen > 0)
		{
			char* log = (char*)malloc(logLen);
			GLsizei written;
			glGetProgramInfoLog(programHandle, logLen,
				&written, log);
			cerr << "Program log : " << endl;
			cerr << log << endl;
		}
	}
}
//完成glew初始化和加载顶点、片段着色器
void init()
{
	//初始化glew扩展库    
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		cout << "Error initializing GLEW: " << glewGetErrorString(err) << endl;
	}
	glEnable(GL_DEPTH_TEST);
	//加载顶点和片段着色器对象并链接到一个程序对象上  
	initShader("shaders/Vert2.vert", "shaders/Frag2.frag");

	glClearColor(0.0, 0.0, 0.0, 0.0);
}

void Reshape(int w, int h)
{
	float ratio;
	if (h == 0) h = 1;
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	ratio = (1.0f * w) / h;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(53.13f, ratio, 0.1f, 10000.0f);
	glMatrixMode(GL_MODELVIEW);
	
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glColor3f(1.0, 1.0, 1.0);

	glLoadIdentity();
	gluLookAt(camX, camY, camZ, 0, 0, 0, 0, 1, 0);

	//	PerVertex Lighting
	/*glUseProgram(programHandle);
	glUniform3f(glGetUniformLocation(programHandle, "l_dir"), l_dir[0], l_dir[1], l_dir[2]);
	glUniform4f(glGetUniformLocation(programHandle, "l_ambient"), l_ambient[0], l_ambient[1], l_ambient[2], l_ambient[3]);
	glUniform4f(glGetUniformLocation(programHandle, "l_diffuse"), l_diffuse[0], l_diffuse[1], l_diffuse[2], l_diffuse[3]);
	glUniform4f(glGetUniformLocation(programHandle, "l_specular"), l_specular[0], l_specular[1], l_specular[2], l_specular[3]);
	glUniform1f(glGetUniformLocation(programHandle, "l_shininess"), l_shininess);
	glUniform4f(glGetUniformLocation(programHandle, "m_ambient"), m_ambient[0], m_ambient[1], m_ambient[2], m_ambient[3]);
	glUniform4f(glGetUniformLocation(programHandle, "m_ambient"), m_ambient[0], m_ambient[1], m_ambient[2], m_ambient[3]);
	glUniform4f(glGetUniformLocation(programHandle, "m_ambient"), m_ambient[0], m_ambient[1], m_ambient[2], m_ambient[3]);
	glUniform1f(glGetUniformLocation(programHandle, "m_shininess"), m_shininess);*/

	//	PerFragment Lighting
	glUseProgram(programHandle);
	glUniform3f(glGetUniformLocation(programHandle, "l_dir"), l_dir[0], l_dir[1], l_dir[2]);
	glUniform4f(glGetUniformLocation(programHandle, "l_ambient"), l_ambient[0], l_ambient[1], l_ambient[2], l_ambient[3]);
	glUniform4f(glGetUniformLocation(programHandle, "l_diffuse"), l_diffuse[0], l_diffuse[1], l_diffuse[2], l_diffuse[3]);
	glUniform4f(glGetUniformLocation(programHandle, "l_specular"), l_specular[0], l_specular[1], l_specular[2], l_specular[3]);
	glUniform1f(glGetUniformLocation(programHandle, "l_shininess"), l_shininess);

	glRotatef(rx1, 0, 1, 0);
	glRotatef(ry1, 1, 0, 0);

	glutSolidTeapot(objectSize);
	glutSwapBuffers();
}

void processKeys(unsigned char key, int xx, int yy) {
	switch (key){
		case 'm':
			glEnable(GL_MULTISAMPLE);
			break;
		case 'n':
			glDisable(GL_MULTISAMPLE);
			break;
		default:
			break;
	}
}


void mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON)
	{
		g_isTracking = true;
		g_mouseX = x;
		g_mouseY = y;
	}
	else g_isTracking = false;

}
void motion(int x, int y)
{
	if (g_isTracking)
	{
		rx1 -= x - g_mouseX;
		ry1 -= y - g_mouseY;

		g_mouseX = x;
		g_mouseY = y;
	}
}

void mouseWheel(int wheel, int direction, int x, int y) {
	r += direction * 0.1f;
	if (r < 0.1f)
		r = 0.1f;

	camX = r * sin(alpha * 3.14f / 180.0f) * cos(beta * 3.14f / 180.0f);
	camZ = r * cos(alpha * 3.14f / 180.0f) * cos(beta * 3.14f / 180.0f);
	camY = r *								 sin(beta * 3.14f / 180.0f);
}
void idle()
{
	glutPostRedisplay();
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_MULTISAMPLE);
	glutInitWindowSize(600, 600);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Hello GLSL");
	init();
	glutReshapeFunc(Reshape);
	glutDisplayFunc(display);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutMouseWheelFunc(mouseWheel);
	glutIdleFunc(idle);

	glutMainLoop();
	return 0;
}