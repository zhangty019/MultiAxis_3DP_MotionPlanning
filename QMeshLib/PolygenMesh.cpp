// PMBody.cpp: implementation of the PMBody class.
//
//////////////////////////////////////////////////////////////////////
#define _CRT_SECURE_NO_DEPRECATE

#include <math.h>
#include <memory.h>

#include "PolygenMesh.h"

#include <QDebug>

#define PI		3.141592654
#define DEGREE_TO_ROTATE(x)		0.0174532922222*x
#define ROTATE_TO_DEGREE(x)		57.295780490443*x

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

PolygenMesh::PolygenMesh(mesh_type type)
{
    ClearAll();
    m_drawListID=-1;
    m_bVertexNormalShading=false;
    isTransparent = false;
    m_drawListNumber = 6;
    meshType = type;
}

PolygenMesh::~PolygenMesh()
{
    ClearAll();
    if (m_drawListID!=-1) glDeleteLists(m_drawListID, m_drawListNumber);
}

//////////////////////////////////////////////////////////////////////
// Implementation
//////////////////////////////////////////////////////////////////////

void PolygenMesh::CompBoundingBox(double boundingBox[])
{
    GLKPOSITION PosMesh;
    GLKPOSITION Pos;
    double xx,yy,zz;

    boundingBox[0]=boundingBox[2]=boundingBox[4]=1.0e+32;
    boundingBox[1]=boundingBox[3]=boundingBox[5]=-1.0e+32;

    for(PosMesh=meshList.GetHeadPosition();PosMesh!=NULL;) {
        QMeshPatch *mesh=(QMeshPatch *)(meshList.GetNext(PosMesh));
        for(Pos=mesh->GetNodeList().GetHeadPosition();Pos!=NULL;) {
            QMeshNode *node=(QMeshNode *)(mesh->GetNodeList().GetNext(Pos));
            node->GetCoord3D(xx,yy,zz);

            if (xx<boundingBox[0]) boundingBox[0]=xx;
            if (xx>boundingBox[1]) boundingBox[1]=xx;
            if (yy<boundingBox[2]) boundingBox[2]=yy;
            if (yy>boundingBox[3]) boundingBox[3]=yy;
            if (zz<boundingBox[4]) boundingBox[4]=zz;
            if (zz>boundingBox[5]) boundingBox[5]=zz;
        }
    }
}

void PolygenMesh::DeleteGLList()
{
    if (m_drawListID!=-1) {
        glDeleteLists(m_drawListID, m_drawListNumber);
        m_drawListID=-1;
    }
}

void PolygenMesh::BuildGLList(bool bVertexNormalShading)
{
    if (m_drawListID!=-1) glDeleteLists(m_drawListID, m_drawListNumber);
    m_drawListID = glGenLists(m_drawListNumber);

    _buildDrawShadeList(bVertexNormalShading);
    _buildDrawMeshList();
    _buildDrawNodeList();
    _buildDrawProfileList();
    _buildDrawFaceNormalList();
    _buildDrawNodeNormalList();
    computeRange();
}

