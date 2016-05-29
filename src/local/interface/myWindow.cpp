#include "myWindow.hpp"


#include "myWidgetGL.hpp"
#include "../../lib/common/error_handling.hpp"
#include "ui_mainwindow.h"
#include "../scene/scene.hpp"
#include <iostream>

myWindow::myWindow(QWidget *parent)
    :QMainWindow(parent),ui(new Ui::MainWindow)
{
    try
    {
        //Setup window layout
        ui->setupUi(this);

        //Create openGL context
        QGLFormat qglFormat;
        qglFormat.setVersion(1,2);

        //Create OpenGL Widget renderer
        glWidget=new myWidgetGL(qglFormat);

        //Add the OpenGL Widget into the layout
        ui->layout_scene->addWidget(glWidget);
    }
    catch(cpe::exception_cpe const& e)
    {
        std::cout<<std::endl<<e.report_exception()<<std::endl;
    }

    //Connect slot and signals
    connect(ui->quit,SIGNAL(clicked()),this,SLOT(action_quit()));
    connect(ui->draw,SIGNAL(clicked()),this,SLOT(action_draw()));
    connect(ui->wireframe,SIGNAL(clicked()),this,SLOT(action_wireframe()));
    connect(ui->saveFace,SIGNAL(clicked()),this,SLOT(action_saveFace()));
    connect(ui->startHandtrack,SIGNAL(clicked()),this,SLOT(action_start_handtrack()));
    connect(ui->showFaceTrack,SIGNAL(clicked()),SLOT(action_faceTrack()));
    connect(ui->showHandTrack,SIGNAL(clicked()),this,SLOT(action_handTrack()));
    //sc = new scene();

}

myWindow::~myWindow()
{
   /* if (sc!=NULL)

        delete sc;*/
}

void myWindow::action_quit()
{
    close();
    cv::destroyAllWindows();
}

void myWindow::action_draw()
{
    glWidget->change_draw_state();
}

void myWindow::action_wireframe()
{
    bool const state_wireframe=ui->wireframe->isChecked();
    glWidget->wireframe(state_wireframe);
}

void myWindow::action_saveFace()
{
    //bool const doSave = ui->saveFace->;
    glWidget->sauvegardeImage(true);
    // sc->saveYourFace(true);
   // scene.saveYourFace(doSave);

}

void myWindow::action_start_handtrack()
{
    //handWindow.show();
    glWidget->start_Handtrack();
}

void myWindow::action_faceTrack()
{

}

void myWindow::action_handTrack()
{
    bool const state_display_handtrack = ui->showHandTrack->isChecked();
    glWidget->DisplayHandtrack(state_display_handtrack);
}
