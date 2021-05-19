#include "stdafx.h"

#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QFileDialog>
#include <QtDebug>
#include <QDesktopWidget>
#include <QCoreApplication>
#include <QMimeData>
#include <QTreeView>
#include <QThread>
#include <QTimer>
#include <QDateTime>
#include <QMessageBox>
#include <QScreen>
#include <QStyleFactory>
#include <fstream>

#include "../GLKLib/GLKCameraTool.h"
#include "../GLKLib/InteractiveTool.h"
#include "../GLKLib/GLKMatrixLib.h"
#include "../GLKLib/GLKGeometry.h"
#include "../QMeshLib/QMeshPatch.h"
#include "../QMeshLib/QMeshTetra.h"
#include "../QMeshLib/QMeshFace.h"
#include "../QMeshLib/QMeshEdge.h"
#include "../QMeshLib/QMeshNode.h"

#include "alphanum.hpp"
#include <dirent.h>
#include <Graph.h>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

	QApplication::setStyle(QStyleFactory::create("Fusion"));

    signalMapper = new QSignalMapper(this);
    addToolBar(ui->toolBar);
    addToolBar(ui->navigationToolBar);
    addToolBar(ui->selectionToolBar);

    createTreeView();
    createActions();

    pGLK = new GLKLib();
    ui->horizontalLayout->addWidget(pGLK);
    ui->horizontalLayout->setMargin(0);
    pGLK->setFocus();

    pGLK->clear_tools();
    pGLK->set_tool(new GLKCameraTool(pGLK,ORBITPAN));
	
	//connect timer with timer function
	connect(&Gcode_timer, SIGNAL(timeout()), this, SLOT(doTimerGcodeMoving()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::createActions()
{
    // file IO
    connect(ui->actionOpen, SIGNAL(triggered(bool)), this, SLOT(open()));
    connect(ui->actionSave, SIGNAL(triggered(bool)), this, SLOT(save()));
	connect(ui->actionSaveSelection, SIGNAL(triggered(bool)), this, SLOT(saveSelection()));
	connect(ui->actionReadSelection, SIGNAL(triggered(bool)), this, SLOT(readSelection()));

    // navigation
    connect(ui->actionFront, SIGNAL(triggered(bool)), signalMapper, SLOT(map()));
    connect(ui->actionBack, SIGNAL(triggered(bool)), signalMapper, SLOT(map()));
    connect(ui->actionTop, SIGNAL(triggered(bool)), signalMapper, SLOT(map()));
    connect(ui->actionBottom, SIGNAL(triggered(bool)), signalMapper, SLOT(map()));
    connect(ui->actionLeft, SIGNAL(triggered(bool)), signalMapper, SLOT(map()));
    connect(ui->actionRight, SIGNAL(triggered(bool)), signalMapper, SLOT(map()));
    connect(ui->actionIsometric, SIGNAL(triggered(bool)), signalMapper, SLOT(map()));
    connect(ui->actionZoom_In, SIGNAL(triggered(bool)), signalMapper, SLOT(map()));
    connect(ui->actionZoom_Out, SIGNAL(triggered(bool)), signalMapper, SLOT(map()));
    connect(ui->actionZoom_All, SIGNAL(triggered(bool)), signalMapper, SLOT(map()));
    connect(ui->actionZoom_Window, SIGNAL(triggered(bool)), signalMapper, SLOT(map()));
    signalMapper->setMapping (ui->actionFront, 0);
    signalMapper->setMapping (ui->actionBack, 1);
    signalMapper->setMapping (ui->actionTop, 2);
    signalMapper->setMapping (ui->actionBottom, 3);
    signalMapper->setMapping (ui->actionLeft, 4);
    signalMapper->setMapping (ui->actionRight, 5);
    signalMapper->setMapping (ui->actionIsometric, 6);
    signalMapper->setMapping (ui->actionZoom_In, 7);
    signalMapper->setMapping (ui->actionZoom_Out, 8);
    signalMapper->setMapping (ui->actionZoom_All, 9);
    signalMapper->setMapping (ui->actionZoom_Window, 10);

    // view
    connect(ui->actionShade, SIGNAL(triggered(bool)), signalMapper, SLOT(map()));
    connect(ui->actionMesh, SIGNAL(triggered(bool)), signalMapper, SLOT(map()));
    connect(ui->actionNode, SIGNAL(triggered(bool)), signalMapper, SLOT(map()));
    connect(ui->actionProfile, SIGNAL(triggered(bool)), signalMapper, SLOT(map()));
    connect(ui->actionFaceNormal, SIGNAL(triggered(bool)), signalMapper, SLOT(map()));
    connect(ui->actionNodeNormal, SIGNAL(triggered(bool)), signalMapper, SLOT(map()));
    signalMapper->setMapping (ui->actionShade, 20);
    signalMapper->setMapping (ui->actionMesh, 21);
    signalMapper->setMapping (ui->actionNode, 22);
    signalMapper->setMapping (ui->actionProfile, 23);
    signalMapper->setMapping (ui->actionFaceNormal, 24);
    signalMapper->setMapping (ui->actionNodeNormal, 25);
    ui->actionShade->setChecked(true);

    connect(ui->actionShifttoOrigin, SIGNAL(triggered(bool)), this, SLOT(shiftToOrigin()));

    // select
    connect(ui->actionSelectNode, SIGNAL(triggered(bool)), signalMapper, SLOT(map()));
    connect(ui->actionSelectEdge, SIGNAL(triggered(bool)), signalMapper, SLOT(map()));
    connect(ui->actionSelectFace, SIGNAL(triggered(bool)), signalMapper, SLOT(map()));
	connect(ui->actionSelectFix, SIGNAL(triggered(bool)), signalMapper, SLOT(map()));
	connect(ui->actionSelectHandle, SIGNAL(triggered(bool)), signalMapper, SLOT(map()));

	signalMapper->setMapping (ui->actionSelectNode, 30);
    signalMapper->setMapping (ui->actionSelectEdge, 31);
    signalMapper->setMapping (ui->actionSelectFace, 32);
	signalMapper->setMapping(ui->actionSelectFix, 33);
	signalMapper->setMapping(ui->actionSelectHandle, 34);


    connect (signalMapper, SIGNAL(mapped(int)), this, SLOT(signalNavigation(int)));

	connect(ui->pushButton_readGcodeSourceData, SIGNAL(released()), this, SLOT(readGcodeSourceData()));
	connect(ui->pushButton_calDWH, SIGNAL(released()), this, SLOT(runDHWcalculation()));
	connect(ui->pushButton_calSingularOpt, SIGNAL(released()), this, SLOT(runSingularityOpt()));
	connect(ui->pushButton_calCollision, SIGNAL(released()), this, SLOT(runCollisionCheck()));
	connect(ui->pushButton_calCollisionElimination, SIGNAL(released()), this, SLOT(runCollisionElimination()));
	connect(ui->pushButton_Gcode_writting, SIGNAL(released()), this, SLOT(runWriteGcode()));
	connect(ui->pushButton_GcodeSimulation, SIGNAL(released()), this, SLOT(runGcodeSimulation()));

	connect(ui->pushButton_calCollision_continuous, SIGNAL(released()), this, SLOT(runContinuousCollisionChecking()));

	//Button
	connect(ui->pushButton_ShowAllLayers, SIGNAL(released()), this, SLOT(viewAllWaypointLayers()));
	connect(ui->spinBox_ShowLayerIndex, SIGNAL(valueChanged(int)), this, SLOT(changeWaypointDisplay()));
	connect(ui->checkBox_showSingularityNode, SIGNAL(released()), this, SLOT(special_Draw_Command()));
	connect(ui->checkBox_showSolveSelection, SIGNAL(released()), this, SLOT(special_Draw_Command()));

	connect(ui->pushButton_test, SIGNAL(released()), this, SLOT(test_button()));
	

}

void MainWindow::open()
{
    QString filenameStr = QFileDialog::getOpenFileName(this, tr("Open File,"), "..", tr(""));
    QFileInfo fileInfo(filenameStr);
    QString fileSuffix = fileInfo.suffix();
    QByteArray filenameArray = filenameStr.toLatin1();
    char *filename = filenameArray.data();

    // set polygen name
    std::string strFilename(filename);
    std::size_t foundStart = strFilename.find_last_of("/");
    std::size_t foundEnd = strFilename.find_last_of(".");
    std::string modelName;
    modelName = strFilename.substr(0,foundEnd);
    modelName = modelName.substr(foundStart+1);
    
    if (QString::compare(fileSuffix,"obj") == 0){
        PolygenMesh *polygenMesh = new PolygenMesh(UNDEFINED);
        polygenMesh->ImportOBJFile(filename,modelName);
        polygenMesh->BuildGLList(polygenMesh->m_bVertexNormalShading);
        pGLK->AddDisplayObj(polygenMesh,true);
        polygenMeshList.AddTail(polygenMesh);
    }

	else if (QString::compare(fileSuffix, "tet") == 0) {
		PolygenMesh *polygenMesh = new PolygenMesh(TET);
		std::cout << filename << std::endl;
		std::cout << modelName << std::endl;
		polygenMesh->ImportTETFile(filename, modelName);
		polygenMesh->BuildGLList(polygenMesh->m_bVertexNormalShading);
		pGLK->AddDisplayObj(polygenMesh, true);
		polygenMeshList.AddTail(polygenMesh);
	}

    updateTree();

    shiftToOrigin();
    pGLK->refresh(true);
}

void MainWindow::save()
{
	PolygenMesh *polygenMesh = getSelectedPolygenMesh();
	if (!polygenMesh)
		polygenMesh = (PolygenMesh*)polygenMeshList.GetHead();
	if (!polygenMesh)
		return;
	QString filenameStr = QFileDialog::getSaveFileName(this, tr("OBJ File Export,"), "..", tr("OBJ(*.obj)"));
	QFileInfo fileInfo(filenameStr);
	QString fileSuffix = fileInfo.suffix();

	if (QString::compare(fileSuffix, "obj") == 0) {
		QFile exportFile(filenameStr);
		if (exportFile.open(QFile::WriteOnly | QFile::Truncate)) {
			QTextStream out(&exportFile);
			for (GLKPOSITION posMesh = polygenMesh->GetMeshList().GetHeadPosition(); posMesh != nullptr;) {
				QMeshPatch *patch = (QMeshPatch*)polygenMesh->GetMeshList().GetNext(posMesh);
				for (GLKPOSITION posNode = patch->GetNodeList().GetHeadPosition(); posNode != nullptr;) {
					QMeshNode *node = (QMeshNode*)patch->GetNodeList().GetNext(posNode);
					double xx, yy, zz;
					node->GetCoord3D(xx, yy, zz);
					float r, g, b;
					node->GetColor(r, g, b);
					out << "v " << xx << " " << yy << " " << zz << " " << node->value1 << endl;
				}
				for (GLKPOSITION posFace = patch->GetFaceList().GetHeadPosition(); posFace != nullptr;) {
					QMeshFace *face = (QMeshFace*)patch->GetFaceList().GetNext(posFace);
					out << "f " << face->GetNodeRecordPtr(0)->GetIndexNo() << " " << face->GetNodeRecordPtr(1)->GetIndexNo() << " " << face->GetNodeRecordPtr(2)->GetIndexNo() << endl;
				}
			}
		}
		exportFile.close();
	}
}

void MainWindow::saveSelection()
{
	//printf("%s exported\n", Model->ModelName);

	PolygenMesh *polygenMesh = getSelectedPolygenMesh();
	if (!polygenMesh)
		polygenMesh = (PolygenMesh*)polygenMeshList.GetHead();
	QMeshPatch *patch = (QMeshPatch*)polygenMesh->GetMeshList().GetHead();

	std::string filename = polygenMesh->getModelName();
	const char * c = filename.c_str();
	char *cstr = new char[filename.length() + 1];
	strcpy(cstr, filename.c_str());

	const char * split = ".";
	char* p = strtok(cstr, split);

	char output_filename[256];
	strcpy(output_filename, "..\\selection_file\\");
	strcat(output_filename, cstr);
	char filetype[64];
	strcpy(filetype, ".txt");
	strcat(output_filename, filetype);

	ofstream nodeSelection(output_filename);
	if (!nodeSelection)
		cerr << "Sorry!We were unable to build the file NodeSelect!\n";
	for (GLKPOSITION Pos = patch->GetNodeList().GetHeadPosition(); Pos;) {
		QMeshNode *CheckNode = (QMeshNode*)patch->GetNodeList().GetNext(Pos);
		nodeSelection << CheckNode->GetIndexNo() << ":";
		//for the selection of fixing part
		if (CheckNode->isFixed == true) nodeSelection << "1:";
		else nodeSelection << "0:";
		//for the selection of hard part
		if (CheckNode->isHandle == true) nodeSelection << "1:" << endl;
		else nodeSelection << "0:" << endl;
	}

	nodeSelection.close();
	printf("Finish output selection \n");
}

void MainWindow::readSelection()
{
	PolygenMesh *polygenMesh = getSelectedPolygenMesh();
	if (!polygenMesh)
		polygenMesh = (PolygenMesh*)polygenMeshList.GetHead();
	QMeshPatch *patch = (QMeshPatch*)polygenMesh->GetMeshList().GetHead();

	std::string filename = polygenMesh->getModelName();
	const char * c = filename.c_str();

	char *cstr = new char[filename.length() + 1];
	strcpy(cstr, filename.c_str());

	const char * split = ".";
	char* p = strtok(cstr, split);

	char input_filename[256];
	strcpy(input_filename, "..\\selection_file\\");
	strcat(input_filename, cstr);
	char filetype[64];
	strcpy(filetype, ".txt");
	strcat(input_filename, filetype);

	ifstream nodeSelect(input_filename);
	if (!nodeSelect)
		cerr << "Sorry!We were unable to open the file!\n";
	vector<int> NodeIndex(patch->GetNodeNumber()), checkNodeFixed(patch->GetNodeNumber()), checkNodeHandle(patch->GetNodeNumber());
	//string line;
	int LineIndex1 = 0;
	string sss;
	while (getline(nodeSelect, sss)){
		const char * c = sss.c_str();
		sscanf(c, "%d:%d:%d", &NodeIndex[LineIndex1], &checkNodeFixed[LineIndex1], &checkNodeHandle[LineIndex1]);
		LineIndex1++;
	}

	nodeSelect.close();
	for (GLKPOSITION Pos = patch->GetNodeList().GetHeadPosition(); Pos;) {
		QMeshNode *CheckNode = (QMeshNode*)patch->GetNodeList().GetNext(Pos);
		if (checkNodeFixed[CheckNode->GetIndexNo() - 1] == 1) CheckNode->isFixed = true;
		if (checkNodeHandle[CheckNode->GetIndexNo() - 1] == 1) CheckNode->isHandle = true;
	}

	for (GLKPOSITION Pos = patch->GetFaceList().GetHeadPosition(); Pos != NULL;)
	{
		QMeshFace* Face = (QMeshFace*)patch->GetFaceList().GetNext(Pos);
		if (Face->GetNodeRecordPtr(0)->isHandle == true &&
			Face->GetNodeRecordPtr(1)->isHandle == true &&
			Face->GetNodeRecordPtr(2)->isHandle == true)
			Face->isHandleDraw = true;
		else Face->isHandleDraw = false;

		if (Face->GetNodeRecordPtr(0)->isFixed == true &&
			Face->GetNodeRecordPtr(1)->isFixed == true &&
			Face->GetNodeRecordPtr(2)->isFixed == true)
			Face->isFixedDraw = true;
		else Face->isFixedDraw = false;
	}
	printf("Finish input selection \n");
	pGLK->refresh(true);

}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
	//QMouseEvent *e = (QMouseEvent*)event;
	//QPoint pos = e->pos();
	//cout << "Mouse position updated" << endl;
	//double wx, wy, wz;
	//pGLK->screen_to_wcl(100.0, 100.0, wx, wy, wz);
	//ui->CorrdinateMouse->setText(QString("X = %1").arg(wx));

	//QString text;
	//text = QString("%1 X %2").arg(event->pos().x()).arg(event->pos().y());
	///** Update the info text */
	//ui->statusBar->showMessage(text);
}

void MainWindow::signalNavigation(int flag)
{
    if (flag <= 10)
        pGLK->setNavigation(flag);
    if (flag >=20 && flag <=25){
        pGLK->setViewModel(flag-20);
        switch (flag) {
        case 20:
            ui->actionShade->setChecked(pGLK->getViewModel(0));
            break;
        case 21:
            ui->actionMesh->setChecked(pGLK->getViewModel(1));
            break;
        case 22:
            ui->actionNode->setChecked(pGLK->getViewModel(2));
            break;
        case 23:
            ui->actionProfile->setChecked(pGLK->getViewModel(3));
            break;
        case 24:
            ui->actionFaceNormal->setChecked(pGLK->getViewModel(4));
            break;
        case 25:
            ui->actionNodeNormal->setChecked(pGLK->getViewModel(5));
            break;
        }
    }
  //  if (flag==30 || flag==31 || flag==32 || flag == 33 || flag == 34){
  //      InteractiveTool *tool;
  //      switch (flag) {
  //      case 30:
  //          tool = new InteractiveTool(pGLK, &polygenMeshList, (GLKMouseTool*)pGLK->GetCurrentTool(), NODE, ui->boxDeselect->isChecked());
  //          break;
  //      case 31:
  //          tool = new InteractiveTool(pGLK, &polygenMeshList, (GLKMouseTool*)pGLK->GetCurrentTool(), EDGE, ui->boxDeselect->isChecked());
  //          break;
  //      case 32:
  //          tool = new InteractiveTool(pGLK, &polygenMeshList, (GLKMouseTool*)pGLK->GetCurrentTool(), FACE, ui->boxDeselect->isChecked());
  //          break;
		//case 33:
		//	tool = new InteractiveTool(pGLK, &polygenMeshList, (GLKMouseTool*)pGLK->GetCurrentTool(), FIX, ui->boxDeselect->isChecked());
		//	break;
		//case 34:
		//	tool = new InteractiveTool(pGLK, &polygenMeshList, (GLKMouseTool*)pGLK->GetCurrentTool(), NHANDLE, ui->boxDeselect->isChecked());
		//	break;
  //      }
  //      pGLK->set_tool(tool);
  //  }
}

void MainWindow::shiftToOrigin()
{
    
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls())
        event->acceptProposedAction();
}

