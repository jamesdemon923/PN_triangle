#include <vector>
#include <stdio.h>
#include <string>
#include <sstream>
#include <cstring>
#include <iostream>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#include "objloader.h"



bool loadQuadOBJ(const char * path, std::vector<glm::vec3> &out_vertices, std::vector<glm::vec3> &out_normals)
{
    printf("Loading OBJ file with quads %s...\n", path);

    std::vector<unsigned int> vertexIndices, normalIndices;
    std::vector<glm::vec3> temp_vertices;
    std::vector<glm::vec3> temp_normals;

    // Additional data structure for storing the adjacency information
    std::map<int, std::vector<glm::vec3>> vertex_to_normals;

    FILE * file = fopen(path, "r");
    if( file == NULL ){
        printf("Impossible to open the file ! Are you in the right path ? See Tutorial 1 for details\n");
        getchar();
        return false;
    }

    while( 1 ){

        char lineHeader[128];
        // read the first word of the line
        int res = fscanf(file, "%s", lineHeader);
        if (res == EOF)
            break; // EOF = End Of File. Quit the loop.

        // else : parse lineHeader

        if ( strcmp( lineHeader, "v" ) == 0 ){
            glm::vec3 vertex;
            fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z );
            temp_vertices.push_back(vertex);
        }else if ( strcmp( lineHeader, "vn" ) == 0 ){
            glm::vec3 normal;
            fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z );
            temp_normals.push_back(normal);
        }else if ( strcmp( lineHeader, "f" ) == 0 ){
            std::string vertex1, vertex2, vertex3, vertex4;
            unsigned int vertexIndex[4], normalIndex[4];
            int matches = fscanf(file, "%d//%d %d//%d %d//%d %d//%d\n", &vertexIndex[0], &normalIndex[0], &vertexIndex[1], &normalIndex[1], &vertexIndex[2], &normalIndex[2], &vertexIndex[3], &normalIndex[3] );
            if (matches != 8){
                printf("ERROR: NO NORMALS PRESENT IN FILE! YOU NEED NORMALS FOR LIGHTING CALCULATIONS!\n");
                printf("File can't be read by our simple parser :-( Try exporting with other options. See the definition of the loadOBJ fuction.\n");
                return false;
            }

            //vertexIndices.push_back(vertexIndex[0]);
            //vertexIndices.push_back(vertexIndex[1]);
            //vertexIndices.push_back(vertexIndex[2]);
            //vertexIndices.push_back(vertexIndex[3]);
            //normalIndices.push_back(normalIndex[0]);
            //normalIndices.push_back(normalIndex[1]);
            //normalIndices.push_back(normalIndex[2]);
            //normalIndices.push_back(normalIndex[3]);

            // Store normals for each vertex
            for (int i = 0; i < 4; ++i) {
                vertex_to_normals[vertexIndex[i] - 1].push_back(temp_normals[normalIndex[i] - 1]);
                vertexIndices.push_back(vertexIndex[i]);
            }

        }else{
            // Probably a comment, eat up the rest of the line
            char stupidBuffer[1000];
            fgets(stupidBuffer, 1000, file);
        }

    }


    //// For each vertex of each quad
    //for( unsigned int i=0; i<vertexIndices.size(); i++ ){

    //    // Get the indices of its attributes
    //    unsigned int vertexIndex = vertexIndices[i];
    //    unsigned int normalIndex = normalIndices[i];

    //    // Get the attributes thanks to the index
    //    glm::vec3 vertex = temp_vertices[ vertexIndex-1 ];
    //    glm::vec3 normal = temp_normals[ normalIndex-1 ];

    //    // Put the attributes in buffers
    //    out_vertices.push_back(vertex);
    //    out_normals .push_back(normal);

    //}


    // Average the normals
    for (const auto& [vertexIndex, vertexNormals] : vertex_to_normals) {
        glm::vec3 sum(0.0f);
        for (const glm::vec3& normal : vertexNormals) {
            sum += glm::normalize(normal);
        }
        glm::vec3 averagedNormal = glm::normalize(sum / static_cast<float>(vertexNormals.size()));

        // Update all vertices with this index
        for (auto& vertexIndex : vertexIndices) {
            if (vertexIndex == vertexIndex) {
                out_normals.push_back(averagedNormal);
                out_vertices.push_back(temp_vertices[vertexIndex - 1]);
            }
        }
    }

    return true;
}

