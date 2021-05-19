#include "GcodeGeneration.h"
#include "GLKGeometry.h"
#include <fstream>
#include "io.h"
#include <Graph.h>

void GcodeGeneration::initial(unsigned int FromIndex, unsigned int ToIndex,
	PolygenMesh* Slices, PolygenMesh* Waypoints, PolygenMesh* ExtruderHead, PolygenMesh* PrintPlatform,
	double E3XoffValue, double E3YoffValue, double lambdaValue,
	bool varyDistance_switch, bool varyHeight_switch, bool varyWidth_switch
	//bool testDistance_switch, bool testHeight_switch, bool testWidth_switch
) {

	LayerInd_From = FromIndex;	LayerInd_To = ToIndex;
	polygenMesh_Slices = Slices; polygenMesh_Waypoints = Waypoints;
	polygenMesh_ExtruderHead = ExtruderHead; polygenMesh_Platform = PrintPlatform;
	E3_Xoff = E3XoffValue; E3_Yoff = E3YoffValue; lambda = lambdaValue;
	vary_Distance = varyDistance_switch; vary_Height = varyHeight_switch; vary_Width = varyWidth_switch;
}

/**************************************/
/* Main Function for varing Extrusion */
/**************************************/

void GcodeGeneration::calHeight() {

	std::cout << "------------------------------------------- Waypoint Height Calculation Running ..." << std::endl;
	long time = clock();

	// get the patch polygenMesh_PrintPlatform
	QMeshPatch* patch_PrintPlatform = (QMeshPatch*)polygenMesh_Platform->GetMeshList().GetHead();

#pragma omp parallel
	{
#pragma omp for  
		for (int omptime = 0; omptime < Core; omptime++) {

			// topLayer --> layer on the highest place [travel head to tail]
			for (GLKPOSITION Pos = polygenMesh_Slices->GetMeshList().GetHeadPosition(); Pos;) {
				QMeshPatch* topLayer = (QMeshPatch*)polygenMesh_Slices->GetMeshList().GetNext(Pos); // order: get data -> pnt move

				if (topLayer->GetIndexNo() < LayerInd_From || topLayer->GetIndexNo() > LayerInd_To) continue;

				if (topLayer->GetIndexNo() % Core != omptime) continue;

				std::vector<QMeshPatch*> bottomLayers;
				//int layerNum = 25; //the number of detected bottom layer

				bottomLayers.push_back(patch_PrintPlatform);
				// construct a bottomLayers[i] to store the point of bottom layers for every toplayer
				for (GLKPOSITION beforePos = polygenMesh_Slices->GetMeshList().Find(topLayer)->prev; beforePos;) {
					QMeshPatch* beforePatch = (QMeshPatch*)polygenMesh_Slices->GetMeshList().GetPrev(beforePos);

					bottomLayers.push_back(beforePatch);
					if (bottomLayers.size() > layerNum) break;
				}


				//--build PQP model
				std::vector<PQP_Model*> bLayerPQP;
				bLayerPQP.resize(bottomLayers.size());
				for (int i = 0; i < bottomLayers.size(); i++) {
					if (bottomLayers[i]->GetNodeNumber() < 3) continue;
					// build PQP model for bottom layers
					PQP_Model* pqpModel = new PQP_Model();
					pqpModel->BeginModel();  int index = 0;
					PQP_REAL p1[3], p2[3], p3[3];

					for (GLKPOSITION Pos = bottomLayers[i]->GetFaceList().GetHeadPosition(); Pos;) {
						QMeshFace* Face = (QMeshFace*)bottomLayers[i]->GetFaceList().GetNext(Pos);

						Face->GetNodeRecordPtr(0)->GetCoord3D(p1[0], p1[1], p1[2]);
						Face->GetNodeRecordPtr(1)->GetCoord3D(p2[0], p2[1], p2[2]);
						Face->GetNodeRecordPtr(2)->GetCoord3D(p3[0], p3[1], p3[2]);

						pqpModel->AddTri(p1, p2, p3, index);
						index++;

					}
					pqpModel->EndModel();
					bLayerPQP[i] = pqpModel;
				}//--build PQP model END

				int layerIndex = topLayer->GetIndexNo();

				GLKPOSITION WayPointPatch_Pos = polygenMesh_Waypoints->GetMeshList().FindIndex(layerIndex);
				QMeshPatch* WayPointPatch = (QMeshPatch*)polygenMesh_Waypoints->GetMeshList().GetAt(WayPointPatch_Pos);

				for (GLKPOSITION Pos = WayPointPatch->GetNodeList().GetHeadPosition(); Pos;) {
					QMeshNode* Node = (QMeshNode*)WayPointPatch->GetNodeList().GetNext(Pos);

					double minHeight = 99999.99;
					for (int i = 0; i < bottomLayers.size(); i++) {
						if (bottomLayers[i]->GetNodeNumber() < 3) continue;

						PQP_DistanceResult dres; dres.last_tri = bLayerPQP[i]->last_tri;
						PQP_REAL p[3];
						p[0] = Node->m_printPos(0); p[1] = Node->m_printPos(1); p[2] = Node->m_printPos(2);
						PQP_Distance(&dres, bLayerPQP[i], p, 0.0, 0.0);

						double Height = dres.Distance(); // Height of this layer
						//int minTriIndex = dres.last_tri->id;	// closest triangle
						if (minHeight > Height) minHeight = Height;
					}
					//cout << minHeight << endl;
					Node->m_DHW(1) = minHeight;
				}

				//	free memory
				for (int i = 0; i < bottomLayers.size(); i++) { delete bLayerPQP[i]; }

			}
		}
	}

	std::printf("TIMER -- Height Calculation takes %ld ms.\n", clock() - time);
	std::cout << "------------------------------------------- Waypoint Height Calculation Finish!\n" << std::endl;
}

void GcodeGeneration::calWidth() {

	std::cout << "------------------------------------------- Waypoint Width Calculation Running ..." << std::endl;
	long time = clock();

#pragma omp parallel
	{
#pragma omp for  
		for (int omptime = 0; omptime < Core; omptime++) {

			for (GLKPOSITION Pos = polygenMesh_Waypoints->GetMeshList().GetHeadPosition(); Pos;) {
				QMeshPatch* layer = (QMeshPatch*)polygenMesh_Waypoints->GetMeshList().GetNext(Pos);

				if (layer->GetIndexNo() < LayerInd_From || layer->GetIndexNo() > LayerInd_To) continue;

				if (layer->GetIndexNo() % Core != omptime) continue;

				for (GLKPOSITION nodePos = layer->GetNodeList().GetHeadPosition(); nodePos;) {
					QMeshNode* Node = (QMeshNode*)layer->GetNodeList().GetNext(nodePos);
					
					double W = 0.0; double minW = 99999.9;

					if (Node->GetIndexNo() == 0) {
						for (GLKPOSITION otherNodePos = layer->GetNodeList().GetHeadPosition(); otherNodePos;) {
							QMeshNode* otherNode = (QMeshNode*)layer->GetNodeList().GetNext(otherNodePos);

							if (otherNode->GetIndexNo() != Node->GetIndexNo() && otherNode->GetIndexNo() != (Node->GetIndexNo() + 1)) {

								W = (Node->m_printPos - otherNode->m_printPos).norm();
								if (W < minW) minW = W;
							}

						}
					}
					else if (Node->GetIndexNo() == (layer->GetNodeNumber() - 1)) {
						for (GLKPOSITION otherNodePos = layer->GetNodeList().GetHeadPosition(); otherNodePos;) {
							QMeshNode* otherNode = (QMeshNode*)layer->GetNodeList().GetNext(otherNodePos);

							if (otherNode->GetIndexNo() != Node->GetIndexNo() && otherNode->GetIndexNo() != (Node->GetIndexNo() - 1)) {

								W = (Node->m_printPos - otherNode->m_printPos).norm();
								if (W < minW) minW = W;
							}

						}
					}
					else {
						for (GLKPOSITION otherNodePos = layer->GetNodeList().GetHeadPosition(); otherNodePos;) {
							QMeshNode* otherNode = (QMeshNode*)layer->GetNodeList().GetNext(otherNodePos);

							if (otherNode->GetIndexNo() != Node->GetIndexNo() && otherNode->GetIndexNo() != (Node->GetIndexNo() - 1)
								&& otherNode->GetIndexNo() != (Node->GetIndexNo() + 1)) {

								W = (Node->m_printPos - otherNode->m_printPos).norm();
								if (W < minW) minW = W;
							}

						}
					}

					Node->m_DHW(2) = minW;
				}
			}
		}
	}
	std::printf("TIMER -- Width Calculation takes %ld ms.\n", clock() - time);
	std::cout << "------------------------------------------- Waypoint Width Calculation Finish!\n" << std::endl;
}

void GcodeGeneration::calDistance() {

	std::cout << "------------------------------------------- Waypoint Distance Calculation Running ..." << std::endl;
	long time = clock();

	double initial_largeJ_Length = 5.0;
	double support_largerJ_Length = 6.0;
	double largeJ_Length = 0.0;// install the above Length Value

#pragma omp parallel
	{
#pragma omp for  
		for (int omptime = 0; omptime < Core; omptime++) {

			for (GLKPOSITION Pos = polygenMesh_Waypoints->GetMeshList().GetHeadPosition(); Pos;) {
				QMeshPatch* layer = (QMeshPatch*)polygenMesh_Waypoints->GetMeshList().GetNext(Pos);

				if (layer->GetIndexNo() < LayerInd_From || layer->GetIndexNo() > LayerInd_To) continue;

				if (layer->GetIndexNo() % Core != omptime) continue;

				if (layer->isSupportLayer) { largeJ_Length = support_largerJ_Length; }
				else { largeJ_Length = initial_largeJ_Length; }

				for (GLKPOSITION nodePos = layer->GetNodeList().GetHeadPosition(); nodePos;) {
					QMeshNode* Node = (QMeshNode*)layer->GetNodeList().GetNext(nodePos);

					double D = 0.0;
					int lines = layer->GetNodeNumber();
					if (Node->GetIndexNo() == (lines - 1)) { D = 0.0; }
					else {

						GLKPOSITION nextPos = layer->GetNodeList().Find(Node)->next;
						QMeshNode* nextNode = (QMeshNode*)layer->GetNodeList().GetAt(nextPos);

						D = (Node->m_printPos - nextNode->m_printPos).norm();

						if (D > largeJ_Length) {
							D = 0.0;								// inject the D to the Node/startPnt of Edge
							Node->Jump_preSecEnd = true;			// end of prev section
							nextNode->Jump_nextSecStart = true;		// start of next section
						}
					}
					Node->m_DHW(0) = D;
				}
			}
		}
	}
	printf("TIMER -- Distance Calculation takes %ld ms.\n", clock() - time);
	std::cout << "------------------------------------------- Waypoint Distance Calculation Finish!\n" << std::endl;
}

void GcodeGeneration::test_DHW() {

	std::cout << "------------------------------------------- Waypoint Height Data Writing ..." << std::endl;

	for (GLKPOSITION Pos = polygenMesh_Waypoints->GetMeshList().GetHeadPosition(); Pos;) {
		QMeshPatch* WayPointPatch = (QMeshPatch*)polygenMesh_Waypoints->GetMeshList().GetNext(Pos);

		if (WayPointPatch->GetIndexNo() < LayerInd_From || WayPointPatch->GetIndexNo() > LayerInd_To) continue;

		char targetFilename[1024];

		if (WayPointPatch->isSupportLayer == true) {
			std::sprintf(targetFilename, "%s%d%s", "../DataSet/Test/test_DHW/", WayPointPatch->GetIndexNo(), "S.txt");
		}
		else {
			std::sprintf(targetFilename, "%s%d%s", "../DataSet/Test/test_DHW/", WayPointPatch->GetIndexNo(), ".txt");
		}

		// cout << targetFilename << endl;

		FILE* fp = fopen(targetFilename, "w");
		if (!fp)	return;

		// danger check: The point is JumpEnd and also JumpStart.
		for (GLKPOSITION Pos = WayPointPatch->GetNodeList().GetHeadPosition(); Pos;) {
			QMeshNode* Node = (QMeshNode*)WayPointPatch->GetNodeList().GetNext(Pos);

			if (Node->Jump_preSecEnd == true && Node->Jump_nextSecStart == true) {
				std::cout << "The point is JumpEnd and also JumpStart, please check" << std::endl; // important issues!
				std::cout << "The Layer Index is " << WayPointPatch->GetIndexNo() << std::endl;
				std::cout << "The Layer name is " << WayPointPatch->patchName << std::endl;
			}

			fprintf(fp, "%f %f %f %d\n", Node->m_DHW(0), Node->m_DHW(1), Node->m_DHW(2), Node->Jump_preSecEnd);
		}

		fclose(fp);

		//cout << "-------------------- Layer " << WayPointPatch->GetIndexNo() << " LayerHeight Data Write" << endl;
	}
	std::cout << "------------------------------------------- Waypoint Height Data Write Finish!\n" << std::endl;

}

