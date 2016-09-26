#include "menuElements.h"
#include "Image.h"
#include "systeminterface.h"
#include "game.h"

void Circles::Draw(float cx,float cy,float r)
{
	glDisable(GL_TEXTURE_2D);
	glLineWidth(3);
	glColor4f(0,0,0,1);
	long nStep=r*3.1416f/10;//every line is 20 pixel
	if(nStep<3) nStep=3;
	glBegin(GL_LINE_LOOP);
	for(long i=0;i<nStep;i++)
	{
		glVertex2f(
			cx+sinf(2*3.1416f*i/nStep)*r,
			cy-cosf(2*3.1416f*i/nStep)*r);
	}
	glEnd();
	glLineWidth(1);
}

void Circles::DrawFill(float cx,float cy,float r,float angle1,float angle2)
{
	glDisable(GL_TEXTURE_2D);
	glLineWidth(3);
	glColor4f(0,0,0,1);
	float delta=(angle2-angle1);
	long nStep=r*fabsf(delta)/20; //every line is 20 pixel
	if(nStep<3) nStep=3;
	glBegin(GL_TRIANGLE_FAN);
	glVertex2f(cx,cy);
	for(long i=0;i<=nStep;i++)
	{
		glVertex2f(
			cx+sinf(angle1+delta*i/nStep)*r,
			cy-cosf(angle1+delta*i/nStep)*r);
	}
	glEnd();
}
