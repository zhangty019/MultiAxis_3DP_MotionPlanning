#pragma once
#include <vector>
#include <../ThirdPartyDependence/eigen3/Eigen/Dense> 
#include <dirent.h>
#include <iostream>
#include <iomanip>

#include "../GLKLib/GLKHeap.h"
#include "../QMeshLib/QMeshPatch.h"
#include "../QMeshLib/PolygenMesh.h"
#include "../QMeshLib/QMeshFace.h"
#include "../QMeshLib/QMeshNode.h"
#include "../ThirdPartyDependence/PQPLib/PQP.h"
#include "../ThirdPartyDependence/QHullLib/qhull_a.h"
#include "../QMeshLib/PMBody.h"

class QMeshPatch;
class PolygenMesh;
typedef struct qHullSet {
	int faceNum;	double* normalVec;	double* offset;
	int vertNum;	double* vertPos;
	unsigned int* faceTable;	//	Note that: the index starts from '1'
}QHULLSET;

struct collision_Node {
	double B_value;
	double C_value;
	QMeshNode* waypoint_N_i;
};

class GcodeGeneration {

public:
	GcodeGeneration() {};
	~GcodeGeneration() {};

	void initial(
		unsigned int FromIndex, unsigned int ToIndex,
		PolygenMesh* Slices, PolygenMesh* Waypoints, PolygenMesh* ExtruderHead, PolygenMesh* PrintPlatform,
		double E3XoffValue, double E3YoffValue, double lambdaValue,
		bool varyDistance_switch, bool varyHeight_switch, bool varyWidth_switch
		//bool testDistance_switch, bool testHeight_switch, bool testWidth_switch
	);

	// Volume Issue
	void calDistance();
	void calHeight();
	void calWidth();
	void test_DHW();
	void initialSmooth();

	// Singular Issue
	void singularityOpt();
	void verifyPosNor(bool checkSwitch);
	void testXYZBCE(bool testXYZBC_E_switch);

	// Collision Issue
	void detectCollision_1();//convex hull detect (fix extruder hull, move point, check point in/out hull) <---- fasest
	void detectCollision_2();//convex hull detect (move extruder hull to model in model coordinate system) <---- easy to understand
	void detectCollision_continuous();//convex hull detect (move extruder hull formed by neighbour tool guesture)

	// Graph Search
	void graph_Search_Shortest_Path();

	// Gcode writing
	void writeGcode(std::string GcodeDir);

private:
	std::vector<QMeshPatch*> getJumpSection_patchSet(QMeshPatch* patch);
	double _safe_acos(double value);
	void markSingularNode(QMeshPatch* patch);
	void filterSingleSingularNode(QMeshPatch* patch);
	void getSingularSec(QMeshPatch* patch, Eigen::MatrixXd& sectionTable);
	void projectAnchorPoint(QMeshPatch* patch);
	void getBCtable2(QMeshPatch* patch, Eigen::MatrixXd& B1C1table, Eigen::MatrixXd& B2C2table);
	void motionPlanning3(
		QMeshPatch* patch, const Eigen::MatrixXd& sectionTable,	const Eigen::MatrixXd& B1C1table, 
		const Eigen::MatrixXd& B2C2table, Eigen::RowVector2d& prevBC);
	bool _chooseB1C1(const Eigen::RowVector2d& B1C1, const Eigen::RowVector2d& B2C2, Eigen::RowVector2d& prevBC);
	double _toLeft(const Eigen::RowVector2d& origin, const Eigen::RowVector2d& startPnt, const Eigen::RowVector2d& endPnt);
	void getXYZ(QMeshPatch* patch);
	void optimizationC(QMeshPatch* patch);
	void DHW2E(QMeshPatch* patch, bool hysteresis_switch);
	double getAngle3D(const Eigen::Vector3d& v1, const Eigen::Vector3d& v2, const bool in_degree);
	int removeDir(std::string dirPath);

	QHULLSET* buildConvexHull_extruderHead(QMeshPatch* eHead);
	QHULLSET* buildConvexHull_extruderHead(const std::vector<Eigen::Vector3d>& eHead_pointSet);
	QHULLSET* _mallocMemoryConvexHull(int faceNum, int vertNum);
	bool _isPntInsideConvexHull(QHULLSET* pConvexHull, double pnt[]);
	void _freeMemoryConvexHull(QHULLSET*& pConvexHull);
	void _locate_EHead_printPos(QMeshPatch* eHeadPatch, Eigen::Vector3d nodePos, Eigen::Vector3d norm);
	void _load_eHead_points(QMeshPatch* eHeadPatch, std::vector<Eigen::Vector3d>& eHead_pointSet, int start_Index);

	void _get_GraphNode_List(QMeshPatch* patch, std::vector<collision_Node>& graph_Node);
	Eigen::Vector3d _calCandidateNormal(Eigen::Vector3d normal, double rad_ZrotateAngle, double rad_XtiltAngle);
	void _install_BC(Eigen::Vector3d temp_Normal, std::vector<collision_Node>& graph_Node, QMeshNode* sourceNode, int cnc_type);
	void _build_Graph(QMeshPatch* patch, std::vector<collision_Node>& graph_Node);
	double _weight_calculation(double B_i, double C_i, double B_ii, double C_ii);

	unsigned int LayerInd_From = 0;// Range start
	unsigned int LayerInd_To = 0;  // Range end
	PolygenMesh* polygenMesh_Slices = NULL;
	PolygenMesh* polygenMesh_Waypoints = NULL;
	PolygenMesh* polygenMesh_ExtruderHead = NULL;
	PolygenMesh* polygenMesh_Platform = NULL;

	double E3_Xoff = 0.0, E3_Yoff = 0.0;   // support extruder offset
	double lambda = 0.0; // singularity tilte angle

	bool vary_Distance = false;
	bool vary_Height = false;
	bool vary_Width = false;
	bool test_Distance = false;
	bool test_Height = false;
	bool test_Width = false;

	int Core = 16;		// My CPU core NUM
	int layerNum = 25;	// The number of detected bottom layer for height calculation
	int layerDepth = 20;// Collision detecte depth // for calibration can choose 1
	int delta_Z = 30, delta_X = 10; // The delta of candidate collision-free Normals

	int CNC_TYPE = 1;	//1- BCtable; 2- AChead; 3- Bhead_Ctable
};