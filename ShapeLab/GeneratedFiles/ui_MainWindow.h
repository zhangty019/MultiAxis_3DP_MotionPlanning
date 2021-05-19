/********************************************************************************
** Form generated from reading UI file 'MainWindow.ui'
**
** Created by: Qt User Interface Compiler version 5.12.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDockWidget>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QTreeView>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionOpen;
    QAction *actionFront;
    QAction *actionBack;
    QAction *actionTop;
    QAction *actionBottom;
    QAction *actionLeft;
    QAction *actionRight;
    QAction *actionIsometric;
    QAction *actionZoom_In;
    QAction *actionZoom_Out;
    QAction *actionZoom_All;
    QAction *actionZoom_Window;
    QAction *actionShade;
    QAction *actionMesh;
    QAction *actionNode;
    QAction *actionSave;
    QAction *actionSelectNode;
    QAction *actionSelectFace;
    QAction *actionShifttoOrigin;
    QAction *actionProfile;
    QAction *actionFaceNormal;
    QAction *actionNodeNormal;
    QAction *actionSelectEdge;
    QAction *actionGenerate;
    QAction *actionTest_1;
    QAction *actionSelectFix;
    QAction *actionSelectHandle;
    QAction *actionSaveSelection;
    QAction *actionReadSelection;
    QAction *actionSelectChamber;
    QAction *actionExport_to_Abaqus_model;
    QWidget *centralWidget;
    QHBoxLayout *horizontalLayout;
    QToolBar *navigationToolBar;
    QStatusBar *statusBar;
    QToolBar *selectionToolBar;
    QDockWidget *dockWidget;
    QWidget *dockWidgetContents;
    QVBoxLayout *verticalLayout;
    QLabel *label_MANY_3DP_CNC_CAM;
    QFrame *line;
    QHBoxLayout *horizontalLayout_12;
    QLabel *label;
    QSpacerItem *horizontalSpacer;
    QCheckBox *checkBox_Yup2Zup;
    QHBoxLayout *horizontalLayout_11;
    QLabel *label_Xmove;
    QDoubleSpinBox *doubleSpinBox_Xmove;
    QLabel *label_Ymove;
    QDoubleSpinBox *doubleSpinBox_Ymove;
    QLabel *label_Zmove;
    QDoubleSpinBox *doubleSpinBox_Zmove;
    QHBoxLayout *horizontalLayout_16;
    QLabel *label_PosNorFile;
    QLineEdit *lineEdit_SorceDataDir;
    QPushButton *pushButton_readGcodeSourceData;
    QLabel *label_WaypointDisplay;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_2;
    QLabel *label_4;
    QSpinBox *spinBox_StartLayerIndex;
    QLabel *label_3;
    QSpinBox *spinBox_ShowLayerIndex;
    QCheckBox *checkBox_EachLayerSwitch;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_currentFile;
    QPushButton *pushButton_ShowAllLayers;
    QFrame *line_2;
    QHBoxLayout *horizontalLayout_4;
    QLabel *label_6;
    QLabel *label_7;
    QSpinBox *spinBox_GcodeGeneFromIndex;
    QLabel *label_8;
    QSpinBox *spinBox_GcodeGeneToIndex;
    QFrame *line_4;
    QLabel *label_14;
    QHBoxLayout *horizontalLayout_6;
    QLabel *label_9;
    QLabel *label_10;
    QDoubleSpinBox *doubleSpinBox_E3XOFF;
    QLabel *label_11;
    QDoubleSpinBox *doubleSpinBox_E3YOFF;
    QLabel *label_12;
    QDoubleSpinBox *doubleSpinBox_lambda;
    QLabel *label_5;
    QHBoxLayout *horizontalLayout_5;
    QCheckBox *checkBox_varyDistance;
    QCheckBox *checkBox_varyHeight;
    QCheckBox *checkBox_varyWidth;
    QCheckBox *checkBox_TestDHW_Switch;
    QPushButton *pushButton_calDWH;
    QFrame *line_3;
    QHBoxLayout *horizontalLayout_7;
    QPushButton *pushButton_calSingularOpt;
    QHBoxLayout *horizontalLayout_8;
    QCheckBox *checkBox_showSingularityNode;
    QCheckBox *checkBox_showSolveSelection;
    QHBoxLayout *horizontalLayout_9;
    QPushButton *pushButton_calCollision;
    QHBoxLayout *horizontalLayout_10;
    QPushButton *pushButton_calCollisionElimination;
    QPushButton *pushButton_calCollision_continuous;
    QHBoxLayout *horizontalLayout_13;
    QPushButton *pushButton_Gcode_writting;
    QHBoxLayout *horizontalLayout_14;
    QPushButton *pushButton_GcodeSimulation;
    QProgressBar *progressBar_GcodeSimulation;
    QCheckBox *checkBox_stopSimulation;
    QTreeView *treeView;
    QPushButton *pushButton_test;
    QPushButton *pushButton_clearAll;
    QMenuBar *menuBar;
    QMenu *menuFile;
    QMenu *menuView;
    QMenu *menuSelect;
    QToolBar *toolBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(1325, 1070);
        MainWindow->setMinimumSize(QSize(0, 0));
        QFont font;
        font.setBold(true);
        font.setWeight(75);
        MainWindow->setFont(font);
        MainWindow->setMouseTracking(true);
        MainWindow->setFocusPolicy(Qt::StrongFocus);
        MainWindow->setAcceptDrops(true);
        actionOpen = new QAction(MainWindow);
        actionOpen->setObjectName(QString::fromUtf8("actionOpen"));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/resource/Open Folder.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionOpen->setIcon(icon);
        actionFront = new QAction(MainWindow);
        actionFront->setObjectName(QString::fromUtf8("actionFront"));
        actionFront->setCheckable(false);
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/resource/Front View.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionFront->setIcon(icon1);
        actionBack = new QAction(MainWindow);
        actionBack->setObjectName(QString::fromUtf8("actionBack"));
        actionBack->setCheckable(false);
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/resource/Back View.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionBack->setIcon(icon2);
        actionTop = new QAction(MainWindow);
        actionTop->setObjectName(QString::fromUtf8("actionTop"));
        actionTop->setCheckable(false);
        QIcon icon3;
        icon3.addFile(QString::fromUtf8(":/resource/Top View.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionTop->setIcon(icon3);
        actionBottom = new QAction(MainWindow);
        actionBottom->setObjectName(QString::fromUtf8("actionBottom"));
        actionBottom->setCheckable(false);
        QIcon icon4;
        icon4.addFile(QString::fromUtf8(":/resource/Bottom View.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionBottom->setIcon(icon4);
        actionLeft = new QAction(MainWindow);
        actionLeft->setObjectName(QString::fromUtf8("actionLeft"));
        QIcon icon5;
        icon5.addFile(QString::fromUtf8(":/resource/Left View.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionLeft->setIcon(icon5);
        actionRight = new QAction(MainWindow);
        actionRight->setObjectName(QString::fromUtf8("actionRight"));
        QIcon icon6;
        icon6.addFile(QString::fromUtf8(":/resource/Right View.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionRight->setIcon(icon6);
        actionIsometric = new QAction(MainWindow);
        actionIsometric->setObjectName(QString::fromUtf8("actionIsometric"));
        QIcon icon7;
        icon7.addFile(QString::fromUtf8(":/resource/Isometric View.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionIsometric->setIcon(icon7);
        actionZoom_In = new QAction(MainWindow);
        actionZoom_In->setObjectName(QString::fromUtf8("actionZoom_In"));
        QIcon icon8;
        icon8.addFile(QString::fromUtf8(":/resource/Zoom In.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionZoom_In->setIcon(icon8);
        actionZoom_Out = new QAction(MainWindow);
        actionZoom_Out->setObjectName(QString::fromUtf8("actionZoom_Out"));
        QIcon icon9;
        icon9.addFile(QString::fromUtf8(":/resource/Zoom Out.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionZoom_Out->setIcon(icon9);
        actionZoom_All = new QAction(MainWindow);
        actionZoom_All->setObjectName(QString::fromUtf8("actionZoom_All"));
        QIcon icon10;
        icon10.addFile(QString::fromUtf8(":/resource/Zoom All.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionZoom_All->setIcon(icon10);
        actionZoom_Window = new QAction(MainWindow);
        actionZoom_Window->setObjectName(QString::fromUtf8("actionZoom_Window"));
        QIcon icon11;
        icon11.addFile(QString::fromUtf8(":/resource/Zoom Window.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionZoom_Window->setIcon(icon11);
        actionShade = new QAction(MainWindow);
        actionShade->setObjectName(QString::fromUtf8("actionShade"));
        actionShade->setCheckable(true);
        QIcon icon12;
        icon12.addFile(QString::fromUtf8(":/resource/Shade.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionShade->setIcon(icon12);
        actionMesh = new QAction(MainWindow);
        actionMesh->setObjectName(QString::fromUtf8("actionMesh"));
        actionMesh->setCheckable(true);
        QIcon icon13;
        icon13.addFile(QString::fromUtf8(":/resource/Mesh.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionMesh->setIcon(icon13);
        actionNode = new QAction(MainWindow);
        actionNode->setObjectName(QString::fromUtf8("actionNode"));
        actionNode->setCheckable(true);
        QIcon icon14;
        icon14.addFile(QString::fromUtf8(":/resource/Node.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionNode->setIcon(icon14);
        actionSave = new QAction(MainWindow);
        actionSave->setObjectName(QString::fromUtf8("actionSave"));
        QIcon icon15;
        icon15.addFile(QString::fromUtf8(":/resource/Save as.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionSave->setIcon(icon15);
        actionSelectNode = new QAction(MainWindow);
        actionSelectNode->setObjectName(QString::fromUtf8("actionSelectNode"));
        QIcon icon16;
        icon16.addFile(QString::fromUtf8(":/resource/selectNode.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionSelectNode->setIcon(icon16);
        actionSelectFace = new QAction(MainWindow);
        actionSelectFace->setObjectName(QString::fromUtf8("actionSelectFace"));
        QIcon icon17;
        icon17.addFile(QString::fromUtf8(":/resource/selectFace.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionSelectFace->setIcon(icon17);
        actionShifttoOrigin = new QAction(MainWindow);
        actionShifttoOrigin->setObjectName(QString::fromUtf8("actionShifttoOrigin"));
        actionProfile = new QAction(MainWindow);
        actionProfile->setObjectName(QString::fromUtf8("actionProfile"));
        actionProfile->setCheckable(true);
        QIcon icon18;
        icon18.addFile(QString::fromUtf8(":/resource/Profile.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionProfile->setIcon(icon18);
        actionFaceNormal = new QAction(MainWindow);
        actionFaceNormal->setObjectName(QString::fromUtf8("actionFaceNormal"));
        actionFaceNormal->setCheckable(true);
        QIcon icon19;
        icon19.addFile(QString::fromUtf8(":/resource/FaceNormal.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionFaceNormal->setIcon(icon19);
        actionNodeNormal = new QAction(MainWindow);
        actionNodeNormal->setObjectName(QString::fromUtf8("actionNodeNormal"));
        actionNodeNormal->setCheckable(true);
        QIcon icon20;
        icon20.addFile(QString::fromUtf8(":/resource/NodeNormal.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionNodeNormal->setIcon(icon20);
        actionSelectEdge = new QAction(MainWindow);
        actionSelectEdge->setObjectName(QString::fromUtf8("actionSelectEdge"));
        QIcon icon21;
        icon21.addFile(QString::fromUtf8(":/resource/selectEdge.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionSelectEdge->setIcon(icon21);
        actionGenerate = new QAction(MainWindow);
        actionGenerate->setObjectName(QString::fromUtf8("actionGenerate"));
        actionTest_1 = new QAction(MainWindow);
        actionTest_1->setObjectName(QString::fromUtf8("actionTest_1"));
        actionSelectFix = new QAction(MainWindow);
        actionSelectFix->setObjectName(QString::fromUtf8("actionSelectFix"));
        QIcon icon22;
        icon22.addFile(QString::fromUtf8(":/resource/selectFix.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionSelectFix->setIcon(icon22);
        actionSelectHandle = new QAction(MainWindow);
        actionSelectHandle->setObjectName(QString::fromUtf8("actionSelectHandle"));
        QIcon icon23;
        icon23.addFile(QString::fromUtf8(":/resource/selectHandle.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionSelectHandle->setIcon(icon23);
        actionSaveSelection = new QAction(MainWindow);
        actionSaveSelection->setObjectName(QString::fromUtf8("actionSaveSelection"));
        QIcon icon24;
        icon24.addFile(QString::fromUtf8(":/resource/SaveSelection.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionSaveSelection->setIcon(icon24);
        actionReadSelection = new QAction(MainWindow);
        actionReadSelection->setObjectName(QString::fromUtf8("actionReadSelection"));
        QIcon icon25;
        icon25.addFile(QString::fromUtf8(":/resource/InputSelection.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionReadSelection->setIcon(icon25);
        actionSelectChamber = new QAction(MainWindow);
        actionSelectChamber->setObjectName(QString::fromUtf8("actionSelectChamber"));
        actionExport_to_Abaqus_model = new QAction(MainWindow);
        actionExport_to_Abaqus_model->setObjectName(QString::fromUtf8("actionExport_to_Abaqus_model"));
        actionExport_to_Abaqus_model->setCheckable(false);
        QIcon icon26;
        icon26.addFile(QString::fromUtf8(":/resource/abaqus logo.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionExport_to_Abaqus_model->setIcon(icon26);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        centralWidget->setMouseTracking(true);
        centralWidget->setAcceptDrops(true);
        horizontalLayout = new QHBoxLayout(centralWidget);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        MainWindow->setCentralWidget(centralWidget);
        navigationToolBar = new QToolBar(MainWindow);
        navigationToolBar->setObjectName(QString::fromUtf8("navigationToolBar"));
        navigationToolBar->setMovable(false);
        navigationToolBar->setIconSize(QSize(25, 25));
        navigationToolBar->setFloatable(false);
        MainWindow->addToolBar(Qt::TopToolBarArea, navigationToolBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        MainWindow->setStatusBar(statusBar);
        selectionToolBar = new QToolBar(MainWindow);
        selectionToolBar->setObjectName(QString::fromUtf8("selectionToolBar"));
        selectionToolBar->setMovable(false);
        selectionToolBar->setIconSize(QSize(25, 25));
        selectionToolBar->setFloatable(false);
        MainWindow->addToolBar(Qt::TopToolBarArea, selectionToolBar);
        dockWidget = new QDockWidget(MainWindow);
        dockWidget->setObjectName(QString::fromUtf8("dockWidget"));
        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Maximum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(dockWidget->sizePolicy().hasHeightForWidth());
        dockWidget->setSizePolicy(sizePolicy);
        dockWidget->setMinimumSize(QSize(350, 984));
        dockWidget->setMaximumSize(QSize(350, 524287));
        dockWidget->setStyleSheet(QString::fromUtf8(""));
        dockWidgetContents = new QWidget();
        dockWidgetContents->setObjectName(QString::fromUtf8("dockWidgetContents"));
        dockWidgetContents->setLayoutDirection(Qt::LeftToRight);
        verticalLayout = new QVBoxLayout(dockWidgetContents);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        label_MANY_3DP_CNC_CAM = new QLabel(dockWidgetContents);
        label_MANY_3DP_CNC_CAM->setObjectName(QString::fromUtf8("label_MANY_3DP_CNC_CAM"));
        QFont font1;
        font1.setPointSize(10);
        font1.setBold(true);
        font1.setWeight(75);
        label_MANY_3DP_CNC_CAM->setFont(font1);

        verticalLayout->addWidget(label_MANY_3DP_CNC_CAM);

        line = new QFrame(dockWidgetContents);
        line->setObjectName(QString::fromUtf8("line"));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);

        verticalLayout->addWidget(line);

        horizontalLayout_12 = new QHBoxLayout();
        horizontalLayout_12->setSpacing(6);
        horizontalLayout_12->setObjectName(QString::fromUtf8("horizontalLayout_12"));
        label = new QLabel(dockWidgetContents);
        label->setObjectName(QString::fromUtf8("label"));
        label->setFont(font);
        label->setStyleSheet(QString::fromUtf8("color: rgb(0, 0, 255);"));

        horizontalLayout_12->addWidget(label);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_12->addItem(horizontalSpacer);

        checkBox_Yup2Zup = new QCheckBox(dockWidgetContents);
        checkBox_Yup2Zup->setObjectName(QString::fromUtf8("checkBox_Yup2Zup"));
        checkBox_Yup2Zup->setChecked(false);

        horizontalLayout_12->addWidget(checkBox_Yup2Zup);


        verticalLayout->addLayout(horizontalLayout_12);

        horizontalLayout_11 = new QHBoxLayout();
        horizontalLayout_11->setSpacing(6);
        horizontalLayout_11->setObjectName(QString::fromUtf8("horizontalLayout_11"));
        label_Xmove = new QLabel(dockWidgetContents);
        label_Xmove->setObjectName(QString::fromUtf8("label_Xmove"));
        label_Xmove->setMaximumSize(QSize(16777215, 16777215));
        QFont font2;
        font2.setPointSize(8);
        label_Xmove->setFont(font2);

        horizontalLayout_11->addWidget(label_Xmove);

        doubleSpinBox_Xmove = new QDoubleSpinBox(dockWidgetContents);
        doubleSpinBox_Xmove->setObjectName(QString::fromUtf8("doubleSpinBox_Xmove"));
        doubleSpinBox_Xmove->setFont(font2);
        doubleSpinBox_Xmove->setMinimum(-150.000000000000000);
        doubleSpinBox_Xmove->setMaximum(150.000000000000000);
        doubleSpinBox_Xmove->setValue(0.000000000000000);

        horizontalLayout_11->addWidget(doubleSpinBox_Xmove);

        label_Ymove = new QLabel(dockWidgetContents);
        label_Ymove->setObjectName(QString::fromUtf8("label_Ymove"));
        label_Ymove->setMaximumSize(QSize(16777215, 16777215));
        label_Ymove->setFont(font2);

        horizontalLayout_11->addWidget(label_Ymove);

        doubleSpinBox_Ymove = new QDoubleSpinBox(dockWidgetContents);
        doubleSpinBox_Ymove->setObjectName(QString::fromUtf8("doubleSpinBox_Ymove"));
        doubleSpinBox_Ymove->setFont(font2);
        doubleSpinBox_Ymove->setMinimum(-150.000000000000000);
        doubleSpinBox_Ymove->setMaximum(150.000000000000000);
        doubleSpinBox_Ymove->setValue(0.000000000000000);

        horizontalLayout_11->addWidget(doubleSpinBox_Ymove);

        label_Zmove = new QLabel(dockWidgetContents);
        label_Zmove->setObjectName(QString::fromUtf8("label_Zmove"));
        label_Zmove->setMaximumSize(QSize(16777215, 16777215));
        label_Zmove->setFont(font2);

        horizontalLayout_11->addWidget(label_Zmove);

        doubleSpinBox_Zmove = new QDoubleSpinBox(dockWidgetContents);
        doubleSpinBox_Zmove->setObjectName(QString::fromUtf8("doubleSpinBox_Zmove"));
        doubleSpinBox_Zmove->setFont(font2);
        doubleSpinBox_Zmove->setValue(40.500000000000000);

        horizontalLayout_11->addWidget(doubleSpinBox_Zmove);


        verticalLayout->addLayout(horizontalLayout_11);

        horizontalLayout_16 = new QHBoxLayout();
        horizontalLayout_16->setSpacing(6);
        horizontalLayout_16->setObjectName(QString::fromUtf8("horizontalLayout_16"));
        label_PosNorFile = new QLabel(dockWidgetContents);
        label_PosNorFile->setObjectName(QString::fromUtf8("label_PosNorFile"));
        QFont font3;
        font3.setPointSize(8);
        font3.setBold(true);
        font3.setWeight(75);
        label_PosNorFile->setFont(font3);

        horizontalLayout_16->addWidget(label_PosNorFile);

        lineEdit_SorceDataDir = new QLineEdit(dockWidgetContents);
        lineEdit_SorceDataDir->setObjectName(QString::fromUtf8("lineEdit_SorceDataDir"));

        horizontalLayout_16->addWidget(lineEdit_SorceDataDir);

        pushButton_readGcodeSourceData = new QPushButton(dockWidgetContents);
        pushButton_readGcodeSourceData->setObjectName(QString::fromUtf8("pushButton_readGcodeSourceData"));
        pushButton_readGcodeSourceData->setMaximumSize(QSize(108, 16777215));
        QFont font4;
        font4.setPointSize(10);
        font4.setBold(true);
        font4.setItalic(false);
        font4.setUnderline(false);
        font4.setWeight(75);
        pushButton_readGcodeSourceData->setFont(font4);
        pushButton_readGcodeSourceData->setStyleSheet(QString::fromUtf8("color: rgb(0, 0, 0);"));

        horizontalLayout_16->addWidget(pushButton_readGcodeSourceData);


        verticalLayout->addLayout(horizontalLayout_16);

        label_WaypointDisplay = new QLabel(dockWidgetContents);
        label_WaypointDisplay->setObjectName(QString::fromUtf8("label_WaypointDisplay"));
        label_WaypointDisplay->setFont(font);
        label_WaypointDisplay->setStyleSheet(QString::fromUtf8("color: rgb(0, 0, 255);"));

        verticalLayout->addWidget(label_WaypointDisplay);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        label_2 = new QLabel(dockWidgetContents);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setFont(font);

        horizontalLayout_2->addWidget(label_2);

        label_4 = new QLabel(dockWidgetContents);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        QFont font5;
        font5.setBold(false);
        font5.setWeight(50);
        label_4->setFont(font5);

        horizontalLayout_2->addWidget(label_4);

        spinBox_StartLayerIndex = new QSpinBox(dockWidgetContents);
        spinBox_StartLayerIndex->setObjectName(QString::fromUtf8("spinBox_StartLayerIndex"));
        spinBox_StartLayerIndex->setFont(font5);

        horizontalLayout_2->addWidget(spinBox_StartLayerIndex);

        label_3 = new QLabel(dockWidgetContents);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setMaximumSize(QSize(20, 16777215));
        label_3->setFont(font5);

        horizontalLayout_2->addWidget(label_3);

        spinBox_ShowLayerIndex = new QSpinBox(dockWidgetContents);
        spinBox_ShowLayerIndex->setObjectName(QString::fromUtf8("spinBox_ShowLayerIndex"));
        spinBox_ShowLayerIndex->setFont(font5);

        horizontalLayout_2->addWidget(spinBox_ShowLayerIndex);

        checkBox_EachLayerSwitch = new QCheckBox(dockWidgetContents);
        checkBox_EachLayerSwitch->setObjectName(QString::fromUtf8("checkBox_EachLayerSwitch"));
        checkBox_EachLayerSwitch->setFont(font);

        horizontalLayout_2->addWidget(checkBox_EachLayerSwitch);


        verticalLayout->addLayout(horizontalLayout_2);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setSpacing(6);
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        label_currentFile = new QLabel(dockWidgetContents);
        label_currentFile->setObjectName(QString::fromUtf8("label_currentFile"));
        label_currentFile->setFont(font5);

        horizontalLayout_3->addWidget(label_currentFile);

        pushButton_ShowAllLayers = new QPushButton(dockWidgetContents);
        pushButton_ShowAllLayers->setObjectName(QString::fromUtf8("pushButton_ShowAllLayers"));

        horizontalLayout_3->addWidget(pushButton_ShowAllLayers);


        verticalLayout->addLayout(horizontalLayout_3);

        line_2 = new QFrame(dockWidgetContents);
        line_2->setObjectName(QString::fromUtf8("line_2"));
        line_2->setFrameShape(QFrame::HLine);
        line_2->setFrameShadow(QFrame::Sunken);

        verticalLayout->addWidget(line_2);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setSpacing(6);
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        label_6 = new QLabel(dockWidgetContents);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        label_6->setFont(font);

        horizontalLayout_4->addWidget(label_6);

        label_7 = new QLabel(dockWidgetContents);
        label_7->setObjectName(QString::fromUtf8("label_7"));
        label_7->setFont(font5);

        horizontalLayout_4->addWidget(label_7);

        spinBox_GcodeGeneFromIndex = new QSpinBox(dockWidgetContents);
        spinBox_GcodeGeneFromIndex->setObjectName(QString::fromUtf8("spinBox_GcodeGeneFromIndex"));
        spinBox_GcodeGeneFromIndex->setFont(font5);

        horizontalLayout_4->addWidget(spinBox_GcodeGeneFromIndex);

        label_8 = new QLabel(dockWidgetContents);
        label_8->setObjectName(QString::fromUtf8("label_8"));
        label_8->setMaximumSize(QSize(20, 16777215));
        label_8->setFont(font5);

        horizontalLayout_4->addWidget(label_8);

        spinBox_GcodeGeneToIndex = new QSpinBox(dockWidgetContents);
        spinBox_GcodeGeneToIndex->setObjectName(QString::fromUtf8("spinBox_GcodeGeneToIndex"));
        spinBox_GcodeGeneToIndex->setFont(font5);

        horizontalLayout_4->addWidget(spinBox_GcodeGeneToIndex);


        verticalLayout->addLayout(horizontalLayout_4);

        line_4 = new QFrame(dockWidgetContents);
        line_4->setObjectName(QString::fromUtf8("line_4"));
        line_4->setFrameShape(QFrame::HLine);
        line_4->setFrameShadow(QFrame::Sunken);

        verticalLayout->addWidget(line_4);

        label_14 = new QLabel(dockWidgetContents);
        label_14->setObjectName(QString::fromUtf8("label_14"));
        QFont font6;
        font6.setUnderline(true);
        label_14->setFont(font6);

        verticalLayout->addWidget(label_14);

        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setSpacing(6);
        horizontalLayout_6->setObjectName(QString::fromUtf8("horizontalLayout_6"));
        label_9 = new QLabel(dockWidgetContents);
        label_9->setObjectName(QString::fromUtf8("label_9"));

        horizontalLayout_6->addWidget(label_9);

        label_10 = new QLabel(dockWidgetContents);
        label_10->setObjectName(QString::fromUtf8("label_10"));
        label_10->setMaximumSize(QSize(20, 16777215));
        label_10->setFont(font5);

        horizontalLayout_6->addWidget(label_10);

        doubleSpinBox_E3XOFF = new QDoubleSpinBox(dockWidgetContents);
        doubleSpinBox_E3XOFF->setObjectName(QString::fromUtf8("doubleSpinBox_E3XOFF"));
        doubleSpinBox_E3XOFF->setFont(font5);
        doubleSpinBox_E3XOFF->setDecimals(1);
        doubleSpinBox_E3XOFF->setMinimum(-10.000000000000000);
        doubleSpinBox_E3XOFF->setMaximum(10.000000000000000);
        doubleSpinBox_E3XOFF->setValue(-3.500000000000000);

        horizontalLayout_6->addWidget(doubleSpinBox_E3XOFF);

        label_11 = new QLabel(dockWidgetContents);
        label_11->setObjectName(QString::fromUtf8("label_11"));
        label_11->setMaximumSize(QSize(20, 16777215));
        label_11->setFont(font5);

        horizontalLayout_6->addWidget(label_11);

        doubleSpinBox_E3YOFF = new QDoubleSpinBox(dockWidgetContents);
        doubleSpinBox_E3YOFF->setObjectName(QString::fromUtf8("doubleSpinBox_E3YOFF"));
        doubleSpinBox_E3YOFF->setFont(font5);
        doubleSpinBox_E3YOFF->setDecimals(1);
        doubleSpinBox_E3YOFF->setMinimum(-10.000000000000000);
        doubleSpinBox_E3YOFF->setMaximum(10.000000000000000);

        horizontalLayout_6->addWidget(doubleSpinBox_E3YOFF);

        label_12 = new QLabel(dockWidgetContents);
        label_12->setObjectName(QString::fromUtf8("label_12"));
        label_12->setFont(font);

        horizontalLayout_6->addWidget(label_12);

        doubleSpinBox_lambda = new QDoubleSpinBox(dockWidgetContents);
        doubleSpinBox_lambda->setObjectName(QString::fromUtf8("doubleSpinBox_lambda"));
        doubleSpinBox_lambda->setFont(font5);
        doubleSpinBox_lambda->setDecimals(1);
        doubleSpinBox_lambda->setMaximum(15.000000000000000);
        doubleSpinBox_lambda->setValue(6.000000000000000);

        horizontalLayout_6->addWidget(doubleSpinBox_lambda);


        verticalLayout->addLayout(horizontalLayout_6);

        label_5 = new QLabel(dockWidgetContents);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setFont(font);
        label_5->setStyleSheet(QString::fromUtf8("color: rgb(0, 0, 255);"));

        verticalLayout->addWidget(label_5);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setSpacing(6);
        horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
        checkBox_varyDistance = new QCheckBox(dockWidgetContents);
        checkBox_varyDistance->setObjectName(QString::fromUtf8("checkBox_varyDistance"));
        checkBox_varyDistance->setFont(font);
        checkBox_varyDistance->setChecked(true);

        horizontalLayout_5->addWidget(checkBox_varyDistance);

        checkBox_varyHeight = new QCheckBox(dockWidgetContents);
        checkBox_varyHeight->setObjectName(QString::fromUtf8("checkBox_varyHeight"));
        checkBox_varyHeight->setFont(font);
        checkBox_varyHeight->setChecked(true);

        horizontalLayout_5->addWidget(checkBox_varyHeight);

        checkBox_varyWidth = new QCheckBox(dockWidgetContents);
        checkBox_varyWidth->setObjectName(QString::fromUtf8("checkBox_varyWidth"));
        checkBox_varyWidth->setFont(font);

        horizontalLayout_5->addWidget(checkBox_varyWidth);

        checkBox_TestDHW_Switch = new QCheckBox(dockWidgetContents);
        checkBox_TestDHW_Switch->setObjectName(QString::fromUtf8("checkBox_TestDHW_Switch"));
        checkBox_TestDHW_Switch->setFont(font);

        horizontalLayout_5->addWidget(checkBox_TestDHW_Switch);


        verticalLayout->addLayout(horizontalLayout_5);

        pushButton_calDWH = new QPushButton(dockWidgetContents);
        pushButton_calDWH->setObjectName(QString::fromUtf8("pushButton_calDWH"));
        pushButton_calDWH->setMinimumSize(QSize(90, 0));
        pushButton_calDWH->setMaximumSize(QSize(16777215, 16777215));
        QFont font7;
        font7.setBold(true);
        font7.setUnderline(false);
        font7.setWeight(75);
        pushButton_calDWH->setFont(font7);
        pushButton_calDWH->setStyleSheet(QString::fromUtf8("color: rgb(0, 0, 80);"));

        verticalLayout->addWidget(pushButton_calDWH);

        line_3 = new QFrame(dockWidgetContents);
        line_3->setObjectName(QString::fromUtf8("line_3"));
        line_3->setFrameShape(QFrame::HLine);
        line_3->setFrameShadow(QFrame::Sunken);

        verticalLayout->addWidget(line_3);

        horizontalLayout_7 = new QHBoxLayout();
        horizontalLayout_7->setSpacing(6);
        horizontalLayout_7->setObjectName(QString::fromUtf8("horizontalLayout_7"));
        pushButton_calSingularOpt = new QPushButton(dockWidgetContents);
        pushButton_calSingularOpt->setObjectName(QString::fromUtf8("pushButton_calSingularOpt"));
        pushButton_calSingularOpt->setMinimumSize(QSize(90, 0));
        pushButton_calSingularOpt->setMaximumSize(QSize(16777215, 16777215));

        horizontalLayout_7->addWidget(pushButton_calSingularOpt);


        verticalLayout->addLayout(horizontalLayout_7);

        horizontalLayout_8 = new QHBoxLayout();
        horizontalLayout_8->setSpacing(6);
        horizontalLayout_8->setObjectName(QString::fromUtf8("horizontalLayout_8"));
        checkBox_showSingularityNode = new QCheckBox(dockWidgetContents);
        checkBox_showSingularityNode->setObjectName(QString::fromUtf8("checkBox_showSingularityNode"));
        checkBox_showSingularityNode->setFont(font5);

        horizontalLayout_8->addWidget(checkBox_showSingularityNode);

        checkBox_showSolveSelection = new QCheckBox(dockWidgetContents);
        checkBox_showSolveSelection->setObjectName(QString::fromUtf8("checkBox_showSolveSelection"));
        checkBox_showSolveSelection->setFont(font5);

        horizontalLayout_8->addWidget(checkBox_showSolveSelection);


        verticalLayout->addLayout(horizontalLayout_8);

        horizontalLayout_9 = new QHBoxLayout();
        horizontalLayout_9->setSpacing(6);
        horizontalLayout_9->setObjectName(QString::fromUtf8("horizontalLayout_9"));
        pushButton_calCollision = new QPushButton(dockWidgetContents);
        pushButton_calCollision->setObjectName(QString::fromUtf8("pushButton_calCollision"));
        pushButton_calCollision->setMinimumSize(QSize(90, 0));
        pushButton_calCollision->setMaximumSize(QSize(16777215, 16777215));

        horizontalLayout_9->addWidget(pushButton_calCollision);


        verticalLayout->addLayout(horizontalLayout_9);

        horizontalLayout_10 = new QHBoxLayout();
        horizontalLayout_10->setSpacing(6);
        horizontalLayout_10->setObjectName(QString::fromUtf8("horizontalLayout_10"));
        pushButton_calCollisionElimination = new QPushButton(dockWidgetContents);
        pushButton_calCollisionElimination->setObjectName(QString::fromUtf8("pushButton_calCollisionElimination"));
        pushButton_calCollisionElimination->setMinimumSize(QSize(90, 0));
        pushButton_calCollisionElimination->setMaximumSize(QSize(16777215, 16777215));

        horizontalLayout_10->addWidget(pushButton_calCollisionElimination);


        verticalLayout->addLayout(horizontalLayout_10);

        pushButton_calCollision_continuous = new QPushButton(dockWidgetContents);
        pushButton_calCollision_continuous->setObjectName(QString::fromUtf8("pushButton_calCollision_continuous"));

        verticalLayout->addWidget(pushButton_calCollision_continuous);

        horizontalLayout_13 = new QHBoxLayout();
        horizontalLayout_13->setSpacing(6);
        horizontalLayout_13->setObjectName(QString::fromUtf8("horizontalLayout_13"));
        pushButton_Gcode_writting = new QPushButton(dockWidgetContents);
        pushButton_Gcode_writting->setObjectName(QString::fromUtf8("pushButton_Gcode_writting"));
        pushButton_Gcode_writting->setMinimumSize(QSize(90, 0));

        horizontalLayout_13->addWidget(pushButton_Gcode_writting);


        verticalLayout->addLayout(horizontalLayout_13);

        horizontalLayout_14 = new QHBoxLayout();
        horizontalLayout_14->setSpacing(6);
        horizontalLayout_14->setObjectName(QString::fromUtf8("horizontalLayout_14"));
        pushButton_GcodeSimulation = new QPushButton(dockWidgetContents);
        pushButton_GcodeSimulation->setObjectName(QString::fromUtf8("pushButton_GcodeSimulation"));

        horizontalLayout_14->addWidget(pushButton_GcodeSimulation);

        progressBar_GcodeSimulation = new QProgressBar(dockWidgetContents);
        progressBar_GcodeSimulation->setObjectName(QString::fromUtf8("progressBar_GcodeSimulation"));
        progressBar_GcodeSimulation->setValue(0);

        horizontalLayout_14->addWidget(progressBar_GcodeSimulation);

        checkBox_stopSimulation = new QCheckBox(dockWidgetContents);
        checkBox_stopSimulation->setObjectName(QString::fromUtf8("checkBox_stopSimulation"));

        horizontalLayout_14->addWidget(checkBox_stopSimulation);


        verticalLayout->addLayout(horizontalLayout_14);

        treeView = new QTreeView(dockWidgetContents);
        treeView->setObjectName(QString::fromUtf8("treeView"));
        treeView->setEnabled(true);
        treeView->setProperty("showDropIndicator", QVariant(true));
        treeView->setIndentation(5);
        treeView->header()->setVisible(false);

        verticalLayout->addWidget(treeView);

        pushButton_test = new QPushButton(dockWidgetContents);
        pushButton_test->setObjectName(QString::fromUtf8("pushButton_test"));

        verticalLayout->addWidget(pushButton_test);

        pushButton_clearAll = new QPushButton(dockWidgetContents);
        pushButton_clearAll->setObjectName(QString::fromUtf8("pushButton_clearAll"));

        verticalLayout->addWidget(pushButton_clearAll);

        dockWidget->setWidget(dockWidgetContents);
        MainWindow->addDockWidget(static_cast<Qt::DockWidgetArea>(2), dockWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1325, 18));
        menuBar->setLayoutDirection(Qt::LeftToRight);
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName(QString::fromUtf8("menuFile"));
        menuView = new QMenu(menuBar);
        menuView->setObjectName(QString::fromUtf8("menuView"));
        menuSelect = new QMenu(menuBar);
        menuSelect->setObjectName(QString::fromUtf8("menuSelect"));
        MainWindow->setMenuBar(menuBar);
        toolBar = new QToolBar(MainWindow);
        toolBar->setObjectName(QString::fromUtf8("toolBar"));
        toolBar->setMovable(false);
        toolBar->setFloatable(false);
        MainWindow->addToolBar(Qt::TopToolBarArea, toolBar);

        navigationToolBar->addAction(actionFront);
        navigationToolBar->addAction(actionBack);
        navigationToolBar->addAction(actionTop);
        navigationToolBar->addAction(actionBottom);
        navigationToolBar->addAction(actionLeft);
        navigationToolBar->addAction(actionRight);
        navigationToolBar->addAction(actionIsometric);
        navigationToolBar->addSeparator();
        navigationToolBar->addAction(actionZoom_In);
        navigationToolBar->addAction(actionZoom_Out);
        navigationToolBar->addAction(actionZoom_All);
        navigationToolBar->addAction(actionZoom_Window);
        navigationToolBar->addSeparator();
        navigationToolBar->addAction(actionShade);
        navigationToolBar->addAction(actionMesh);
        navigationToolBar->addAction(actionNode);
        navigationToolBar->addAction(actionProfile);
        navigationToolBar->addAction(actionFaceNormal);
        navigationToolBar->addAction(actionNodeNormal);
        selectionToolBar->addAction(actionSaveSelection);
        selectionToolBar->addAction(actionReadSelection);
        selectionToolBar->addSeparator();
        selectionToolBar->addAction(actionSelectNode);
        selectionToolBar->addAction(actionSelectEdge);
        selectionToolBar->addAction(actionSelectFace);
        selectionToolBar->addAction(actionSelectFix);
        selectionToolBar->addAction(actionSelectHandle);
        menuBar->addAction(menuFile->menuAction());
        menuBar->addAction(menuView->menuAction());
        menuBar->addAction(menuSelect->menuAction());
        menuFile->addAction(actionOpen);
        menuFile->addAction(actionSave);
        menuFile->addAction(actionSaveSelection);
        menuFile->addAction(actionReadSelection);
        menuView->addAction(actionFront);
        menuView->addAction(actionBack);
        menuView->addAction(actionTop);
        menuView->addAction(actionBottom);
        menuView->addAction(actionLeft);
        menuView->addAction(actionRight);
        menuView->addAction(actionIsometric);
        menuView->addSeparator();
        menuView->addAction(actionZoom_In);
        menuView->addAction(actionZoom_Out);
        menuView->addAction(actionZoom_All);
        menuView->addAction(actionZoom_Window);
        menuView->addSeparator();
        menuView->addAction(actionShade);
        menuView->addAction(actionMesh);
        menuView->addAction(actionNode);
        menuView->addAction(actionProfile);
        menuView->addSeparator();
        menuView->addAction(actionShifttoOrigin);
        menuSelect->addAction(actionSelectNode);
        menuSelect->addAction(actionSelectEdge);
        menuSelect->addAction(actionSelectFace);
        menuSelect->addSeparator();
        menuSelect->addAction(actionSelectFix);
        menuSelect->addAction(actionSelectHandle);
        menuSelect->addSeparator();
        toolBar->addAction(actionOpen);
        toolBar->addAction(actionSave);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", nullptr));
        actionOpen->setText(QApplication::translate("MainWindow", "Open", nullptr));
        actionFront->setText(QApplication::translate("MainWindow", "Front", nullptr));
        actionBack->setText(QApplication::translate("MainWindow", "Back", nullptr));
        actionTop->setText(QApplication::translate("MainWindow", "Top", nullptr));
        actionBottom->setText(QApplication::translate("MainWindow", "Bottom", nullptr));
        actionLeft->setText(QApplication::translate("MainWindow", "Left", nullptr));
        actionRight->setText(QApplication::translate("MainWindow", "Right", nullptr));
        actionIsometric->setText(QApplication::translate("MainWindow", "Isometric", nullptr));
        actionZoom_In->setText(QApplication::translate("MainWindow", "Zoom In", nullptr));
        actionZoom_Out->setText(QApplication::translate("MainWindow", "Zoom Out", nullptr));
        actionZoom_All->setText(QApplication::translate("MainWindow", "Zoom All", nullptr));
        actionZoom_Window->setText(QApplication::translate("MainWindow", "Zoom Window", nullptr));
        actionShade->setText(QApplication::translate("MainWindow", "Shade", nullptr));
        actionMesh->setText(QApplication::translate("MainWindow", "Mesh", nullptr));
        actionNode->setText(QApplication::translate("MainWindow", "Node", nullptr));
        actionSave->setText(QApplication::translate("MainWindow", "Save", nullptr));
        actionSelectNode->setText(QApplication::translate("MainWindow", "Node", nullptr));
        actionSelectFace->setText(QApplication::translate("MainWindow", "Face", nullptr));
        actionShifttoOrigin->setText(QApplication::translate("MainWindow", "Shift to Origin", nullptr));
        actionProfile->setText(QApplication::translate("MainWindow", "Profile", nullptr));
        actionFaceNormal->setText(QApplication::translate("MainWindow", "FaceNormal", nullptr));
        actionNodeNormal->setText(QApplication::translate("MainWindow", "NodeNormal", nullptr));
        actionSelectEdge->setText(QApplication::translate("MainWindow", "Edge", nullptr));
        actionGenerate->setText(QApplication::translate("MainWindow", "Generate", nullptr));
        actionTest_1->setText(QApplication::translate("MainWindow", "Test_1", nullptr));
        actionSelectFix->setText(QApplication::translate("MainWindow", "Fix", nullptr));
        actionSelectHandle->setText(QApplication::translate("MainWindow", "Handle & Rigid", nullptr));
        actionSaveSelection->setText(QApplication::translate("MainWindow", "Save selection", nullptr));
        actionReadSelection->setText(QApplication::translate("MainWindow", "Read selection", nullptr));
        actionSelectChamber->setText(QApplication::translate("MainWindow", "Select Chamber (SORO)", nullptr));
        actionExport_to_Abaqus_model->setText(QApplication::translate("MainWindow", "Export to Abaqus model", nullptr));
        navigationToolBar->setWindowTitle(QApplication::translate("MainWindow", "navigationToolBar", nullptr));
        selectionToolBar->setWindowTitle(QApplication::translate("MainWindow", "selectionToolBar", nullptr));
        label_MANY_3DP_CNC_CAM->setText(QApplication::translate("MainWindow", "CASE2021 Singular-Aware Opt", nullptr));
        label->setText(QApplication::translate("MainWindow", "Coordinate:", nullptr));
        checkBox_Yup2Zup->setText(QApplication::translate("MainWindow", "Yup -> Zup ", nullptr));
        label_Xmove->setText(QApplication::translate("MainWindow", "X", nullptr));
        label_Ymove->setText(QApplication::translate("MainWindow", "Y", nullptr));
        label_Zmove->setText(QApplication::translate("MainWindow", "Z", nullptr));
        label_PosNorFile->setText(QApplication::translate("MainWindow", "File Dir:", nullptr));
        lineEdit_SorceDataDir->setText(QApplication::translate("MainWindow", "model_1", nullptr));
        pushButton_readGcodeSourceData->setText(QApplication::translate("MainWindow", "Read Data", nullptr));
        label_WaypointDisplay->setText(QApplication::translate("MainWindow", "Model Display:", nullptr));
        label_2->setText(QApplication::translate("MainWindow", "Show:", nullptr));
        label_4->setText(QApplication::translate("MainWindow", "From", nullptr));
        label_3->setText(QApplication::translate("MainWindow", "to", nullptr));
        checkBox_EachLayerSwitch->setText(QApplication::translate("MainWindow", "each", nullptr));
        label_currentFile->setText(QApplication::translate("MainWindow", "Current Layer", nullptr));
        pushButton_ShowAllLayers->setText(QApplication::translate("MainWindow", "Show ALL Layer", nullptr));
        label_6->setText(QApplication::translate("MainWindow", "Opt Computation:", nullptr));
        label_7->setText(QApplication::translate("MainWindow", "From", nullptr));
        label_8->setText(QApplication::translate("MainWindow", "to", nullptr));
        label_14->setText(QApplication::translate("MainWindow", "Key parameter", nullptr));
        label_9->setText(QApplication::translate("MainWindow", "Offset:", nullptr));
        label_10->setText(QApplication::translate("MainWindow", "X", nullptr));
        label_11->setText(QApplication::translate("MainWindow", "Y", nullptr));
        label_12->setText(QApplication::translate("MainWindow", "lamda", nullptr));
        label_5->setText(QApplication::translate("MainWindow", "Filament Volume:", nullptr));
        checkBox_varyDistance->setText(QApplication::translate("MainWindow", "D", nullptr));
        checkBox_varyHeight->setText(QApplication::translate("MainWindow", "H", nullptr));
        checkBox_varyWidth->setText(QApplication::translate("MainWindow", "W", nullptr));
        checkBox_TestDHW_Switch->setText(QApplication::translate("MainWindow", "Test", nullptr));
        pushButton_calDWH->setText(QApplication::translate("MainWindow", "1.Variable Filament Calculation", nullptr));
        pushButton_calSingularOpt->setText(QApplication::translate("MainWindow", "2.Singularity Optimization", nullptr));
        checkBox_showSingularityNode->setText(QApplication::translate("MainWindow", "Singular Node", nullptr));
        checkBox_showSolveSelection->setText(QApplication::translate("MainWindow", "Solve Selection", nullptr));
        pushButton_calCollision->setText(QApplication::translate("MainWindow", "3.Collision Check", nullptr));
        pushButton_calCollisionElimination->setText(QApplication::translate("MainWindow", "4.Collision Elimination", nullptr));
        pushButton_calCollision_continuous->setText(QApplication::translate("MainWindow", "Continuous Collision Checking", nullptr));
        pushButton_Gcode_writting->setText(QApplication::translate("MainWindow", "5.G Code Writting ", nullptr));
        pushButton_GcodeSimulation->setText(QApplication::translate("MainWindow", "Simulation", nullptr));
        checkBox_stopSimulation->setText(QApplication::translate("MainWindow", "Stop", nullptr));
        pushButton_test->setText(QApplication::translate("MainWindow", "PushButton_test", nullptr));
        pushButton_clearAll->setText(QApplication::translate("MainWindow", "Clear All", nullptr));
        menuFile->setTitle(QApplication::translate("MainWindow", "File", nullptr));
        menuView->setTitle(QApplication::translate("MainWindow", "View", nullptr));
        menuSelect->setTitle(QApplication::translate("MainWindow", "Select", nullptr));
        toolBar->setWindowTitle(QApplication::translate("MainWindow", "toolBar", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
