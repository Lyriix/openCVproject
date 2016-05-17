

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
    //face=faceTrack();


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
    //texture_avatar = load_texture_file("data/Megaman/texture.png");
    //tex_head = load_texture_file("data/Megaman/head.png");

    mesh_avatar = load_mesh_file("data/Megaman/Megaman.obj");
    mesh_avatar.transform_apply_scale(0.2f,0.2f,0.2f);
    mesh_avatar_opengl.fill_vbo(mesh_avatar);
    //model();
    scene::load_avatar();

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



}

void scene::load_avatar()
{
    //Model* model = new Model();
    model* mod = new model();
    // Files
    std::string srcData = "data/Megaman/";
    std::string nameOBJ = "Megaman";
    std::string filepathOBJ = srcData + nameOBJ +".obj";
    std::string filepathMTL = srcData + nameOBJ +".mtl";
    std::string filepathH = srcData + nameOBJ +".h";
    std::string filepathC = srcData + nameOBJ +".c";

    //Model Info
    mod->getObjInfo(filepathOBJ);
    mod->setMaterials(filepathMTL);
    //Model data
    float positions[mod->getPositions()][3];
    float texels[mod->getTexels()][2];
    int faces[mod->getFaces()][6];
    string* materials = new string[mod->getMaterials()];

    float diffuses[mod->getMaterials()][3];
    float speculars[mod->getMaterials()][3];

    int firsts[mod->getMaterials()];
    int counts[mod->getMaterials()];

    mod->extractMtlData(filepathMTL, materials, diffuses, speculars);
    mod->extractObjData(filepathOBJ, positions, texels, faces, materials, mod->getMaterials());
    mod->writeH(filepathH,nameOBJ);
    mod->writeCvertices(filepathC,nameOBJ);
    mod->writeCpositions(filepathC,nameOBJ,faces,positions,counts);
    mod->writeCtexels(filepathC,nameOBJ,faces,texels);
/*

    //Model Info
    model->getOBJinfo(filepathOBJ);
    std::cout<<model->getPositions()<<std::endl;
    model->setMaterials(filepathMTL);

    // Model Data
    float positions[model->getPositions()][3];
    float texels[model->getTexels()][2];
    float normals[model->getNormals()][3];
    int faces[model->getFaces()][6];
    string* materials = new string[model->getMaterials()];
    float diffuses[model->getMaterials()][3];
    float speculars[model->getMaterials()][3];
    model->extractMTLdata(filepathMTL, materials, diffuses, speculars);
    model->extractOBJdata(filepathOBJ, positions, texels, normals, faces, materials, model->getMaterials());

    int firsts[model->getMaterials()];
    int counts[model->getMaterials()];

    //Write H fille
    model->writeH(filepathH, nameOBJ);
    std::cout<<"tata"<<std::endl;
    std::cout<<model->getPositions()<<" "<<model->getMaterials()<<std::endl;
    //Write C File
    model->writeCvertices(filepathC, nameOBJ);
    model->writeCpositions(filepathC, nameOBJ,  faces, positions, counts);
    model->writeCtexels(filepathC, nameOBJ, faces, texels);
    model->writeCnormals(filepathC, nameOBJ, faces,normals);
    model->writeCmaterials(filepathC, nameOBJ,firsts, counts);
    model->writeCdiffuses(filepathC, nameOBJ, diffuses);
    model->writeCspeculars(filepathC, nameOBJ, speculars);
    */
}
// https://www.raywenderlich.com/48297/export-blender-models-opengl-es-part-2
void scene::draw_scene()
{

    prepare_shader(shader_mesh);
    glBindTexture(GL_TEXTURE_2D,texture_default);  PRINT_OPENGL_ERROR();
    mesh_ground_opengl.draw();


    prepare_shader(shader_cube);
    glUniform1f(get_uni_loc(shader_cube,"time"),time_advance);

    // Get webcam image
    cv::Mat frame;
    capture >> frame;
    analyse_image(frame);
    // Generate OpenGL texture from webcam image
    GLuint texture_webcam = generate_texture_webcam(frame);
 //Generate texture from ROI
    GLuint texture_webcamROI = generate_texture_webcam(faceROI);//generate_avatar_head_texture(frame);
    // Draw the object with the webcam texture
    glBindTexture(GL_TEXTURE_2D,texture_webcam);  PRINT_OPENGL_ERROR();
    mesh_cube_opengl.draw();
    glBindTexture(GL_TEXTURE_2D,texture_webcamROI);
    mesh_head_opengl.draw();
    //glBindTexture(GL_TEXTURE_2D,tex_head);
    mesh_avatar_opengl.draw();
    // Delete the texture on the GPU
    glDeleteTextures(1, &texture_webcam);  PRINT_OPENGL_ERROR();



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
    capture=cv::VideoCapture(0);
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
   // cv::namedWindow("wind");
}




void scene::analyse_image(cv::Mat& frame)
{

    cv::pyrDown(frame,frame);
    cv::cvtColor( frame, frame_gray, CV_BGR2GRAY );
    cv::equalizeHist( frame_gray, frame_gray );

    int i;

    haar_cascade.detectMultiScale(frame_gray,faces,1.1,3,0|CV_HAAR_SCALE_IMAGE);

    for(i= 0 ; i<faces.size(); ++i)
    {

        //  cv::Point center(faces[i].x + faces[i].width*0.5, faces[i].y + faces[i].height*0.5);
        //cv::ellipse(frame, center, cv::Size( faces[i].width*0.5, faces[i].height*0.5), 0, 0, 360, cv::Scalar( 255, 0, 255 ), 4, 8, 0);
        if(faces[i].x >0 && faces[i].x + faces[i].width < frame.cols && faces[i].y + faces[i].height < frame.rows)
        {
             cv::rectangle(frame, cv::Point(faces[i].x, faces[i].y), cv::Point(faces[i].x +faces[i].width, faces[i].y + faces[i].height),CV_RGB(255,0,0),1,8,0);
        //faceROI = frame(faces[i]);
           faceROI=cv::Mat(frame,faces[i]);
           cv::resize(faceROI,faceROI,cv::Size(100,100));
        }
    }
// imshow("wind", faceROI);


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



    glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,im.cols,im.rows,0,in_color,GL_UNSIGNED_BYTE,im.ptr()); PRINT_OPENGL_ERROR();
    return texture_id;
}




scene::scene()
    :shader_mesh(0),shader_cube(0),time_advance(0)
{}


GLuint scene::load_texture_file(std::string const& filename)
{
    return pwidget->load_texture_file(filename);
}

void scene::set_widget(myWidgetGL* widget_param)
{
    pwidget=widget_param;
}


