// Ressource inspirée de https://www.raywenderlich.com/48297/export-blender-models-opengl-es-part-2 et de DR : mesh_io_obj.cpp

#include <GL/glew.h>

#include "scene.hpp"
#include "../../lib/opengl/glutils.hpp"
#include "../../lib/perlin/perlin.hpp"
#include "../../lib/interface/camera_matrices.hpp"
#include "../interface/myWidgetGL.hpp"
#include <cmath>
#include "faceTrack.hpp"
#include <string>
#include <sstream>
#include "../../lib/mesh/mesh_io.hpp"
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/objdetect/objdetect.hpp>



using namespace cpe;



void scene::load_scene()
{
    load_common_data();
    start_webcam();

    //get webcam dimension
    int width=0;
    int height=0;
    get_webcam_size(width,height);
    float const ratio = static_cast<float>(height)/width;


    //*****************************************//
    // Cube with webcam texture                //
    //*****************************************//

    mesh_cube = load_mesh_file("data/cube.obj");
    mesh_cube.transform_apply_translation({-0.5f, 0.0f,-0.5f});
    mesh_cube.transform_apply_scale(1.0f,ratio,1.0f);

    mesh_cube.fill_empty_field_by_default();
    mesh_cube.fill_color(vec3(1.0,1.0,1.0));

    mesh_cube_opengl.fill_vbo(mesh_cube);

    //*****************************************//
    // Sphere with webcam texture              //
    //*****************************************//
    scene::generate_avatar_head();

    //*****************************************//
    // AVATAR                                  //
    //*****************************************//
    texture_avatar_body = load_texture_file("data/Megaman/texture.png");
    mesh_avatar_body = load_mesh_file("data/Megaman/Megaman_body_3.obj");

    mesh_avatar_body.transform_apply_scale(0.2f,0.2f,0.2f);
    mesh_avatar_body_opengl.fill_vbo(mesh_avatar_body);

    texture_avatar_head = load_texture_file("data/Megaman/gatoune.jpg");
    mesh_avatar_head = load_mesh_file("data/Megaman/Megaman_head5.obj");
    mesh_avatar_head.transform_apply_scale(0.2f,0.2f,0.2f);
    mesh_avatar_head_opengl.fill_vbo(mesh_avatar_head);



    //*****************************************//
    // Ground                                  //
    //*****************************************//

    mesh_ground.add_vertex(vec3(-2.0f, 0.0f,-2.0f));
    mesh_ground.add_vertex(vec3(-2.0f, 0.0f, 2.0f));
    mesh_ground.add_vertex(vec3( 2.0f, 0.0f, 2.0f));
    mesh_ground.add_vertex(vec3( 2.0f, 0.0f,-2.0f));

    mesh_ground.add_texture_coord({0.0f,0.0f});
    mesh_ground.add_texture_coord({0.0f,1.0f});
    mesh_ground.add_texture_coord({1.0f,1.0f});
    mesh_ground.add_texture_coord({1.0f,0.0f});


    mesh_ground.add_triangle_index({0,2,1});
    mesh_ground.add_triangle_index({0,3,2});

    mesh_ground.fill_empty_field_by_default();
    mesh_ground.fill_color(vec3(0.7,0.9,0.6));

    mesh_ground_opengl.fill_vbo(mesh_ground);


    //******************************************//
    // Hand                                     //
    //******************************************//

    mesh_hand = load_mesh_file("data/hand.obj");
    mesh_hand.transform_apply_scale(0.2f,0.2f,0.2f);
    mesh_hand.fill_empty_field_by_default();
    mesh_hand.fill_color(cpe::vec3(1.0,1.0,1.0));


}



