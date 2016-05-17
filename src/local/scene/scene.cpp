

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
    texture_avatar = load_texture_file("data/Megaman/texture.png");
    //tex_head = load_texture_file("data/Megaman/head.png");

    mesh_avatar = load_mesh_file("data/Megaman/Megaman_Retex.obj");
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
    Model* mod = new Model();
    // Files
    std::string srcData = "data/Megaman/";
    std::string nameOBJ = "Megaman";
    std::string filepathOBJ = srcData + nameOBJ +".obj";
    std::string filepathMTL = srcData + nameOBJ +".mtl";
    std::string filepathH = srcData + nameOBJ +".h";
    std::string filepathC = srcData + nameOBJ +".c";
    //string* materials = new string[14];
    //std::vector<std::string> materials;

    //Model Info
    mod->set_nbMaterials() = mod->getMTLinfo(filepathMTL);
    mod->getOBJinfo(filepathOBJ);

    //Model data
  //  mod->extractMTLdata(filepathMTL);
    mod->extractOBJdata(filepathOBJ, mod->get_nbMaterials());
    std::vector<std::string>::iterator it_begin = mod->list_mat_tex.begin(), it_end = mod->list_mat_tex.end();
    for( ;
        it_begin != it_end;
        it_begin++)
    {
        std::string aa = *it_begin;
        std::cout<<aa<<std::endl;
    }


    //   std::vector<cpe::mesh> meshes_avatar;
    // std::vector<cpe::mesh_opengl> meshes_avatar_opengl;

    std::cout<<mod->get_vertex()<<" "<<mod->get_texture_coord()<<" "<<mod->get_normals()<<" "<<mod->get_faces()<<" "<<mod->get_vertices()<<std::endl;

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
    glBindTexture(GL_TEXTURE_2D,texture_avatar);
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


