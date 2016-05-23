

#pragma once

#ifndef SCENE_HPP
#define SCENE_HPP

#include <GL/gl.h>
#include <GL/glew.h>

#include "../../lib/3d/mat3.hpp"
#include "../../lib/3d/vec3.hpp"
#include "../../lib/mesh/mesh.hpp"
#include "../../lib/mesh/mesh_parametric.hpp"
#include "../../lib/opengl/mesh_opengl.hpp"
#include "../../lib/interface/camera_matrices.hpp"
#include "faceTrack.hpp"


#include <vector>
#include <opencv2/opencv.hpp>


class myWidgetGL;

class scene
{
public:

    scene();



    /** \brief Method called only once at the beginning (load off files ...) */
    void load_scene();

    /** \brief Method called at every frame */
    void draw_scene();

    /** Set the pointer to the parent Widget */
    void set_widget(myWidgetGL* widget_param);


private:

    /** Load a texture from a given file and returns its id */
    GLuint load_texture_file(std::string const& filename);

    /** Load common data such as shader and files */
    void load_common_data();

    /** Load the recorder from the webcam */
    void start_webcam();
    /** Get the with and height of the image of the webcam */
    void get_webcam_size(int &width,int &height);
    /** Generate an openglGL texture from the OpenCV image */
    GLuint generate_texture_webcam(cv::Mat const& im);
    /** Generate an openGL texture from the openCV image for avatar head */
    GLuint generate_avatar_head_texture(cv::Mat const& im);
    /** Load a shader and send the camera matrices */
    void prepare_shader(GLuint shader_id);
    /** Perform some image analysis */
    void analyse_image(cv::Mat &frame);

    /** Generate avatar head */
    void generate_avatar_head();

    void load_avatar();

    /** Access to the parent object */
    myWidgetGL* pwidget;

    /** Default id for the texture (white texture) */
    GLuint texture_default;

    /** The id of the shader do draw meshes */
    GLuint shader_mesh;
    /** The id of the shader do draw the textured cube */
    GLuint shader_cube;


    cpe::mesh mesh_ground;
    cpe::mesh_opengl mesh_ground_opengl;

    cpe::mesh mesh_cube;
    cpe::mesh_opengl mesh_cube_opengl;

    cpe::mesh mesh_head;
    cpe::mesh_opengl mesh_head_opengl;

    cpe::mesh mesh_sphere;
    cpe::mesh_opengl mesh_sphere_opengl;

    cpe::mesh mesh_avatar_body;
    cpe::mesh_opengl mesh_avatar_body_opengl;
    GLuint texture_avatar_body;
    cpe::mesh mesh_avatar_head;
    cpe::mesh_opengl mesh_avatar_head_opengl;
    GLuint texture_avatar_head;


    /** OpenCV webcam capture interface */
    cv::VideoCapture capture;

    /** Time parameter */
    int time_advance;

    /** facetrack */
    cv::Mat frame_gray;
    cv::CascadeClassifier haar_cascade;
    std::vector<cv::Rect> faces;
    cv::Mat faceROI;




};

#endif