void scene::draw_scene()
{
    prepare_shader(shader_mesh);
    glBindTexture(GL_TEXTURE_2D,texture_default);  PRINT_OPENGL_ERROR();
    mesh_ground_opengl.draw();


    prepare_shader(shader_cube);
    glUniform1f(get_uni_loc(shader_cube,"time"),time_advance);

    //********* Get webcam image ***********//
    cv::Mat frame;
    capture >> frame;
    nbVisage = analyse_image(frame);

    //*********** Generate OpenGL texture from webcam image************//
    GLuint texture_webcam = generate_texture_webcam(frame); //NB : on s'en sert pas pour le moment

    //************* Draw the avatar body ***************//
    glBindTexture(GL_TEXTURE_2D,texture_avatar_body); PRINT_OPENGL_ERROR();
    mesh_avatar_body_opengl.draw();

    //********** Draw the avatar head ***********//
    if(getSaveYourFace() == true)//save f
    {
        if(aFaceIsAlreadySaved)
        { // Delete the texture on the GPU si il y en avait déja une
            glDeleteTextures(1,&texture_webcamROI);
        }
        saveYourFace(true);//Sauvegarde de la texture
        aFaceIsAlreadySaved = true;
    }

    if(texture_webcamROI != NULL) //Affichage de la texture sauvegardée
    {
        glBindTexture(GL_TEXTURE_2D,texture_webcamROI); PRINT_OPENGL_ERROR();
        mesh_avatar_head_opengl.draw();
    }

    //**************** Initialisation de la détection et du suivi de main **************//
    if(initHand == true)
    {
        startHandtrack(frame);
    }
    //Si la detection de main à été initilialisé on effectue son suivi
    if(cpt != 0)
    {
        hand1.Detect_Hand(capture);
    }

    // ********** Affichage d'une fenetre avec le visga trackée (lorsqu'il y en a une seule, pour deux peut etre probème, en tout cas indeterminé) à améliorer avec prise en compte des vecteurs peut etre
    if(displayFace)
    {
        cv::imshow("facetrack",faceROI);
    }
    if(no_displayFace)
    {
        cv::destroyWindow("facetrack");
        no_displayFace = false;
    }

    //********** Déplacement de la main, si la detection de main n'est pas initialisé, pas de mouvement ************//
    move_hand();


    glDeleteTextures(1, &texture_webcam);  PRINT_OPENGL_ERROR(); //NB : elle existe mais on s'en sert pas pour le moemnt
}


void scene::saveYourFace(const bool valid)
{
    //Get webcam image
    cv::Mat frame;
    capture >> frame;

    // int nbVisage = analyse_image(frame);
    if(valid == true)
    {
        if(nbVisage==0)
        {
            cv::putText(frame,"Pas de visage à sauvegarder",cv::Point(40,40),1,1,cv::Scalar(255,0,0)); //marche pas dans le contexte oGL

            if(texture_webcamROI != NULL)
                glDeleteTextures(1,&texture_webcamROI);
        }
        if(nbVisage==1)
        {
            cv::flip(faceROI,faceROI,0);
            texture_webcamROI = generate_texture_webcam(faceROI);
            this->savef == true;

            setSaveYourFace(false);
        }
    }

}

void scene::startHandtrack(cv::Mat frame)
{
    hand1.Col_split_begin = 0;
    hand1.Col_split_end = frame.rows;
    hand1.setname_img("Hand 1");
    hand1.setname_trackbar("Trackbars hand 1");
    hand1.initHandTrack(capture);
    hand1.init_Trackbars();
    hand1.average(capture);
    cpt = 1;
    initHand = false;
}

void scene::move_hand()
{
    int width=0;
    int height=0;
    get_webcam_size(width,height);

    if(cpt == 1){
        PosInit.x = width/2;
        PosInit.y = height/2;
        cpt++;
    }

    //On recupere le rectangle correspondant à la main :
    cv::Point PosCourante;
    HandGesture hg = hand1.gethg();

    //Si on a une main on récupère la position
    if(hg.isHand == 1){
        PosCourante.x = hg.bRect.tl().x + hg.bRect.width/2.0;
        PosCourante.y = hg.bRect.tl().y + hg.bRect.height/2.0;
    }

    delta.x = PosCourante.x - PosInit.x;
    delta.y = PosCourante.y - PosInit.y;


    for(int k = 0; k < mesh_hand.size_vertex(); k++)
    {
        //Déplacement du dinosaure dans l'espace
        mesh_hand.vertex(k).y() = mesh_hand.vertex(k).y() - delta.y/200.0;
        mesh_hand.vertex(k).x() = mesh_hand.vertex(k).x() - delta.x/200.0;
    }

    PosInit = PosCourante;

    mesh_hand_opengl.fill_vbo(mesh_hand);
    glBindTexture(GL_TEXTURE_2D,texture_default);  PRINT_OPENGL_ERROR();
    mesh_hand_opengl.draw();
}