void GcodeGeneration::initialSmooth() {

	for (GLKPOSITION Pos = polygenMesh_Waypoints->GetMeshList().GetHeadPosition(); Pos;) {
		QMeshPatch* WayPointPatch = (QMeshPatch*)polygenMesh_Waypoints->GetMeshList().GetNext(Pos);

		if (WayPointPatch->GetIndexNo() < LayerInd_From || WayPointPatch->GetIndexNo() > LayerInd_To) continue;

		int patch_NodeNum = WayPointPatch->GetNodeNumber();
		std::vector<bool> fix_Flag(patch_NodeNum);
		std::vector<Eigen::Vector3d> NodeNormal_temp(patch_NodeNum); // [Nx Ny Nz fix_flag]

		for (GLKPOSITION Pos = WayPointPatch->GetNodeList().GetHeadPosition(); Pos;) {
			QMeshNode* Node = (QMeshNode*)WayPointPatch->GetNodeList().GetNext(Pos);

			/*fixed at first / end / jump_start / jump_end points*/
			if (Node->GetIndexNo() == 0 || Node->GetIndexNo() == patch_NodeNum - 1
				|| Node->Jump_preSecEnd || Node->Jump_nextSecStart) {
				fix_Flag[Node->GetIndexNo()] = true;
			}
			else { fix_Flag[Node->GetIndexNo()] = false; }

			NodeNormal_temp[Node->GetIndexNo()] = Node->m_printNor;

		}

		//smooth normal by (n-1) + X*(n) + (n+1)
		for (int i = 0; i < fix_Flag.size(); i++) {

			if (fix_Flag[i] == false) {
				NodeNormal_temp[i] = (NodeNormal_temp[i - 1] + 0.5 * NodeNormal_temp[i] + NodeNormal_temp[i + 1]).normalized();
			}

		}

		for (GLKPOSITION Pos = WayPointPatch->GetNodeList().GetHeadPosition(); Pos;) {
			QMeshNode* Node = (QMeshNode*)WayPointPatch->GetNodeList().GetNext(Pos);

			Node->m_printNor = NodeNormal_temp[Node->GetIndexNo()];
			Node->SetNormal(Node->m_printNor(0), Node->m_printNor(1), Node->m_printNor(2));
		}
	}
	std::cout << "------------------------------------------- Initial Smooth Finish!" << std::endl;
}

/*******************************************/
/* Main Function for Singular Optimization */
/*******************************************/
void GcodeGeneration::singularityOpt() {

	std::cout << "------------------------------------------- XYZBCE Calculation running ... " << std::endl;
	long time = clock();

#pragma omp parallel
	{
#pragma omp for  
		for (int omptime = 0; omptime < Core; omptime++) {

			for (GLKPOSITION Pos = polygenMesh_Waypoints->GetMeshList().GetHeadPosition(); Pos;) {
				QMeshPatch* WayPointPatch = (QMeshPatch*)polygenMesh_Waypoints->GetMeshList().GetNext(Pos);

				if (WayPointPatch->GetIndexNo() < LayerInd_From || WayPointPatch->GetIndexNo() > LayerInd_To) continue;
				if (WayPointPatch->GetIndexNo() % Core != omptime) continue;

				std::vector<QMeshPatch*> layerJumpPatchSet = getJumpSection_patchSet(WayPointPatch);

				Eigen::RowVector2d prevBC = { 0.0,0.0 };
				// give the message of BC for the first Node
				for (GLKPOSITION Pos = WayPointPatch->GetNodeList().GetHeadPosition(); Pos;) {
					QMeshNode* Node = (QMeshNode*)WayPointPatch->GetNodeList().GetNext(Pos);

					// solve 1
					prevBC(0) = ROTATE_TO_DEGREE(-_safe_acos(Node->m_printNor(2)));
					prevBC(1) = ROTATE_TO_DEGREE(-atan2(Node->m_printNor(1), Node->m_printNor(0)));

					// solve 2
					double C2temp = prevBC(1) + 180.0;
					if (C2temp > 180.0)	C2temp -= 360.0; // control the range of C2 into the (-180,180]

					if (abs(C2temp) < abs(prevBC(1))) {
						prevBC(0) = -prevBC(0);
						prevBC(1) = C2temp;
					}
					break;
				}
				//cout << "prevBC: " << prevBC << endl;

				for (int Index = 0; Index < layerJumpPatchSet.size(); Index++) {
					//1.0 find the singularity waypoints
					markSingularNode(layerJumpPatchSet[Index]);
					//1.1 filter single singular waypoint (XXOXX -> XXXXX)
					filterSingleSingularNode(layerJumpPatchSet[Index]);

					Eigen::MatrixXd sectionTable, B1C1table, B2C2table;
					//2.0 get the range of singularity Sections
					getSingularSec(layerJumpPatchSet[Index], sectionTable);
					//2.1 project normal to the singular region boundary and check
					projectAnchorPoint(layerJumpPatchSet[Index]);

					//3. calculate the 2 solves baced on kinematics of CNC
					getBCtable2(layerJumpPatchSet[Index], B1C1table, B2C2table);
					//4. Main singularity optimization algorithm
					motionPlanning3(layerJumpPatchSet[Index], sectionTable, B1C1table, B2C2table, prevBC);
					//5. reset steps: CNC XYZ calculation and E(=DHW) calculation
					getXYZ(layerJumpPatchSet[Index]);
					DHW2E(layerJumpPatchSet[Index], true);
				}

				//aim to eliminate the -pi to pi sharp change
				optimizationC(WayPointPatch);

				// from delta_E of each point to E in Gcode
				double E = 0.0;
				for (GLKPOSITION Pos = WayPointPatch->GetNodeList().GetHeadPosition(); Pos;) {
					QMeshNode* Node = (QMeshNode*)WayPointPatch->GetNodeList().GetNext(Pos);
					E = E + Node->m_XYZBCE(5);
					Node->m_XYZBCE(5) = E;
				}
			}
		}
	}

	std::cout << "-------------------------------------------" << std::endl;
	std::printf("TIMER -- XYZBCE Calculation takes %ld ms.\n", clock() - time);
	std::cout << "------------------------------------------- XYZBCE Calculation Finish!\n " << std::endl;

}

std::vector<QMeshPatch*> GcodeGeneration::getJumpSection_patchSet(QMeshPatch* patch) {

	// Get the Jump section Num
	int JumpPatchNum = 1;
	for (GLKPOSITION Pos_Node = patch->GetNodeList().GetHeadPosition(); Pos_Node;) {
		QMeshNode* Node = (QMeshNode*)patch->GetNodeList().GetNext(Pos_Node);

		if (Node->Jump_nextSecStart == true) JumpPatchNum++;
	}

	// molloc the space for each jumpPatch
	std::vector<QMeshPatch*> layerJumpPatchSet(JumpPatchNum);
	for (int i = 0; i < JumpPatchNum; i++) {
		layerJumpPatchSet[i] = new QMeshPatch();
		layerJumpPatchSet[i]->rootPatch_jumpPatch = patch;
	}

	// Add node into each JumpPatch
	int Jump_PatchIndex = 0;
	int JumpPatch_NodeIndex = 0;
	for (GLKPOSITION Pos_Node = patch->GetNodeList().GetHeadPosition(); Pos_Node;) {
		QMeshNode* Node = (QMeshNode*)patch->GetNodeList().GetNext(Pos_Node);

		if (Node->Jump_nextSecStart == true) {
			Jump_PatchIndex++;
			JumpPatch_NodeIndex = 0;
		}

		layerJumpPatchSet[Jump_PatchIndex]->GetNodeList().AddTail(Node);
		Node->Jump_SecIndex = JumpPatch_NodeIndex;
		JumpPatch_NodeIndex++;
	}
	//std::cout << "-----------------------------------" << std::endl;
	//std::cout << "--> Split ToolPath into JumpSection" << std::endl;

	return layerJumpPatchSet;
}

double GcodeGeneration::_safe_acos(double value) {
	if (value <= -1.0) {
		return PI;
	}
	else if (value >= 1.0) {
		return 0;
	}
	else {
		return acos(value);
	}
}

void GcodeGeneration::markSingularNode(QMeshPatch* patch) {

	for (GLKPOSITION Pos = patch->GetNodeList().GetHeadPosition(); Pos;) {
		QMeshNode* Node = (QMeshNode*)patch->GetNodeList().GetNext(Pos);

		Eigen::Vector2d Cspece_Coord;
		// Cal C space Coordinate : Cspece = Nx/Nz, Ny/Nz;
		Cspece_Coord << Node->m_printNor(0) / Node->m_printNor(2),
			Node->m_printNor(1) / Node->m_printNor(2);

		double R = Cspece_Coord.norm();
		double radLambda = DEGREE_TO_ROTATE(lambda);

		if (R < tan(radLambda)) Node->isSingularNode = true;
	}
}

void GcodeGeneration::filterSingleSingularNode(QMeshPatch* patch) {

	//protect
	if (patch->GetNodeNumber() < 4) return;

	std::vector<QMeshNode*> nodeSet(patch->GetNodeNumber());
	std::vector<bool> kept_Singular_Flag(patch->GetNodeNumber());

	int tempIndex = 0;
	for (GLKPOSITION Pos = patch->GetNodeList().GetHeadPosition(); Pos;) {
		QMeshNode* Node = (QMeshNode*)patch->GetNodeList().GetNext(Pos);

		nodeSet[tempIndex] = Node;
		kept_Singular_Flag[tempIndex] = Node->isSingularNode;
		tempIndex++;
	}


	// remove OXX ... XOX ... XXO
	for (int i = 0; i < kept_Singular_Flag.size(); i++) {

		if (i == 0) {
			if (kept_Singular_Flag[i] == false && kept_Singular_Flag[i + 1] == true && kept_Singular_Flag[i + 2] == true) {
				nodeSet[i]->isSingularNode = true;
			}
		}
		else if (i == (kept_Singular_Flag.size() - 1)) {
			if (kept_Singular_Flag[i - 2] == true && kept_Singular_Flag[i - 1] == true && kept_Singular_Flag[i] == false) {
				nodeSet[i]->isSingularNode = true;
			}
		}
		else {
			if (kept_Singular_Flag[i -1] == true && kept_Singular_Flag[i] == false && kept_Singular_Flag[i+1] == true) {
				nodeSet[i]->isSingularNode = true;
			}
		}
	}

	// remove XOOX
	if (patch->GetNodeNumber() < 5) return;
	for (int i = 0; i < kept_Singular_Flag.size(); i++) {	
		kept_Singular_Flag[i] = nodeSet[i]->isSingularNode;	
	}
	for (int i = 0; i < kept_Singular_Flag.size() - 3; i++) {

		if (kept_Singular_Flag[i] == true
			&& kept_Singular_Flag[i + 1] == false
			&& kept_Singular_Flag[i + 2] == false
			&& kept_Singular_Flag[i + 3] == true) {
			nodeSet[i + 1]->isSingularNode = true;
			nodeSet[i + 2]->isSingularNode = true;
		}
	}
	// remove XOOOX
	if (patch->GetNodeNumber() < 6) return;
	for (int i = 0; i < kept_Singular_Flag.size(); i++) {
		kept_Singular_Flag[i] = nodeSet[i]->isSingularNode;
	}
	for (int i = 0; i < kept_Singular_Flag.size() - 4; i++) {

		if (kept_Singular_Flag[i] == true
			&& kept_Singular_Flag[i + 1] == false
			&& kept_Singular_Flag[i + 2] == false
			&& kept_Singular_Flag[i + 3] == false
			&& kept_Singular_Flag[i + 4] == true) {
			nodeSet[i + 1]->isSingularNode = true;
			nodeSet[i + 2]->isSingularNode = true;
			nodeSet[i + 3]->isSingularNode = true;
		}
	}
}

void GcodeGeneration::getSingularSec(QMeshPatch* patch, Eigen::MatrixXd& sectionTable) {

	int lines = patch->GetNodeNumber();
	std::vector<int> srtPntIndTable, endPntIndTable;

	for (int i = 0; i < lines - 1; i++) {

		GLKPOSITION Node_Pos = patch->GetNodeList().FindIndex(i);
		QMeshNode* Node = (QMeshNode*)patch->GetNodeList().GetAt(Node_Pos);

		GLKPOSITION nextNode_Pos = patch->GetNodeList().FindIndex(i)->next;
		QMeshNode* nextNode = (QMeshNode*)patch->GetNodeList().GetAt(nextNode_Pos);


		if ((Node->isSingularNode == false && nextNode->isSingularNode == true)
			|| (Node->isSingularNode == true && Node->Jump_SecIndex == 0)) {
			srtPntIndTable.push_back(Node->Jump_SecIndex);
			Node->isSingularSecStartNode = true;
		}
		if ((Node->isSingularNode == true && nextNode->isSingularNode == false)
			|| (nextNode->isSingularNode == true && nextNode->Jump_SecIndex == lines - 1)) {
			endPntIndTable.push_back(nextNode->Jump_SecIndex);
			nextNode->isSingularSecEndNode = true;
		}
	}

	if (srtPntIndTable.size() == endPntIndTable.size()) sectionTable.resize(srtPntIndTable.size(), 2);
	else std::cout << "ERROR : srtPntIndTable.size() != endPntIndTable.size()" << std::endl;

	for (int i = 0; i < srtPntIndTable.size(); i++) {
		sectionTable(i, 0) = srtPntIndTable[i];
		sectionTable(i, 1) = endPntIndTable[i];
	}
	//std::cout << "sectionTable:\n"<<sectionTable << std::endl;
}

void GcodeGeneration::projectAnchorPoint(QMeshPatch* patch) {

	for (GLKPOSITION Pos = patch->GetNodeList().GetHeadPosition(); Pos;) {
		QMeshNode* Node = (QMeshNode*)patch->GetNodeList().GetNext(Pos);

		if (Node->isSingularSecStartNode == true && Node->isSingularSecEndNode == true) {
			std::cout << "Error: the normal of anchor point cannot move to the boundary of singular region" << std::endl;
			std::cout << "Error: as one normal cannot move to double directions" << std::endl;
		}

		if (Node->GetIndexNo() == 0 || Node->GetIndexNo() == (patch->GetNodeNumber() - 1)) continue;


		if (Node->isSingularSecStartNode == true || Node->isSingularSecEndNode == true) {

			Eigen::Vector3d m_printNor_before = Node->m_printNor;

			double anchor_Nz = cos(DEGREE_TO_ROTATE(lambda));
			double temp_k = Node->m_printNor(1) / Node->m_printNor(0);
			double anchor_Nx = sqrt((1 - anchor_Nz * anchor_Nz) / (1 + temp_k * temp_k));
			if (Node->m_printNor(0) < 0.0) anchor_Nx = -anchor_Nx;
			double anchor_Ny = anchor_Nx * temp_k;

			Node->SetNormal(anchor_Nx, anchor_Ny, anchor_Nz);
			Node->SetNormal_last(anchor_Nx, anchor_Ny, anchor_Nz);
			Node->m_printNor << anchor_Nx, anchor_Ny, anchor_Nz;

			//cal the angle of before and after of anchor normal
			if (false) {
				double change = ROTATE_TO_DEGREE(
					_safe_acos(
						m_printNor_before.dot(Node->m_printNor)
						/ m_printNor_before.norm() / Node->m_printNor.norm()));
				std::cout << " the angle of before and after of anchor normal is " << change << std::endl;
			}
		}
	}
}

