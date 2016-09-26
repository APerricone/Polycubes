#include "menuElements.h"
#include "Image.h"
#include "systeminterface.h"
#include "game.h"

void Arrow2D::DrawUp(float x0,float y0,float x1,float y1,bool white,bool hole)
{
	glDisable(GL_TEXTURE_2D);
	if(white) glColor4f(1,1,1,1); else glColor4f(0,0,0,1);
	if(hole)
	{
		glLineWidth(3);
		glBegin(GL_LINE_LOOP);
	}
	else
		glBegin(GL_TRIANGLE_STRIP);
	float nx=(x1+x0)*0.5f;
	glVertex2f(nx,y0);
	glVertex2f(x0,y1);
	glVertex2f(x1,y1);
	glEnd();
	if(hole) glLineWidth(1);
}

void Arrow2D::DrawDown(float x0,float y0,float x1,float y1,bool white,bool hole)
{
	glDisable(GL_TEXTURE_2D);
	if(white) glColor4f(1,1,1,1); else glColor4f(0,0,0,1);
	if(hole)
	{
		glLineWidth(3);
		glBegin(GL_LINE_LOOP);
	}
	else
		glBegin(GL_TRIANGLE_STRIP);
	float nx=(x1+x0)*0.5f;
	glVertex2f(x0,y0);
	glVertex2f(x1,y0);
	glVertex2f(nx,y1);
	glEnd();
	if(hole) glLineWidth(1);
}

void Arrow2D::DrawRight(float x0,float y0,float x1,float y1,bool white,bool hole)
{
	glDisable(GL_TEXTURE_2D);
	if(white) glColor4f(1,1,1,1); else glColor4f(0,0,0,1);
	if(hole)
	{
		glLineWidth(3);
		glBegin(GL_LINE_LOOP);
	}
	else
		glBegin(GL_TRIANGLE_STRIP);
	float ny=(y1+y0)*0.5f;
	glVertex2f(x0,y0);
	glVertex2f(x0,y1);
	glVertex2f(x1,ny);
	glEnd();
	if(hole) glLineWidth(1);
}

void Arrow2D::DrawLeft(float x0,float y0,float x1,float y1,bool white,bool hole)
{
	glDisable(GL_TEXTURE_2D);
	if(white) glColor4f(1,1,1,1); else glColor4f(0,0,0,1);
	if(hole)
	{
		glLineWidth(3);
		glBegin(GL_LINE_LOOP);
	}
	else
		glBegin(GL_TRIANGLE_STRIP);
	float ny=(y1+y0)*0.5f;
	glVertex2f(x0,ny);
	glVertex2f(x1,y0);
	glVertex2f(x1,y1);
	glEnd();
	if(hole) glLineWidth(1);
}