void MainWindow::dropEvent(QDropEvent *event)
{
    QString filenameStr;
    foreach (const QUrl &url, event->mimeData()->urls())
        filenameStr = url.toLocalFile();
    QByteArray filenameArray = filenameStr.toLatin1();
    char *filename = filenameArray.data();

    PolygenMesh *polygenMesh = new PolygenMesh(UNDEFINED);

    // set polygen name
    std::string strFilename(filename);
    std::size_t foundStart = strFilename.find_last_of("/");
    std::size_t foundEnd = strFilename.find_last_of(".");
    std::string modelName;
    modelName = strFilename.substr(0,foundEnd);
    modelName = modelName.substr(foundStart+1);
    int i = 0;
    for (GLKPOSITION pos=polygenMeshList.GetHeadPosition(); pos!=nullptr;){
        PolygenMesh *polygen = (PolygenMesh*)polygenMeshList.GetNext(pos);
        std::string name = (polygen->getModelName()).substr(0,(polygen->getModelName()).find(' '));
        if (name == modelName)
            i++;
    }
    if (i > 0)
        modelName += " "+std::to_string(i);

	QFileInfo fileInfo(filenameStr);
	QString fileSuffix = fileInfo.suffix();
	if (QString::compare(fileSuffix, "obj") == 0) {
		polygenMesh->ImportOBJFile(filename, modelName);
	}
	else if (QString::compare(fileSuffix, "tet") == 0) {
		polygenMesh->ImportTETFile(filename, modelName);
        polygenMesh->meshType = TET;
	}
	polygenMesh->m_bVertexNormalShading = false;	
    polygenMesh->BuildGLList(polygenMesh->m_bVertexNormalShading);
    pGLK->AddDisplayObj(polygenMesh,true);
    polygenMeshList.AddTail(polygenMesh);
    
    updateTree();
}

