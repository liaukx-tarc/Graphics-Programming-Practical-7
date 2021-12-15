#include <Windows.h>
#include <gl/GL.h>
#include <gl/GLU.h>
#include <math.h>
#include <string>
#include <stdio.h>

#pragma comment (lib, "GLU32.lib")
#pragma comment (lib, "OpenGL32.lib")

#define WINDOW_TITLE "OpenGL Window"
#define WINDOW_WIDTH 720.0f
#define WINDOW_HIGH 720.0f
#define WINDOWS_FIX (WINDOW_HIGH/WINDOW_WIDTH)

GLUquadricObj* var = NULL;

float timer = 0, tz = 0, pry = 0, xMax, yMax, zoomSpeed, moveSpeed, bridgeAngle = 0;
int oNear, oFar, pZMin, pZMax, angle;
bool viewOrtho = true;

int qNo;

//Texture
GLuint texture[16];
BITMAP BMP;

int textureNum = 0;

LRESULT WINAPI WindowProcedure(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE)
		{
			PostQuitMessage(0);
		}

		else if (wParam == VK_TAB)
		{
			viewOrtho = !viewOrtho;
			tz = 0;
		}

		else if (wParam == VK_DOWN)
		{
			if (!viewOrtho)
			{
				if (tz < pZMax)
					tz += zoomSpeed;
			}
		}


		else if (wParam == VK_UP)
		{
			if (!viewOrtho)
			{
				if (tz >= pZMin)
					tz -= zoomSpeed;
			}
		}


		else if (wParam == 'Q')
		{
			angle -= 10;
		}

		else if (wParam == 'E')
		{
			angle += 10;
		}

		else if (wParam == 'W')
		{
			if (bridgeAngle < 60)
				bridgeAngle += moveSpeed;
		}

		else if (wParam == 'S')
		{
			if (bridgeAngle > 0)
				bridgeAngle -= moveSpeed;
		}

		else if (wParam == '1')
		{
			qNo = 1;
		}

		else if (wParam == '2')
		{
			qNo = 2;
		}

		else if (wParam == '3')
		{
			qNo = 3;
		}

		else if (wParam == '4')
		{
			qNo = 4;
		}

		else if (wParam == 'T')
		{
			textureNum++;

			if (textureNum > 2)
			{
				textureNum = 0;
			}
		}

	default:
		break;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}
//--------------------------------------------------------------------

bool initPixelFormat(HDC hdc)
{
	PIXELFORMATDESCRIPTOR pfd;
	ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));

	pfd.cAlphaBits = 8;
	pfd.cColorBits = 32;
	pfd.cDepthBits = 24;
	pfd.cStencilBits = 0;

	pfd.dwFlags = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;

	pfd.iLayerType = PFD_MAIN_PLANE;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;

	// choose pixel format returns the number most similar pixel format available
	int n = ChoosePixelFormat(hdc, &pfd);

	// set pixel format returns whether it sucessfully set the pixel format
	if (SetPixelFormat(hdc, n, &pfd))
	{
		return true;
	}
	else
	{
		return false;
	}
}
//--------------------------------------------------------------------

GLuint loadTexture(LPCSTR filename)
{
	GLuint texture = 0;

	//Step 2
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	HBITMAP hBMP = (HBITMAP)LoadImage(GetModuleHandle(NULL), 
		filename, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_LOADFROMFILE);

	GetObject(hBMP, sizeof(BMP), &BMP);

	//Step 4
	glEnable(GL_TEXTURE_2D);

	glGenTextures(1, &texture);

	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, BMP.bmWidth, BMP.bmHeight, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, BMP.bmBits);

	DeleteObject(hBMP);
	return texture;
}

void inti()
{
	var = gluNewQuadric();

	oNear = -10;
	oFar = 10;
	pZMin = 1;
	pZMax = 21;
	xMax = 10.0f;
	yMax = 10.0f;
	zoomSpeed = 0.5f;
	moveSpeed = 1.0f;

	qNo = 1;

	texture[0] = loadTexture("brick.bmp");
	texture[1] = loadTexture("Box.bmp");
	texture[2] = loadTexture("metal.bmp");

	//Ice Cream
	texture[3] = loadTexture("Cone.bmp");
	texture[4] = loadTexture("IceCream.bmp");
	texture[5] = loadTexture("IceCream2.bmp");
	texture[6] = loadTexture("biscuits.bmp");
	texture[7] = loadTexture("cherry.bmp");
	texture[8] = loadTexture("room.bmp");
	texture[9] = loadTexture("room2.bmp");
	texture[10] = loadTexture("Pillar.bmp");
	texture[11] = loadTexture("base.bmp");
	texture[12] = loadTexture("roof.bmp");
	texture[13] = loadTexture("road.bmp");
	texture[14] = loadTexture("structure.bmp");
	texture[15] = loadTexture("sea.bmp");
}

void changeView()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	if (viewOrtho)
	{
		glOrtho(-xMax, xMax, -yMax, yMax, oNear, oFar);
		glRotatef(45, 1.0f, 0.0f, 0.0f);
		glTranslatef(0.0f, -2.0f, 0.0f);
	}

	else
	{
		gluPerspective(60, 1.0, 1, -1);
		glFrustum(-xMax / 8.0f, xMax / 8.0f, -yMax / 8.0f, yMax / 8.0f, pZMin, pZMax);;
		glTranslatef(0.0f, -6.0f, 10.0f);
	}

}

