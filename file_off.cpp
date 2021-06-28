#include "file_off.hpp"


Edge* Mesh::getEdge(int id1, int id2) {
    
    if (edges->size() == 0)
        return NULL;

    for (int i = 0; i < edges->size(); i++) {

        if (edges->at(i)->v1->id == id1 && edges->at(i)->v2->id == id2)
            return edges->at(i);

        if (edges->at(i)->v2->id == id1 && edges->at(i)->v1->id == id2)
            return edges->at(i);

    }

    return NULL;
}

int Mesh::init(const char* file_dir) {

    // Carrega ficheiro (fomato off)
    std::ifstream input( file_dir );

    int countReadedVertexes = 0;

    while (!input.eof()) {

        string line;
        getline(input, line);

        if (line.length() == 0 || line[0] == '#' || line.compare("OFF") == 0)
            continue;

        stringstream lineStream(line);

        // Primeira linha: [nº de vértices] [nº de faces] [nº de arestas]
        if (Mesh::vertexSize == -1) {
            lineStream >> Mesh::vertexSize >> Mesh::faceSize >> Mesh::edgeSize;
            vertexes = new vector<Vertex*>();
            faces = new vector<Face*>();
            edges = new vector<Edge*>();
            continue;
        }

        // Ler vértices
        if (countReadedVertexes < Mesh::vertexSize) {

            Point *p = new Point();
            lineStream >> p->x >> p->y >> p->z;

            Vertex *v1 = new Vertex(countReadedVertexes, p);
            vertexes->push_back(v1);

            countReadedVertexes++;
            continue;
        }

        // Ler faces
        int vertexesInFace;
        lineStream >> vertexesInFace;

        int lastVertex = -1, initialVertex = -1;

        Face *face = new Face();

        for (int i = 0; i < vertexesInFace; i++) {

            int vertexId;
            lineStream >> vertexId;

            if (lastVertex == -1) {
                lastVertex = vertexId;
                initialVertex = vertexId;
                continue;
            }

            Edge* e = getEdge(lastVertex, vertexId);
            if (e != NULL) {

                face->edges->push_back(e);

            } else {

                e = new Edge();
                e->v1 = vertexes->at(lastVertex);
                e->v2 = vertexes->at(vertexId);

                edges->push_back(e); // mesh

                // connections
                face->edges->push_back(e);

                vertexes->at(lastVertex)->edges->push_back(e);
                vertexes->at(vertexId)->edges->push_back(e);
            }

            lastVertex = vertexId;

            if (i == vertexesInFace - 1) { // Ultima edge (liga ao primeiro vertex)

                Edge* e = getEdge(vertexId, initialVertex);
                if (e != NULL) {

                    face->edges->push_back(e);

                } else {

                    e = new Edge();
                    e->v1 = vertexes->at(vertexId);
                    e->v2 = vertexes->at(initialVertex);

                    Mesh::edges->push_back(e); // mesh
                    
                    // connections
                    face->edges->push_back(e);

                    vertexes->at(vertexId)->edges->push_back(e);
                    vertexes->at(initialVertex)->edges->push_back(e);
                }

            }

        }

        Color *color = new Color(0.1, 0.1, 0.1);

        lineStream >> color->r >> color->g >> color->b;

        face->color = color;
        
        faces->push_back(face); // mesh
    }

    return 0;
}