void GcodeGeneration::getBCtable2(QMeshPatch* patch, Eigen::MatrixXd& B1C1table, Eigen::MatrixXd& B2C2table) {

	int lines = patch->GetNodeNumber();

	B1C1table = Eigen::MatrixXd::Zero(lines, 2);	B2C2table = Eigen::MatrixXd::Zero(lines, 2);

	for (GLKPOSITION Pos = patch->GetNodeList().GetHeadPosition(); Pos;) {
		QMeshNode* Node = (QMeshNode*)patch->GetNodeList().GetNext(Pos);

		int i = Node->Jump_SecIndex;

		// solve 1
		// B1 deg // -acos(Nz)
		B1C1table(i, 0) = ROTATE_TO_DEGREE(-_safe_acos(Node->m_printNor(2)));
		// C1 deg //-atan2(Ny, Nx)
		B1C1table(i, 1) = ROTATE_TO_DEGREE(-atan2(Node->m_printNor(1), Node->m_printNor(0)));

		// solve 2
		// B2 deg // acos(Nz)
		B2C2table(i, 0) = -B1C1table(i, 0);
		// C2 deg //-atan2(Ny, Nx) +/- 180
		double C2temp = B1C1table(i, 1) + 180.0;
		if (C2temp > 180.0)C2temp -= 360.0; // control the range of C2 into the (-180,180]
		B2C2table(i, 1) = C2temp;
	}
}

void GcodeGeneration::motionPlanning3(
	QMeshPatch* patch, const Eigen::MatrixXd& sectionTable, const Eigen::MatrixXd& B1C1table, 
	const Eigen::MatrixXd& B2C2table, Eigen::RowVector2d& prevBC) {

	int lines = patch->GetNodeNumber();
	Eigen::MatrixXd BC_Matrix(lines, 2); BC_Matrix = Eigen::MatrixXd::Zero(lines, 2);
	std::vector<int> solveFlag(lines);// 1 -> solve 1 // 2 -> solve 2
	// std::vector<int> insertNum(lines);// insertNum for large BC change at the beginning point
	int sectionNumber = 0;
	int sectionAmount = sectionTable.rows();

	int i = 0;

	while (i < lines) {
		//all points of current path are OUT of the sigularity region
		if (sectionAmount == 0) {

			if (_chooseB1C1(B1C1table.row(i), B2C2table.row(i), prevBC)) {
				prevBC << B1C1table.row(i);
				solveFlag[i] = 1;
			}
			else {
				prevBC << B2C2table.row(i);
				solveFlag[i] = 2;
			}

			BC_Matrix.row(i) = prevBC;
			i = i + 1;
		}
		else {
			Eigen::RowVector2d tempBC;
			//all points of current path are IN the sigularity region
			if (i == sectionTable(sectionNumber, 0) && i == 0 && sectionTable(sectionNumber, 1) == (lines - 1)) {
				for (int secLineIndex = i; secLineIndex < lines; secLineIndex++) {

					tempBC = { 0.0, prevBC(1) };
					prevBC = tempBC;
					solveFlag[secLineIndex] = 1;
					BC_Matrix.row(secLineIndex) = prevBC;
				}
				i = lines;
			}
			// start from the singularity region (end in singularity region or not)
			else if (i == sectionTable(sectionNumber, 0) && i == 0 && sectionTable(sectionNumber, 1) != (lines - 1)) {

				int secEndIndex = sectionTable(sectionNumber, 1);

				for (int secLineIndex = i; secLineIndex < secEndIndex; secLineIndex++) {

					if (_chooseB1C1(B1C1table.row(secEndIndex), B2C2table.row(secEndIndex), prevBC)) {
						tempBC << B1C1table.row(secEndIndex);
						solveFlag[secLineIndex] = 1;
					}
					else {
						tempBC << B2C2table.row(secEndIndex);
						solveFlag[secLineIndex] = 2;
					}

					prevBC = tempBC;
					BC_Matrix.row(secLineIndex) = prevBC;

				}

				i = secEndIndex;
				if (sectionNumber != (sectionAmount - 1))	sectionNumber++;
			}
			// end in the singularity region / finish path
			else if (i == sectionTable(sectionNumber, 0) && i != 0 && sectionTable(sectionNumber, 1) == (lines - 1)) {

				int secStartIndex = sectionTable(sectionNumber, 0);

				for (int secLineIndex = i; secLineIndex < lines; secLineIndex++) {

					if (_chooseB1C1(B1C1table.row(secStartIndex), B2C2table.row(secStartIndex), prevBC)) {
						tempBC << B1C1table.row(secStartIndex);
						solveFlag[secLineIndex] = 1;
					}
					else {
						tempBC << B2C2table.row(secStartIndex);
						solveFlag[secLineIndex] = 2;
					}

					prevBC = tempBC;
					BC_Matrix.row(secLineIndex) = prevBC;
				}

				i = lines;
			}
			// path passes through the sigularity region
			else if (i == sectionTable(sectionNumber, 0) && i != 0 && sectionTable(sectionNumber, 1) != (lines - 1)) {

				// give the message to anchor point (start point)
				int secStartIndex = sectionTable(sectionNumber, 0);
				if (_chooseB1C1(B1C1table.row(secStartIndex), B2C2table.row(secStartIndex), prevBC)) {
					prevBC << B1C1table.row(secStartIndex);
					solveFlag[secStartIndex] = 1;
				}
				else {
					prevBC << B2C2table.row(secStartIndex);
					solveFlag[secStartIndex] = 2;
				}

				// record the deg_BC of secStart point
				Eigen::RowVector2d startPntBC = prevBC;

				// decide the solve of End point
				int secEndIndex = sectionTable(sectionNumber, 1);
				int pointNum = secEndIndex - secStartIndex;

				double rad_end_B1 = DEGREE_TO_ROTATE(B1C1table(secEndIndex, 0));	double rad_end_C1 = DEGREE_TO_ROTATE(B1C1table(secEndIndex, 1));
				double rad_end_B2 = DEGREE_TO_ROTATE(B2C2table(secEndIndex, 0));	double rad_end_C2 = DEGREE_TO_ROTATE(B2C2table(secEndIndex, 1));
				double rad_start_B = DEGREE_TO_ROTATE(startPntBC(0));				double rad_start_C = DEGREE_TO_ROTATE(startPntBC(1));

				Eigen::Vector2d v_start_C = { cos(rad_start_C),sin(rad_start_C) };
				Eigen::Vector2d v_end_C1 = { cos(rad_end_C1),sin(rad_end_C1) };
				Eigen::Vector2d v_end_C2 = { cos(rad_end_C2),sin(rad_end_C2) };
				//compute the actural angle of 2 vectors
				double rad_end_C1_start_C = _safe_acos(v_end_C1.dot(v_start_C));		double rad_end_B1_start_B = rad_end_B1 - rad_start_B;
				double rad_end_C2_start_C = _safe_acos(v_end_C2.dot(v_start_C));		double rad_end_B2_start_B = rad_end_B2 - rad_start_B;
				//get rad_C/B_start_end
				double rad_C_start_end = 0.0;
				double rad_B_start_end = 0.0;
				Eigen::Vector2d v_end_C = { 0.0,0.0 };

				int solveFlag_passThrough = 0; // 1 -> solve 1 // 2 -> solve 2

				if ((rad_end_C1_start_C) <= (rad_end_C2_start_C)) {
					rad_C_start_end = rad_end_C1_start_C;
					rad_B_start_end = rad_end_B1_start_B;
					v_end_C = v_end_C1;
					solveFlag_passThrough = 1;
				}
				else {
					rad_C_start_end = rad_end_C2_start_C;
					rad_B_start_end = rad_end_B2_start_B;
					v_end_C = v_end_C2;
					solveFlag_passThrough = 2;
				}

				//decide the rotation direction of C axis
				double sign = _toLeft({ 0.0,0.0 }, v_start_C, v_end_C);

				//get tha delta Angel of deg_B/C
				double C_delta_Angle = ROTATE_TO_DEGREE(rad_C_start_end) / pointNum;
				double B_delta_Angle = ROTATE_TO_DEGREE(rad_B_start_end) / pointNum;

				unsigned int times = 0;
				for (int secLineIndex = secStartIndex; secLineIndex < secEndIndex; secLineIndex++) {

					tempBC(0) = startPntBC(0) + times * B_delta_Angle;
					tempBC(1) = startPntBC(1) + sign * times * C_delta_Angle;

					prevBC = tempBC;

					if (prevBC(1) > 180.0) prevBC(1) -= 360.0;
					if (prevBC(1) < -180.0) prevBC(1) += 360.0;

					solveFlag[secLineIndex] = solveFlag_passThrough;
					BC_Matrix.row(secLineIndex) = prevBC;

					times++;
				}

				i = secEndIndex;

				if (sectionNumber != (sectionAmount - 1))	sectionNumber = sectionNumber + 1;

			}
			// other points out of the singularity region
			else {

				if (_chooseB1C1(B1C1table.row(i), B2C2table.row(i), prevBC)) {

					prevBC << B1C1table.row(i);
					solveFlag[i] = 1;
				}
				else {
					prevBC << B2C2table.row(i);
					solveFlag[i] = 2;
				}

				BC_Matrix.row(i) = prevBC;
				i = i + 1;
			}
		}
	}

	for (GLKPOSITION Pos = patch->GetNodeList().GetHeadPosition(); Pos;) {
		QMeshNode* Node = (QMeshNode*)patch->GetNodeList().GetNext(Pos);

		int nodeIndex = Node->Jump_SecIndex;
		Node->m_XYZBCE(3) = BC_Matrix(nodeIndex, 0); //deg
		Node->m_XYZBCE(4) = BC_Matrix(nodeIndex, 1); //deg

		Node->solveSeclct = solveFlag[nodeIndex];
		//cout << Node->solveSeclct << endl;
		//Node->insertNodesNum = insertNum[nodeIndex];

	}
}

bool GcodeGeneration::_chooseB1C1(const Eigen::RowVector2d& B1C1, const Eigen::RowVector2d& B2C2, Eigen::RowVector2d& prevBC) {

	double rad_B1 = DEGREE_TO_ROTATE(B1C1(0));	double rad_C1 = DEGREE_TO_ROTATE(B1C1(1));
	double rad_B2 = DEGREE_TO_ROTATE(B2C2(0));	double rad_C2 = DEGREE_TO_ROTATE(B2C2(1));
	double rad_Bp = DEGREE_TO_ROTATE(prevBC(0)); double rad_Cp = DEGREE_TO_ROTATE(prevBC(1));

	Eigen::Vector2d v_Cp = { cos(rad_Cp),sin(rad_Cp) };
	Eigen::Vector2d v_C1 = { cos(rad_C1),sin(rad_C1) };
	Eigen::Vector2d v_C2 = { cos(rad_C2),sin(rad_C2) };
	//compute the actural angle

	double rad_v_C1_v_Cp = _safe_acos(v_C1.dot(v_Cp));		double rad_B1_rad_Bp = abs(rad_B1 - rad_Bp);
	double rad_v_C2_v_Cp = _safe_acos(v_C2.dot(v_Cp));		double rad_B2_rad_Bp = abs(rad_B2 - rad_Bp);

	bool isB1C1 = true;

	if ((rad_v_C1_v_Cp + rad_B1_rad_Bp) > (rad_v_C2_v_Cp + rad_B2_rad_Bp)) {
		isB1C1 = false;

		//std::cout << "----------------------------\n use 2 solve" << std::endl;
		//std::cout << "B1C1 = " << B1C1 << std::endl;
		//std::cout << "B2C2 = " << B2C2 << std::endl;
		//std::cout << "prevBC = " << prevBC << std::endl;
		//std::cout << "rad_v_C1_v_Cp = " << rad_v_C1_v_Cp << std::endl;
		//std::cout << "rad_B1_rad_Bp = " << rad_B1_rad_Bp << std::endl;
		//std::cout << "rad_v_C2_v_Cp = " << rad_v_C2_v_Cp << std::endl;
		//std::cout << "rad_B2_rad_Bp = " << rad_B2_rad_Bp << std::endl;
	}

	return isB1C1;

}

double GcodeGeneration::_toLeft(
	const Eigen::RowVector2d& origin_p, const Eigen::RowVector2d& startPnt_q, const Eigen::RowVector2d& endPnt_s) {

	double Area2 = origin_p(0) * startPnt_q(1) - origin_p(1) * startPnt_q(0)
		+ startPnt_q(0) * endPnt_s(1) - startPnt_q(1) * endPnt_s(0)
		+ endPnt_s(0) * origin_p(1) - endPnt_s(1) * origin_p(0);

	bool isLeft = -1.0;
	if (Area2 > 0.0) isLeft = 1.0;

	return isLeft;
}