void DrawRoom()
{
	glPushMatrix();

	glBindTexture(GL_TEXTURE_2D, texture[8]);
	//Front
	glBegin(GL_QUADS);
	glTexCoord2f(0, 1);
	/*Vertex 1*/ glVertex3f(0.0f, 1.0f, 0.0f);
	glTexCoord2f(1, 1);
	/*Vertex 2*/ glVertex3f(1.0f, 1.0f, 0.0f);
	glTexCoord2f(1, 0);
	/*Vertex 4*/ glVertex3f(1.0f, 0.0f, 0.0f);
	glTexCoord2f(0, 0);
	/*Vertex 3*/ glVertex3f(0.0f, 0.0f, 0.0f);
	glEnd();

	//Back
	glBegin(GL_QUADS);
	glTexCoord2f(0, 1);
	/*Vertex 5*/ glVertex3f(0.0f, 1.0f, 1.0f);
	glTexCoord2f(0, 0);
	/*Vertex 7*/ glVertex3f(0.0f, 0.0f, 1.0f);
	glTexCoord2f(1, 0);
	/*Vertex 8*/ glVertex3f(1.0f, 0.0f, 1.0f);
	glTexCoord2f(1, 1);
	/*Vertex 6*/ glVertex3f(1.0f, 1.0f, 1.0f);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, texture[9]);
	//Left
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	/*Vertex 3*/ glVertex3f(0.0f, 0.0f, 0.0f);
	glTexCoord2f(1, 0);
	/*Vertex 7*/ glVertex3f(0.0f, 0.0f, 1.0f);
	glTexCoord2f(1, 1);
	/*Vertex 5*/ glVertex3f(0.0f, 1.0f, 1.0f);
	glTexCoord2f(0, 1);
	/*Vertex 1*/ glVertex3f(0.0f, 1.0f, 0.0f);
	glEnd();

	//Right
	glBegin(GL_QUADS);
	glTexCoord2f(1, 1);
	/*Vertex 6*/ glVertex3f(1.0f, 1.0f, 1.0f);
	glTexCoord2f(1, 0);
	/*Vertex 8*/ glVertex3f(1.0f, 0.0f, 1.0f);
	glTexCoord2f(0, 0);
	/*Vertex 4*/ glVertex3f(1.0f, 0.0f, 0.0f);
	glTexCoord2f(0, 1);
	/*Vertex 2*/ glVertex3f(1.0f, 1.0f, 0.0f);
	glEnd();

	//Up
	glBindTexture(GL_TEXTURE_2D, texture[11]);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	/*Vertex 1*/ glVertex3f(0.0f, 1.0f, 0.0f);
	glTexCoord2f(1, 0);
	/*Vertex 2*/ glVertex3f(1.0f, 1.0f, 0.0f);
	glTexCoord2f(1, 1);
	/*Vertex 6*/ glVertex3f(1.0f, 1.0f, 1.0f);
	glTexCoord2f(0, 1);
	/*Vertex 5*/ glVertex3f(0.0f, 1.0f, 1.0f);
	glEnd();

	//Down
	glBegin(GL_QUADS);
	glTexCoord2f(1, 1);
	/*Vertex 8*/ glVertex3f(1.0f, 0.0f, 1.0f);
	glTexCoord2f(1, 0);
	/*Vertex 4*/ glVertex3f(1.0f, 0.0f, 0.0f);
	glTexCoord2f(0, 0);
	/*Vertex 3*/ glVertex3f(0.0f, 0.0f, 0.0f);
	glTexCoord2f(0, 1);
	/*Vertex 7*/ glVertex3f(0.0f, 0.0f, 1.0f);
	glEnd();

	glPopMatrix();
}

void DrawBase()
{
	glPushMatrix();

	glBindTexture(GL_TEXTURE_2D, texture[11]);
	//Front
	glBegin(GL_QUADS);
	glTexCoord2f(0, 1);
	/*Vertex 1*/ glVertex3f(0.0f, 1.0f, 0.0f);
	glTexCoord2f(1, 1);
	/*Vertex 2*/ glVertex3f(1.0f, 1.0f, 0.0f);
	glTexCoord2f(1, 0);
	/*Vertex 4*/ glVertex3f(1.0f, 0.0f, 0.0f);
	glTexCoord2f(0, 0);
	/*Vertex 3*/ glVertex3f(0.0f, 0.0f, 0.0f);
	glEnd();

	//Back
	glBegin(GL_QUADS);
	glTexCoord2f(0, 1);
	/*Vertex 5*/ glVertex3f(0.0f, 1.0f, 1.0f);
	glTexCoord2f(0, 0);
	/*Vertex 7*/ glVertex3f(0.0f, 0.0f, 1.0f);
	glTexCoord2f(1, 0);
	/*Vertex 8*/ glVertex3f(1.0f, 0.0f, 1.0f);
	glTexCoord2f(1, 1);
	/*Vertex 6*/ glVertex3f(1.0f, 1.0f, 1.0f);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, texture[11]);
	//Left
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	/*Vertex 3*/ glVertex3f(0.0f, 0.0f, 0.0f);
	glTexCoord2f(1, 0);
	/*Vertex 7*/ glVertex3f(0.0f, 0.0f, 1.0f);
	glTexCoord2f(1, 1);
	/*Vertex 5*/ glVertex3f(0.0f, 1.0f, 1.0f);
	glTexCoord2f(0, 1);
	/*Vertex 1*/ glVertex3f(0.0f, 1.0f, 0.0f);
	glEnd();

	//Right
	glBegin(GL_QUADS);
	glTexCoord2f(1, 1);
	/*Vertex 6*/ glVertex3f(1.0f, 1.0f, 1.0f);
	glTexCoord2f(1, 0);
	/*Vertex 8*/ glVertex3f(1.0f, 0.0f, 1.0f);
	glTexCoord2f(0, 0);
	/*Vertex 4*/ glVertex3f(1.0f, 0.0f, 0.0f);
	glTexCoord2f(0, 1);
	/*Vertex 2*/ glVertex3f(1.0f, 1.0f, 0.0f);
	glEnd();

	//Up
	glBindTexture(GL_TEXTURE_2D, texture[11]);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	/*Vertex 1*/ glVertex3f(0.0f, 1.0f, 0.0f);
	glTexCoord2f(1, 0);
	/*Vertex 2*/ glVertex3f(1.0f, 1.0f, 0.0f);
	glTexCoord2f(1, 1);
	/*Vertex 6*/ glVertex3f(1.0f, 1.0f, 1.0f);
	glTexCoord2f(0, 1);
	/*Vertex 5*/ glVertex3f(0.0f, 1.0f, 1.0f);
	glEnd();

	//Down
	glBegin(GL_QUADS);
	glTexCoord2f(1, 1);
	/*Vertex 8*/ glVertex3f(1.0f, 0.0f, 1.0f);
	glTexCoord2f(1, 0);
	/*Vertex 4*/ glVertex3f(1.0f, 0.0f, 0.0f);
	glTexCoord2f(0, 0);
	/*Vertex 3*/ glVertex3f(0.0f, 0.0f, 0.0f);
	glTexCoord2f(0, 1);
	/*Vertex 7*/ glVertex3f(0.0f, 0.0f, 1.0f);
	glEnd();

	glPopMatrix();
}