static AABB load_obj(const std::string &filename, const std::string &base_dir, std::vector<Mesh> &meshes, std::vector<Mesh>& silh)
{

    std::stringstream ss;
    std::vector<tinyobj::material_t> materials;
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;

    AABB aabb;

    std::string err;
    if ( !tinyobj::LoadObj(&attrib, &shapes, &materials, &err, filename.c_str(), base_dir.c_str()) )
    {
        throw std::runtime_error("Failed to load model : " + filename);
        return aabb;
    }

    if (materials.size() == 0)
    {
        throw std::runtime_error("No material found in model : " + filename);
        return aabb;
    }

    //< There is a bug here: err may contain multiple '\n' terminated string
    //< Research how to output multiple line log by spdlog
    if (!err.empty())
        printf("%s", err.c_str());

    meshes.resize(materials.size());

    //< is this macro: FLT_MAX OS dependent ?
    //< should always prefer os independent ones
    glm::vec3 pmin(FLT_MAX), pmax(-FLT_MAX);


    // Additional data structure for storing the adjacency information
    std::map<int, std::vector<glm::vec3>> vertex_to_normals;

    std::vector<SimpleVertex> silh_vertices;

    // Loop over shapes
    for (const auto &shape : shapes)
    {
        size_t index_offset = 0, face = 0;

        //< Loop over all faces(polygon) in a mesh. vertex number of face maybe variable
        //< for triangle num_face_vertices = 3
        for (const auto &num_face_vertex : shape.mesh.num_face_vertices)
        {
            int mat = shape.mesh.material_ids[face];
            // Loop over triangles in the face.
            for (size_t v = 0; v < num_face_vertex; ++v)
            {
                tinyobj::index_t index = shape.mesh.indices[index_offset + v];

                meshes.at(mat).vertices.emplace_back(); //< use "material id" to index mesh : mesh <--> material id is one-one map
                auto &vert = meshes.at(mat).vertices.back();
                vert.vertex_index = index.vertex_index;

                vert.position =
                {
                        attrib.vertices[3 * index.vertex_index + 0],
                        attrib.vertices[3 * index.vertex_index + 1],
                        attrib.vertices[3 * index.vertex_index + 2],
                        1.0f
                };
                pmin = glm::min(glm::vec3(vert.position), pmin);
                pmax = glm::max(glm::vec3(vert.position), pmax);
                if (~index.normal_index) //< -1 == 0xFFFFFFFF, it is equal to if (index.normal_index != -1)
                {

                    glm::vec3 normal =
                    {
                        attrib.normals[3 * index.normal_index + 0],
                        attrib.normals[3 * index.normal_index + 1],
                        attrib.normals[3 * index.normal_index + 2]
                    };


                    vertex_to_normals[index.vertex_index].push_back(normal);
                }
                else
                {
                    throw std::runtime_error("No normal channel found in vertex");
                    return aabb;
                }
            }
            index_offset += num_face_vertex;
            face++;
        }
    }

    // Average the normals
    for (auto& mesh : meshes) {
        for (const auto& [vertexIndex, vertexNormals] : vertex_to_normals) {
            glm::vec3 sum(0.0f);
            for (const glm::vec3& normal : vertexNormals) {
                sum += glm::normalize(normal);
            }
            glm::vec3 averagedNormal = glm::normalize(sum / static_cast<float>(vertexNormals.size()));

            // Update all vertices with this index
            for (auto& vert : mesh.vertices) {
                if (vert.vertex_index == vertexIndex) {
                    vert.normal = averagedNormal;
                    silh_vertices.push_back(vert);
                }
            }
        }
    }
    Mesh silhou;
    silhou.vertices = silh_vertices;
    silh.push_back(silhou);

    aabb.Extend(pmin);
    aabb.Extend(pmax);

    ss << "After binning," << meshes.size() << " meshes constructed(each mesh contains only one material)" << "\n"
       << "Mesh file : " << filename << "\n"
       << aabb.str()
    ;

    printf("%s \n", ss.str().c_str());

#if 0
    //< normalize all the vertex to [-1, 1]
    glm::vec3 extent3 = pmax - pmin;
    float extent = glm::max(extent3.x, glm::max(extent3.y, extent3.z)) * 0.5f;
    float inv_extent = 1.0f / extent;

    glm::vec3 center = (pmax + pmin) * 0.5f;

    for (auto &m : meshes)
        for (auto &v : m.vertices)
            v.position = (v.position - center) * inv_extent;

    Log("[SCENE]: load {} meshes from file {}\t vertex position normalized to [-1, 1]", meshes.size(), filename);

#endif

    return aabb;
}