void GcodeGeneration::getXYZ(QMeshPatch* patch) {

	for (GLKPOSITION Pos = patch->GetNodeList().GetHeadPosition(); Pos;) {
		QMeshNode* Node = (QMeshNode*)patch->GetNodeList().GetNext(Pos);

		double Px = Node->m_printPos(0);
		double Py = Node->m_printPos(1);
		double Pz = Node->m_printPos(2);

		double B = DEGREE_TO_ROTATE(Node->m_XYZBCE(3));// rad
		double C = DEGREE_TO_ROTATE(Node->m_XYZBCE(4));// rad

		Node->m_XYZBCE(0) = cos(B) * cos(C) * Px - cos(B) * sin(C) * Py + sin(B) * Pz;
		Node->m_XYZBCE(1) = sin(C) * Px + cos(C) * Py;
		Node->m_XYZBCE(2) = -sin(B) * cos(C) * Px + sin(B) * sin(C) * Py + cos(B) * Pz;

		if (Node->m_XYZBCE(2) < 0.0) { Node->negativeZ = true; }
	}
}

void GcodeGeneration::optimizationC(QMeshPatch* patch) {

	for (int loop = 0; loop < 20; loop++) {

		double threshhold = 180.0;

		for (GLKPOSITION Pos = patch->GetNodeList().GetHeadPosition(); Pos;) {
			QMeshNode* Node = (QMeshNode*)patch->GetNodeList().GetNext(Pos);

			double C = Node->m_XYZBCE(4); // deg

			if (Node->GetIndexNo() == 0) continue;
			GLKPOSITION prevPos = patch->GetNodeList().Find(Node)->prev;
			QMeshNode* prevNode = (QMeshNode*)patch->GetNodeList().GetAt(prevPos);
			double preC = prevNode->m_XYZBCE(4);

			if (C - preC < -threshhold) {
				C = C + 360;
			}
			else if (C - preC > threshhold) {
				C = C - 360;
			}
			else {}

			Node->m_XYZBCE(4) = C;
		}
	}
}

void GcodeGeneration::DHW2E(QMeshPatch* patch, bool hysteresis_switch) {

	// E = E + ratio * height * length * width;
	// Dicided by CNC W.R.T(E:Volume:E = 0.48)

	double ratio = 0.55;//0.55
	double D, H, W;

	// optimize the Hysteresis of extruder
	std::vector<double> Hysteresis_Ks = { 2.5,2.0,1.7,1.5,1.25 };
	std::vector<double> Hysteresis_Ke = { 0.9,0.9,0.9,0.8,0.7 };
	int lines = patch->GetNodeNumber();
	std::vector<double> deltaE(lines);

	for (GLKPOSITION Pos = patch->GetNodeList().GetHeadPosition(); Pos;) {
		QMeshNode* Node = (QMeshNode*)patch->GetNodeList().GetNext(Pos);

		if (vary_Distance == true) D = Node->m_DHW(0);
		else D = 2;
		if (vary_Height == true) H = Node->m_DHW(1);
		else H = 0.5;
		if (vary_Width == true) W = Node->m_DHW(2);
		else W = 0.7;

		deltaE[Node->Jump_SecIndex] = ratio * H * D * W;
	}

	if (lines >= (Hysteresis_Ks.size() + Hysteresis_Ke.size()) && hysteresis_switch) {

		for (int i = 0; i < lines; i++) {

			if (i >= 0 && i < Hysteresis_Ks.size()) {
				deltaE[i] = deltaE[i] * Hysteresis_Ks[i];
			}

			if (i >= (lines - Hysteresis_Ke.size()) && i < lines) {
				deltaE[i] = deltaE[i] * Hysteresis_Ke[i - lines + Hysteresis_Ke.size()];
			}
		}
	}

	for (GLKPOSITION Pos = patch->GetNodeList().GetHeadPosition(); Pos;) {
		QMeshNode* Node = (QMeshNode*)patch->GetNodeList().GetNext(Pos);

		Node->m_XYZBCE(5) = deltaE[Node->Jump_SecIndex];
	}

}

void GcodeGeneration::verifyPosNor(bool checkSwitch) {

	if (checkSwitch == false) return;

	std::cout << "------------------------------------------- PosNor verification running ... " << std::endl;
	for (GLKPOSITION Pos = polygenMesh_Waypoints->GetMeshList().GetHeadPosition(); Pos;) {
		QMeshPatch* WayPointPatch = (QMeshPatch*)polygenMesh_Waypoints->GetMeshList().GetNext(Pos);

		if (WayPointPatch->GetIndexNo() < LayerInd_From || WayPointPatch->GetIndexNo() > LayerInd_To) continue;

		for (GLKPOSITION Pos = WayPointPatch->GetNodeList().GetHeadPosition(); Pos;) {
			QMeshNode* Node = (QMeshNode*)WayPointPatch->GetNodeList().GetNext(Pos);

			double X = Node->m_XYZBCE(0);	double Y = Node->m_XYZBCE(1);	double Z = Node->m_XYZBCE(2);
			double B = DEGREE_TO_ROTATE(Node->m_XYZBCE(3));
			double C = DEGREE_TO_ROTATE(Node->m_XYZBCE(4));

			double finalNx = -cos(C) * sin(B);
			double finalNy = sin(B) * sin(C);
			double finalNz = cos(B);
			double finalPx = cos(C) * cos(B) * X + sin(C) * Y - cos(C) * sin(B) * Z;
			double finalPy = -sin(C) * cos(B) * X + cos(C) * Y + sin(B) * sin(C) * Z;
			double finalPz = sin(B) * X + cos(B) * Z;

			bool equalPx = (abs(finalPx - Node->m_printPos(0)) < 0.001) ? true : false;
			bool equalPy = (abs(finalPy - Node->m_printPos(1)) < 0.001) ? true : false;
			bool equalPz = (abs(finalPz - Node->m_printPos(2)) < 0.001) ? true : false;

			if ((equalPx == false) || (equalPy == false) || (equalPz == false)) {
				std::cout << "--------------------------------" << std::endl;
				std::cout << "Layer " << WayPointPatch->GetIndexNo() << " Point Index " << Node->GetIndexNo() << std::endl;
				std::cout << "final Position" << std::endl;
				std::cout << finalPx << "\n" << finalPy << "\n" << finalPz << std::endl;
				std::cout << "print Position\n" << Node->m_printPos << std::endl;
			}

			Eigen::Vector3d finalNormal = { finalNx, finalNy, finalNz };
			double angle = getAngle3D(finalNormal, Node->m_printNor, true);
			//if (angle >= 0.0001) {
			if (angle > lambda * 2) {
				//if (Node->isSingularNode) cout << "this is a singular node";
				std::cout << "--------------------------------" << std::endl;
				std::cout << "Layer " << WayPointPatch->GetIndexNo() << " Point Index " << Node->GetIndexNo() << std::endl;
				std::cout << "The angle is " << angle << std::endl;
				std::cout << "final Normal\n" << finalNormal.transpose() << std::endl;
				std::cout << "print Normal\n" << Node->m_printNor.transpose() << std::endl;
			}

			//update the normal after singular optimization
			Node->SetNormal(finalNormal(0), finalNormal(1), finalNormal(2));// for show the final normal on the GUI
			Node->SetNormal_last(finalNormal(0), finalNormal(1), finalNormal(2));
			Node->m_printNor = finalNormal;
		}

	}

	std::cout << "------------------------------------------- PosNor verification Finish!\n" << std::endl;

}

double GcodeGeneration::getAngle3D(const Eigen::Vector3d& v1, const Eigen::Vector3d& v2, const bool in_degree){
	//compute the actural angle
	double rad = v1.normalized().dot(v2.normalized());//dot product
	if (rad < -1.0)
		rad = -1.0;
	else if (rad > 1.0)
		rad = 1.0;
	return (in_degree ? _safe_acos(rad) * 180.0 / PI : _safe_acos(rad));
}

void GcodeGeneration::testXYZBCE(bool testXYZBC_E_switch) {

	if (testXYZBC_E_switch == false)	return;
	std::cout << "------------------------------------------- XYZBCD Data Writing ..." << std::endl;

	removeDir("../DataSet/Test/test_XYZBCE");

	for (GLKPOSITION Pos = polygenMesh_Waypoints->GetMeshList().GetHeadPosition(); Pos;) {
		QMeshPatch* WayPointPatch = (QMeshPatch*)polygenMesh_Waypoints->GetMeshList().GetNext(Pos);

		if (WayPointPatch->GetIndexNo() < LayerInd_From || WayPointPatch->GetIndexNo() > LayerInd_To) continue;

		char targetFilename[1024];

		if (WayPointPatch->isSupportLayer == true) {
			std::sprintf(targetFilename, "%s%d%s", "../DataSet/Test/test_XYZBCE/", WayPointPatch->GetIndexNo(), "S.txt");
		}
		else {
			std::sprintf(targetFilename, "%s%d%s", "../DataSet/Test/test_XYZBCE/", WayPointPatch->GetIndexNo(), ".txt");
		}

		// cout << targetFilename << endl;

		FILE* fp = fopen(targetFilename, "w");
		if (!fp)	return;

		for (GLKPOSITION Pos = WayPointPatch->GetNodeList().GetHeadPosition(); Pos;) {
			QMeshNode* Node = (QMeshNode*)WayPointPatch->GetNodeList().GetNext(Pos);

			Eigen::MatrixXd XYZBCE = Node->m_XYZBCE;

			fprintf(fp, "%f %f %f %f %f %f %d %d %d\n",
				XYZBCE(0), XYZBCE(1), XYZBCE(2), XYZBCE(3), XYZBCE(4), XYZBCE(5),
				Node->Jump_preSecEnd, Node->Jump_nextSecStart, 0);
		}

		std::fclose(fp);

		//cout << "-------------------- Layer " << WayPointPatch->GetIndexNo() << " XYZBCD Data Write" << endl;	
	}
	std::cout << "------------------------------------------- XYZBCD Data Write Finish!\n" << std::endl;
}

int GcodeGeneration::removeDir(std::string dirPath) {

	struct _finddata_t fb;   //find the storage structure of the same properties file.
	std::string path;
	intptr_t    handle;
	int  resultone;
	int   noFile;            // the tag for the system's hidden files

	noFile = 0;
	handle = 0;

	path = dirPath + "/*";

	handle = _findfirst(path.c_str(), &fb);

	//find the first matching file
	if (handle != -1)
	{
		//find next matching file
		while (0 == _findnext(handle, &fb))
		{
			// "." and ".." are not processed
			noFile = strcmp(fb.name, "..");

			if (0 != noFile)
			{
				path.clear();
				path = dirPath + "/" + fb.name;

				//fb.attrib == 16 means folder
				if (fb.attrib == 16)
				{
					removeDir(path);
				}
				else
				{
					//not folder, delete it. if empty folder, using _rmdir instead.
					remove(path.c_str());
				}
			}
		}
		// close the folder and delete it only if it is closed. For standard c, using closedir instead(findclose -> closedir).
		// when Handle is created, it should be closed at last.
		_findclose(handle);
		return 0;
	}
}

/*******************************************/
/* Main Function for Collision Checking */
/*******************************************/