void PolygenMesh::_buildDrawShadeList(bool bVertexNormalShading)
{
    glNewList(m_drawListID, GL_COMPILE);

    glEnable(GL_NORMALIZE);
    glEnable(GL_LIGHTING);

    //drawOriginalCoordinate();
    if (isTransparent) {
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_FALSE);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
    
    /*------------- Draw Face -------------*/
    if (this->meshType == LAYER) {
        for (GLKPOSITION Pos = meshList.GetHeadPosition(); Pos != NULL; ) {
            QMeshPatch* mesh = (QMeshPatch*)(meshList.GetNext(Pos));
            float rr, gg, bb;

            if (mesh->drawThisPatch == false) continue;

            glBegin(GL_TRIANGLES);
            for (GLKPOSITION PosFace = (mesh->GetFaceList()).GetHeadPosition(); PosFace != NULL;) {
                QMeshFace* face = (QMeshFace*)((mesh->GetFaceList()).GetNext(PosFace));

                if (mesh->isSupportLayer) {
                    glColor3f(0.8, 0.8, 0.8);
                }
                else {
                    _changeValueToColor(mesh->GetIndexNo(), rr, gg, bb);
                    glColor3f(rr, gg, bb);
                }
                this->drawSingleFace(face);

            }
            glEnd();
        }
    }
    
    /*------------- Draw Face -------------*/
    if (this->meshType == CNC_PARTS) {

        for (GLKPOSITION Pos = meshList.GetHeadPosition(); Pos != NULL; ) {
            QMeshPatch* mesh = (QMeshPatch*)(meshList.GetNext(Pos));
            float rr, gg, bb;

            if (mesh->drawThisPatch == false) continue;

            glBegin(GL_TRIANGLES);
            for (GLKPOSITION PosFace = (mesh->GetFaceList()).GetHeadPosition(); PosFace != NULL;) {
                QMeshFace* face = (QMeshFace*)((mesh->GetFaceList()).GetNext(PosFace));

                glColor3f(0.8, 0.8, 0.8);

                if (this->getModelName() == "PrintPlatform")    glColor3f(0.3, 0.3, 0.3);
                if (mesh->patchName == "X_axis")                glColor3f(0.2, 0.5, 0.8);
                if (mesh->patchName == "Y_axis")                glColor3f(0.5, 0.5, 0.5);
                if (mesh->patchName == "Z_axis")                glColor3f(0.6, 0.6, 0.6);
                if (mesh->patchName == "B_axis")                glColor3f(0.5, 0.5, 0.5);
                if (mesh->patchName == "base")                  glColor3f(0.2, 0.2, 0.2);
                if (this->getModelName() == "ExtruderHead")     glColor3f(0.8, 0.8, 0.8);

                this->drawSingleFace(face);

            }
            glEnd();
        }
    }

    if (isTransparent) {
        glDisable(GL_BLEND);
        glDepthMask(GL_TRUE);
    }

    glEndList();
}

void PolygenMesh::_changeValueToColor(int nType, float & nRed, float & nGreen, float & nBlue)
{
    float color[][3]={
        {220,20,60},
        {107,200,35},
        {30,144,255},
        {255,105,180},
        {244,164,96},
        {176,196,222},
        {255,100,70},
        {128,255,128},
        {128,128,255},
        {255,255,128},
        {0,128,0},
        {255,128,255},
        {255,214,202},
        {128,128,192},
        {255,165,0}, //orange
        {255,128,192},
//		{39, 64, 139},//RoyalBlue
        {128,128,64},
        {0,255,255},
        {238,130,238},//violet
        {220,220,220},//gainsboro
        {188, 143, 143}, // rosy brown
        {46, 139, 87},//sea green
        {210, 105, 30 },//chocolate
        {237, 150, 100},
        {100, 149, 237},//cornflower blue
        {243, 20, 100},
        // 26th
        {0,0,0}
    };

//	printf("%d ",nType);
    nRed=color[nType%25][0]/255.0f;
    nGreen=color[nType%25][1]/255.0f;
    nBlue=color[nType%25][2]/255.0f;
}

