#include "dynamicMesh.h"
#include "systemInterface.h"
#include "game.h"


DynamicMesh::listVertex *DynamicMesh::AddVertexToTempList(const Vertex &v)
{
#ifdef _DEBUG
	if(!Create) { message("DynamicMesh::AddVertexToTempList","ERROR, Wrong moment (Create=false)"); DebugBreak(); }
#endif
	listVertex *u;
	if(tmpVertices)
	{
		u=tmpVertices;
		while(u)
		{
			if(u->v==v) return u;
			u=u->next;
		}
	}
	listVertex *nv=new listVertex();
	nv->v=v;
	nv->next=tmpVertices;
	tmpVertices=nv;
	return nv;
}

DynamicMesh::listTriangle *DynamicMesh::AddTriangleToTempList(listVertex *a,listVertex *b,listVertex *c)
{
#ifdef _DEBUG
	if(!Create) { message("DynamicMesh::AddTriangleToTempList","ERRORE, Wrong moment (Create=false)"); DebugBreak(); }
	if(a==0) { message("DynamicMesh::AddTriangleToTempList","param a == NULL"); DebugBreak(); }
	if(b==0) { message("DynamicMesh::AddTriangleToTempList","param b == NULL"); DebugBreak(); }
	if(c==0) { message("DynamicMesh::AddTriangleToTempList","param c == NULL"); DebugBreak(); }
#endif
	listTriangle *u;
	if(tmpTriangles)
	{
		u=tmpTriangles;
		while(u)
		{
			if((u->a==a)&&(u->b==b)&&(u->c==c))  return u;
			if((u->a==b)&&(u->b==c)&&(u->c==a))  return u;
			if((u->a==c)&&(u->b==a)&&(u->c==b))  return u;
			u=u->next;
		}
	}
	listTriangle *nt=new listTriangle();
	nt->a=a; nt->b=b; nt->c=c;
	nt->next=tmpTriangles;
	tmpTriangles=nt;
	return nt;
}

unsigned short DynamicMesh::FindVertex(const DynamicMesh::Vertex &v)
{
#ifdef _DEBUG
	if(nVertices==0) { message("DynamicMesh::FindVertex","ERROR, non si può cercare nel nulla (nVertices=0)"); DebugBreak(); }
	if(vertices==0) { message("DynamicMesh::FindVertex","ERROR, non si può cercare nel nulla (vertices=0)"); DebugBreak(); }
#endif

	for(unsigned short i=0;i<nVertices;i++)
		if(vertices[i]==v) return i;
	return 0x0FFFF;
}

void DynamicMesh::Begin()
{
#ifdef _DEBUG
	if(Create) { message("DynamicMesh::Begin","ERROR, wrong moment (Create=true)"); DebugBreak(); }
	if(nVertices) { message("DynamicMesh::Begin","ERROR, already init (nVertices!=0)"); DebugBreak(); }
	if(nTriangle) { message("DynamicMesh::Begin","ERROR, already init (nTriangle!=0)"); DebugBreak(); }
#endif
	Create=true;
	tmpTriangles=NULL;
	tmpVertices=NULL;
}

void DynamicMesh::AddTrianglePos(const Vertex &a,const Vertex &b,const Vertex &c)
{
#ifdef _DEBUG
	if(!Create) { message("DynamicMesh::AddTrianglePos","ERROR, wrong moment(Create=false)"); DebugBreak(); }
#endif
	float la=(a.pos-b.pos).GetLen();
	float lb=(b.pos-c.pos).GetLen();
	float lc=(c.pos-a.pos).GetLen();
	float semip=(la+lb+lc)/2;
	float area=sqrtf(semip*(semip-la)*(semip-lb)*(semip-lc));
	if(area<1e-6) return;
	AddTriangleToTempList(
		AddVertexToTempList(a),
		AddVertexToTempList(b),
		AddVertexToTempList(c));
}

void DynamicMesh::TransformAll(const Matrix& m)
{
	V3D t;
	if(Create)
	{
		listVertex *uv;
		uv=tmpVertices;
		while(uv)
		{
			t=uv->v.pos;  m.Transform(&uv->v.pos,t);
			//t=uv->v.nor;  m.TransformNormal(&uv->v.nor,t);
			uv=uv->next;
		}
	} else
	{
		unsigned short i;
		for(i=0;i<nVertices;i++)
		{
			t=vertices[i].pos;  m.Transform(&vertices[i].pos,t);
			//t=vertices[i].nor;  m.TransformNormal(&vertices[i].nor,t);
		}
	}
}
void DynamicMesh::CalcBox(Box3 *d)
{
	d->Reset();
	if(Create)
	{
		listVertex *uv=tmpVertices;
		while(uv)
		{
			d->Add(uv->v.pos);
			uv=uv->next;
		}
	} else
	{
		for(int i=0;i<nVertices;i++)
			d->Add(vertices[i].pos);

	}
}

