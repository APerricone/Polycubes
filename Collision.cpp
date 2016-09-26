#include "Hole.h"

void Hole::InitCollision()
{
}

void Hole::UpdateCollision()
{
	B3D p;
	infOn.Set(100,100,100);
	supOn.Set(-100,-100,-100);
	for(p.x=0;p.x<sideA;p.x++)
		for(p.y=0;p.y<sideB;p.y++)
			for(p.z=0;p.z<depth;p.z++)
			{
				if(GetBOCube(p)->on)
				{
					if(infOn.x>p.x) infOn.x=p.x;
					if(infOn.y>p.y) infOn.y=p.y;
					if(infOn.z>p.z) infOn.z=p.z;
					if(supOn.x<p.x) supOn.x=p.x;
					if(supOn.y<p.y) supOn.y=p.y;
					if(supOn.z<p.z) supOn.z=p.z;
				}
			}
}
//History of CheckCollision Radiosity with 6x6x6 hole...
//4300msec checked a lot of GetBOCube
//3700msec using deltaPointer
//1700msec With deltaplanes
//1100msec Release version
//700msec with bbox of filled zone con ottimizzazione bbox
//500msec Release
//1200msec multi-thread without bbox
//800msec release
//2200msec quality+1
//7800mesc quality+2
//600msec multi-thread with bbox
//500msec Release (500 is the inferior limit)
//1000 Release new lighting
//900 Release without Revert3D
//500 Release with bbox
//800 quality+1
//2300 quality+2 (you can see multithreat-Revert3D=3.39 times more quick)
//9500 quality+2 without bbox
//2700 quality+1 without bbox
//in last we have from 4300 to 900 --> 470%
unsigned long nRay;
bool Hole::CheckCollision(
		const V3D &a,const B3D &cb3d,
		const V3D &b,const B3D &bb3d,bool side)
{
	nRay++;
	//*
	if(!side)
	{
		if(infOn.x>supOn.x)
			return false; //empy hole
		if((cb3d.x<infOn.x)&&(bb3d.x<infOn.x)) return false;
		if((cb3d.y<infOn.y)&&(bb3d.y<infOn.y)) return false;
		if((cb3d.z<infOn.z)&&(bb3d.z<infOn.z)) return false;
		if((cb3d.x>supOn.x)&&(bb3d.x>supOn.x)) return false;
		if((cb3d.y>supOn.y)&&(bb3d.y>supOn.y)) return false;
		if((cb3d.z>supOn.z)&&(bb3d.z>supOn.z)) return false;
	}//*/
	B3D sd(1,1,1);
	V3D d=b-a;
	V3D deltaPlanes(1,1,1);
	if(d.x<0) { sd.x=-1; deltaPlanes.x=-1; }
	if(d.y<0) { sd.y=-1; deltaPlanes.y=-1; }
	if(d.z<0) { sd.z=-1; deltaPlanes.z=-1; }
	V3D nextPlanes;
	V3D distToNextPlanes;
	B3D deltaPointer;//(cb3d.x,cb3d.y,cb3d.z);
	BOCube *bo=GetBOCube(cb3d);
	deltaPointer.x=sd.x;
	deltaPointer.y=sd.y*(sideA+2);
	deltaPointer.z=-sd.z*(sideA+2)*(sideB+2);
	nextPlanes=Get3D(cb3d);
	nextPlanes+=deltaPlanes*0.5f;
	distToNextPlanes.x=(nextPlanes.x-a.x)/d.x;
	distToNextPlanes.y=(nextPlanes.y-a.y)/d.y;
	distToNextPlanes.z=(nextPlanes.z-a.z)/d.z;
	if(d.x==0) distToNextPlanes.x=1e8;
	if(d.y==0) distToNextPlanes.y=1e8;
	if(d.z==0) distToNextPlanes.z=1e8;
	float k;
	while(1)
	{
		if(bo->on)
		{
			return (bo->on!=3);
		}
		if(distToNextPlanes.x<distToNextPlanes.y)
			if(distToNextPlanes.x<distToNextPlanes.z)
			{
				k=distToNextPlanes.x;
				bo+=deltaPointer.x;
				nextPlanes.x+=deltaPlanes.x;
				distToNextPlanes.x=(nextPlanes.x-a.x)/d.x;
			} else
			{
				k=distToNextPlanes.z;
				bo+=deltaPointer.z;
				nextPlanes.z+=deltaPlanes.z;
				distToNextPlanes.z=(nextPlanes.z-a.z)/d.z;
			} else
			if(distToNextPlanes.y<distToNextPlanes.z)
			{
				k=distToNextPlanes.y;
				bo+=deltaPointer.y;
				nextPlanes.y+=deltaPlanes.y;
				distToNextPlanes.y=(nextPlanes.y-a.y)/d.y;
			} else
			{
				k=distToNextPlanes.z;
				bo+=deltaPointer.z;
				nextPlanes.z+=deltaPlanes.z;
				distToNextPlanes.z=(nextPlanes.z-a.z)/d.z;
			}
		if(k>=1-1e-3f) return false;
	}
	return false;
}