void scene::generate_avatar_head()
{
    float l=0.05f;
    float h = 0.2f;
    float L=0.1f;
    //vertex

    mesh_head.add_vertex(vec3(-L,0.0f,-l)); //0
    mesh_head.add_vertex(vec3(-L,0.0f,l));//1
    mesh_head.add_vertex(vec3(L,0.0f,l));//2
    mesh_head.add_vertex(vec3(L,0.0f,-l));//3
    mesh_head.add_vertex(vec3(-L,h,-l));//4
    mesh_head.add_vertex(vec3(-L,h,l));//5
    mesh_head.add_vertex(vec3(L,h,l));//6
    mesh_head.add_vertex(vec3(L,h,-l));//7


    mesh_head.add_texture_coord({0.0f, 0.0f});
    mesh_head.add_texture_coord({0.0f,1.0f});
    mesh_head.add_texture_coord({1.0f,1.0f});
    mesh_head.add_texture_coord({1.0f,0.0f});

    mesh_head.add_texture_coord({0.0f, 0.0f});
    mesh_head.add_texture_coord({0.0f,1.0f});
    mesh_head.add_texture_coord({1.0f,1.0f});
    mesh_head.add_texture_coord({1.0f,0.0f});


    //triangle index
    mesh_head.add_triangle_index({0,1,3});
    mesh_head.add_triangle_index({3,1,2});
    mesh_head.add_triangle_index({3,2,7});
    mesh_head.add_triangle_index({7,2,6});
    mesh_head.add_triangle_index({7,5,4});
    mesh_head.add_triangle_index({6,5,7});
    mesh_head.add_triangle_index({6,5,1});
    mesh_head.add_triangle_index({6,1,2});
    mesh_head.add_triangle_index({7,0,3});
    mesh_head.add_triangle_index({7,4,0});
    mesh_head.add_triangle_index({1,4,5});
    mesh_head.add_triangle_index({1,0,4});

    mesh_head.fill_empty_field_by_default();
    mesh_head.fill_color(vec3(0.7,0.9,0.6));


    mesh_head.transform_apply_rotation({0.0f,0.0f,0.0f},90*M_PI/180);
    mesh_head.transform_apply_translation({0.0f,1.0f,-0.0f});
    mesh_head_opengl.fill_vbo(mesh_head);


}

void scene::start_webcam()
{
    //NB : initialisation de capture sur la Webcam dans le constructeur de scene pour plus de fléxibilité durant le développement
    if(!capture.isOpened())
    {
        std::cerr<<"Failed to open Camera"<<std::endl;
        exit(1);
    }
}

void scene::load_common_data()
{
    texture_default=load_texture_file("data/white.jpg");

    shader_mesh = read_shader("shaders/shader_mesh.vert",
                              "shaders/shader_mesh.frag"); PRINT_OPENGL_ERROR();

    shader_cube = read_shader("shaders/shader_cube.vert",
                              "shaders/shader_cube.frag"); PRINT_OPENGL_ERROR();
    haar_cascade.load("haarcascade_frontalface_alt.xml");
}



