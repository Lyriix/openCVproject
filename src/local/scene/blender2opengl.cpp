
#include "blender2opengl .hpp"
#include "../../lib/common/error_handling.hpp"


Model::Model():vertex(0),vertices(0),normals(0),color(0),texture_coord(0),faces(0)
{}


void Model::getOBJinfo(const std::string& fp)
{


    //Open the OJ File
    std::ifstream inOBJ(fp.c_str());
    if(!inOBJ.good())
        throw cpe::exception_cpe("Cannot open file" +fp,MACRO_EXCEPTION_PARAMETER);

    std::string buffer;

    //Lecture du fichier entier
    while(inOBJ.good() == true)
    {
        //read line
        std::getline(inOBJ,buffer);

        if(buffer.size()>0) //si on est pas sur une ligne vide
        {
            std::stringstream tokens_buffer(buffer); //IO string
            std::string first_word; //premier mot du buffer dans le token
            tokens_buffer>>first_word;

            //vertices
            if(first_word =="v")
                vertex++;
            //texture
            if(first_word=="vt")
                texture_coord++;
            //normals
            if(first_word=="vn")
                normals++;
            //faces; connectivity
            if(first_word=="f")
                faces++;
        }
        vertices = vertex *3;
    }
}
void Model::extractOBJdata(const std::string& fp, int m )
{
    //Model model;
    //Compteurs
    int p=0,t=0,f=0;
    //Index
    int mtl=0;

    //Open the .obj
    std::ifstream in(fp.c_str());

    if(!in.good())
        throw cpe::exception_cpe("Cannot open file "+fp,MACRO_EXCEPTION_PARAMETER);

    std::string buffer;

    //lecture du fichier entier
    while(in.good()==true)
    {
        //Lecture d'une ligne mise dans le buffer
        std::getline(in,buffer);
        if(buffer.size()>0) // si la ligne n'est pas vide
        {
            std::stringstream tokens_buffer(buffer);
            std::string first_word;
            tokens_buffer>>first_word;
            if(first_word.length()>0 && first_word[0] != '#')
            {
                //vertices  //read vertex obj
                if(first_word=="v"){ read_vertex_obj(tokens_buffer,*this); }
                 //read texture obj
                if(first_word=="vt"){}
             //read normal obj
                if(first_word=="vn"){}
                //read material obj
                if(first_word=="usemtl")
                {
                    std::string material = buffer.substr(first_word.size());
                    read_name_mtl(tokens_buffer,*this);
                    for(int i=0; i<m; i++)
                        if(material.compare(this->list_mat_tex[i]) == 0)
                            mtl=i;
                }

                if(first_word=="f"){read_face_obj(tokens_buffer,*this,mtl);}
                //read connectivity/faces obj

            }
        }
    }
}
int Model::getMTLinfo(const string &fp)
{
    int m=0;
    //Open mtl file
    std::ifstream in(fp.c_str());
    if(!in.good())
        throw cpe::exception_cpe("Cannot read mtl file"+fp, MACRO_EXCEPTION_PARAMETER);

    std::string buffer;
    //Lecture du fichier entier
    while(in.good() == true)
    {
        //lecture d'une ligne
        std::getline(in,buffer);
        if(buffer.size()>0) //Si la ligne n'est pas vide
        {
            std::stringstream tokens_buffer(buffer);
            std::string first_word;
            if(first_word.length()>0 && first_word[0] != '#')
            {
                if(first_word=="newmtl")
                    m++; //nombre de matériaux
            }
        }
    }
    in.close(); //Close tje mtl file
    return m;
}
void Model::extractMTLdata(const string &fp)
{
    //Compteurs
    int m=0, d=0, s=0; //materiaux, diffuse, specular
    //Open mtl file
    std::ifstream in(fp.c_str());

    if(!in.good())
        throw cpe::exception_cpe("Cannot open mtl file "+fp,MACRO_EXCEPTION_PARAMETER);

    std::string buffer;

    //Lecture du fichier en entier
    while(in.good() == true)
    {
        //Lecture d'une ligne
        std::getline(in,buffer);

        if(buffer.size()>0) //Si la ligne n'est pas vide
        {
            std::stringstream tokens_bufer(buffer);
            std::string first_word="\0";
             //   std::cout<<first_word<<std::endl;
            tokens_bufer>>first_word;
            //std::cout<<first_word<<std::endl;
            if(first_word.length()>0 && first_word[0] != '#')
            {
                std::cout<<first_word<<std::endl;
                //Names
                if(first_word=="newmtl")
                {
                    read_name_mtl(tokens_bufer,*this);
                }
                //Diffuse
                if(first_word=="Kd"){}
                //Speculars
                if(first_word=="Ks"){}
            }

        }
    }
    //Close MTL
    in.close();
}