void DrawSea()
{
	glPushMatrix();
	glColor4f(1.0f, 1.0f, 1.0f, 0.8f);
	
	glBindTexture(GL_TEXTURE_2D, texture[15]);

	///Front
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 1.0f);
	/*Vertex 1*/ glVertex3f(0.0f, 1.0f, 0.0f);
	glTexCoord2f(1.0f, 1.0f);
	/*Vertex 2*/ glVertex3f(1.0f, 1.0f, 0.0f);
	glTexCoord2f(1.0f, 0.0f);
	/*Vertex 4*/ glVertex3f(1.0f, 0.0f, 0.0f);
	glTexCoord2f(0.0f, 0.0f);
	/*Vertex 3*/ glVertex3f(0.0f, 0.0f, 0.0f);
	glEnd();

	//Back
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 1.0f);
	/*Vertex 5*/ glVertex3f(0.0f, 1.0f, 1.0f);
	glTexCoord2f(0.0f, 0.0f);
	/*Vertex 7*/ glVertex3f(0.0f, 0.0f, 1.0f);
	glTexCoord2f(1.0f, 0.0f);
	/*Vertex 8*/ glVertex3f(1.0f, 0.0f, 1.0f);
	glTexCoord2f(1.0f, 1.0f);
	/*Vertex 6*/ glVertex3f(1.0f, 1.0f, 1.0f);
	glEnd();

	//Left
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	/*Vertex 3*/ glVertex3f(0.0f, 0.0f, 0.0f);
	glTexCoord2f(0.0f, 1.0f);
	/*Vertex 7*/ glVertex3f(0.0f, 0.0f, 1.0f);
	glTexCoord2f(1.0f, 1.0f);
	/*Vertex 5*/ glVertex3f(0.0f, 1.0f, 1.0f);
	glTexCoord2f(1.0f, 0.0f);
	/*Vertex 1*/ glVertex3f(0.0f, 1.0f, 0.0f);
	glEnd();

	//Right
	glBegin(GL_QUADS);
	glTexCoord2f(1.0f, 1.0f);
	/*Vertex 6*/ glVertex3f(1.0f, 1.0f, 1.0f);
	glTexCoord2f(0.0f, 1.0f);
	/*Vertex 8*/ glVertex3f(1.0f, 0.0f, 1.0f);
	glTexCoord2f(0.0f, 0.0f);
	/*Vertex 4*/ glVertex3f(1.0f, 0.0f, 0.0f);
	glTexCoord2f(1.0f, 0.0f);
	/*Vertex 2*/ glVertex3f(1.0f, 1.0f, 0.0f);
	glEnd();

	//Up
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	/*Vertex 1*/ glVertex3f(0.0f, 1.0f, 0.0f);
	glTexCoord2f(1.0f, 0.0f);
	/*Vertex 2*/ glVertex3f(1.0f, 1.0f, 0.0f);
	glTexCoord2f(1.0f, 1.0f);
	/*Vertex 6*/ glVertex3f(1.0f, 1.0f, 1.0f);
	glTexCoord2f(0.0f, 1.0f);
	/*Vertex 5*/ glVertex3f(0.0f, 1.0f, 1.0f);
	glEnd();

	//Down
	glBegin(GL_QUADS);
	glTexCoord2f(1.0f, 1.0f);
	/*Vertex 8*/ glVertex3f(1.0f, 0.0f, 1.0f);
	glTexCoord2f(1.0f, 0.0f);
	/*Vertex 4*/ glVertex3f(1.0f, 0.0f, 0.0f);
	glTexCoord2f(0.0f, 0.0f);
	/*Vertex 3*/ glVertex3f(0.0f, 0.0f, 0.0f);
	glTexCoord2f(0.0f, 1.0f);
	/*Vertex 7*/ glVertex3f(0.0f, 0.0f, 1.0f);
	glEnd();

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	glPopMatrix();
}