//convex hull detect (fix extruder hull, move point, check point in/out hull) <---- fasest
void GcodeGeneration::detectCollision_1() {

	std::cout << "------------------------------------------- Collision Detection Running ..." << std::endl;
	long time = clock();

	QMeshPatch* eHead = (QMeshPatch*)polygenMesh_ExtruderHead->GetMeshList().GetHead();

	for (GLKPOSITION eHeadPos = eHead->GetNodeList().GetHeadPosition(); eHeadPos;) {
		QMeshNode* eHeadNode = (QMeshNode*)eHead->GetNodeList().GetNext(eHeadPos);

		eHeadNode->SetCoord3D(eHeadNode->m_printPos(0), eHeadNode->m_printPos(1), eHeadNode->m_printPos(2));
	}
	QHULLSET* eHeadConvexFront = buildConvexHull_extruderHead(eHead);

#pragma omp parallel
	{
#pragma omp for  
		for (int omptime = 0; omptime < Core; omptime++) {

			for (GLKPOSITION Pos = polygenMesh_Waypoints->GetMeshList().GetHeadPosition(); Pos;) {
				QMeshPatch* WayPointPatch = (QMeshPatch*)polygenMesh_Waypoints->GetMeshList().GetNext(Pos);

				if (WayPointPatch->GetIndexNo() < LayerInd_From || WayPointPatch->GetIndexNo() > LayerInd_To) continue;

				if (WayPointPatch->GetIndexNo() % Core != omptime) continue;

				bool collisionPatch = false;

				for (GLKPOSITION Pos = WayPointPatch->GetNodeList().GetHeadPosition(); Pos;) {
					QMeshNode* Node = (QMeshNode*)WayPointPatch->GetNodeList().GetNext(Pos);
					// restore the collision flag status
					Node->isCollision = false;
					Node->iscollided = false;

					double printHeadPos_x = Node->m_XYZBCE(0);
					double printHeadPos_y = Node->m_XYZBCE(1);
					double printHeadPos_z = Node->m_XYZBCE(2);
					double platformRad_B = DEGREE_TO_ROTATE(Node->m_XYZBCE(3));
					double platformRad_C = DEGREE_TO_ROTATE(Node->m_XYZBCE(4));

					// Test previous points in the same layer
					GLKPOSITION prevPos = polygenMesh_Waypoints->GetMeshList().Find(WayPointPatch);
					QMeshPatch* prevWayPointPatch = (QMeshPatch*)polygenMesh_Waypoints->GetMeshList().GetAt(prevPos);

					for (GLKPOSITION prevNodePos = prevWayPointPatch->GetNodeList().GetHeadPosition(); prevNodePos;) {
						QMeshNode* prevNode = (QMeshNode*)prevWayPointPatch->GetNodeList().GetNext(prevNodePos);

						if (prevNode->GetIndexNo() >= Node->GetIndexNo()) break;

						double Px = prevNode->m_printPos(0);
						double Py = prevNode->m_printPos(1);
						double Pz = prevNode->m_printPos(2);
						double B = platformRad_B;
						double C = platformRad_C;

						double X = cos(B) * cos(C) * Px - cos(B) * sin(C) * Py + sin(B) * Pz - printHeadPos_x;
						double Y = sin(C) * Px + cos(C) * Py - printHeadPos_y;
						double Z = -sin(B) * cos(C) * Px + sin(B) * sin(C) * Py + cos(B) * Pz - printHeadPos_z;

						double pnt[3] = { X,Y,Z };
						bool isInHull = _isPntInsideConvexHull(eHeadConvexFront, pnt);
						if (isInHull) {
							collisionPatch = true;
							Node->isCollision = true;
							prevNode->iscollided = true;
							//std::cout << "Layer:[" << WayPointPatch->GetIndexNo() << "]\tPnt Index:[" << Node->GetIndexNo() << "]\t-COLLISION (same layer)." << std::endl;
							//std::cout << "Collided point: Index:[" << prevNode->GetIndexNo() << "]" << std::endl;
							break;
						}
					}


					// Test previous layer
					int layerLoop = 0;
					for (GLKPOSITION prevLayerPos = polygenMesh_Waypoints->GetMeshList().Find(WayPointPatch)->prev; prevLayerPos;) {
						QMeshPatch* prevLayerWayPointPatch = (QMeshPatch*)polygenMesh_Waypoints->GetMeshList().GetPrev(prevLayerPos);

						if (layerLoop > layerDepth) break;

						for (GLKPOSITION prevLayerNodePos = prevLayerWayPointPatch->GetNodeList().GetHeadPosition(); prevLayerNodePos;) {
							QMeshNode* prevLayerNode = (QMeshNode*)prevLayerWayPointPatch->GetNodeList().GetNext(prevLayerNodePos);

							double Px = prevLayerNode->m_printPos(0);
							double Py = prevLayerNode->m_printPos(1);
							double Pz = prevLayerNode->m_printPos(2);
							double B = platformRad_B;
							double C = platformRad_C;

							double X = cos(B) * cos(C) * Px - cos(B) * sin(C) * Py + sin(B) * Pz - printHeadPos_x;
							double Y = sin(C) * Px + cos(C) * Py - printHeadPos_y;
							double Z = -sin(B) * cos(C) * Px + sin(B) * sin(C) * Py + cos(B) * Pz - printHeadPos_z;

							double pnt[3] = { X,Y,Z };
							bool isInHull = _isPntInsideConvexHull(eHeadConvexFront, pnt);

							if (isInHull) {
								collisionPatch = true;
								Node->isCollision = true;
								prevLayerNode->iscollided = true;
								//std::cout << "Layer:[" << WayPointPatch->GetIndexNo() << "]\tPnt Index:[" << Node->GetIndexNo() << "]\t-COLLISION (different Layer)." << std::endl;
								//std::cout << "Collided point: Layer:[" << prevLayerWayPointPatch->GetIndexNo() << "]\tPnt Index:[" << prevLayerNode->GetIndexNo() << "]" << std::endl;
								break;
							}
						}
						layerLoop++;
					}

					

					// Test plateform collision
					QMeshPatch* plateform_Patch = (QMeshPatch*)polygenMesh_Platform->GetMeshList().GetHead();
					for (GLKPOSITION plateform_NodePos = plateform_Patch->GetNodeList().GetHeadPosition(); plateform_NodePos;) {
						QMeshNode* plateform_Node = (QMeshNode*)plateform_Patch->GetNodeList().GetNext(plateform_NodePos);

						double Px = plateform_Node->m_printPos(0);
						double Py = plateform_Node->m_printPos(1);
						double Pz = plateform_Node->m_printPos(2);
						double B = platformRad_B;
						double C = platformRad_C;

						double X = cos(B) * cos(C) * Px - cos(B) * sin(C) * Py + sin(B) * Pz - printHeadPos_x;
						double Y = sin(C) * Px + cos(C) * Py - printHeadPos_y;
						double Z = -sin(B) * cos(C) * Px + sin(B) * sin(C) * Py + cos(B) * Pz - printHeadPos_z;

						double pnt[3] = { X,Y,Z };
						bool isInHull = _isPntInsideConvexHull(eHeadConvexFront, pnt);

						if (isInHull) {
							collisionPatch = true;
							Node->isCollision = true;
							plateform_Node->iscollided = true;
							//std::cout << "Layer:[" << WayPointPatch->GetIndexNo() << "]\tPnt Index:[" << Node->GetIndexNo() << "]\t-COLLISION ( Plateform )." << std::endl;
							//std::cout << "Collided point: Index:[" << plateform_Patch->GetIndexNo() << "]" << std::endl;
							break;
						}

					}

				}
				if (collisionPatch) { std::cout << "Collision patch Index " << WayPointPatch->GetIndexNo() << std::endl; }
				//std::cout << "\n----------------------- Layer " << WayPointPatch->GetIndexNo() << " detected.\n" << std::endl;
			}
		}
	}
	_freeMemoryConvexHull(eHeadConvexFront);
	printf("TIMER -- Collision Detection takes %ld ms.\n", clock() - time);
	std::cout << "------------------------------------------- Collision Detection Finish!\n" << std::endl;
}
//convex hull detect (move extruder hull to model in model coordinate system) <---- easy to understand
void GcodeGeneration::detectCollision_2() {

	std::cout << "------------------------------------------- Collision Detection Running ..." << std::endl;
	long time = clock();

	QMeshPatch* eHeadPatch = (QMeshPatch*)polygenMesh_ExtruderHead->GetMeshList().GetHead();
	QMeshPatch* plateform_Patch = (QMeshPatch*)polygenMesh_Platform->GetMeshList().GetHead();

	for (GLKPOSITION Pos = polygenMesh_Waypoints->GetMeshList().GetHeadPosition(); Pos;) {
		QMeshPatch* WayPointPatch = (QMeshPatch*)polygenMesh_Waypoints->GetMeshList().GetNext(Pos);

		bool collisionPatch = false;
		if (WayPointPatch->GetIndexNo() < LayerInd_From || WayPointPatch->GetIndexNo() > LayerInd_To) continue;

		for (GLKPOSITION Pos = WayPointPatch->GetNodeList().GetHeadPosition(); Pos;) {
			QMeshNode* Node = (QMeshNode*)WayPointPatch->GetNodeList().GetNext(Pos);

			Node->isCollision = false;
			//Node->iscollided = false;

			Eigen::Vector3d nodePos, norm;
			Node->GetCoord3D(nodePos(0), nodePos(1), nodePos(2));
			Node->GetNormal(norm(0), norm(1), norm(2));
			_locate_EHead_printPos(eHeadPatch, nodePos, norm);
			QHULLSET* eHeadConvexFront = buildConvexHull_extruderHead(eHeadPatch);

			// Test all of the previous-layers' node +-+-+ all of the previous-node before the deteted node at the same layer
			std::vector<QMeshPatch*> check_below_WpSet;
			int layerLoop = 0;
			// collect the Waypoint patch before the printed node now
			for (GLKPOSITION prevPos = polygenMesh_Waypoints->GetMeshList().Find(WayPointPatch); prevPos;) {
				QMeshPatch* prevWpPatch = (QMeshPatch*)polygenMesh_Waypoints->GetMeshList().GetPrev(prevPos);

				if (layerLoop > layerDepth) break;

				check_below_WpSet.push_back(prevWpPatch);

				layerLoop++;
			}
			// speed up the code about the _checkSingleNodeCollision();
#pragma omp parallel
			{
#pragma omp for  

				for (int i = 0; i < check_below_WpSet.size(); i++) {

					for (GLKPOSITION prevWpNodePos = check_below_WpSet[i]->GetNodeList().GetHeadPosition(); prevWpNodePos;) {
						QMeshNode* prevWpNode = (QMeshNode*)check_below_WpSet[i]->GetNodeList().GetNext(prevWpNodePos);

						if (WayPointPatch->GetIndexNo() == check_below_WpSet[i]->GetIndexNo()) {
							if (prevWpNode->GetIndexNo() >= Node->GetIndexNo()) continue;
						}

						Eigen::Vector3d node_coord3D = prevWpNode->m_printPos;
						double pnt[3] = { node_coord3D[0],node_coord3D[1],node_coord3D[2] };
						bool isInHull = _isPntInsideConvexHull(eHeadConvexFront, pnt);

						if (isInHull) {
							collisionPatch = true;
							Node->isCollision = true;
							prevWpNode->iscollided = true;
							//std::cout << "Layer:[" << WayPointPatch->GetIndexNo() << "]\tPnt Index:[" << Node->GetIndexNo() << "]\t-COLLISION (different Layer)." << std::endl;
							//std::cout << "Collided point: Layer:[" << check_below_WpSet[i]->GetIndexNo() << "]\tPnt Index:[" << prevWpNode->GetIndexNo() << "]" << std::endl;	
							break;
						}
					}
					if (collisionPatch) break;
				}
				//check platform nodes
				for (GLKPOSITION plateform_NodePos = plateform_Patch->GetNodeList().GetHeadPosition(); plateform_NodePos;) {
					QMeshNode* plateform_Node = (QMeshNode*)plateform_Patch->GetNodeList().GetNext(plateform_NodePos);

					Eigen::Vector3d node_coord3D = plateform_Node->m_printPos;
					double pnt[3] = { node_coord3D[0],node_coord3D[1],node_coord3D[2] };
					bool isInHull = _isPntInsideConvexHull(eHeadConvexFront, pnt);

					if (isInHull) {
						collisionPatch = true;
						Node->isCollision = true;
						plateform_Node->iscollided = true;
						//std::cout << "Layer:[" << WayPointPatch->GetIndexNo() << "]\tPnt Index:[" << Node->GetIndexNo() << "]\t-COLLISION (different Layer)." << std::endl;
						//std::cout << "Collided point: Layer:[" << check_below_WpSet[i]->GetIndexNo() << "]\tPnt Index:[" << prevWpNode->GetIndexNo() << "]" << std::endl;	
						break;
					}
				}
			}
			_freeMemoryConvexHull(eHeadConvexFront);
		}
		if (collisionPatch) {
			std::cout << " --> Collision patch Index " << WayPointPatch->GetIndexNo() << std::endl;
		}
		else {
			std::cout << " --> Collision-free patch Index " << WayPointPatch->GetIndexNo() << std::endl;
		}
	}
	std::printf("\nTIMER -- Collision Detection takes %ld ms.\n", clock() - time);
	std::cout << "------------------------------------------- Collision Detection Finish!\n" << std::endl;
}