///////////////////////////// Class MeshBin //////////////////////////////////////////////

    MeshBin::MeshBin(const std::string &filename)
        : m_max_object_num(256)
    {
        m_aabb = load_obj(filename, "Model/", m_meshes, m_silh);
        create_vaos();
        sil_create_vaos(updateSil);
    }

    void MeshBin::update_silhouette(std::vector<Mesh>& new_silh)
    {
        updateSil = true;
        m_silh = new_silh;
        sil_create_vaos(updateSil);
    }

    void MeshBin::create_vaos()
    {
        for (int i = 0; i < m_meshes.size(); ++i)
        {
            GLenum errorCheckValue = glGetError();

            m_vb_size[m_object_num] = m_meshes[i].vertices.size() * sizeof(SimpleVertex);
            m_vertex_num[m_object_num] = m_meshes[i].vertices.size();

            const size_t vertexStride = sizeof(SimpleVertex);
            const size_t normalOffset = sizeof(m_meshes[i].vertices[0].position);

            glGenVertexArrays(1, &m_vao_id[m_object_num]);
            glBindVertexArray(m_vao_id[m_object_num]);

            glGenBuffers(1, &m_vbo_id[m_object_num]);
            glBindBuffer(GL_ARRAY_BUFFER, m_vbo_id[m_object_num]);
            glBufferData(GL_ARRAY_BUFFER, m_vb_size[m_object_num], m_meshes[i].vertices.data(), GL_STATIC_DRAW);

            glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, vertexStride, 0); // Position
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, vertexStride, (GLvoid*)normalOffset); // Normal

            glEnableVertexAttribArray(0);
            glEnableVertexAttribArray(1);

            for (const auto& vertex : m_meshes[i].vertices) {
                std::cout << "Normal: (" << vertex.normal.x << ", " << vertex.normal.y << ", " << vertex.normal.z << ")" << std::endl;
            }

            glBindVertexArray(0);

            errorCheckValue = glGetError();
            if (errorCheckValue != GL_NO_ERROR)
            {
                fprintf(stderr, "Error: Could not create a VBO: %s\n", gluErrorString(errorCheckValue));
            }
            m_object_num++;
        }
    }

    void MeshBin::sil_create_vaos(bool updateSil)
    {
        for (int i = 0; i < m_silh.size(); ++i)
        {
            GLenum errorCheckValue = glGetError();

            if (updateSil)
            {
                glDeleteBuffers(1, &sil_vbo_id[m_silh_num]);
                glDeleteVertexArrays(1, &sil_vao_id[m_silh_num]);
                m_silh_num--;
            }

            sil_vb_size[m_silh_num] = m_silh[i].vertices.size() * sizeof(SimpleVertex);
            sil_vertex_num[m_silh_num] = m_silh[i].vertices.size();

            std::cout << "The number of the silhouette vertices is " << sil_vertex_num[m_silh_num] << std::endl;

            const size_t vertexStride = sizeof(SimpleVertex);
            const size_t normalOffset = sizeof(m_silh[i].vertices[0].position);

            glGenVertexArrays(1, &sil_vao_id[m_silh_num]);
            glBindVertexArray(sil_vao_id[m_silh_num]);

            glGenBuffers(1, &sil_vbo_id[m_silh_num]);
            glBindBuffer(GL_ARRAY_BUFFER, sil_vbo_id[m_silh_num]);
            glBufferData(GL_ARRAY_BUFFER, sil_vb_size[m_silh_num], m_silh[i].vertices.data(), GL_STATIC_DRAW);

            glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, vertexStride, 0); // Position
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, vertexStride, (GLvoid*)normalOffset); // Normal

            glEnableVertexAttribArray(0);
            glEnableVertexAttribArray(1);

            glBindVertexArray(0);

            errorCheckValue = glGetError();
            if (errorCheckValue != GL_NO_ERROR)
            {
                fprintf(stderr, "Error: Could not create a VBO: %s\n", gluErrorString(errorCheckValue));
            }
            m_silh_num++;
        }
    }