void DrawCubeQNO2()
{
	glPushMatrix();

	//Front
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 1.0f);
	/*Vertex 1*/ glVertex3f(0.0f, 1.0f, 0.0f);
	glTexCoord2f(1.0f, 1.0f);
	/*Vertex 2*/ glVertex3f(1.0f, 1.0f, 0.0f);
	glTexCoord2f(1.0f, 0.0f);
	/*Vertex 4*/ glVertex3f(1.0f, 0.0f, 0.0f);
	glTexCoord2f(0.0f, 0.0f);
	/*Vertex 3*/ glVertex3f(0.0f, 0.0f, 0.0f);
	glEnd();

	//Back
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 1.0f);
	/*Vertex 5*/ glVertex3f(0.0f, 1.0f, 1.0f);
	glTexCoord2f(0.0f, 0.0f);
	/*Vertex 7*/ glVertex3f(0.0f, 0.0f, 1.0f);
	glTexCoord2f(1.0f, 0.0f);
	/*Vertex 8*/ glVertex3f(1.0f, 0.0f, 1.0f);
	glTexCoord2f(1.0f, 1.0f);
	/*Vertex 6*/ glVertex3f(1.0f, 1.0f, 1.0f);
	glEnd();

	//Left
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	/*Vertex 3*/ glVertex3f(0.0f, 0.0f, 0.0f);
	glTexCoord2f(0.0f, 1.0f);
	/*Vertex 7*/ glVertex3f(0.0f, 0.0f, 1.0f);
	glTexCoord2f(1.0f, 1.0f);
	/*Vertex 5*/ glVertex3f(0.0f, 1.0f, 1.0f);
	glTexCoord2f(1.0f, 0.0f);
	/*Vertex 1*/ glVertex3f(0.0f, 1.0f, 0.0f);
	glEnd();

	//Right
	glBegin(GL_QUADS);
	glTexCoord2f(1.0f, 1.0f);
	/*Vertex 6*/ glVertex3f(1.0f, 1.0f, 1.0f);
	glTexCoord2f(0.0f, 1.0f);
	/*Vertex 8*/ glVertex3f(1.0f, 0.0f, 1.0f);
	glTexCoord2f(0.0f, 0.0f);
	/*Vertex 4*/ glVertex3f(1.0f, 0.0f, 0.0f);
	glTexCoord2f(1.0f, 0.0f);
	/*Vertex 2*/ glVertex3f(1.0f, 1.0f, 0.0f);
	glEnd();

	//Up
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	/*Vertex 1*/ glVertex3f(0.0f, 1.0f, 0.0f);
	glTexCoord2f(1.0f, 0.0f);
	/*Vertex 2*/ glVertex3f(1.0f, 1.0f, 0.0f);
	glTexCoord2f(1.0f, 1.0f);
	/*Vertex 6*/ glVertex3f(1.0f, 1.0f, 1.0f);
	glTexCoord2f(0.0f, 1.0f);
	/*Vertex 5*/ glVertex3f(0.0f, 1.0f, 1.0f);
	glEnd();

	//Down
	glBegin(GL_QUADS);
	glTexCoord2f(1.0f, 1.0f);
	/*Vertex 8*/ glVertex3f(1.0f, 0.0f, 1.0f);
	glTexCoord2f(1.0f, 0.0f);
	/*Vertex 4*/ glVertex3f(1.0f, 0.0f, 0.0f);
	glTexCoord2f(0.0f, 0.0f);
	/*Vertex 3*/ glVertex3f(0.0f, 0.0f, 0.0f);
	glTexCoord2f(0.0f, 1.0f);
	/*Vertex 7*/ glVertex3f(0.0f, 0.0f, 1.0f);
	glEnd();

	glPopMatrix();
}

void DrawCylinder()
{
	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, texture[10]);
	gluQuadricDrawStyle(var, GLU_FILL);
	gluQuadricTexture(var, texture[10]);
	gluCylinder(var, 0.5f, 0.5f, 1.3f, 10, 10);
	glPopMatrix();
}

void DrawPyramid()
{
	glPushMatrix();

	glBegin(GL_TRIANGLES);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(1.0f, 0.0f, -1.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-1.0f, 0.0f, -1.0f);
	glTexCoord2f(0.5f, 1.0f);
	glVertex3f(0.0f, 1.0f, 0.0f);
	glEnd();

	glBegin(GL_TRIANGLES);
	glTexCoord2f(0.5f, 1.0f);
	glVertex3f(0.0f, 1.0f, 0.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-1.0f, 0.0f, -1.0f);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-1.0f, 0.0f, 1.0f);
	glEnd();

	glBegin(GL_TRIANGLES);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(1.0f, 0.0f, 1.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-1.0f, 0.0f, 1.0f);
	glTexCoord2f(0.5f, 1.0f);
	glVertex3f(0.0f, 1.0f, 0.0f);
	glEnd();

	glBegin(GL_TRIANGLES);
	glTexCoord2f(0.5f, 1.0f);
	glVertex3f(0.0f, 1.0f, 0.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(1.0f, 0.0f, 1.0f);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(1.0f, 0.0f, -1.0f);
	glEnd();

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-1.0f, 0.0f, 1.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(1.0f, 0.0f, 1.0f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(1.0f, 0.0f, -1.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-1.0f, 0.0f, -1.0f);
	glEnd();

	glPopMatrix();
}

void DrawPyramidRoof()
{
	glPushMatrix();

	glBindTexture(GL_TEXTURE_2D, texture[12]);
	glBegin(GL_TRIANGLES);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(1.0f, 0.0f, -1.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-1.0f, 0.0f, -1.0f);
	glTexCoord2f(0.5f, 1.0f);
	glVertex3f(0.0f, 1.0f, 0.0f);
	glEnd();

	glBegin(GL_TRIANGLES);
	glTexCoord2f(0.5f, 1.0f);
	glVertex3f(0.0f, 1.0f, 0.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-1.0f, 0.0f, -1.0f);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-1.0f, 0.0f, 1.0f);
	glEnd();

	glBegin(GL_TRIANGLES);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(1.0f, 0.0f, 1.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-1.0f, 0.0f, 1.0f);
	glTexCoord2f(0.5f, 1.0f);
	glVertex3f(0.0f, 1.0f, 0.0f);
	glEnd();

	glBegin(GL_TRIANGLES);
	glTexCoord2f(0.5f, 1.0f);
	glVertex3f(0.0f, 1.0f, 0.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(1.0f, 0.0f, 1.0f);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(1.0f, 0.0f, -1.0f);
	glEnd();

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-1.0f, 0.0f, 1.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(1.0f, 0.0f, 1.0f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(1.0f, 0.0f, -1.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-1.0f, 0.0f, -1.0f);
	glEnd();

	glPopMatrix();
}

void DrawCone(GLenum mode)
{
	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, texture[12]);
	gluQuadricDrawStyle(var, mode);
	gluQuadricTexture(var, texture[12]);
	gluCylinder(var, 0.5f, 0.0f, 1.3f, 10, 10);
	glPopMatrix();
}

void DrawLevel()
{
	//ROOM
	glPushMatrix();

	glScalef(2.0f, 1.3f, 2.0f);
	DrawRoom();

	glPopMatrix();

	//Pillar
	glPushMatrix();
	glLineWidth(1.0);
	glRotatef(-90, 1.0f, 0.0f, 0.0f);

	DrawCylinder();

	glPushMatrix();
	glTranslatef(2.0f, 0.0f, 0.0f);
	DrawCylinder();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(2.0f, -2.0f, 0.0f);
	DrawCylinder();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0f, -2.0f, 0.0f);
	DrawCylinder();
	glPopMatrix();

	glPopMatrix();
}

