#ifndef FILE_OFF_H
#define FILE_OFF_H

#include <iostream>
#include <sstream>
#include <cstdlib>
using namespace std;
#include <fstream>
#include <string>
#include <vector>

// GLM header file
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
using namespace glm;

class Face;
class Vertex;
class Edge;

class Point {
public:
    float x, y, z;

    
};

class Color {
public:
    float r, g, b;

    Color(float r, float g, float b) {
        this->r = r;
        this->g = g;
        this->b = b;
    }
};

class Mesh {

    public: 

        int vertexSize = -1, faceSize = -1, edgeSize = -1;

        vector<Vertex*> *vertexes;
        vector<Face*> *faces;
        vector<Edge*> *edges;

        Edge* getEdge(int id1, int id2);
        int init(const char* file_dir);

};

// Estrutura Winged-edge
class Edge {
    public:
        Edge *e1, *e2, *e3, *e4;
        Face *f1, *f2;
        Vertex *v1, *v2;
};

class Face {
    public:

        vector<Edge*> *edges; // Arestas adjacentes

        Color *color;

        Face() {
            edges = new vector<Edge*>();
        }

};

class Vertex {
    public:

        Point *point;
        int id;

        vector<Edge*> *edges; // Arestas incidentes

        Vertex(int id1, Point *p) {
            id = id1;
            point = p;
            edges = new vector<Edge*>();
        }
};

#endif // FILE_OFF_H