void DynamicMesh::End()
{
#ifdef _DEBUG
	if(!Create) { message("DynamicMesh::End","ERRORE, wrong moment (Create=false)"); DebugBreak(); }
	if(tmpVertices==0) { message("DynamicMesh::End","ERROR, eh? (tmpVertices=0)"); DebugBreak(); }
	if(tmpTriangles==0) { message("DynamicMesh::End","ERROR, eh? (tmpTriangles=0)"); DebugBreak(); }
#endif
	Create=false;
	listVertex *uv;
	listTriangle *ut;

	nVertices=nTriangle=0;
	uv=tmpVertices;
	while(uv) { nVertices++; uv=uv->next; }
	ut=tmpTriangles;
	while(ut) { nTriangle++; ut=ut->next; }

	vertices=new Vertex[nVertices];
	indices=new unsigned short[nTriangle*3];

	nVertices=nTriangle=0;
	uv=tmpVertices;
	while(uv) { vertices[nVertices]=uv->v; nVertices++; uv=uv->next;  }
	ut=tmpTriangles;
	while(ut)
	{
		indices[nTriangle*3+0]=FindVertex(ut->a->v);
		indices[nTriangle*3+1]=FindVertex(ut->b->v);
		indices[nTriangle*3+2]=FindVertex(ut->c->v);
		nTriangle++;
		ut=ut->next;
	}


	while(tmpVertices)
	{
		uv=tmpVertices->next;
		delete tmpVertices;
		tmpVertices=uv;
	}
	ut=tmpTriangles;
	while(tmpTriangles)
	{
		ut=tmpTriangles->next;
		delete tmpTriangles;
		tmpTriangles=ut;
	}
}

void DynamicMesh::Draw()
{
#ifndef NOGL
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glVertexPointer(3,GL_FLOAT,sizeof(Vertex),&vertices->pos);
	glTexCoordPointer(2,GL_FLOAT,sizeof(Vertex),&vertices->tex);
	glDrawElements(GL_TRIANGLES,nTriangle*3,GL_UNSIGNED_SHORT,indices);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
	/*
	unsigned short i,j;
	glDisable(GL_TEXTURE_2D);
	glColor3f(1,1,0);
	glBegin(GL_LINES);
	for(j=i=0;i<nTriangle;i++,j+=3)
	{
		glVertex3fv((float*)&vertices[indices[j+0]].pos); glVertex3fv((float*)&vertices[indices[j+1]].pos);
		glVertex3fv((float*)&vertices[indices[j+1]].pos); glVertex3fv((float*)&vertices[indices[j+2]].pos);
		glVertex3fv((float*)&vertices[indices[j+2]].pos); glVertex3fv((float*)&vertices[indices[j+0]].pos);
	}
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glColor3f(1,1,1);
	//*/
#endif
}

void DynamicMesh::Deinit()
{
	listVertex *uv;
	listTriangle *ut;
	while(tmpVertices)
	{
		uv=tmpVertices->next;
		delete tmpVertices;
		tmpVertices=uv;
	}
	ut=tmpTriangles;
	while(tmpTriangles)
	{
		ut=tmpTriangles->next;
		delete tmpTriangles;
		tmpTriangles=ut;
	}

	if(vertices) delete [] vertices;
	if(indices) delete [] indices;
	vertices=0; indices=0;
	nVertices=0; nTriangle=0;
}

DynamicCage::listVertex *DynamicCage::AddVertexToTempList(const V3D &v)
{
#ifdef _DEBUG
	if(!Create) { message("DynamicCage::AddVertexToTempList","ERROR, wrong moment (Create=false)"); DebugBreak(); }
#endif
	listVertex *u;
	if(tmpVertices)
	{
		u=tmpVertices;
		while(u)
		{
			if(u->v==v) return u;
			u=u->next;
		}
	}
	listVertex *nv=new listVertex();
	nv->v=v;
	nv->next=tmpVertices;
	tmpVertices=nv;
	return nv;
}

DynamicCage::listLines *DynamicCage::AddLineToTempList(listVertex *a,listVertex *b)
{
#ifdef _DEBUG
	if(!Create) { message("DynamicCage::AddLineToTempList","ERROR, wrong moment (Create=false)"); DebugBreak(); }
	if(a==0) { message("DynamicCage::AddLineToTempList","param a == NULL"); DebugBreak(); }
	if(b==0) { message("DynamicCage::AddLineToTempList","param b == NULL"); DebugBreak(); }
#endif
	listLines *u;
	if(tmpLines)
	{
		u=tmpLines;
		while(u)
		{
			if((u->a==a)&&(u->b==b)) return u;
			if((u->a==b)&&(u->b==a)) return u;
			u=u->next;
		}
	}
	listLines *nl=new listLines();
	nl->next=0;
	nl->a=a; nl->b=b;
	nl->next=tmpLines;
	tmpLines=nl;
	return nl;
}