void DrawRoof()
{
	glPushMatrix();
	DrawPyramidRoof();

	DrawPyramidRoof();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(1.0f, 0.0f, 1.0f);
	glRotatef(-90, 1.0f, 0.0f, 0.0f);
	DrawCone(GLU_FILL);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(1.0f, 0.0f, -1.0f);
	glRotatef(-90, 1.0f, 0.0f, 0.0f);
	DrawCone(GLU_FILL);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-1.0f, 0.0f, -1.0f);
	glRotatef(-90, 1.0f, 0.0f, 0.0f);
	DrawCone(GLU_FILL);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-1.0f, 0.0f, 1.0f);
	glRotatef(-90, 1.0f, 0.0f, 0.0f);
	DrawCone(GLU_FILL);
	glPopMatrix();
}

void DrawRoadLine()
{
	glBegin(GL_POLYGON);
	/*Vertex 1*/ glVertex3f(0.0f, 1.0f, 0.0f);
	/*Vertex 2*/ glVertex3f(1.0f, 1.0f, 0.0f);
	/*Vertex 6*/ glVertex3f(1.0f, 1.0f, 1.0f);
	/*Vertex 5*/ glVertex3f(0.0f, 1.0f, 1.0f);
	glEnd();
}

void DrawBridge()
{
	glPushMatrix();

	glColor3f(0.0f, 0.0f, 0.0f);
	//Front
	glBegin(GL_POLYGON);
	/*Vertex 1*/ glVertex3f(0.0f, 1.0f, 0.0f);
	/*Vertex 2*/ glVertex3f(1.0f, 1.0f, 0.0f);
	/*Vertex 4*/ glVertex3f(1.0f, 0.0f, 0.0f);
	/*Vertex 3*/ glVertex3f(0.0f, 0.0f, 0.0f);
	glEnd();

	//Back
	glBegin(GL_POLYGON);
	/*Vertex 5*/ glVertex3f(0.0f, 1.0f, 1.0f);
	/*Vertex 7*/ glVertex3f(0.0f, 0.0f, 1.0f);
	/*Vertex 8*/ glVertex3f(1.0f, 0.0f, 1.0f);
	/*Vertex 6*/ glVertex3f(1.0f, 1.0f, 1.0f);
	glEnd();

	//Left
	glBegin(GL_POLYGON);
	/*Vertex 3*/ glVertex3f(0.0f, 0.0f, 0.0f);
	/*Vertex 7*/ glVertex3f(0.0f, 0.0f, 1.0f);
	/*Vertex 5*/ glVertex3f(0.0f, 1.0f, 1.0f);
	/*Vertex 1*/ glVertex3f(0.0f, 1.0f, 0.0f);
	glEnd();

	//Right
	glBegin(GL_POLYGON);
	/*Vertex 6*/ glVertex3f(1.0f, 1.0f, 1.0f);
	/*Vertex 8*/ glVertex3f(1.0f, 0.0f, 1.0f);
	/*Vertex 4*/ glVertex3f(1.0f, 0.0f, 0.0f);
	/*Vertex 2*/ glVertex3f(1.0f, 1.0f, 0.0f);
	glEnd();

	

	//Down
	glBegin(GL_POLYGON);
	/*Vertex 8*/ glVertex3f(1.0f, 0.0f, 1.0f);
	/*Vertex 4*/ glVertex3f(1.0f, 0.0f, 0.0f);
	/*Vertex 3*/ glVertex3f(0.0f, 0.0f, 0.0f);
	/*Vertex 7*/ glVertex3f(0.0f, 0.0f, 1.0f);
	glEnd();

	glColor3f(1.0f, 1.0f, 1.0f);
	//Up
	glBindTexture(GL_TEXTURE_2D, texture[13]);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	/*Vertex 1*/ glVertex3f(0.0f, 1.0f, 0.0f);
	glTexCoord2f(0, 1);
	/*Vertex 2*/ glVertex3f(1.0f, 1.0f, 0.0f);
	glTexCoord2f(1, 1);
	/*Vertex 6*/ glVertex3f(1.0f, 1.0f, 1.0f);
	glTexCoord2f(1, 0);
	/*Vertex 5*/ glVertex3f(0.0f, 1.0f, 1.0f);
	glEnd();

	glPopMatrix();
}