void MainWindow::createTreeView()
{
    treeModel = new QStandardItemModel();
    ui->treeView->setModel(treeModel);
    ui->treeView->setHeaderHidden(true);
    ui->treeView->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->treeView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->treeView->expandAll();
}

void MainWindow::updateTree()
{
    treeModel->clear();
    for (GLKPOSITION pos=polygenMeshList.GetHeadPosition(); pos!=nullptr;){
        PolygenMesh *polygenMesh = (PolygenMesh*)polygenMeshList.GetNext(pos);
        QString modelName = QString::fromStdString(polygenMesh->getModelName());
        QStandardItem *modelListItem = new QStandardItem(modelName);
        modelListItem->setCheckable(true);
        modelListItem->setCheckState(Qt::Checked);
        treeModel->appendRow(modelListItem);
    }
	pGLK->refresh(true);
}

PolygenMesh *MainWindow::getSelectedPolygenMesh()
{
    if (!treeModel->hasChildren())
        return nullptr;
    QModelIndex index = ui->treeView->currentIndex();
    QString selectedModelName = index.data(Qt::DisplayRole).toString();
    for (GLKPOSITION pos=polygenMeshList.GetHeadPosition(); pos!=nullptr;){
        PolygenMesh *polygenMesh = (PolygenMesh*)polygenMeshList.GetNext(pos);
        QString modelName = QString::fromStdString(polygenMesh->getModelName());
        if (QString::compare(selectedModelName,modelName) == 0)
            return polygenMesh;
    }
    return nullptr;
}

void MainWindow::on_pushButton_clearAll_clicked()
{
    int i = 0;
    for (GLKPOSITION pos=polygenMeshList.GetHeadPosition(); pos!=nullptr; i++){
        PolygenMesh *polygenMesh = (PolygenMesh*)polygenMeshList.GetNext(pos);
        QMeshPatch *patch = (QMeshPatch*)polygenMesh->GetMeshList().GetHead();
        if (i<2)
            continue;
        for (GLKPOSITION pos2=patch->GetFaceList().GetHeadPosition(); pos2!=nullptr;){
            QMeshFace *face = (QMeshFace*)patch->GetFaceList().GetNext(pos2);
            face->m_nIdentifiedPatchIndex = 0;
        }
    }
    pGLK->refresh(true);
}

void MainWindow::on_treeView_clicked(const QModelIndex &index)
{
    ui->treeView->currentIndex();
    QStandardItem *modelListItem = treeModel->itemFromIndex(index);
    ui->treeView->setCurrentIndex(index);
    PolygenMesh *polygenMesh = getSelectedPolygenMesh();
    if (modelListItem->checkState() == Qt::Checked)
        polygenMesh->bShow = true;
    else
        polygenMesh->bShow = false;
    pGLK->refresh(true);
}