void PolygenMesh::_buildDrawMeshList()
{
    GLKPOSITION Pos;
    GLKPOSITION PosEdge;
    float rr, gg, bb;

    if (meshList.GetCount()==0) return;

    glNewList(m_drawListID+1, GL_COMPILE);
    glDisable(GL_LIGHTING);
    // glLineWidth(0.7);
    if (edgeColor)  glLineWidth(1.0);
    glLineWidth(1.0);

    if (this->meshType == WAYPOINT) {
        glLineWidth(10.0);
        for (GLKPOSITION Pos = meshList.GetHeadPosition(); Pos != NULL; ) {
            QMeshPatch* mesh = (QMeshPatch*)(meshList.GetNext(Pos));

            if (mesh->drawThisPatch == false) continue;

            glBegin(GL_LINES);

            for (GLKPOSITION PosEdge = (mesh->GetEdgeList()).GetHeadPosition(); PosEdge != NULL;) {
                QMeshEdge* edge = (QMeshEdge*)((mesh->GetEdgeList()).GetNext(PosEdge));


                if (edge->GetStartPoint()->Jump_preSecEnd == false && edge->GetEndPoint()->Jump_nextSecStart == false) {
                    if (mesh->isSupportLayer) glColor3f(0.6, 0.6, 0.6);
                    else glColor3f(0.05, 0.4, 0.2);
                    
                }
                else if ((edge->GetStartPoint()->Jump_preSecEnd == true && edge->GetEndPoint()->Jump_nextSecStart == true) 
                    || (edge->GetEndPoint()->Jump_preSecEnd == true && edge->GetStartPoint()->Jump_nextSecStart == true)) {
                    if (mesh->isSupportLayer) glColor3f(0.0, 0.4, 0.9); 
                    else glColor3f(0.9, 0.4, 0);

                }

                if(edge->isCollision) glColor3f(1.0, 0.0, 0.0);

                this->drawSingleEdge(edge);
            }
            glEnd();
        }
    }

    if (this->meshType == CNC_PARTS) {
        glLineWidth(1.0);
        for (GLKPOSITION Pos = meshList.GetHeadPosition(); Pos != NULL; ) {
            QMeshPatch* mesh = (QMeshPatch*)(meshList.GetNext(Pos));

            if (mesh->drawThisPatch == false) continue;

            glBegin(GL_LINES);

            for (GLKPOSITION PosEdge = (mesh->GetEdgeList()).GetHeadPosition(); PosEdge != NULL;) {
                QMeshEdge* edge = (QMeshEdge*)((mesh->GetEdgeList()).GetNext(PosEdge));

                // only show the boundary eadge
                Eigen::Vector3d normal_Lface, normal_Rface;
                edge->GetLeftFace()->GetNormal(normal_Lface(0), normal_Lface(1), normal_Lface(2));
                normal_Lface = normal_Lface.normalized();
                edge->GetRightFace()->GetNormal(normal_Rface(0), normal_Rface(1), normal_Rface(2));
                normal_Rface = normal_Rface.normalized();

                double radCOS = normal_Lface.dot(normal_Rface);
                double angle = ROTATE_TO_DEGREE(acos(radCOS));

                if (angle > 25.0) {
                    rr = 0.1; gg = 0.1; bb = 0.1;
                    glColor3f(rr, gg, bb);
                    this->drawSingleEdge(edge);
                }

            }

            glEnd();
        }
    }

    glEndList();
}

void PolygenMesh::_buildDrawNodeList()
{
    GLKPOSITION Pos;
    GLKPOSITION PosNode;
    float rr, gg, bb;

    if (meshList.GetCount()==0) return;

    glNewList(m_drawListID+2, GL_COMPILE);
    glDisable(GL_LIGHTING);
    glEnable(GL_POINT_SMOOTH);
    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

    if (this->meshType == WAYPOINT) {
        glPointSize(8.0);

        for (GLKPOSITION Pos = meshList.GetHeadPosition(); Pos != NULL; ) {
            QMeshPatch* mesh = (QMeshPatch*)(meshList.GetNext(Pos));

            if (mesh->drawThisPatch == false) continue;

            glBegin(GL_POINTS);

            for (GLKPOSITION PosNode = (mesh->GetNodeList()).GetHeadPosition(); PosNode != NULL;) {
                QMeshNode* node = (QMeshNode*)((mesh->GetNodeList()).GetNext(PosNode));
                // for initial layers
                rr = 0.1; gg = 0.8; bb = 0.8;
                // for support layers
                if (mesh->isSupportLayer) { rr = 0.75; gg = 0.75; bb = 0.75; }

                // for the original part Z height
                if (node->negativeZz) { rr = 1.0; gg = 0.5; bb = 0.2; }

                // for the endpoint
//                if (node->GetIndexNo() == 0) { rr = 0.8; gg = 0.8; bb = 0; }
//                if (node->GetIndexNo() == (mesh->GetNodeNumber() - 1)) { rr = 0.39; gg = 0.58; bb = 0.93; }

                // for jump endpoint
//                if (node->Jump_preSecEnd) { rr = 0; gg = 0; bb = 1; }
//                if (node->Jump_nextSecStart) { rr = 0; gg = 1; bb = 0; }

                // for collision detection.
//                if (node->isCollision) { rr = 1; gg = 0; bb = 0; }
//                if (node->iscollided) { rr = 0; gg = 0; bb = 0; }

                // for solve selection
                if (mesh->drawSolveSelect) {
                    if (node->solveSeclct == 1) { rr = 1; gg = 0; bb = 0; }
                    else if (node->solveSeclct == 2) { rr = 0; gg = 0; bb = 1; }
                    else { rr = 0.1; gg = 0.7; bb = 0.5; }
                }

                // for singular checking
                if (mesh->drawSingularity) {
                    if (node->isSingularNode) {
                        rr = 0.99; gg = 0.70; bb = 0.00; }
                    else if (node->isSingularSecEndNode || node->isSingularSecStartNode) {
                        //rr = 0.2; gg = 0.2; bb = 0.3; 
                    }
                    else {}
                }

                glColor3f(rr, gg, bb);
                drawSingleNode(node);
            }
            glEnd();
        }
    }

    if (this->meshType == CNC_PARTS) {
        glPointSize(4.0);
        for (GLKPOSITION Pos = meshList.GetHeadPosition(); Pos != NULL; ) {
            QMeshPatch* mesh = (QMeshPatch*)(meshList.GetNext(Pos));

            glBegin(GL_POINTS);

            for (GLKPOSITION PosNode = (mesh->GetNodeList()).GetHeadPosition(); PosNode != NULL;) {
                QMeshNode* node = (QMeshNode*)((mesh->GetNodeList()).GetNext(PosNode));

                if (node->isPrintPnt) {
                    rr = 1; gg = 0; bb = 0;
                    //_changeValueToColor(node->m_BoxPos(3), rr, gg, bb);
                    glColor3f(rr, gg, bb);
                    drawSingleNode(node);
                }
            }
            glEnd();
        }
    }



    glEndList();
}