void DrawStructure()
{
	glPushMatrix();
	glScalef(6.0f, 1.0f, 0.3f);
	glTranslatef(0.0f, 0.0f, 0.5f);
	glColor3f(1.0, 1.0, 1.00);

	//Front
	glBindTexture(GL_TEXTURE_2D, texture[14]);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 1);
	/*Vertex 1*/ glVertex3f(0.0f, 1.0f, 0.0f);
	glTexCoord2f(1, 1);
	/*Vertex 2*/ glVertex3f(1.0f, 1.0f, 0.0f);
	glTexCoord2f(1, 0);
	/*Vertex 4*/ glVertex3f(1.0f, 0.0f, 0.0f);
	glTexCoord2f(0, 0);
	/*Vertex 3*/ glVertex3f(0.0f, 0.0f, 0.0f);
	glEnd();

	//Back
	glBegin(GL_QUADS);
	glTexCoord2f(0, 1);
	/*Vertex 5*/ glVertex3f(0.0f, 1.0f, 1.0f);
	glTexCoord2f(0, 0);
	/*Vertex 7*/ glVertex3f(0.0f, 0.0f, 1.0f);
	glTexCoord2f(1, 0);
	/*Vertex 8*/ glVertex3f(1.0f, 0.0f, 1.0f);
	glTexCoord2f(1, 1);
	/*Vertex 6*/ glVertex3f(1.0f, 1.0f, 1.0f);
	glEnd();

	//Down
	glBegin(GL_QUADS);
	/*Vertex 8*/ glVertex3f(1.0f, 0.0f, 1.0f);
	/*Vertex 4*/ glVertex3f(1.0f, 0.0f, 0.0f);
	/*Vertex 3*/ glVertex3f(0.0f, 0.0f, 0.0f);
	/*Vertex 7*/ glVertex3f(0.0f, 0.0f, 1.0f);
	glEnd();
	
	glPopMatrix();
}

void DrawRoad()
{
	//Road
	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 0.3f);
	glScalef(2.6f, 0.3f, 1.5f);
	
	//Front
	glColor3f(0.0f, 0.0f, 0.0f);
	glBegin(GL_QUADS);
	/*Vertex 1*/ glVertex3f(0.0f, 1.0f, 0.0f);
	/*Vertex 2*/ glVertex3f(1.0f, 1.0f, 0.0f);
	/*Vertex 4*/ glVertex3f(1.0f, 0.0f, 0.0f);
	/*Vertex 3*/ glVertex3f(0.0f, 0.0f, 0.0f);
	glEnd();

	//Back
	glBegin(GL_QUADS);
	/*Vertex 5*/ glVertex3f(0.0f, 1.0f, 1.0f);
	/*Vertex 7*/ glVertex3f(0.0f, 0.0f, 1.0f);
	/*Vertex 8*/ glVertex3f(1.0f, 0.0f, 1.0f);
	/*Vertex 6*/ glVertex3f(1.0f, 1.0f, 1.0f);
	glEnd();

	//Left
	glBegin(GL_QUADS);
	/*Vertex 3*/ glVertex3f(0.0f, 0.0f, 0.0f);
	/*Vertex 7*/ glVertex3f(0.0f, 0.0f, 1.0f);
	/*Vertex 5*/ glVertex3f(0.0f, 1.0f, 1.0f);
	/*Vertex 1*/ glVertex3f(0.0f, 1.0f, 0.0f);
	glEnd();

	//Right
	glBegin(GL_QUADS);
	/*Vertex 6*/ glVertex3f(1.0f, 1.0f, 1.0f);
	/*Vertex 8*/ glVertex3f(1.0f, 0.0f, 1.0f);
	/*Vertex 4*/ glVertex3f(1.0f, 0.0f, 0.0f);
	/*Vertex 2*/ glVertex3f(1.0f, 1.0f, 0.0f);
	glEnd();

	//Down
	glBegin(GL_QUADS);
	/*Vertex 8*/ glVertex3f(1.0f, 0.0f, 1.0f);
	/*Vertex 4*/ glVertex3f(1.0f, 0.0f, 0.0f);
	/*Vertex 3*/ glVertex3f(0.0f, 0.0f, 0.0f);
	/*Vertex 7*/ glVertex3f(0.0f, 0.0f, 1.0f);
	glEnd();

	glColor3f(1.0f, 1.0f, 1.0f);
	//Up
	glBindTexture(GL_TEXTURE_2D, texture[13]);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	/*Vertex 1*/ glVertex3f(0.0f, 1.0f, 0.0f);
	glTexCoord2f(0, 1);
	/*Vertex 2*/ glVertex3f(1.0f, 1.0f, 0.0f);
	glTexCoord2f(1, 1);
	/*Vertex 6*/ glVertex3f(1.0f, 1.0f, 1.0f);
	glTexCoord2f(1, 0);
	/*Vertex 5*/ glVertex3f(0.0f, 1.0f, 1.0f);
	glEnd();

	glPopMatrix();
}