/***************************************************************/
/*The Main Function of Singular-Aware Motion Planning Algorithm*/
/***************************************************************/

void MainWindow::readGcodeSourceData() {
    std::cout << "Read Gcode Source Data__ Running ..." << std::endl;

	//test C:\PHD\Code\CASE2021_code\DataSet\Sorce\topology

    string PosNorFileDir = "../DataSet/Sorce/" + (ui->lineEdit_SorceDataDir->text()).toStdString() + "/waypoint";
    string LayerFileDir = "../DataSet/Sorce/" + (ui->lineEdit_SorceDataDir->text()).toStdString() + "/layer";;

    Xmove = ui->doubleSpinBox_Xmove->value();    Ymove = ui->doubleSpinBox_Ymove->value();    Zmove = ui->doubleSpinBox_Zmove->value();

    _natSort(PosNorFileDir, wayPointFileCell);
    ui->spinBox_ShowLayerIndex->setMaximum(wayPointFileCell.size() - 1);
    ui->spinBox_StartLayerIndex->setMaximum(wayPointFileCell.size() - 1);
    ui->label_WaypointDisplay->setText(QString("Waypoint Layer Range: [0,%1]").arg(wayPointFileCell.size() - 1));
    ui->spinBox_GcodeGeneFromIndex->setMaximum(wayPointFileCell.size() - 1);
    ui->spinBox_GcodeGeneToIndex->setMaximum(wayPointFileCell.size() - 1);
    ui->spinBox_GcodeGeneToIndex->setValue(wayPointFileCell.size() - 1);
    readWayPointData(PosNorFileDir); // have getten the right waypoints with right Zup and move distace


    _natSort(LayerFileDir, sliceSetFileCell);
    if (wayPointFileCell.size() != sliceSetFileCell.size()) {
        std::cout << "Error: Layers file num != Waypoints file num" << std::endl;
        return;
    }
    else { readSliceData(LayerFileDir); }

    readExtruderHeadfile("extruderHead.obj");
    readPlatformfile("printingPlatform.obj");

    viewAllWaypointLayers();

    std::cout << "Read Gcode Source Data__ END.\n" << std::endl;

    ui->pushButton_calDWH->setEnabled(true);

}

void MainWindow::runDHWcalculation() {

	std::cout << "New GcodeGeneration class building ... \n" << std::endl;
	GcodeGene = new GcodeGeneration();

	PolygenMesh* Slices; PolygenMesh* Waypoints; PolygenMesh* ExtruderHead; PolygenMesh* PrintPlatform;
	Slices = _detectPolygenMesh(LAYER, "Slices", false);
	Waypoints = _detectPolygenMesh(WAYPOINT, "Waypoints", false);
	ExtruderHead = _detectPolygenMesh(CNC_PARTS, "ExtruderHead", false);
	PrintPlatform = _detectPolygenMesh(CNC_PARTS, "PrintPlatform", false);
	if (Slices == NULL || Waypoints == NULL || ExtruderHead == NULL || PrintPlatform == NULL) {
		std::cerr << "There is no PolygenMesh!" << std::endl; return;
	}

	ui->checkBox_varyDistance->setCheckState(Qt::Checked); // it is necessary for all calculation, so defalt

	GcodeGene->initial(
		ui->spinBox_GcodeGeneFromIndex->value(), ui->spinBox_GcodeGeneToIndex->value(),
		Slices, Waypoints, ExtruderHead, PrintPlatform,
		ui->doubleSpinBox_E3XOFF->value(), ui->doubleSpinBox_E3YOFF->value(),
		ui->doubleSpinBox_lambda->value(),
		ui->checkBox_varyDistance->isChecked(), ui->checkBox_varyHeight->isChecked(), ui->checkBox_varyWidth->isChecked()
	);

	std::cout << "Finish building and initialization.\n" << std::endl;

	if (ui->checkBox_varyDistance->isChecked() && operationTime == 0) {
		GcodeGene->calDistance();
		GcodeGene->initialSmooth();
		operationTime++;
	}

	if (ui->checkBox_varyHeight->isChecked())		GcodeGene->calHeight();

	if (ui->checkBox_varyWidth->isChecked()) 		GcodeGene->calWidth();

	if (ui->checkBox_TestDHW_Switch->isChecked())	GcodeGene->test_DHW();

	viewAllWaypointLayers();
}

void MainWindow::runSingularityOpt() {

	GcodeGene->singularityOpt();
	GcodeGene->verifyPosNor(true);

	viewAllWaypointLayers();
}

void MainWindow::runCollisionCheck() {

	GcodeGene->detectCollision_1();

	viewAllWaypointLayers();
}

void MainWindow::runCollisionElimination() {

	GcodeGene->graph_Search_Shortest_Path();
	GcodeGene->testXYZBCE(true);
	viewAllWaypointLayers();
}

void MainWindow::runWriteGcode() {

	string targetFileName = (ui->lineEdit_SorceDataDir->text()).toStdString() + "_Gcode.txt";
	GcodeGene->writeGcode(targetFileName);
}

void MainWindow::runGcodeSimulation() {

	string FileName = (ui->lineEdit_SorceDataDir->text()).toStdString() + "_Gcode.txt";
	simuLayerInd = ui->spinBox_GcodeGeneFromIndex->value() - 1;//  will meet "G1 F1500" at the first several lines, so -1

	readGcodeFile(Gcode_Table, FileName);
	ui->progressBar_GcodeSimulation->setRange(0, Gcode_Table.rows() - 1);

	std::vector<std::string> CNCfileSet = { "B_axis" ,"Z_axis" ,"Y_axis","X_axis", "base" };
	readCNCfile(CNCfileSet);
	viewAllWaypointLayers();
	// close the display of Slice for speed up the simulation
	PolygenMesh* Slices;
	Slices = _detectPolygenMesh(LAYER, "Slices", false);
	if (Slices != NULL) {
		for (GLKPOSITION Pos = Slices->GetMeshList().GetHeadPosition(); Pos;) {
			QMeshPatch* patch = (QMeshPatch*)Slices->GetMeshList().GetNext(Pos);
			patch->drawThisPatch = false;
		}
	}
	
	gcodetimerItertime = 0;
	Gcode_timer.start(20);

	std::cout << "------------------------------------------- G code Simulation Running ..." << std::endl;

}

void MainWindow::runContinuousCollisionChecking() {

	GcodeGene->detectCollision_continuous();
	viewAllWaypointLayers();
}

/***************************************************************/
/*The atom Function of Singular-Aware Motion Planning Algorithm*/
/***************************************************************/

void MainWindow::_natSort(string dirctory, vector<string>& fileNameCell) {

    if (fileNameCell.empty() == false) return;

    DIR* dp;
    struct dirent* ep;
    string fullDir = dirctory;
    //cout << fullDir << endl;
    dp = opendir(fullDir.c_str());
    //dp = opendir("../Waypoints");

    if (dp != NULL) {
        while (ep = readdir(dp)) {
            //cout << ep->d_name << endl;
            if ((string(ep->d_name) != ".") && (string(ep->d_name) != "..")) {
                //cout << ep->d_name << endl;
                fileNameCell.push_back(string(ep->d_name));
            }
        }
        (void)closedir(dp);
    }
    else {
        perror("Couldn't open the directory");
    }
    //resort the files with nature order
    sort(fileNameCell.begin(), fileNameCell.end(), doj::alphanum_less<std::string>());

}