void PolygenMesh::_buildDrawProfileList()
{
//    GLKPOSITION Pos;
//    GLKPOSITION PosEdge;
//    double xx,yy,zz;

//    if (meshList.GetCount()==0) return;

//    glNewList(m_drawListID+3, GL_COMPILE);
}

void PolygenMesh::_buildDrawFaceNormalList()
{
    if (meshList.GetCount() == 0) return;
    if (this->meshType == CNC_PARTS || this->meshType == WAYPOINT) return;

    glNewList(m_drawListID + 4, GL_COMPILE);
    glDisable(GL_LIGHTING);

    glColor3f(0.5, 0.0, 0.5);

    glLineWidth(1.0);

    if ((this->meshType == LAYER)) {
        glBegin(GL_LINES);
        for (GLKPOSITION meshPos = meshList.GetHeadPosition(); meshPos != NULL;) {
            QMeshPatch* mesh = (QMeshPatch*)(meshList.GetNext(meshPos));

            if (mesh->drawThisPatch == false) continue;

            QMeshEdge* edge = (QMeshEdge*)mesh->GetEdgeList().GetHead();
            double length = edge->CalLength();
            for (GLKPOSITION Pos = mesh->GetFaceList().GetHeadPosition(); Pos != NULL;) {
                QMeshFace* face = (QMeshFace*)(mesh->GetFaceList().GetNext(Pos));
                double x, y, z, nx, ny, nz;
                face->CalCenterPos(x, y, z);
                face->CalPlaneEquation();
                face->GetNormal(nx, ny, nz);
                glVertex3d(x, y, z);
                glVertex3d(x + nx * length, y + ny * length, z + nz * length);
            }
        }
        glEnd();
    }

    glEndList();
}

void PolygenMesh::_buildDrawNodeNormalList()
{
    if (meshList.GetCount() == 0) return;
    if (this->meshType == CNC_PARTS || this->meshType == LAYER) return;

    glNewList(m_drawListID + 5, GL_COMPILE);
    glDisable(GL_LIGHTING);

    glColor3f(0.0, 0.5, 0.0);

    glLineWidth(1.0);

    if (this->meshType == WAYPOINT) {
        glBegin(GL_LINES);
        for (GLKPOSITION meshPos = meshList.GetHeadPosition(); meshPos != NULL;) {
            QMeshPatch* mesh = (QMeshPatch*)(meshList.GetNext(meshPos));

            if (mesh->drawThisPatch == false) continue;

            QMeshEdge* edge = (QMeshEdge*)mesh->GetEdgeList().GetHead();
            double length = edge->CalLength();

            for (GLKPOSITION Pos = mesh->GetNodeList().GetHeadPosition(); Pos != NULL;) {
                QMeshNode* node = (QMeshNode*)(mesh->GetNodeList().GetNext(Pos));
                double x, y, z, nx, ny, nz;
                node->GetCoord3D(x, y, z);
                node->GetNormal(nx, ny, nz);

                glVertex3d(x, y, z);
                glVertex3d(x + nx * length, y + ny * length, z + nz * length);
            }
        }
        glEnd();
    }
    
    glEndList();
}