void DrawBottomLevel()
{
	glPushMatrix();

	//Left
	glPushMatrix();
	glScalef(2.0f, 1.3f, 0.25f);
	glTranslatef(0.0f, 0.0f, 0.0f);

	DrawRoom();
	glPopMatrix();

	//Right
	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 1.8f);
	glScalef(2.0f, 1.3f, 0.25f);

	DrawRoom();
	glPopMatrix();

	//Road
	glPushMatrix();
	glTranslatef(-0.4f, 0.0f, -0.06f);
	glScalef(1.2, 1.0f, 1.0f);

	DrawRoad();
	glPopMatrix();

	//Pillar
	glPushMatrix();
	glLineWidth(1.0);
	glRotatef(-90, 1.0f, 0.0f, 0.0f);

	DrawCylinder();

	glPushMatrix();
	glTranslatef(2.0f, 0.0f, 0.0f);
	DrawCylinder();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(2.0f, -2.0f, 0.0f);
	DrawCylinder();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0f, -2.0f, 0.0f);
	DrawCylinder();
	glPopMatrix();

	glPopMatrix();

	glPopMatrix();
}

void DrawLine()
{
	glBegin(GL_LINES);
	glColor3f(0.118, 0.565, 1.00);
	glLineWidth(5);
	glVertex3f(3.0f, 5.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 0.3f);

	glVertex3f(3.0f, 5.0f, 2.1f);
	glVertex3f(0.0f, 0.0f, 1.8f);
	glColor3f(1, 1, 1);
	glEnd();
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	changeView();

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0f, 0.0f, tz);

	if (qNo == 1)
	{
		glRotatef(45, 0.0f, 1.0f, 0.0f);
		glRotatef(angle, 1.0f, 1.0f, 1.0f);
		glColor3f(1.0f, 1.0f, 1.0f);
		glBindTexture(GL_TEXTURE_2D, texture[textureNum]);
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

		glPushMatrix();
		glScalef(5.0f, 5.0f, 5.0f);

		DrawPyramid();

		glPopMatrix();
	}

	if (qNo == 2)
	{
		glRotatef(45, 0.0f, 1.0f, 0.0f);
		glRotatef(angle, 1.0f, 1.0f, 1.0f);
		glColor3f(1.0f, 1.0f, 1.0f);

		glBindTexture(GL_TEXTURE_2D, texture[textureNum]);
		
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

		glPushMatrix();
		glScalef(2.0f, 2.0f, 2.0f);
		DrawCubeQNO2();

		glPopMatrix();
	}

	if (qNo == 3)
	{
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glScalef(10.0f, 10.0f, 10.0f);

		glPushMatrix();
		glRotatef(-10, 1.0f, 0.0f, 0.0f);
		glTranslatef(0.0f, -0.8f, 0.0f);
		glRotatef(angle / 2, 0.0f, 1.0f, 0.0f);
		glRotatef(-90, 1.0f, 0.0f, 0.0f);

		glBindTexture(GL_TEXTURE_2D, texture[3]);
		gluQuadricDrawStyle(var, GLU_FILL);
		gluQuadricTexture(var, texture[3]);
		gluCylinder(var, 0, 0.3, 0.8, 20, 20);

		glPopMatrix();

		//
		glPushMatrix();
		glRotatef(angle, 0.0f, 1.0f, 0.0f);

		glPushMatrix();
		glTranslatef(0.0f, 0.03f, 0.0f);
		glScalef(3.0f, 3.0f, 3.0f);
		glScalef(1.9f, 1.5f, 1.9f);
		glBindTexture(GL_TEXTURE_2D, texture[4]);
		gluQuadricDrawStyle(var, GLU_FILL);
		gluQuadricTexture(var, texture[4]);
		gluSphere(var, 0.05f, 50, 50);
		glPopMatrix();

		glPushMatrix();
		glTranslatef(0.0f, 0.3f, 0.0f);
		glScalef(3.0f, 3.0f, 3.0f);
		glScalef(1.9f, 1.5f, 1.9f);
		glBindTexture(GL_TEXTURE_2D, texture[5]);
		gluQuadricDrawStyle(var, GLU_FILL);
		gluQuadricTexture(var, texture[5]);
		gluSphere(var, 0.04f, 50, 50);
		glPopMatrix();

		//Biscuit stick
		glPushMatrix();
		glTranslatef(0.1f, 0.45f, 0.0f);
		glRotatef(-15, 1.0f, 0.0f, 1.0f);
		glRotatef(-90, 1.0f, 0.0f, 0.0f);
		glBindTexture(GL_TEXTURE_2D, texture[6]);
		gluQuadricDrawStyle(var, GLU_FILL);
		gluQuadricTexture(var, texture[6]);
		gluCylinder(var, 0.05f, 0.05f, 0.2f, 10, 10);

		glPopMatrix();

		//Cherry
		glPushMatrix();

		glTranslatef(-0.1f, 0.4f, 0.15f);
		glRotatef(30, 1.5f, 0.0f, 1.0f);
		glBindTexture(GL_TEXTURE_2D, texture[7]);
		gluQuadricDrawStyle(var, GLU_FILL);
		gluQuadricTexture(var, texture[7]);
		gluSphere(var, 0.07f, 50, 50);

		glBegin(GL_LINES);
		glVertex3f(-0.0f, 0.0f, 0.0f);
		glVertex3f(-0.0f, 0.1f, 0.0f);

		glVertex3f(-0.0f, 0.1f, 0.0f);
		glVertex3f(-0.03f, 0.15f, 0.0f);
		glEnd();
		glPopMatrix();

		glPopMatrix();
	}

	if (qNo == 4)
	{
		glRotatef(angle, 0.0f, 1.0f, 0.0f);
		glClearColor(0.000f, 0.749f, 1.000f, 0.0f);

		//Left Tower
		glPushMatrix();
		glTranslatef(-5.0f, 0.0f, 0.0f);
		DrawLevel();

		glPushMatrix();
		glTranslatef(0.0f, 1.3f, 0.0f);
		DrawLevel();
		glPopMatrix();

		glPushMatrix();
		glTranslatef(0.0f, 2.6f, 0.0f);
		DrawLevel();
		glPopMatrix();

		glPushMatrix();
		glTranslatef(1.0f, 3.9f, 1.0f);
		DrawRoof();
		glPopMatrix();

		glPushMatrix();
		glTranslatef(0.0f, -1.3f, 0.0f);
		DrawBottomLevel();
		glPopMatrix();

		glPushMatrix();
		glTranslatef(-0.75f, -3.3f, -0.75f);
		glScalef(3.5f, 2.0f, 3.5f);
		DrawBase();
		glPopMatrix();

		//Road
		glPushMatrix();
		glTranslatef(10.6f, -1.3f, 0.0f);
		DrawRoad();
		glPopMatrix();

		glPushMatrix();
		glTranslatef(13.2f, -1.3f, 0.0f);
		DrawRoad();
		glPopMatrix();

		glPopMatrix();

		//Line
		glPushMatrix();
		glTranslatef(-8.0f, -1.0f, 0.0f);
		DrawLine();
		glPopMatrix();

		//Right Tower
		glPushMatrix();
		glTranslatef(3.7f, 0.0f, 0.0f);
		DrawLevel();

		glPushMatrix();
		glTranslatef(0.0f, 1.3f, 0.0f);
		DrawLevel();
		glPopMatrix();

		glPushMatrix();
		glTranslatef(0.0f, 2.6f, 0.0f);
		DrawLevel();
		glPopMatrix();

		glPushMatrix();
		glTranslatef(1.0f, 3.9f, 1.0f);
		DrawRoof();
		glPopMatrix();

		glPushMatrix();
		glTranslatef(0.0f, -1.3f, 0.0f);
		DrawBottomLevel();
		glPopMatrix();

		glPushMatrix();
		glTranslatef(-0.75f, -3.3f, -0.75f);
		glScalef(3.5f, 2.0f, 3.5f);
		DrawBase();
		glPopMatrix();

		//Road
		glPushMatrix();
		glTranslatef(-11.5f, -1.3f, 0.0f);
		DrawRoad();
		glPopMatrix();

		glPushMatrix();
		glTranslatef(-14.1f, -1.3f, 0.0f);
		DrawRoad();
		glPopMatrix();

		glPopMatrix();

		//Line
		glPushMatrix();
		glTranslatef(9.0f, -1.0f, 2.1f);
		glRotatef(180, 0.0f, 1.0f, 0.0f);
		DrawLine();
		glPopMatrix();

		//Bridge
		glPushMatrix();
		glTranslatef(-2.7f, -1.3f, 0.25f);
		glRotatef(bridgeAngle, 0.0f, 0.0f, 1.0f);
		glScalef(3.0f, 0.3f, 1.5f);
		DrawBridge();
		glPopMatrix();

		glPushMatrix();
		glTranslatef(3.3f, -1.3f, 0.25f);
		glRotatef(180, 0.0f, 1.0f, 0.0f);
		glRotatef(bridgeAngle, 0.0f, 0.0f, 1.0f);
		glScalef(3.0f, 0.3f, -1.5f);
		DrawBridge();
		glPopMatrix();

		//Structure
		//back
		glPushMatrix();
		glTranslatef(-2.6f, 3.0f, -0.25f);
		DrawStructure();
		glPopMatrix();

		//Fornt
		glPushMatrix();
		glTranslatef(-2.6f, 3.0f, 1.75f);
		DrawStructure();
		glPopMatrix();


		//Sea
		glPushMatrix();
		glTranslatef(-50.0f, -12.0f, -50.0f);
		glScalef(100.0f, 10.0, 100.0f);
		DrawSea();
		glPopMatrix();
	}

	//--------------------------------
	//	End of OpenGL drawing
	//--------------------------------
}
//--------------------------------------------------------------------