void GcodeGeneration::detectCollision_continuous() {

	std::cout << "------------------------------------------- Continuous Collision Detection Running ..." << std::endl;
	long time = clock();

	QMeshPatch* eHeadPatch = (QMeshPatch*)polygenMesh_ExtruderHead->GetMeshList().GetHead();
	QMeshPatch* plateform_Patch = (QMeshPatch*)polygenMesh_Platform->GetMeshList().GetHead();

	for (GLKPOSITION Pos = polygenMesh_Waypoints->GetMeshList().GetHeadPosition(); Pos;) {
		QMeshPatch* WayPointPatch = (QMeshPatch*)polygenMesh_Waypoints->GetMeshList().GetNext(Pos);

		bool collisionPatch = false;
		if (WayPointPatch->GetIndexNo() < LayerInd_From || WayPointPatch->GetIndexNo() > LayerInd_To) continue;

		for (GLKPOSITION Pos = WayPointPatch->GetEdgeList().GetHeadPosition(); Pos;) {
			QMeshEdge* Edge = (QMeshEdge*)WayPointPatch->GetEdgeList().GetNext(Pos);

			Edge->isCollision = false;
			
			//skip the jump edges
			if ((Edge->GetStartPoint()->Jump_preSecEnd == true && Edge->GetEndPoint()->Jump_nextSecStart == true)
				|| (Edge->GetEndPoint()->Jump_preSecEnd == true && Edge->GetStartPoint()->Jump_nextSecStart == true)) {

				continue;
			}

			Eigen::Vector3d nodePos_1, norm_1, nodePos_2, norm_2;

			Edge->GetStartPoint()->GetCoord3D(nodePos_1(0), nodePos_1(1), nodePos_1(2));
			Edge->GetEndPoint()->GetCoord3D(nodePos_2(0), nodePos_2(1), nodePos_2(2));

			Edge->GetStartPoint()->GetNormal(norm_1(0), norm_1(1), norm_1(2));
			Edge->GetEndPoint()->GetNormal(norm_2(0), norm_2(1), norm_2(2));

			//build a point set for convex hull building which contains two set of nodes of eHead
			std::vector<Eigen::Vector3d> eHead_pointSet(2 * eHeadPatch->GetNodeNumber());

			_locate_EHead_printPos(eHeadPatch, nodePos_1, norm_1);
			//load the coordinate of ehead node in start point
			_load_eHead_points(eHeadPatch, eHead_pointSet, 0);

			_locate_EHead_printPos(eHeadPatch, nodePos_2, norm_2);
			//load the coordinate of ehead node in end point
			_load_eHead_points(eHeadPatch, eHead_pointSet, eHeadPatch->GetNodeNumber());

			//std::cout << "-------------------------\n";
			//for (int jk = 0; jk < eHead_pointSet.size(); jk++) {std::cout << eHead_pointSet[jk].transpose() << std::endl;}

			QHULLSET* eHeadConvexFront = buildConvexHull_extruderHead(eHeadPatch);

			// Test all of the previous-layers' node +-+-+ all of the previous-node before the deteted node at the same layer
			std::vector<QMeshPatch*> check_below_WpSet;
			int layerLoop = 0;
			// collect the Waypoint patch before the printed node now
			for (GLKPOSITION prevPos = polygenMesh_Waypoints->GetMeshList().Find(WayPointPatch); prevPos;) {
				QMeshPatch* prevWpPatch = (QMeshPatch*)polygenMesh_Waypoints->GetMeshList().GetPrev(prevPos);

				if (layerLoop > layerDepth) break;

				check_below_WpSet.push_back(prevWpPatch);

				layerLoop++;
			}
			// speed up the code about the _checkSingleNodeCollision();
#pragma omp parallel
			{
#pragma omp for  

				for (int i = 0; i < check_below_WpSet.size(); i++) {

					for (GLKPOSITION prevWpNodePos = check_below_WpSet[i]->GetNodeList().GetHeadPosition(); prevWpNodePos;) {
						QMeshNode* prevWpNode = (QMeshNode*)check_below_WpSet[i]->GetNodeList().GetNext(prevWpNodePos);

						if (WayPointPatch->GetIndexNo() == check_below_WpSet[i]->GetIndexNo()) {
							if (prevWpNode->GetIndexNo() >= Edge->GetStartPoint()->GetIndexNo()
								|| prevWpNode->GetIndexNo() >= Edge->GetEndPoint()->GetIndexNo())
								continue;
						}

						Eigen::Vector3d node_coord3D = prevWpNode->m_printPos;
						double pnt[3] = { node_coord3D[0],node_coord3D[1],node_coord3D[2] };
						bool isInHull = _isPntInsideConvexHull(eHeadConvexFront, pnt);

						if (isInHull) {
							collisionPatch = true;
							Edge->isCollision = true;
							prevWpNode->iscollided = true;
							//std::cout << "Layer:[" << WayPointPatch->GetIndexNo() << "]\tPnt Index:[" << Node->GetIndexNo() << "]\t-COLLISION (different Layer)." << std::endl;
							//std::cout << "Collided point: Layer:[" << check_below_WpSet[i]->GetIndexNo() << "]\tPnt Index:[" << prevWpNode->GetIndexNo() << "]" << std::endl;	
							break;
						}
					}
					if (collisionPatch) break;
				}
				//check platform nodes
				for (GLKPOSITION plateform_NodePos = plateform_Patch->GetNodeList().GetHeadPosition(); plateform_NodePos;) {
					QMeshNode* plateform_Node = (QMeshNode*)plateform_Patch->GetNodeList().GetNext(plateform_NodePos);

					Eigen::Vector3d node_coord3D = plateform_Node->m_printPos;
					double pnt[3] = { node_coord3D[0],node_coord3D[1],node_coord3D[2] };
					bool isInHull = _isPntInsideConvexHull(eHeadConvexFront, pnt);

					if (isInHull) {
						collisionPatch = true;
						Edge->isCollision = true;
						plateform_Node->iscollided = true;
						//std::cout << "Layer:[" << WayPointPatch->GetIndexNo() << "]\tPnt Index:[" << Node->GetIndexNo() << "]\t-COLLISION (different Layer)." << std::endl;
						//std::cout << "Collided point: Layer:[" << check_below_WpSet[i]->GetIndexNo() << "]\tPnt Index:[" << prevWpNode->GetIndexNo() << "]" << std::endl;	
						break;
					}
				}
			}
			_freeMemoryConvexHull(eHeadConvexFront);
		}
		if (collisionPatch) {
			std::cout << " --> Collision patch Index " << WayPointPatch->GetIndexNo() << std::endl;
		}
		else {
			std::cout << " --> Collision-free patch Index " << WayPointPatch->GetIndexNo() << std::endl;
		}
	}
	std::printf("\nTIMER -- Collision Detection takes %ld ms.\n", clock() - time);
	std::cout << "------------------------------------------- Collision Detection Finish!\n" << std::endl;
}

QHULLSET* GcodeGeneration::buildConvexHull_extruderHead(QMeshPatch* eHead) {
	facetT* facet;		vertexT* vertex, ** vertexp;
	int i, index, pntNum, num, stIndex;			float pos[3];
	double vec[3][3], dir[3], v1[3], v2[3], pp[3];
	QHULLSET* newConvexFront = NULL; // new convexhull used for checking

	//-------------------------------------------------------------------------------------
	//	Step 1: initialization

	pntNum = eHead->GetNodeNumber();
	double* pntArray = (double*)malloc(sizeof(double) * 3 * pntNum); //points use to compute convex hull

	int nodeIndex = 0;
	for (GLKPOSITION posMesh = eHead->GetNodeList().GetHeadPosition(); posMesh != nullptr;) {
		QMeshNode* node = (QMeshNode*)eHead->GetNodeList().GetNext(posMesh);
		node->GetCoord3D(pp[0], pp[1], pp[2]);
		for (int i = 0; i < 3; i++)
			pntArray[nodeIndex * 3 + i] = pp[i];
		nodeIndex++;
	}

	//-------------------------------------------------------------------------------------
	//	Step 2: computaing the convex-hull
	qh_init_A(stdin, stdout, stderr, 0, NULL);
	qh_initflags("Qt Qx");
	qh_init_B(pntArray, pntNum, 3, false);
	qh_qhull();
	qh_check_output();
	qh_triangulate();
	if (qh VERIFYoutput && !qh STOPpoint && !qh STOPcone) qh_check_points();

	//-------------------------------------------------------------------------------------
	//	Step 3: output the results of convex-hull computation
	int nodeNum = 0, faceNum = 0;
	faceNum = qh_qh.num_facets;		nodeNum = qh_qh.num_vertices;
	//printf("Convex-Hull: %d faces with %d vertices\n",faceNum,nodeNum);
	if (faceNum > 0 && nodeNum > 0) {
		newConvexFront = _mallocMemoryConvexHull(faceNum, nodeNum);
		//---------------------------------------------------------------------------------
		index = 0;
		FORALLvertices{
			vertex->id = index;	// before this assignment, "vertex->id" contains the id of input vertices
		newConvexFront->vertPos[index * 3] = vertex->point[0];
		newConvexFront->vertPos[index * 3 + 1] = vertex->point[1];
		newConvexFront->vertPos[index * 3 + 2] = vertex->point[2];
		index++;
		}
			//---------------------------------------------------------------------------------
		index = 0;
		FORALLfacets{
			newConvexFront->normalVec[index * 3] = facet->normal[0];
		newConvexFront->normalVec[index * 3 + 1] = facet->normal[1];
		newConvexFront->normalVec[index * 3 + 2] = facet->normal[2];
		newConvexFront->offset[index] = facet->offset;
		//	It has been verified all normal[] vectors generated by qhull library are pointing outwards and are unit-vectors 
		//		(verified by the function -- QuadTrglMesh* convexHullGeneration(QuadTrglMesh* inputMesh)  ).

		int i = 0;
		FOREACHvertex_(facet->vertices) {
			newConvexFront->faceTable[index * 3 + i] = vertex->id + 1; //index start from 1;
			//newConvexFront->faceTable[index * 3 + i] = vertex->id; //index start from 0;

			SET(vec[i],vertex->point);
			i++;
			if (i >= 3) break; // Note that it could be a facet with more than 3 vertices if not applying "qh_triangulate();"
		}

		//-----------------------------------------------------------------------------
		//	Check if the vertices on this face is given in the anti-clockwise order
		SUB(v1,vec[1],vec[0]);
		SUB(v2,vec[2],vec[0]);
		CROSS(dir,v1,v2);
		if (DOT(dir,facet->normal) < 0) {
			unsigned int temp = newConvexFront->faceTable[index * 3];
			newConvexFront->faceTable[index * 3] = newConvexFront->faceTable[index * 3 + 2];
			newConvexFront->faceTable[index * 3 + 2] = temp;
		}

		index++;
		}
	}

	//-------------------------------------------------------------------------------------
	//	Step 4: free the memory
	int curlong, totlong;
	qh_freeqhull(false);
	qh_memfreeshort(&curlong, &totlong);
	if (curlong || totlong) fprintf(stderr, "qhull internal warning (main): did not free %d bytes of long memory (%d pieces)\n", totlong, curlong);
	//-------------------------------------------------------------------------------------
	free(pntArray);

	return newConvexFront;
}

QHULLSET* GcodeGeneration::buildConvexHull_extruderHead(const std::vector<Eigen::Vector3d>& eHead_pointSet) {

	facetT* facet;		vertexT* vertex, ** vertexp;
	int i, index, pntNum, num, stIndex;			float pos[3];
	double vec[3][3], dir[3], v1[3], v2[3]; //pp[3]
	QHULLSET* newConvexFront = NULL; // new convexhull used for checking

	//-------------------------------------------------------------------------------------
	//	Step 1: initialization

	pntNum = eHead_pointSet.size();
	double* pntArray = (double*)malloc(sizeof(double) * 3 * pntNum); //points use to compute convex hull

	//int nodeIndex = 0;
	//for (GLKPOSITION posMesh = eHead->GetNodeList().GetHeadPosition(); posMesh != nullptr;) {
	//	QMeshNode* node = (QMeshNode*)eHead->GetNodeList().GetNext(posMesh);
	//	node->GetCoord3D(pp[0], pp[1], pp[2]);
	//	for (int i = 0; i < 3; i++)
	//		pntArray[nodeIndex * 3 + i] = pp[i];
	//	nodeIndex++;
	//}

	for (int nodeIndex = 0; nodeIndex < pntNum; nodeIndex++) {

		for (int i = 0; i < 3; i++)
			pntArray[nodeIndex * 3 + i] = eHead_pointSet[nodeIndex][i];
	}

	//-------------------------------------------------------------------------------------
	//	Step 2: computaing the convex-hull
	qh_init_A(stdin, stdout, stderr, 0, NULL);
	qh_initflags("Qt Qx");
	qh_init_B(pntArray, pntNum, 3, false);
	qh_qhull();
	qh_check_output();
	qh_triangulate();
	if (qh VERIFYoutput && !qh STOPpoint && !qh STOPcone) qh_check_points();

	//-------------------------------------------------------------------------------------
	//	Step 3: output the results of convex-hull computation
	int nodeNum = 0, faceNum = 0;
	faceNum = qh_qh.num_facets;		nodeNum = qh_qh.num_vertices;
	//printf("Convex-Hull: %d faces with %d vertices\n",faceNum,nodeNum);
	if (faceNum > 0 && nodeNum > 0) {
		newConvexFront = _mallocMemoryConvexHull(faceNum, nodeNum);
		//---------------------------------------------------------------------------------
		index = 0;
		FORALLvertices{
			vertex->id = index;	// before this assignment, "vertex->id" contains the id of input vertices
		newConvexFront->vertPos[index * 3] = vertex->point[0];
		newConvexFront->vertPos[index * 3 + 1] = vertex->point[1];
		newConvexFront->vertPos[index * 3 + 2] = vertex->point[2];
		index++;
		}
			//---------------------------------------------------------------------------------
		index = 0;
		FORALLfacets{
			newConvexFront->normalVec[index * 3] = facet->normal[0];
		newConvexFront->normalVec[index * 3 + 1] = facet->normal[1];
		newConvexFront->normalVec[index * 3 + 2] = facet->normal[2];
		newConvexFront->offset[index] = facet->offset;
		//	It has been verified all normal[] vectors generated by qhull library are pointing outwards and are unit-vectors 
		//		(verified by the function -- QuadTrglMesh* convexHullGeneration(QuadTrglMesh* inputMesh)  ).

		int i = 0;
		FOREACHvertex_(facet->vertices) {
			newConvexFront->faceTable[index * 3 + i] = vertex->id + 1; //index start from 1;
			//newConvexFront->faceTable[index * 3 + i] = vertex->id; //index start from 0;

			SET(vec[i],vertex->point);
			i++;
			if (i >= 3) break; // Note that it could be a facet with more than 3 vertices if not applying "qh_triangulate();"
		}

		//-----------------------------------------------------------------------------
		//	Check if the vertices on this face is given in the anti-clockwise order
		SUB(v1,vec[1],vec[0]);
		SUB(v2,vec[2],vec[0]);
		CROSS(dir,v1,v2);
		if (DOT(dir,facet->normal) < 0) {
			unsigned int temp = newConvexFront->faceTable[index * 3];
			newConvexFront->faceTable[index * 3] = newConvexFront->faceTable[index * 3 + 2];
			newConvexFront->faceTable[index * 3 + 2] = temp;
		}

		index++;
		}
	}

	//-------------------------------------------------------------------------------------
	//	Step 4: free the memory
	int curlong, totlong;
	qh_freeqhull(false);
	qh_memfreeshort(&curlong, &totlong);
	if (curlong || totlong) fprintf(stderr, "qhull internal warning (main): did not free %d bytes of long memory (%d pieces)\n", totlong, curlong);
	//-------------------------------------------------------------------------------------
	free(pntArray);

	return newConvexFront;
}

QHULLSET* GcodeGeneration::_mallocMemoryConvexHull(int faceNum, int vertNum)
{
	QHULLSET* pConvexHull;

	pConvexHull = (QHULLSET*)malloc(sizeof(QHULLSET));
	pConvexHull->faceNum = faceNum;
	pConvexHull->normalVec = (double*)malloc(sizeof(double) * 3 * faceNum);
	pConvexHull->offset = (double*)malloc(sizeof(double) * faceNum);

	pConvexHull->faceTable = (unsigned int*)malloc(sizeof(unsigned int) * 3 * faceNum);

	pConvexHull->vertNum = vertNum;
	pConvexHull->vertPos = (double*)malloc(sizeof(double) * 3 * vertNum);

	return pConvexHull;
}

bool GcodeGeneration::_isPntInsideConvexHull(QHULLSET* pConvexHull, double pnt[]) {
	double normVec[3], offValue;

	for (int i = 0; i < pConvexHull->faceNum; i++) {
		normVec[0] = pConvexHull->normalVec[i * 3];
		normVec[1] = pConvexHull->normalVec[i * 3 + 1];
		normVec[2] = pConvexHull->normalVec[i * 3 + 2];
		offValue = pConvexHull->offset[i];
		if ((DOT(pnt, normVec) + offValue) >= 0.0) return false;

	}
	return true;
}