void MainWindow::_modifyCoord(QMeshPatch* patchFile) {

	bool upY2upZ = ui->checkBox_Yup2Zup->isChecked();

	for (GLKPOSITION Pos = patchFile->GetNodeList().GetHeadPosition(); Pos;) {
		QMeshNode* Node = (QMeshNode*)patchFile->GetNodeList().GetNext(Pos);

		double xx, yy, zz, nx, ny, nz;
		Node->GetCoord3D(xx, yy, zz);
		Node->GetNormal(nx, ny, nz);

		if (upY2upZ) {

			double tempPosYZ = yy;
			double tempNorYZ = ny;

			yy = -zz;
			zz = tempPosYZ;

			ny = -nz;
			nz = tempNorYZ;
		}

		Node->SetCoord3D_last(xx, yy, zz);// base data of MoveModel
		Node->SetNormal_last(nx, ny, nz);
	}
}

PolygenMesh* MainWindow::_buildPolygenMesh(mesh_type type, std::string name) {

	PolygenMesh* newMesh = new PolygenMesh(type);
	newMesh->setModelName(name);
	polygenMeshList.AddTail(newMesh);
	pGLK->AddDisplayObj(newMesh, true);
	newMesh->BuildGLList(newMesh->m_bVertexNormalShading);
	updateTree();
	return newMesh;

}

//detectType:true -> typeSelect; false -> nameSelect; 
PolygenMesh* MainWindow::_detectPolygenMesh(mesh_type type, std::string name, bool detectType) {

	PolygenMesh* detectedMesh = NULL;
	for (GLKPOSITION pos = polygenMeshList.GetHeadPosition(); pos != nullptr;) {
		PolygenMesh* thispolygenMesh = (PolygenMesh*)polygenMeshList.GetNext(pos);
		if (detectType == true) {
			if (thispolygenMesh->meshType == type) {
				detectedMesh = thispolygenMesh; break;
			}
		}
		else {
			if (thispolygenMesh->getModelName() == name) {
				detectedMesh = thispolygenMesh; break;
			}
		}
	}
	return detectedMesh;

}

void MainWindow::viewAllWaypointLayers() {
	for (GLKPOSITION pos = polygenMeshList.GetHeadPosition(); pos != nullptr;) {
		PolygenMesh* polygenMesh = (PolygenMesh*)polygenMeshList.GetNext(pos);
		if ("Waypoints" != polygenMesh->getModelName() && "Slices" != polygenMesh->getModelName()) continue;

		for (GLKPOSITION posMesh = polygenMesh->GetMeshList().GetHeadPosition(); posMesh != nullptr;) {
			QMeshPatch* Patch = (QMeshPatch*)polygenMesh->GetMeshList().GetNext(posMesh);
			Patch->drawThisPatch = true;
		}
	}
	pGLK->refresh(true);
	pGLK->Zoom_All_in_View();
}

void MainWindow::changeWaypointDisplay() {

	bool single = ui->checkBox_EachLayerSwitch->isChecked();
	int startLayerIndex = ui->spinBox_StartLayerIndex->value();
	ui->spinBox_ShowLayerIndex->setMinimum(startLayerIndex);
	int currentLayerIndex = ui->spinBox_ShowLayerIndex->value();

	for (GLKPOSITION pos = polygenMeshList.GetHeadPosition(); pos != nullptr;) {
		PolygenMesh* polygenMesh = (PolygenMesh*)polygenMeshList.GetNext(pos);
		if ("Waypoints" != polygenMesh->getModelName()
			&& "Slices" != polygenMesh->getModelName())
			continue;

		int index = 0;

		for (GLKPOSITION posMesh = polygenMesh->GetMeshList().GetHeadPosition(); posMesh != nullptr;) {
			QMeshPatch* Patch = (QMeshPatch*)polygenMesh->GetMeshList().GetNext(posMesh);

			Patch->drawThisPatch = false;

			if (index >= startLayerIndex) {
				if (single == true) {
					if (index == currentLayerIndex)
						Patch->drawThisPatch = true;
				}
				else {
					if (index <= currentLayerIndex)
						Patch->drawThisPatch = true;
				}

				if (index == currentLayerIndex) {
					if ("Waypoints" == polygenMesh->getModelName())
						ui->label_currentFile->setText(QString::fromStdString(("File: " + Patch->patchName)));
				}
			}
			index++;
		}
	}
	pGLK->refresh(true);
}

void MainWindow::special_Draw_Command() {

	for (GLKPOSITION pos = polygenMeshList.GetHeadPosition(); pos != nullptr;) {
		PolygenMesh* polygenMesh = (PolygenMesh*)polygenMeshList.GetNext(pos);

		if (polygenMesh->meshType != WAYPOINT) continue;

		for (GLKPOSITION posMesh = polygenMesh->GetMeshList().GetHeadPosition(); posMesh != nullptr;) {
			QMeshPatch* Patch = (QMeshPatch*)polygenMesh->GetMeshList().GetNext(posMesh);

			Patch->drawSingularity = false;
			Patch->drawSolveSelect = false;

			if (ui->checkBox_showSingularityNode->isChecked()) {
				Patch->drawSingularity = true;
			}

			if (ui->checkBox_showSolveSelection->isChecked()) {
				Patch->drawSolveSelect = true;
			}
		}
	}
	pGLK->refresh(true);

}

void MainWindow::readWayPointData(std::string packName) {

	PolygenMesh* waypointSet = NULL;
	waypointSet = _detectPolygenMesh(WAYPOINT, "Waypoints", false);

	if (waypointSet == NULL) {

		waypointSet = _buildPolygenMesh(WAYPOINT, "Waypoints");

		//read waypoint files and build mesh_patches
		char filename[1024];
		for (int i = 0; i < wayPointFileCell.size(); i++) {

			sprintf(filename, "%s%s%s", packName.c_str(), "/", wayPointFileCell[i].data());

			QMeshPatch* waypoint = new QMeshPatch;
			waypoint->SetIndexNo(waypointSet->GetMeshList().GetCount()); //index begin from 0
			waypointSet->GetMeshList().AddTail(waypoint);
			waypoint->patchName = wayPointFileCell[i].data();

			// isSupportLayer
			string::size_type supportFlag = wayPointFileCell[i].find("S");
			if (supportFlag == string::npos)	waypoint->isSupportLayer = false;
			else	waypoint->isSupportLayer = true;

			waypoint->inputPosNorFile(filename, ui->checkBox_Yup2Zup->isChecked());

			_modifyCoord(waypoint);//modifyCoord upY->upZ
		}
	}

	// MoveModel(Xmove, Ymove, Zmove);
	for (GLKPOSITION patchPos = waypointSet->GetMeshList().GetHeadPosition(); patchPos;) {
		QMeshPatch* waypoint_patch = (QMeshPatch*)waypointSet->GetMeshList().GetNext(patchPos);
		for (GLKPOSITION Pos = waypoint_patch->GetNodeList().GetHeadPosition(); Pos;) {
			QMeshNode* Node = (QMeshNode*)waypoint_patch->GetNodeList().GetNext(Pos);

			double xx, yy, zz, nx, ny, nz;
			Node->GetCoord3D_last(xx, yy, zz);
			Node->GetNormal_last(nx, ny, nz);

			xx += Xmove;	yy += Ymove;	zz += Zmove;

			// check the origial waypoints' zz coordinate
			Node->negativeZz = false;
			if (zz <= 40.0) Node->negativeZz = true; // 40.0 is the height of platform

			Node->SetCoord3D(xx, yy, zz);
			Node->SetNormal(nx, ny, nz);

			Node->m_printPos << xx, yy, zz;
			Node->m_printNor << nx, ny, nz;
			Node->m_printNor = Node->m_printNor.normalized();

		}
	}
	//viewAllWaypointLayers();
	std::cout << "------------------------------------------- WayPoints Load Finish!" << std::endl;
}