void clear()
{
	gluDeleteQuadric(var);

	for (int i = 0; i < sizeof(texture); i++)
	{
		glDeleteTextures(1, &texture[i]);
	}
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int nCmdShow)
{
	WNDCLASSEX wc;
	ZeroMemory(&wc, sizeof(WNDCLASSEX));

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.hInstance = GetModuleHandle(NULL);
	wc.lpfnWndProc = WindowProcedure;
	wc.lpszClassName = WINDOW_TITLE;
	wc.style = CS_HREDRAW | CS_VREDRAW;

	if (!RegisterClassEx(&wc)) return false;

	HWND hWnd = CreateWindow(WINDOW_TITLE, WINDOW_TITLE, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, WINDOW_WIDTH, WINDOW_HIGH,
		NULL, NULL, wc.hInstance, NULL);

	//--------------------------------
	//	Initialize window for OpenGL
	//--------------------------------

	HDC hdc = GetDC(hWnd);

	//	initialize pixel format for the window
	initPixelFormat(hdc);

	//	get an openGL context
	HGLRC hglrc = wglCreateContext(hdc);

	//	make context current
	if (!wglMakeCurrent(hdc, hglrc)) return false;

	//--------------------------------
	//	End initialization
	//--------------------------------

	ShowWindow(hWnd, nCmdShow);

	MSG msg;
	ZeroMemory(&msg, sizeof(msg));

	inti();

	while (true)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT) break;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		display();

		SwapBuffers(hdc);
	}

	clear();

	UnregisterClass(WINDOW_TITLE, wc.hInstance);

	return true;
}
//--------------------------------------------------------------------