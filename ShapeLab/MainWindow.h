#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSignalMapper>
#include <QStandardItemModel>
#include "../GLKLib/GLKLib.h"
#include "../QMeshLib/PolygenMesh.h"
#include <omp.h>
#include <QTimer>
#include <QLabel>
#include "GcodeGeneration.h"

#define PI		3.141592654
#define DEGREE_TO_ROTATE(x)		0.0174532922222*x
#define ROTATE_TO_DEGREE(x)		57.295780490443*x

using namespace std;

class DeformTet;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
	// Qtimer - defined function
    void doTimerGcodeMoving();

private:
    Ui::MainWindow *ui;
    GLKLib *pGLK;

    /* add for Gcode generation */
    QTimer Gcode_timer; //Gcode Simulation timer
    int gcodetimerItertime;
    int simuLayerInd;
    Eigen::MatrixXf Gcode_Table;
    //unsigned int operationTime = 0;
    /* ------------------------ */
	GLKObList polygenMeshList;

private:
    void createActions();
    void createTreeView();
	void showTetraDeformationRatio();
	void MoveHandleRegion();
	void QTgetscreenshoot();

    PolygenMesh *getSelectedPolygenMesh();

    QSignalMapper *signalMapper;
    QStandardItemModel *treeModel;

	DeformTet *Deformation;

private:
    // functions for self-use
    void _natSort(std::string dirctory, vector<std::string>& fileNameCell);       // for Nature Sorting
    void _modifyCoord(QMeshPatch* patchFile);                           // for modifyCoord upY->upZ

    PolygenMesh* _buildPolygenMesh(mesh_type type, std::string name);
    PolygenMesh* _detectPolygenMesh(mesh_type type, std::string name, bool detectType);

    void readWayPointData(std::string packName);                             // for WayPoint Data loading
    void readSliceData(std::string sliceSetName);                            // for Slice Data loading
    void readExtruderHeadfile(std::string extruderHeadName);                 // for Extruder Head file loading
    void readPlatformfile(std::string platformName);                         // for Platform file loading
    void readGcodeFile(Eigen::MatrixXf& Gcode_Table, std::string FileName);  // for simulation
    void readCNCfile(std::vector<std::string> CNCfileSet);                   // for CNC model loading

    // variables for self-use
    GcodeGeneration* GcodeGene;                                         // for globally use GcodeGene
    vector<std::string> wayPointFileCell;	                                // Waypoints Dir Files
    vector<std::string> sliceSetFileCell;	                                // Layers Dir Files
    double Xmove, Ymove, Zmove;                                         // Model Movement Distance
    unsigned int operationTime = 0;                                     // for smooth only one time


protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);

private slots:
    void open();
    void save();
	void saveSelection();
	void readSelection();

    void signalNavigation(int flag);
    void shiftToOrigin();
    void updateTree();
	void mouseMoveEvent(QMouseEvent *event);
    void on_pushButton_clearAll_clicked();
    void on_treeView_clicked(const QModelIndex &index);

	/*Button Function*/
    void readGcodeSourceData();
    void runDHWcalculation();
    void runSingularityOpt();
    void runCollisionCheck();
    void runCollisionElimination();
    void runWriteGcode();
    void runGcodeSimulation();
    void runContinuousCollisionChecking();

    /*This is for Display*/
    void changeWaypointDisplay();
    void viewAllWaypointLayers();
    void special_Draw_Command();

    /*This is for function test*/
    void test_button();
};

#endif // MAINWINDOW_H