std::vector<int> split_face_data(std::string const& face_data_str)
{
    std::vector<int> data;
    std::string current_str=face_data_str;

    while(current_str.length()>0)
    {
        while(current_str.length()>0 && current_str[0]=='/')
        {
            std::string temp_str=current_str.substr(1,current_str.length()-1);
            current_str=temp_str;
        }

        int value;
        std::stringstream tokenizer(current_str);
        tokenizer>>value;
        data.push_back(value);

        std::string residual;
        tokenizer>>residual;

        current_str=residual;
    }

    return data;
}
void read_vertex_obj(std::stringstream& tokens, Model& mod)
{
    cpe::vec3 v;
    tokens >> v.x();
    tokens >> v.y();
    tokens >> v.z();
    mod.vertices_tab.push_back(v);
}

void read_material_obj(std::stringstream& tokens, Model& mod)
{
    /*
    std::string name_tex;
    tokens >> name_tex;
    mod.list_mat_tex.push_back(name_tex);
    */
}

void read_face_obj(std::stringstream& tokens, Model& mod, int mtl)
{
    std::vector<std::vector<int>> face_data;
    while(tokens.good())
    {
        std::string polygon_index_str;
        tokens>>polygon_index_str;
        std::vector<int> const current_data = split_face_data(polygon_index_str);
        face_data.push_back(current_data);
    }
    std::vector<int> temp_vertex;
    std::vector<int> temp_texture;
    for(auto const& index_face : face_data)
    {
        int const N_dim = index_face.size();
        for(int k_dim=0; k_dim<N_dim; ++k_dim)
        {
            int const value = index_face[k_dim];
            switch(k_dim)
            {
            case 0:
                temp_vertex.push_back(value-1);
                std::cout<<value-1<<std::endl;
                break;
            case 1:
                temp_texture.push_back(value-1);
                break;
            default:
                std::cerr<<"Error reading file"<<std::endl;
                exit(1);
            }
        }
    }

    if(temp_vertex.size()>0)
        mod.faces_tab_vertex.push_back(temp_vertex);
    if(temp_texture.size()>0)
        mod.faces_tab_texel.push_back(temp_texture);
}

void read_name_mtl(std::stringstream& tokens, Model& mod)
{
    std::string name_texture;
    tokens >> name_texture;
    mod.list_mat_tex.push_back(name_texture);

}

int Model::get_vertex() const { return this->vertex;}
int& Model::set_vertex() {return vertex; }
int Model::get_vertices() const {return vertices; }
int& Model::set_vertices(){return vertices; }
int Model::get_normals() const{return normals; }
int& Model::set_normals(){return normals; }
int Model::get_color() const{return color; }
int& Model::set_color(){return color; }
int Model::get_texture_coord() const{return texture_coord; }
int& Model::set_texture_coord(){return texture_coord; }
int Model::get_faces() const{return faces; }
int& Model::set_faces(){return faces; }
int Model::get_nbMaterials() const {return nbMaterials ;}
int& Model::set_nbMaterials(){ return nbMaterials ;}