void PolygenMesh::drawOriginalCoordinate() {
	double axisLeng = 30.0;
	glLineWidth(2.0);
	glBegin(GL_LINES);

	// X-axis - Red Color
	glColor3f(1.0, 0.0, 0.0); glVertex3d(0.0, 0.0, 0.0);
	glVertex3d(axisLeng, 0.0, 0.0);

	// Y-axis - green Color
	glColor3f(0.0, 1.0, 0.0);
	glVertex3d(0.0, 0.0, 0.0); glVertex3d(0.0, axisLeng, 0.0);

	// Z-axis - black Color
	glColor3f(0.0, 0.0, 0.0);
	glVertex3d(0.0, 0.0, 0.0); glVertex3d(0.0, 0.0, axisLeng);

	//glColor3f(1, 0.1, 0.1);glVertex3d(0.0, 0.0, 0.0);
	//glVertex3d(-7.72805,- 11.4536,- 3.48036); //voxel searching debug

	glEnd();

	
}

void PolygenMesh::drawShade()
{
    if (meshList.IsEmpty()) {glDeleteLists(m_drawListID, m_drawListNumber); m_drawListID=-1; return;}
    glCallList(m_drawListID);
}

void PolygenMesh::drawMesh()
{
    if (meshList.IsEmpty()) {glDeleteLists(m_drawListID, m_drawListNumber); m_drawListID=-1; return;}
    glCallList(m_drawListID+1);
}

void PolygenMesh::drawNode()
{
    if (meshList.IsEmpty()) {glDeleteLists(m_drawListID, m_drawListNumber); m_drawListID=-1; return;}
    glCallList(m_drawListID+2);
}

void PolygenMesh::drawProfile()
{
    if (meshList.IsEmpty()) {glDeleteLists(m_drawListID, m_drawListNumber); m_drawListID=-1; return;}
    glCallList(m_drawListID+3);
}

void PolygenMesh::drawFaceNormal()
{
    if (meshList.IsEmpty()) {glDeleteLists(m_drawListID, m_drawListNumber); m_drawListID=-1; return;}
    glCallList(m_drawListID+4);
}

void PolygenMesh::drawNodeNormal()
{
    if (meshList.IsEmpty()) {glDeleteLists(m_drawListID, m_drawListNumber); m_drawListID=-1; return;}
    glCallList(m_drawListID+5);
}

void PolygenMesh::ClearAll()
{
    GLKPOSITION Pos;

    for(Pos=meshList.GetHeadPosition();Pos!=NULL;) {
        QMeshPatch *mesh=(QMeshPatch *)(meshList.GetNext(Pos));
        delete mesh;
    }
    meshList.RemoveAll();
}

void PolygenMesh::computeRange()
{
    double range=0.0,ll,xx,yy,zz;
    GLKPOSITION Pos;
    GLKPOSITION PosNode;

    for(Pos=meshList.GetHeadPosition();Pos!=NULL;) {
        QMeshPatch *mesh=(QMeshPatch *)(meshList.GetNext(Pos));
        for(PosNode=(mesh->GetNodeList()).GetHeadPosition();PosNode!=NULL;) {
            QMeshNode *node=(QMeshNode *)((mesh->GetNodeList()).GetNext(PosNode));

            node->GetCoord3D(xx,yy,zz);
            ll=xx*xx+yy*yy+zz*zz;

            if (ll>range) range=ll;
        }
    }

    m_range=(float)(sqrt(range));
}