void GcodeGeneration::_freeMemoryConvexHull(QHULLSET*& pConvexHull)
{
	free((pConvexHull->normalVec));
	free((pConvexHull->offset));
	free((pConvexHull->faceTable));
	free((pConvexHull->vertPos));
	free(pConvexHull);

	pConvexHull = NULL;
}

void GcodeGeneration::_locate_EHead_printPos(QMeshPatch* eHeadPatch, Eigen::Vector3d nodePos, Eigen::Vector3d norm) {
	// rotate the nozzle mesh to print orientation and move to the tip position
	Eigen::Vector3d eHeadInitNorm = { 0, 0, 1.0 };
	Eigen::Matrix3d rotationMatrix;

	rotationMatrix = Eigen::Quaterniond().setFromTwoVectors(eHeadInitNorm, norm);

	//std::cout << rotationMatrix << std::endl;

	for (GLKPOSITION Pos = eHeadPatch->GetNodeList().GetHeadPosition(); Pos;) {
		QMeshNode* eHeadNode = (QMeshNode*)eHeadPatch->GetNodeList().GetNext(Pos);

		Eigen::Vector3d eHeadNodePos;

		eHeadNode->GetCoord3D_last(eHeadNodePos(0), eHeadNodePos(1), eHeadNodePos(2));
		//nozzleNodePos = rotationMatrix * nozzleNodePos;
		eHeadNodePos = rotationMatrix * eHeadNodePos + nodePos;

		eHeadNode->SetCoord3D(eHeadNodePos(0), eHeadNodePos(1), eHeadNodePos(2));
		//std::cout << nozzleNodePos(0) << "," << nozzleNodePos(1) << "," << nozzleNodePos(2) << std::endl;
	}
}

void GcodeGeneration::_load_eHead_points(QMeshPatch* eHeadPatch, std::vector<Eigen::Vector3d>& eHead_pointSet, int start_Index) {

	int index = start_Index;
	for (GLKPOSITION Pos = eHeadPatch->GetNodeList().GetHeadPosition(); Pos;) {
		QMeshNode* eHeadNode = (QMeshNode*)eHeadPatch->GetNodeList().GetNext(Pos);

		Eigen::Vector3d eHeadNodePos;
		eHeadNode->GetCoord3D(eHeadNodePos(0), eHeadNodePos(1), eHeadNodePos(2));

		eHead_pointSet[index] = eHeadNodePos;
		index++;
	}
}

/*******************************************/
/* Main Function for Graph Search */
/*******************************************/
void GcodeGeneration::graph_Search_Shortest_Path() {

	std::cout << "------------------------------------------- Graph Search running ... " << std::endl;
	long time = clock();

	for (GLKPOSITION Pos = polygenMesh_Waypoints->GetMeshList().GetHeadPosition(); Pos;) {
		QMeshPatch* WayPointPatch = (QMeshPatch*)polygenMesh_Waypoints->GetMeshList().GetNext(Pos);

		if (WayPointPatch->GetIndexNo() < LayerInd_From || WayPointPatch->GetIndexNo() > LayerInd_To) continue;

		std::vector<QMeshPatch*> layerJumpPatchSet = getJumpSection_patchSet(WayPointPatch);

		//each Jump Section
		for (int Index = 0; Index < layerJumpPatchSet.size(); Index++) {
			//1. generate a vector to store each graph_Node in the form of collision_Node(a temp struct)
			std::vector<collision_Node> graph_Node;
			_get_GraphNode_List(layerJumpPatchSet[Index], graph_Node);
			_build_Graph(layerJumpPatchSet[Index], graph_Node);	
			getXYZ(layerJumpPatchSet[Index]);
		}
		//aim to eliminate the -pi to pi sharp change
		optimizationC(WayPointPatch);
	}

	std::cout << "-------------------------------------------" << std::endl;
	std::printf("TIMER -- Graph Search takes %ld ms.\n", clock() - time);
	std::cout << "------------------------------------------- Graph Search Finish!\n " << std::endl;

}

void GcodeGeneration::_get_GraphNode_List(QMeshPatch* patch, std::vector<collision_Node>& graph_Node) {

	QMeshPatch* eHeadPatch = (QMeshPatch*)polygenMesh_ExtruderHead->GetMeshList().GetHead();

	for (GLKPOSITION Pos = patch->GetNodeList().GetHeadPosition(); Pos;) {
		QMeshNode* Node = (QMeshNode*)patch->GetNodeList().GetNext(Pos);

		if (Node->isCollision) {

			double candidate_normal_NUM = 0;
			for (int ZrotateAngle = 0; ZrotateAngle < 360; ZrotateAngle = ZrotateAngle + delta_Z) {
				for (int XtiltAngle = 90; XtiltAngle > 0; XtiltAngle = XtiltAngle - delta_X) {

					double rad_ZrotateAngle = DEGREE_TO_ROTATE(ZrotateAngle);
					double rad_XtiltAngle = DEGREE_TO_ROTATE(XtiltAngle);

					Eigen::Vector3d candidateNor = _calCandidateNormal(Node->m_printNor, rad_ZrotateAngle, rad_XtiltAngle);
					//cout << "candidateNor:\n " << candidateNor << endl;

					bool iscollision_candidate_cNode = false;
					_locate_EHead_printPos(eHeadPatch, Node->m_printPos, candidateNor);
					QHULLSET* eHeadConvexFront = buildConvexHull_extruderHead(eHeadPatch);
					// Test all of the previous-layers' node +-+-+ all of the previous-node before the deteted node at the same layer
					std::vector<QMeshPatch*> check_below_WpSet;
					int layerLoop = 0;
					// collect the Waypoint patch before the printed node now
					for (GLKPOSITION prevPos = polygenMesh_Waypoints->GetMeshList().Find(patch->rootPatch_jumpPatch); prevPos;) {
						QMeshPatch* prevWpPatch = (QMeshPatch*)polygenMesh_Waypoints->GetMeshList().GetPrev(prevPos);

						if (layerLoop > layerDepth) break;

						check_below_WpSet.push_back(prevWpPatch);

						layerLoop++;
					}
					// speed up the code about the _checkSingleNodeCollision();
#pragma omp parallel
					{
#pragma omp for  
						for (int i = 0; i < check_below_WpSet.size(); i++) {

							for (GLKPOSITION prevWpNodePos = check_below_WpSet[i]->GetNodeList().GetHeadPosition(); prevWpNodePos;) {
								QMeshNode* prevWpNode = (QMeshNode*)check_below_WpSet[i]->GetNodeList().GetNext(prevWpNodePos);

								if (patch->rootPatch_jumpPatch->GetIndexNo() == check_below_WpSet[i]->GetIndexNo()) {
									if (prevWpNode->GetIndexNo() >= Node->GetIndexNo()) continue;
								}

								Eigen::Vector3d node_coord3D = prevWpNode->m_printPos;
								double pnt[3] = { node_coord3D[0],node_coord3D[1],node_coord3D[2] };
								bool isInHull = _isPntInsideConvexHull(eHeadConvexFront, pnt);

								if (isInHull) {
									iscollision_candidate_cNode = true;
									break;
								}
							}
							if (iscollision_candidate_cNode == true) break;
						}
					}

					if (iscollision_candidate_cNode == false) {
						candidate_normal_NUM++;
						_install_BC(candidateNor, graph_Node, Node, CNC_TYPE);

					}
				}
			}
			// no collision-free candidate, use the orginal one (temp)
			if (candidate_normal_NUM == 0) {
				_install_BC(Node->m_printNor, graph_Node, Node, CNC_TYPE);
			}
		}
		else {
			_install_BC(Node->m_printNor, graph_Node, Node, CNC_TYPE);
		}
	}

	//protect: no candidate normal is danger
	for (GLKPOSITION Pos = patch->GetNodeList().GetHeadPosition(); Pos;) {
		QMeshNode* Node = (QMeshNode*)patch->GetNodeList().GetNext(Pos);

		bool exist_Candidate = false;
		for (int i = 0; i < graph_Node.size(); i++) {

			if (graph_Node[i].waypoint_N_i == Node) {
				exist_Candidate = true;
				break;
			}
		}
		if (!exist_Candidate) std::cout << "The Node "<< Node->GetIndexNo() 
			<< " in patch " << patch->rootPatch_jumpPatch->GetIndexNo() 
			<< " has no collision-free normal!" << std::endl;
	}
}

Eigen::Vector3d GcodeGeneration::_calCandidateNormal(Eigen::Vector3d normal, double rad_ZrotateAngle, double rad_XtiltAngle) {

	Eigen::Matrix3d Xrot_Matrix, Zrot_Matrix, Xback_Matrix, Zback_Matrix;
	Eigen::Vector2d normalXY_temp;
	Eigen::Vector3d candidateNormal, candidateNor_temp;

	normalXY_temp << normal(0), normal(1);// (nx, ny)
	double alpha = atan2(normal(0), normal(1));// normal Z rotate
	double beta = atan2(normalXY_temp.norm(), normal(2));// normal X rotate
	Xback_Matrix << 1, 0, 0, 0, cos(-beta), -sin(-beta), 0, sin(-beta), cos(-beta);
	Zback_Matrix << cos(-alpha), -sin(-alpha), 0, sin(-alpha), cos(-alpha), 0, 0, 0, 1;

	Xrot_Matrix << 1, 0, 0, 0, cos(rad_XtiltAngle), -sin(rad_XtiltAngle), 0, sin(rad_XtiltAngle), cos(rad_XtiltAngle);
	Zrot_Matrix << cos(rad_ZrotateAngle), -sin(rad_ZrotateAngle), 0, sin(rad_ZrotateAngle), cos(rad_ZrotateAngle), 0, 0, 0, 1;
	candidateNor_temp = (Zrot_Matrix * Xrot_Matrix).col(2);// extract last vector of Z direction

	candidateNormal = Zback_Matrix * Xback_Matrix * candidateNor_temp;
	return candidateNormal;
}

void GcodeGeneration::_install_BC(Eigen::Vector3d temp_Normal, std::vector<collision_Node>& graph_Node, QMeshNode* sourceNode, int cnc_type) {

	if (cnc_type == 1) {

		collision_Node cNode_1;
		cNode_1.B_value = ROTATE_TO_DEGREE(-_safe_acos(temp_Normal(2)));
		cNode_1.C_value = ROTATE_TO_DEGREE(-atan2(temp_Normal(1), temp_Normal(0)));
		cNode_1.waypoint_N_i = sourceNode;

		// solve 2
		collision_Node cNode_2;
		cNode_2.B_value = -cNode_1.B_value;
		double C2temp = cNode_1.C_value + 180.0;
		if (C2temp > 180.0)	C2temp -= 360.0; // control the range of C2 into the (-180,180]
		cNode_2.C_value = C2temp;
		cNode_2.waypoint_N_i = sourceNode;

		//keep the first is less than second BC solve
		if (abs(cNode_1.C_value) < abs(cNode_2.C_value)) {
			graph_Node.push_back(cNode_1);
			graph_Node.push_back(cNode_2);
		}
		else {
			graph_Node.push_back(cNode_2);
			graph_Node.push_back(cNode_1);
		}
	}
}

void GcodeGeneration::_build_Graph(QMeshPatch* patch, std::vector<collision_Node>& graph_Node) {

	int V = graph_Node.size();
	std::vector<int> startNode_NoSet;
	std::vector<int> endNode_NoSet;

	//get index of end Nodes of graph
	for (int i = 0; i < graph_Node.size(); i++) {
	
		if (graph_Node[i].waypoint_N_i->Jump_SecIndex == 0)
			startNode_NoSet.push_back(i);

		if (graph_Node[i].waypoint_N_i->Jump_SecIndex == (patch->GetNodeNumber() - 1))
			endNode_NoSet.push_back(i);
	}

	Graph g(V, endNode_NoSet);

	for (int i = 0; i < graph_Node.size(); i++) {
		for (int j = (i + 1); j < graph_Node.size(); j++) {

			if ((graph_Node[i].waypoint_N_i->Jump_SecIndex + 1)
				== graph_Node[j].waypoint_N_i->Jump_SecIndex ) {

				double weight = _weight_calculation(
					graph_Node[i].B_value, graph_Node[i].C_value, 
					graph_Node[j].B_value, graph_Node[j].C_value);

				//if (i == graph_Node.size() / 2) weight *= 5;

				g.addEdge(i, weight, j);
			}
		}
	}

	g.shortestPath(0);
	for (int i = 0; i < g.path.size(); i++) {
		graph_Node[g.path[i]].waypoint_N_i->m_XYZBCE(3) = graph_Node[g.path[i]].B_value;
		graph_Node[g.path[i]].waypoint_N_i->m_XYZBCE(4) = graph_Node[g.path[i]].C_value;
		//std::cout << g.path[i] << " ";
	}
	//std::cout << std::endl;

	//BC -> normal -> show
	for (GLKPOSITION Pos = patch->GetNodeList().GetHeadPosition(); Pos;) {
		QMeshNode* Node = (QMeshNode*)patch->GetNodeList().GetNext(Pos);

		double B = DEGREE_TO_ROTATE(Node->m_XYZBCE(3));
		double C = DEGREE_TO_ROTATE(Node->m_XYZBCE(4));

		double finalNx = -cos(C) * sin(B);
		double finalNy = sin(B) * sin(C);
		double finalNz = cos(B);

		Node->m_printNor << finalNx, finalNy, finalNz;
		Node->SetNormal(finalNx, finalNy, finalNz);
	}
	
}