void MainWindow::readSliceData(std::string sliceSetName) {

	PolygenMesh* sliceSet = NULL;
	sliceSet = _detectPolygenMesh(LAYER, "Slices", false);

	if (sliceSet == NULL) {

		sliceSet = _buildPolygenMesh(LAYER, "Slices");

		//read slice files and build mesh_patches
		char filename[1024];
		for (int i = 0; i < sliceSetFileCell.size(); i++)
		{
			sprintf(filename, "%s%s%s", sliceSetName.c_str(), "/", sliceSetFileCell[i].data());
			//cout << filename << endl;

			QMeshPatch* slice = new QMeshPatch;
			slice->SetIndexNo(sliceSet->GetMeshList().GetCount()); //index begin from 0
			sliceSet->GetMeshList().AddTail(slice);
			slice->patchName = sliceSetFileCell[i].data();

			// isSupportLayer
			string::size_type supportFlag = sliceSetFileCell[i].find("S");
			if (supportFlag == string::npos)	slice->isSupportLayer = false;
			else	slice->isSupportLayer = true;

			//choose the method for reading files with different types
			string targetFileType = ".obj";
			string fileName = sliceSetFileCell[i].data();
			if (fileName.find(targetFileType))	slice->inputOBJFile(filename);
			else slice->inputOFFFile(filename);

			_modifyCoord(slice);//modifyCoord upY->upZ
		}
	}

	// MoveModel(Xmove, Ymove, Zmove);
	for (GLKPOSITION patchPos = sliceSet->GetMeshList().GetHeadPosition(); patchPos;) {
		QMeshPatch* slice_patch = (QMeshPatch*)sliceSet->GetMeshList().GetNext(patchPos);
		for (GLKPOSITION Pos = slice_patch->GetNodeList().GetHeadPosition(); Pos;) {
			QMeshNode* Node = (QMeshNode*)slice_patch->GetNodeList().GetNext(Pos);

			double xx, yy, zz, nx, ny, nz;
			Node->GetCoord3D_last(xx, yy, zz);

			xx += Xmove;	yy += Ymove;	zz += Zmove;

			Node->SetCoord3D(xx, yy, zz);
		}
	}

	std::cout << "------------------------------------------- Slices Load Finish!" << std::endl;
}

void MainWindow::readExtruderHeadfile(std::string extruderHeadName) {

	// isBuilt
	if (_detectPolygenMesh(CNC_PARTS, "ExtruderHead", false) != NULL) return;

	PolygenMesh* extruderHead = _buildPolygenMesh(CNC_PARTS, "ExtruderHead");

	//read slice files and build mesh_patches
	char filename[1024];

	sprintf(filename, "%s%s", "../DataSet/Sorce/CNCmodel/", extruderHeadName.c_str());
	//cout << filename << endl;

	QMeshPatch* eHead = new QMeshPatch;
	eHead->SetIndexNo(extruderHead->GetMeshList().GetCount()); //index begin from 0
	extruderHead->GetMeshList().AddTail(eHead);
	eHead->inputOBJFile(filename);

	// enlarge 1.1x eHead
	/*double ratio = 1.1;
	for (GLKPOSITION eHeadPos = eHead->GetNodeList().GetHeadPosition(); eHeadPos;) {
		QMeshNode* eHeadNode = (QMeshNode*)eHead->GetNodeList().GetNext(eHeadPos);

		double xx, yy, zz;
		eHeadNode->GetCoord3D(xx, yy, zz);
		if (xx == 0.0 && yy == 0.0 && zz == 0.0) eHeadNode->isPrintPnt = true;

		xx *= ratio; yy *= ratio; zz *= ratio;
		eHeadNode->SetCoord3D(xx, yy, zz);
	}*/
	// record the original(= print type: Zup + 0,0,0 ) message
	for (GLKPOSITION eHeadPos = eHead->GetNodeList().GetHeadPosition(); eHeadPos;) {
		QMeshNode* node = (QMeshNode*)eHead->GetNodeList().GetNext(eHeadPos);
		double xx, yy, zz;
		node->GetCoord3D(xx, yy, zz);
		if (xx == 0.0 && yy == 0.0 && zz == 0.0) node->isPrintPnt = true;
		node->m_printPos << xx, yy, zz;
	}

	std::cout << "------------------------------------------- Extruder Head Load Finish!" << std::endl;

}

void MainWindow::readPlatformfile(std::string platformName) {

	// isBuilt
	if (_detectPolygenMesh(CNC_PARTS, "PrintPlatform", false) != NULL) return;

	PolygenMesh* platform = _buildPolygenMesh(CNC_PARTS, "PrintPlatform");

	//read slice files and build mesh_patches
	char filename[1024];

	sprintf(filename, "%s%s", "../DataSet/Sorce/CNCmodel/", platformName.c_str());
	//cout << filename << endl;

	QMeshPatch* platformPatch = new QMeshPatch;
	platformPatch->SetIndexNo(platform->GetMeshList().GetCount()); //index begin from 0
	platform->GetMeshList().AddTail(platformPatch);
	platformPatch->inputOBJFile(filename);

	double xmin, ymin, zmin, xmax, ymax, zmax;
	platformPatch->ComputeBoundingBox(xmin, ymin, zmin, xmax, ymax, zmax);

	//std::cout << xmin <<" "<< ymin << " " << zmin << " " << xmax << " " << ymax << " " << zmax << endl;

	double Xshift = (xmax + xmin) / 2;
	double Yshift = (ymax + ymin) / 2;
	double Zshift = zmax;

	double pp[3];
	for (GLKPOSITION Pos = platformPatch->GetNodeList().GetHeadPosition(); Pos;) {
		QMeshNode* Node = (QMeshNode*)platformPatch->GetNodeList().GetNext(Pos);
		Node->GetCoord3D(pp[0], pp[1], pp[2]);
		Node->SetCoord3D(pp[0] - Xshift, pp[1] - Yshift, pp[2] - Zshift + 40.0);
		//Node->SetCoord3D(pp[0] - Xshift, pp[1] - Yshift, pp[2] - Zshift + 0.0);
		// record a base Position of TET node
		Node->GetCoord3D(pp[0], pp[1], pp[2]);
		Node->m_printPos << pp[0], pp[1], pp[2];
	}

	std::cout << "------------------------------------------- Platform Load Finish!" << std::endl;

}