void PolygenMesh::_changeValueToColor(double maxValue, double minValue, double Value,
                                 float & nRed, float & nGreen, float & nBlue)
{
//	Value=fabs(Value);

    if (Value<=minValue)
    {
        nRed=0.0;
        nGreen=0.0;
        nBlue=0.0;
        return;
    }

    if ((maxValue-minValue)<0.000000000001)
    {
        nRed=0.0;
        nGreen=0.0;
        nBlue=1.0;
        return;
    }

    double temp=(Value-minValue)/(maxValue-minValue);

//    nRed=(float)(1.0-temp);	nGreen=(float)(1.0-temp); nBlue=(float)(1.0-temp);	return;

    if (temp>0.75)
    {
        nRed=1;
        nGreen=(float)(1.0-(temp-0.75)/0.25);
        if (nGreen<0) nGreen=0.0f;
        nBlue=0;
        return;
    }
    if (temp>0.5)
    {
        nRed=(float)((temp-0.5)/0.25);
        nGreen=1;
        nBlue=0;
        return;
    }
    if (temp>0.25)
    {
        nRed=0;
        nGreen=1;
        nBlue=(float)(1.0-(temp-0.25)/0.25);
        return;
    }
    else
    {
        nRed=0;
        nGreen=(float)(temp/0.25);
        nBlue=1;
    }

//    double t1,t2,t3;
//    t1=0.75;
//    t2=0.5;
//    t3=0.25;
//    if (temp>t1)
//    {
//        nRed=1;
//        nGreen=0.8-(float)(temp-t1)/(1-t1)*0.42;
//        if (nGreen<0.38) nGreen=0.38f;
//        nBlue=0.62-(float)(temp-t1)/(1-t1)*0.4;
//        if (nBlue<0.22) nBlue=0.22;
//        return;
//    }
//    if (temp>t2)
//    {
//        nRed=1;
//        nGreen=1.0-(float)(temp-t2)/(t1-t2)*0.2;
//        if (nGreen<0.8) nGreen=0.8f;
//        nBlue=0.75-(float)(temp-t2)/(t1-t2)*0.13;
//        if (nBlue<0.62) nBlue=0.62f;
//        return;
//    }
//    if (temp>t3)
//    {
//        nRed=(float)(temp-t3)/(t2-t3)*0.31+0.69;
//        if (nRed>1.0) nRed=1.0f;
//        nGreen=(float)(temp-t3)/(t2-t3)*0.09+0.91;
//        if (nGreen>1.0) nGreen=1.0f;
//        nBlue=0.95-(float)(temp-t3)/(t2-t3)*0.2;
//        if (nBlue<0.75) nBlue=0.75f;
//        return;
//    }
//    else
//    {
//        nRed=(float)temp/t3*0.47+0.22;
//        if (nRed>0.69) nRed=0.69f;
//        nGreen=(float)temp/t3*0.53+0.38;
//        if (nGreen>0.91) nGreen=0.91f;
//        nBlue=1.0-(float)temp/t3*0.05;
//        if (nBlue<0.95) nBlue=0.95f;
//        return;
//    }
}

void PolygenMesh::ImportOBJFile(char *filename, std::string modelName)
{
    QMeshPatch *newMesh = new QMeshPatch;
    if (newMesh->inputOBJFile(filename)){
        meshList.AddTail(newMesh);
        computeRange();
        setModelName(modelName);
    }
    else
        delete newMesh;
}

void PolygenMesh::ImportTETFile(char *filename, std::string modelName)
{
	QMeshPatch *newMesh = new QMeshPatch;
	if (newMesh->inputTETFile(filename, false)) {
		meshList.AddTail(newMesh);
		computeRange();
		setModelName(modelName);
	}
	else
		delete newMesh;
}

void PolygenMesh::drawSingleEdge(QMeshEdge* edge) {

    double xx, yy, zz;
    edge->GetStartPoint()->GetCoord3D(xx, yy, zz);
    glVertex3d(xx, yy, zz);
    edge->GetEndPoint()->GetCoord3D(xx, yy, zz);
    glVertex3d(xx, yy, zz);

}

void PolygenMesh::drawSingleNode(QMeshNode* node) {

    double nx, ny, nz, xx, yy, zz;
    node->GetNormal(nx, ny, nz);
    node->GetCoord3D(xx, yy, zz);
    glNormal3d(nx, ny, nz);
    glVertex3d(xx, yy, zz);

}

void PolygenMesh::drawSingleFace(QMeshFace* face) {

    double xx, yy, zz, dd;
    for (int i = 0; i < 3; i++) {
        QMeshNode* node = face->GetNodeRecordPtr(i);

        if (m_bVertexNormalShading) {
            double normal[3];
            node->CalNormal(normal); glNormal3dv(normal);
        }
        else {
            face->CalPlaneEquation();
            face->GetPlaneEquation(xx, yy, zz, dd);
            glNormal3d(xx, yy, zz);
        }

        node->GetCoord3D(xx, yy, zz);
        glVertex3d(xx, yy, zz);
    }
}