// Analyse pour faceTrack
int scene::analyse_image(cv::Mat& frame)
{

    cv::pyrDown(frame,frame);
    cv::cvtColor( frame, frame_gray, CV_BGR2GRAY );
    cv::equalizeHist( frame_gray, frame_gray );

    int i;

    haar_cascade.detectMultiScale(frame_gray,faces,1.1,3,0|CV_HAAR_SCALE_IMAGE);

    for(i= 0 ; i<faces.size(); ++i)
    {
        if(faces[i].x >0 && faces[i].x + faces[i].width < frame.cols && faces[i].y + faces[i].height < frame.rows)
        {
            cv::Point center(faces[i].x + faces[i].width*0.5, faces[i].y + faces[i].height*0.5);
            cv::ellipse(frame, center, cv::Size( faces[i].width*0.5, faces[i].height*0.5), 0, 0, 360, cv::Scalar( 255, 0, 255 ), 4, 8, 0);
            //cv::rectangle(frame, cv::Point(faces[i].x, faces[i].y), cv::Point(faces[i].x +faces[i].width, faces[i].y + faces[i].height),CV_RGB(255,0,0),1,8,0);
            //faceROI = frame(faces[i]);

            faceROI=cv::Mat(frame,faces[i]);
            cv::resize(faceROI,faceROI,cv::Size(480,720));
        }
    }
    return faces.size();
}

void scene::prepare_shader(GLuint const shader_id)
{
    //Setup uniform parameters
    glUseProgram(shader_id);                                                                           PRINT_OPENGL_ERROR();

    //Get cameras parameters (modelview,projection,normal).
    camera_matrices const& cam=pwidget->camera();

    //Set Uniform data to GPU
    glUniformMatrix4fv(get_uni_loc(shader_id,"camera_modelview"),1,false,cam.modelview.pointer());     PRINT_OPENGL_ERROR();
    glUniformMatrix4fv(get_uni_loc(shader_id,"camera_projection"),1,false,cam.projection.pointer());   PRINT_OPENGL_ERROR();
    glUniformMatrix4fv(get_uni_loc(shader_id,"normal_matrix"),1,false,cam.normal.pointer());           PRINT_OPENGL_ERROR();

}

void scene::get_webcam_size(int &width,int &height)
{
    cv::Mat frame;
    capture>>frame;

    width = frame.cols;
    height = frame.rows;
}

GLuint scene::generate_texture_webcam(cv::Mat const& im)
{
    GLuint texture_id;
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);  PRINT_OPENGL_ERROR();

    GLenum in_color = GL_BGR;
    if (im.channels() == 1)
        in_color = GL_LUMINANCE;

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); PRINT_OPENGL_ERROR();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); PRINT_OPENGL_ERROR();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); PRINT_OPENGL_ERROR();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); PRINT_OPENGL_ERROR();

    glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,im.cols,im.rows,0,in_color,GL_UNSIGNED_BYTE,im.ptr()); PRINT_OPENGL_ERROR();

    return texture_id;
}
GLuint scene::generate_avatar_head_texture(const cv::Mat &im)
{
    GLuint texture_id;
    glGenTextures(1,&texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id); //PRINT_OPENGL_ERROR;

    GLenum in_color = GL_BGR;
    if(im.channels() == 1)
        in_color = GL_LUMINANCE;

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); PRINT_OPENGL_ERROR();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); PRINT_OPENGL_ERROR();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); PRINT_OPENGL_ERROR();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); PRINT_OPENGL_ERROR();

    glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,im.cols,im.rows,0,in_color,GL_UNSIGNED_BYTE,im.ptr()); PRINT_OPENGL_ERROR();
    return texture_id;
}




scene::scene()
    :shader_mesh(0),shader_cube(0),time_advance(0),capture(0),savef(false),initHand(false),cpt(0),aFaceIsAlreadySaved(false),displayFace(false),no_displayFace(false)
{}


GLuint scene::load_texture_file(std::string const& filename)
{
    return pwidget->load_texture_file(filename);
}

void scene::set_widget(myWidgetGL* widget_param)
{
    pwidget=widget_param;
}


void scene::setSaveYourFace(bool valid)
{
    this->savef = valid;
}

bool scene::getSaveYourFace() const
{
    return this->savef;
}

void scene::setInitHand(bool valid)
{
    this->initHand = valid;
    cpt= 0;
}


void scene::display_handtrack(bool valid)
{
    hand1.setDisplayImages(valid);
}

void scene::display_facetrack(bool isValid)
{
    if(isValid)
    {
        this->displayFace = true;
        this->no_displayFace = false;
    }
    else
    {
        this->no_displayFace = true;
        this->displayFace = false;
    }

}