double GcodeGeneration::_weight_calculation(double B_i,double C_i,double B_ii, double C_ii) {

	double rad_B_i = DEGREE_TO_ROTATE(B_i);	
	double rad_C_i = DEGREE_TO_ROTATE(C_i);
	double rad_B_ii = DEGREE_TO_ROTATE(B_ii);
	double rad_C_ii = DEGREE_TO_ROTATE(C_ii);

	Eigen::Vector2d v_C_i = { cos(rad_C_i),sin(rad_C_i) };
	Eigen::Vector2d v_C_ii = { cos(rad_C_ii),sin(rad_C_ii) };

	//compute the actural angle of 2 vectors
	double rad_delta_B = abs(rad_B_i - rad_B_ii);
	double rad_delta_C = _safe_acos(v_C_i.dot(v_C_ii));

	return (rad_delta_B + rad_delta_C);							//L1-Norm
	//return std::sqrt(pow(rad_delta_B, 2) + pow(rad_delta_C, 2));	//L2-Norm
}

/*******************************************/
/* Main Function for Gcode Write */
/*******************************************/
void GcodeGeneration::writeGcode(std::string GcodeDir) {
	std::cout << "------------------------------------------- " << GcodeDir << " Gcode Writing ..." << std::endl;

	// Define the basic parameter
	double Z_home = 230;						// The hight of Home point; / mm
	double Z_high = 50;							// The hight of G1 point(for safety); / mm
	double Z_compensateUpDistance = 2;			// The hight of waiting distance of extruder; / mm

	int sExtruder = 3;							// Support extruder num
	int iExtruder = 1;							// Initial extruder num

	int iTemperature = 205;						// The temperature of Extruder Initial
	int sTemperature = 205;						// The temperature of Extruder Support

	int F_G0_XYBC = 5250;						// Speed of G0 move of XYBC
	int F_G0_Z = 4750;							// Speed of G0 move of Z
	int F_G1_support = 2500;					// Speed of G1 support material (normal 2ed~layers)
	int F_G1_original = 1500;					// Speed of G1 original material (normal 2ed~layers)
	int F_G1_1stlayer = 1500;					// Speed of G1(special 1st layer)
	int F_PumpBack = 4800;						// Speed of F_PumpBack
	int F_PumpCompensate = 4550;				// Speed of PumpCompensate

	double E_PumpBack = -6;						// The extruder pump back Xmm
	double E_PumpCompensate = 9;				// The extruder pump compensate Xmm
	double E_PumpCompensateL1 = 12;				// The extruder pump compensate for 1st layer Xmm
	double E_PumpCompensateNewE = 12;			// The extruder pump compensate for new type layer Xmm

	char targetFilename[1024];
	std::sprintf(targetFilename, "%s%s", "../DataSet/Gcode/", GcodeDir.c_str());
	FILE* fp = fopen(targetFilename, "w");
	if (!fp) {
		perror("Couldn't open the directory");
		return;
	}

	// Record the max Z for security consideration (the first printed layer)
	GLKPOSITION layer1st_Pos = polygenMesh_Waypoints->GetMeshList().FindIndex(LayerInd_From);
	QMeshPatch* layer1st_WayPointPatch = (QMeshPatch*)polygenMesh_Waypoints->GetMeshList().GetAt(layer1st_Pos);

	double Z_max = -99999.9;
	for (GLKPOSITION Pos = layer1st_WayPointPatch->GetNodeList().GetHeadPosition(); Pos;) {
		QMeshNode* Node = (QMeshNode*)layer1st_WayPointPatch->GetNodeList().GetNext(Pos);

		if (Node->m_XYZBCE(2) > Z_max) { Z_max = Node->m_XYZBCE(2); }
	}
	// Record the layer type of the first printed layer
	bool IsSupportLayer_last = layer1st_WayPointPatch->isSupportLayer;

	// Give the start message of G_code
	std::fprintf(fp, "G90\n");
	//std::fprintf(fp, "M104 S200 T2\n");// special solution for the the MANY 3DP CNC
	if (layer1st_WayPointPatch->isSupportLayer == true) {
		std::fprintf(fp, "M104 S%d T%d\n", sTemperature, sExtruder);
		std::fprintf(fp, "G28 X0.000 Y0.000 Z%.3f B0.000 C0.000 F%d\n", Z_home, F_G0_XYBC);
		std::fprintf(fp, "M109 S%d T%d\n", sTemperature, sExtruder);
	}
	else {
		std::fprintf(fp, "M104 S%d T%d\n", iTemperature, iExtruder);
		std::fprintf(fp, "G28 X0.000 Y0.000 Z%.3f B0.000 C0.000 F%d\n", Z_home, F_G0_XYBC);
		std::fprintf(fp, "M109 S%d T%d\n", iTemperature, iExtruder);
	}

	for (GLKPOSITION Pos = polygenMesh_Waypoints->GetMeshList().GetHeadPosition(); Pos;) {
		QMeshPatch* WayPointPatch = (QMeshPatch*)polygenMesh_Waypoints->GetMeshList().GetNext(Pos);

		if (WayPointPatch->GetIndexNo() < LayerInd_From || WayPointPatch->GetIndexNo() > LayerInd_To) continue;

		bool showOnece = true; // show the Z < 0 once

		for (GLKPOSITION Pos = WayPointPatch->GetNodeList().GetHeadPosition(); Pos;) {
			QMeshNode* Node = (QMeshNode*)WayPointPatch->GetNodeList().GetNext(Pos);
			double X = Node->m_XYZBCE(0); double Y = Node->m_XYZBCE(1); double Z = Node->m_XYZBCE(2);
			double B = Node->m_XYZBCE(3); double C = Node->m_XYZBCE(4); double E = Node->m_XYZBCE(5);

			if (WayPointPatch->GetIndexNo() == 0) E *= 1.5; // increase extrusion of 1st layer
			if (WayPointPatch->GetIndexNo() == LayerInd_From) E *= 1.1;// increase the extrusion of the first printed layer
			if (WayPointPatch->isSupportLayer) E *= 0.9; // decrease extrusion of support layer (temp)

			// check the huge change of C angle
			if (Node->GetIndexNo() != 0)
			{
				GLKPOSITION prevPos = WayPointPatch->GetNodeList().Find(Node)->prev;
				QMeshNode* prevNode = (QMeshNode*)WayPointPatch->GetNodeList().GetAt(prevPos);

				double C_prev = prevNode->m_XYZBCE(4);

				if (abs(C - C_prev) > 300
					&& prevNode->Jump_preSecEnd == false && Node->Jump_nextSecStart == false
					) {

					Node->isHighLight = true;
					std::cerr << "abs(C - C_prev) ERROR! " << abs(C - C_prev) << std::endl;
					std::cout << "WayPointPatch->GetIndexNo() " << WayPointPatch->GetIndexNo() << std::endl;
				}

			}
			// check the negative Z motion
			if (Z < 0.0 && showOnece) {
				std::cout << "Layer: " << WayPointPatch->GetIndexNo() << " Z < 0.0 hit the bottom " << std::endl;
				showOnece = false;
			}

			//modify the xy-offset according to the extruder
			if (WayPointPatch->isSupportLayer == true) { X = X + E3_Xoff;	Y = Y + E3_Yoff; }
			// Record the max Z for security consideration (rest layers)
			if (Z > Z_max) { Z_max = Z; }

			// Add some auxiliary G code
			if (Node->GetIndexNo() == 0) {// for the 1st point

				if (WayPointPatch->GetIndexNo() == LayerInd_From) {// for the 1st(LayerInd_From) printed layer
					// move to start of printing location
					std::fprintf(fp, "G0 X%.3f Y%.3f B%.3f C%.3f F%d\n", X, Y, B, C, F_G0_XYBC);
					// slowly lower for printing
					std::fprintf(fp, "G0 Z%.3f F%d\n", (Z_max + Z_compensateUpDistance), F_G0_Z);
					// zero extruded length(set E axis to 0)
					std::fprintf(fp, "G92 E0\n");
					std::fprintf(fp, "G1 E%.3f F%d\n", E_PumpCompensateL1, F_PumpCompensate);
					std::fprintf(fp, "G92 E0\n");
					std::fprintf(fp, "G1 F%d\n", F_G1_1stlayer);
				}
				//new layer and same extruder
				else if (WayPointPatch->isSupportLayer == IsSupportLayer_last) {
					std::fprintf(fp, "G92 E0\n");
					std::fprintf(fp, "G0 E%.3f F%d\n", E_PumpBack, F_PumpBack);
					std::fprintf(fp, "G92 E0\n");
					// return to the safe point Z_max + Z_high
					std::fprintf(fp, "G0 Z%.3f F%d\n", (Z_max + Z_high), F_G0_Z);
					// move to start of printing location
					std::fprintf(fp, "G0 X%.3f Y%.3f B%.3f C%.3f F%d\n", X, Y, B, C, F_G0_XYBC);
					// slowly lower for printing
					std::fprintf(fp, "G0 Z%.3f F%d\n", (Z + Z_compensateUpDistance), F_G0_Z);

					if (WayPointPatch->isSupportLayer == true) {
						std::fprintf(fp, "G1 E%.3f F%d\n", E_PumpCompensate * 1.2, F_PumpCompensate);
						std::fprintf(fp, "G92 E0\n");
						std::fprintf(fp, "G1 F%d\n", F_G1_support);
					}
					else {
						std::fprintf(fp, "G1 E%.3f F%d\n", E_PumpCompensate, F_PumpCompensate);
						std::fprintf(fp, "G92 E0\n");
						std::fprintf(fp, "G1 F%d\n", F_G1_original);
					}

				}
				// case: exchange extruder
				else {
					std::fprintf(fp, "G92 E0\n");
					std::fprintf(fp, "G0 E%.3f F%d\n", E_PumpBack, F_PumpBack);
					// return to the home point Z_home
					std::fprintf(fp, "G0 Z%.3f F%d\n", Z_home, F_G0_Z);

					// change extruder
					if (WayPointPatch->isSupportLayer == true){
						std::fprintf(fp, "M104 S%d T%d\n", sTemperature, sExtruder);
					}
					else { 
						std::fprintf(fp, "M104 S%d T%d\n", iTemperature, iExtruder); 
					}
					std::fprintf(fp, "G92 E0\n");

					// move to start of printing location
					std::fprintf(fp, "G0 X%.3f Y%.3f B%.3f C%.3f F%d\n", X, Y, B, C, F_G0_XYBC);
					std::fprintf(fp, "G0 Z%.3f F%d\n", (Z + Z_compensateUpDistance), F_G0_Z);

					if (WayPointPatch->isSupportLayer == true) {
						// slowly lower for printing
						std::fprintf(fp, "G1 E%.3f F%d\n", E_PumpCompensateNewE * 1.2, F_PumpCompensate);
						std::fprintf(fp, "G92 E0\n");
						std::fprintf(fp, "G1 F%d\n", F_G1_support);
					}
					else {
						// slowly lower for printing
						std::fprintf(fp, "G1 E%.3f F%d\n", E_PumpCompensateNewE, F_PumpCompensate);
						std::fprintf(fp, "G92 E0\n");
						std::fprintf(fp, "G1 F%d\n", F_G1_original);
					}
				}
				std::fprintf(fp, "G1 X%.3f Y%.3f Z%.3f B%.3f C%.3f E%.3f\n", X, Y, Z, B, C, E);
			}
			else {
				// Consider the waypoints with too large Length //OR large Singularity areas
				if (Node->Jump_nextSecStart
					//|| Node->isSingularSecEndNode 	
					//|| Node->isCollisionStart 	|| Node->isCollisionEnd 
					//|| Node->isNegZStart || Node->isNegZEnd
					) {

					if (WayPointPatch->isSupportLayer == true) {

						std::fprintf(fp, "G0 E%.3f F%d\n", (E + E_PumpBack * 1.30), F_PumpBack);
						std::fprintf(fp, "G0 Z%.3f F%d\n", (Z_max + Z_high), F_G0_Z);
						std::fprintf(fp, "G0 X%.3f Y%.3f B%.3f C%.3f F%d\n", X, Y, B, C, F_G0_XYBC);
						std::fprintf(fp, "G0 Z%.3f F%d\n", (Z + Z_compensateUpDistance), F_G0_Z);
						std::fprintf(fp, "G0 E%.3f F%d\n", (E - 0.1), F_PumpCompensate);
						std::fprintf(fp, "G0 Z%.3f E%.3f F%d\n", Z, E, F_G1_support);
					}
					else {

						std::fprintf(fp, "G0 E%.3f F%d\n", (E + E_PumpBack * 0.95), F_PumpBack);
						std::fprintf(fp, "G0 Z%.3f F%d\n", (Z_max + Z_high), F_G0_Z);
						std::fprintf(fp, "G0 X%.3f Y%.3f B%.3f C%.3f F%d\n", X, Y, B, C, F_G0_XYBC);
						std::fprintf(fp, "G0 Z%.3f F%d\n", (Z + Z_compensateUpDistance), F_G0_Z);
						std::fprintf(fp, "G0 E%.3f F%d\n", (E - 0.1), F_PumpCompensate);
						std::fprintf(fp, "G0 Z%.3f E%.3f F%d\n", Z, E, F_G1_original);
					}
				}
				std::fprintf(fp, "G1 X%.3f Y%.3f Z%.3f B%.3f C%.3f E%.3f\n", X, Y, Z, B, C, E);
			}
		}
		IsSupportLayer_last = WayPointPatch->isSupportLayer;
	}

	std::fprintf(fp, "G92 E0\n");
	std::fprintf(fp, "G0 E%.3f F%d\n", E_PumpBack, F_G0_XYBC); // PumpBack
	std::fprintf(fp, "G0 Z%.3f F%d\n", Z_home, F_G0_Z); // return to the home point Z_home
	std::fprintf(fp, "G0 X0 Y0 B0 C0 F%d\n", F_G0_XYBC);
	std::fprintf(fp, "M30\n");// Stop all of the motion

	std::fclose(fp);

	std::cout << "------------------------------------------- " << GcodeDir << " Gcode Write Finish!\n" << std::endl;
}