unsigned short DynamicCage::FindVertex(const V3D &v)
{
#ifdef _DEBUG
	if(nVertices==0) { message("DynamicMesh::FindVertex","ERROR, You can not search in an empty place (nVertices=0)"); DebugBreak(); }
	if(vertices==0) { message("DynamicMesh::FindVertex","ERROR, You can not search in an empty place (vertices=0)"); DebugBreak(); }
#endif

	for(unsigned short i=0;i<nVertices;i++)
		if(vertices[i]==v) return i;
	return 0x0FFFF;
}

void DynamicCage::Begin()
{
#ifdef _DEBUG
	if(Create) { message("DynamicCage::Begin","ERROR, wrong moment (Create=true)"); DebugBreak(); }
	if(nVertices) { message("DynamicCage::Begin","ERROR, already do (nVertices!=0)"); DebugBreak(); }
	if(nLines) { message("DynamicCage::Begin","ERROR, already do (nLines!=0)"); DebugBreak(); }
#endif
	Create=true;
	tmpLines=NULL;
	tmpVertices=NULL;
}

void DynamicCage::AddLine(const V3D &a,const V3D &b)
{
#ifdef _DEBUG
	if(!Create) { message("DynamicMesh::AddTrianglePos","ERROR, wrong moment (Create=false)"); DebugBreak(); }
#endif
	float l=(a-b).GetLen();
	if(l<1e-6) return;
	AddLineToTempList(
		AddVertexToTempList(a),
		AddVertexToTempList(b));
}

void DynamicCage::TransformAll(const Matrix& m)
{
	V3D t;
	if(Create)
	{
		listVertex *uv;
		uv=tmpVertices;
		while(uv)
		{
			t=uv->v;  m.Transform(&uv->v,t);
			uv=uv->next;
		}
	} else
	{
		unsigned short i;
		for(i=0;i<nVertices;i++)
		{
			t=vertices[i];  m.Transform(&vertices[i],t);
		}
	}
}

void DynamicCage::CalcBox(Box3 *d)
{
	d->Reset();
	if(Create)
	{
		listVertex *uv=tmpVertices;
		while(uv)
		{
			d->Add(uv->v);
			uv=uv->next;
		}
	} else
	{
		for(int i=0;i<nVertices;i++)
			d->Add(vertices[i]);

	}
}

void DynamicCage::End()
{
#ifdef _DEBUG
	if(!Create) { message("DynamicCage::End","ERROR, wrong moment (Create=false)"); DebugBreak(); }
	if(tmpVertices==0) { message("DynamicCage::End","ERROR, eh? (tmpVertices=0)"); DebugBreak(); }
	if(tmpLines==0) { message("DynamicCage::End","ERROR, eh? (tmpLines=0)"); DebugBreak(); }
#endif
	Create=false;
	listVertex *uv;
	listLines *ul;

	nVertices=nLines=0;
	uv=tmpVertices;
	while(uv) { nVertices++; uv=uv->next; }
	ul=tmpLines;
	while(ul) { nLines++; ul=ul->next; }

	vertices=new V3D[nVertices];
	indices=new unsigned short[nLines*2];

	nVertices=nLines=0;
	uv=tmpVertices;
	while(uv) { vertices[nVertices]=uv->v; nVertices++; uv=uv->next;  }
	ul=tmpLines;
	while(ul)
	{
		indices[nLines*2+0]=FindVertex(ul->a->v);
		indices[nLines*2+1]=FindVertex(ul->b->v);
		nLines++;
		ul=ul->next;
	}

	while(tmpVertices)
	{
		uv=tmpVertices->next;
		delete tmpVertices;
		tmpVertices=uv;
	}
	ul=tmpLines;
	while(tmpLines)
	{
		ul=tmpLines->next;
		delete tmpLines;
		tmpLines=ul;
	}
}

void DynamicCage::Draw()
{
#ifndef NOGL
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3,GL_FLOAT,sizeof(V3D),vertices);
	glDrawElements(GL_LINES,nLines*2,GL_UNSIGNED_SHORT,indices);
	glDisableClientState(GL_VERTEX_ARRAY);
#endif
}

void DynamicCage::Deinit()
{
	listVertex *uv;
	listLines *ul;
	while(tmpVertices)
	{
		uv=tmpVertices->next;
		delete tmpVertices;
		tmpVertices=uv;
	}
	ul=tmpLines;
	while(tmpLines)
	{
		ul=tmpLines->next;
		delete tmpLines;
		tmpLines=ul;
	}

	if(vertices) delete [] vertices;
	if(indices) delete [] indices;
	vertices=0; indices=0;
	nVertices=0; nLines=0;
}