void MainWindow::readGcodeFile(Eigen::MatrixXf& Gcode_Table, std::string FileName) {

	double E3_Xoff = ui->doubleSpinBox_E3XOFF->value();
	double E3_Yoff = ui->doubleSpinBox_E3YOFF->value();

	char targetFilename[1024];
	std::sprintf(targetFilename, "%s%s", "../DataSet/Gcode/", FileName.c_str());
	FILE* fp; char linebuf[2048];
	double machine_X = 0.0, machine_Y = 0.0, machine_Z = 300.0, machine_B = 0.0, machine_C = 0.0;
	fp = fopen(targetFilename, "r");
	if (!fp) {
		printf("===============================================\n");
		printf("Can not open the data file - Gcode File Import!\n");
		printf("===============================================\n");
		return;
	}

	//get the num of lines in Gcode file.
	int lines = 0;
	while (true) {
		fgets(linebuf, 255, fp);
		if (feof(fp)) break;
		lines++;
	}
	std::fclose(fp);
	//std::cout << lines << std::endl;

	fp = fopen(targetFilename, "r");
	Gcode_Table = Eigen::MatrixXf::Zero(lines, 6);
	bool T3_flag = false;
	for (int i = 0; i < lines; i++) {

		double newLayerFlag = 0.0;// DOUBLE type is for the compactness of data structure

		fgets(linebuf, 255, fp);
		//std::cout << linebuf;

		string str = linebuf;
		string::size_type position_X = str.find("X");	string::size_type position_Y = str.find("Y");	string::size_type position_Z = str.find("Z");
		string::size_type position_B = str.find("B");	string::size_type position_C = str.find("C");
		string::size_type position_E = str.find("E");	string::size_type position_F = str.find("F");

		string::size_type position_M104 = str.find("M104");
		string::size_type position_T1 = str.find("T1"); string::size_type position_T2 = str.find("T2");	string::size_type position_T3 = str.find("T3");
		if ((position_M104 != string::npos) && (position_T3 != string::npos)) T3_flag = true;
		if ((position_M104 != string::npos) && (position_T2 != string::npos)) T3_flag = false;
		if ((position_M104 != string::npos) && (position_T1 != string::npos)) T3_flag = false;
		//std::cout << position_X << " " << position_Y << " " << position_Z << " " << position_B << " " << position_C << std::endl;

		string::size_type GFlag = str.find("G");
		if (GFlag != string::npos) {
			// G28 X0.000 Y0.000 Z200.000 B0.000 C0.000 F5250
			if (position_X != str.npos && position_Y != str.npos && position_Z != str.npos
				&& position_B != str.npos && position_C != str.npos
				&& position_E == str.npos && position_F != str.npos) {

				string X_temp = str.substr(position_X + 1, position_Y - position_X - 2);
				string Y_temp = str.substr(position_Y + 1, position_Z - position_Y - 2);
				string Z_temp = str.substr(position_Z + 1, position_B - position_Z - 2);
				string B_temp = str.substr(position_B + 1, position_C - position_B - 2);
				string C_temp = str.substr(position_C + 1, position_F - position_C - 2);

				machine_X = atof(X_temp.c_str());	machine_Y = atof(Y_temp.c_str());	machine_Z = atof(Z_temp.c_str());
				machine_B = atof(B_temp.c_str());	machine_C = atof(C_temp.c_str());

			}
			// G1 X-18.558 Y-22.022 Z0.443 B0.000 C0.000 E0.519
			else if (position_X != str.npos && position_Y != str.npos && position_Z != str.npos
				&& position_B != str.npos && position_C != str.npos
				&& position_E != str.npos && position_F == str.npos) {

				string X_temp = str.substr(position_X + 1, position_Y - position_X - 2);
				string Y_temp = str.substr(position_Y + 1, position_Z - position_Y - 2);
				string Z_temp = str.substr(position_Z + 1, position_B - position_Z - 2);
				string B_temp = str.substr(position_B + 1, position_C - position_B - 2);
				string C_temp = str.substr(position_C + 1, position_E - position_C - 2);

				machine_X = atof(X_temp.c_str());	machine_Y = atof(Y_temp.c_str());	machine_Z = atof(Z_temp.c_str());
				machine_B = atof(B_temp.c_str());	machine_C = atof(C_temp.c_str());

			}
			// G0 X-18.558 Y-22.022 B0.000 C0.000 F5250
			else if (position_X != str.npos && position_Y != str.npos && position_Z == str.npos
				&& position_B != str.npos && position_C != str.npos
				&& position_E == str.npos && position_F != str.npos) {

				string X_temp = str.substr(position_X + 1, position_Y - position_X - 2);
				string Y_temp = str.substr(position_Y + 1, position_B - position_Y - 2);
				string B_temp = str.substr(position_B + 1, position_C - position_B - 2);
				string C_temp = str.substr(position_C + 1, position_F - position_C - 2);

				machine_X = atof(X_temp.c_str());	machine_Y = atof(Y_temp.c_str());
				machine_B = atof(B_temp.c_str());	machine_C = atof(C_temp.c_str());
			}
			// G0 Z2.940 F4750
			else if (position_X == str.npos && position_Y == str.npos && position_Z != str.npos
				&& position_B == str.npos && position_C == str.npos
				&& position_E == str.npos && position_F != str.npos) {

				string Z_temp = str.substr(position_Z + 1, position_F - position_Z - 2);

				machine_Z = atof(Z_temp.c_str());
			}
			// G0 Z3.022 E562.31 F4750
			else if (position_X == str.npos && position_Y == str.npos && position_Z != str.npos
				&& position_B == str.npos && position_C == str.npos
				&& position_E != str.npos && position_F != str.npos) {

				string Z_temp = str.substr(position_Z + 1, position_E - position_Z - 2);

				machine_Z = atof(Z_temp.c_str());

			}
			// G1 F1500 // for new layer flag
			else if (position_X == str.npos && position_Y == str.npos && position_Z == str.npos
				&& position_B == str.npos && position_C == str.npos
				&& position_E == str.npos && position_F != str.npos) {

				newLayerFlag = 1.0;
			}
			// test for special case
			// else { cout << "------------------------------------------ some special case" << endl; }

		}
		// test for special case
		// else { cout << "------------------------------------------ some special case" << endl; }
		//std::cout << "X: " << machine_X << " Y: " << machine_Y << " Z: " << machine_Z
		//	<< " B: " << machine_B << " C: " << machine_C << std::endl << std::endl;

		if (T3_flag == true) {
			Gcode_Table.row(i) << machine_X - E3_Xoff, machine_Y - E3_Yoff, machine_Z, machine_B, machine_C, newLayerFlag;
		}
		else {
			Gcode_Table.row(i) << machine_X, machine_Y, machine_Z, machine_B, machine_C, newLayerFlag;
		}

	}
	std::fclose(fp);
	std::cout << "------------------------------------------- Gcode Load Finish!" << endl;
}

void MainWindow::readCNCfile(std::vector<std::string> CNCfileSet) {

	// isBuilt
	if (_detectPolygenMesh(CNC_PARTS, "CNC_Model", false) != NULL) return;

	PolygenMesh* cncModelSet = _buildPolygenMesh(CNC_PARTS, "CNC_Model");

	//read CNC files and build mesh_patches
	char filename[1024];

	for (int i = 0; i < CNCfileSet.size(); i++) {
		sprintf(filename, "%s%s%s", "../DataSet/Sorce/CNCmodel/", CNCfileSet[i].c_str(), ".obj");
		//cout << filename << endl;

		QMeshPatch* cncPatch = new QMeshPatch;
		cncPatch->SetIndexNo(cncModelSet->GetMeshList().GetCount()); //index begin from 0
		cncModelSet->GetMeshList().AddTail(cncPatch);
		cncPatch->inputOBJFile(filename);
		cncPatch->patchName = CNCfileSet[i].data();

		//cout << "CNCfileSet[i].data() " << CNCfileSet[i].data()	<< " CNCfileSet[i] " << CNCfileSet[i] << endl;

		// record the original(= print type: Zup + 0,0,0) message
		for (GLKPOSITION Pos = cncPatch->GetNodeList().GetHeadPosition(); Pos;) {
			QMeshNode* node = (QMeshNode*)cncPatch->GetNodeList().GetNext(Pos);
			double xx, yy, zz;
			node->GetCoord3D(xx, yy, zz);
			node->m_printPos << xx, yy, zz;
		}
	}

	std::cout << "-------------------------------------------- CNC Load Finish!" << std::endl;
}

void MainWindow::doTimerGcodeMoving(){
	int simuLayerInd_1st = ui->spinBox_GcodeGeneFromIndex->value();
	//int simuLayerInd_1st = 0;
	bool singleLayer = ui->checkBox_EachLayerSwitch->isChecked();

	double machine_X = Gcode_Table(gcodetimerItertime, 0);
	double machine_Y = Gcode_Table(gcodetimerItertime, 1);
	double machine_Z = Gcode_Table(gcodetimerItertime, 2);
	double machine_B = Gcode_Table(gcodetimerItertime, 3);
	double machine_C = Gcode_Table(gcodetimerItertime, 4);
	double newLayerFlag = Gcode_Table(gcodetimerItertime, 5);


	PolygenMesh* waypoints; PolygenMesh* extruderHead; PolygenMesh* platform; PolygenMesh* cnc;

	/*for real simulation include inserted waypoint*/
	waypoints = _detectPolygenMesh(WAYPOINT, "Waypoints", false);
	//waypoints = polygenMesh_XYZBCE;

	extruderHead = _detectPolygenMesh(CNC_PARTS, "ExtruderHead", false);
	platform = _detectPolygenMesh(CNC_PARTS, "PrintPlatform", false);
	cnc = _detectPolygenMesh(CNC_PARTS, "CNC_Model", false);
	if (waypoints == NULL || extruderHead == NULL || platform == NULL || cnc == NULL) {
		cerr << "There is no PolygenMesh" << endl; return;
	}

	if (newLayerFlag != 0.0) {
		simuLayerInd++;
		std::cout << "Simulating the printing of Layer " << simuLayerInd << std::endl;
	}

	// Extruder Head move		/ Z
	GLKPOSITION eHeadPos = extruderHead->GetMeshList().GetHeadPosition();
	QMeshPatch* eHeadPatch = (QMeshPatch*)extruderHead->GetMeshList().GetAt(eHeadPos);
	for (GLKPOSITION Pos = eHeadPatch->GetNodeList().GetHeadPosition(); Pos;) {
		QMeshNode* node = (QMeshNode*)eHeadPatch->GetNodeList().GetNext(Pos);

		double eHead_Px = node->m_printPos(0);
		double eHead_Py = node->m_printPos(1);
		double eHead_Pz = node->m_printPos(2);

		node->SetCoord3D(eHead_Px, eHead_Py, eHead_Pz + machine_Z);
	}

	// Platform move			/-X/-Y  & rotate B/C 
	GLKPOSITION platformPos = platform->GetMeshList().GetHeadPosition();
	QMeshPatch* platformPatch = (QMeshPatch*)platform->GetMeshList().GetAt(platformPos);
	for (GLKPOSITION Pos = platformPatch->GetNodeList().GetHeadPosition(); Pos;) {
		QMeshNode* node = (QMeshNode*)platformPatch->GetNodeList().GetNext(Pos);

		Eigen::Vector3d platformGuesture = node->calPlatformGuesture(node->m_printPos, machine_X, machine_Y, machine_B, machine_C);

		node->SetCoord3D(platformGuesture[0], platformGuesture[1], platformGuesture[2]);
	}

	// Model move				/-X/-Y  & rotate B/C 
	for (GLKPOSITION waypointsPos = waypoints->GetMeshList().GetHeadPosition(); waypointsPos;) {
		QMeshPatch* WayPointsPatch = (QMeshPatch*)waypoints->GetMeshList().GetNext(waypointsPos);
		// show one layer
		if (singleLayer == true) {
			if (WayPointsPatch->GetIndexNo() != simuLayerInd) {
				WayPointsPatch->drawThisPatch = false;	continue;
			}
			else {
				WayPointsPatch->drawThisPatch = true;
			}
		}
		// show before layers
		else {
			if (WayPointsPatch->GetIndexNo() <= simuLayerInd && WayPointsPatch->GetIndexNo() >= simuLayerInd_1st) {
				WayPointsPatch->drawThisPatch = true;
			}
			else {
				WayPointsPatch->drawThisPatch = false; 	continue;
			}
		}

		for (GLKPOSITION Pos = WayPointsPatch->GetNodeList().GetHeadPosition(); Pos;) {
			QMeshNode* node = (QMeshNode*)WayPointsPatch->GetNodeList().GetNext(Pos);

			Eigen::Vector3d waypointsGuesture = node->calPlatformGuesture(node->m_printPos, machine_X, machine_Y, machine_B, machine_C);
			node->SetCoord3D(waypointsGuesture[0], waypointsGuesture[1], waypointsGuesture[2]);
		}
	}

	// CNC move
	for (GLKPOSITION cncPos = cnc->GetMeshList().GetHeadPosition(); cncPos;) {
		QMeshPatch* cncPatch = (QMeshPatch*)cnc->GetMeshList().GetNext(cncPos);

		for (GLKPOSITION Pos = cncPatch->GetNodeList().GetHeadPosition(); Pos;) {
			QMeshNode* node = (QMeshNode*)cncPatch->GetNodeList().GetNext(Pos);

			Eigen::Vector3d cncGuesture;

			if (cncPatch->patchName == "B_axis") {
				cncGuesture = node->calPlatformGuesture(node->m_printPos, machine_X, machine_Y, machine_B, 0.0);
			}

			if (cncPatch->patchName == "Z_axis") {
				cncGuesture << node->m_printPos(0), node->m_printPos(1), node->m_printPos(2) + machine_Z;
			}

			if (cncPatch->patchName == "Y_axis") {
				cncGuesture = node->calPlatformGuesture(node->m_printPos, machine_X, machine_Y, 0.0, 0.0);
			}

			if (cncPatch->patchName == "X_axis") {
				cncGuesture = node->calPlatformGuesture(node->m_printPos, machine_X, 0.0, 0.0, 0.0);
			}

			if (cncPatch->patchName == "base") {
				cncGuesture << node->m_printPos(0), node->m_printPos(1), node->m_printPos(2);
			}

			node->SetCoord3D(cncGuesture[0], cncGuesture[1], cncGuesture[2]);
		}
	}


	gcodetimerItertime++;
	ui->progressBar_GcodeSimulation->setValue(gcodetimerItertime);
	if (gcodetimerItertime >= Gcode_Table.rows()) {
		Gcode_timer.stop();
		std::cout << "------------------------------------------- G code Simulation Finish!" << endl;
	}
	pGLK->refresh(true);

	bool stopSimulation_switch = ui->checkBox_stopSimulation->isChecked();
	if (stopSimulation_switch == true) {
		Gcode_timer.stop();
		ui->checkBox_stopSimulation->setCheckState(Qt::Unchecked);
		std::cout << "------------------------------------------- Quit G code Simulation !" << endl;
	}

}

//function test
void MainWindow::test_button() {
	// create the graph given in above fugure 
	int V = 11;
	std::vector<int> endNode_NoSet = { 9,10 };
	Graph g(V, endNode_NoSet);

	//  making above shown graph 
	g.addEdge(0, 4, 1);		g.addEdge(0, 1, 2);
	g.addEdge(1, 8, 3);		g.addEdge(1, 11, 4);	g.addEdge(1, 8, 5);	g.addEdge(1, 11,6);
	g.addEdge(2, 8, 3);		g.addEdge(2, 11, 4);	g.addEdge(2, 8, 5);	g.addEdge(2, 1, 6);
	g.addEdge(3, 7, 7);		g.addEdge(3, 2, 8);		g.addEdge(4, 7, 7);	g.addEdge(4, 2, 8);
	g.addEdge(5, 7, 7);		g.addEdge(5, 2, 8);
	g.addEdge(6, 1, 7);		g.addEdge(6, 2, 8);
	g.addEdge(7, 4, 9);		g.addEdge(7, 1, 10);
	g.addEdge(8, 14, 9);	g.addEdge(8, 10, 10);

	g.shortestPath(0);
	std::cout << "0 ";
	for (int i = 0; i < g.path.size(); i++) {
		std::cout << g.path[i] << " ";
	}
	std::cout << std::endl;
}