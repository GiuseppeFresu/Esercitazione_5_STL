#include "Utils.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>

namespace PolygonalLibrary
{
    bool ImportMesh(const string& filepath,
                    PolygonalMesh& mesh)
    {
        if(!ImportCell0Ds(filepath + "/Cell0Ds.csv",
                           mesh))
        {
            return false;
        }
        else
        {
            cout << "Cell0D marker:" << endl;
            for(auto it = mesh.Cell0DMarkers.begin(); it != mesh.Cell0DMarkers.end(); it++)
            {
                cout << "key:\t" << it -> first << "\t values:";
                for(const unsigned int id : it -> second)
                    cout << "\t" << id;

                cout << endl;
            }
        }

        if(!ImportCell1Ds(filepath + "/Cell1Ds.csv",
                           mesh))
        {
            return false;
        }
        else
        {
            cout << "Cell1D marker:" << endl;
            for(auto it = mesh.Cell1DMarkers.begin(); it != mesh.Cell1DMarkers.end(); it++)
            {
                cout << "key:\t" << it -> first << "\t values:";
                for(const unsigned int id : it -> second)
                    cout << "\t" << id;

                cout << endl;
            }
        }

        if(!ImportCell2Ds(filepath + "/Cell2Ds.csv",
                           mesh))
        {
            return false;
        }
        else
        {
            for(unsigned int c = 0; c < mesh.NumberCell2D; c++)
            {
                unsigned int numEdges = mesh.Cell2DEdges[c].size();

                const vector<unsigned int> &edges = mesh.Cell2DEdges[c];

                for(unsigned int e = 0; e < numEdges; e++)
                {
                    const unsigned int origin = mesh.Cell1DVertices[edges[e]][0];
                    const unsigned int end = mesh.Cell1DVertices[edges[e]][1];

                    auto findOrigin = find(mesh.Cell2DVertices[c].begin(), mesh.Cell2DVertices[c].end(), origin);
                    if (findOrigin == mesh.Cell2DVertices[c].end())
                    {
                        cerr << "Wrong mesh" << endl;
                        return 2;
                    }

                    auto findEnd = find(mesh.Cell2DVertices[c].begin(), mesh.Cell2DVertices[c].end(), end);
                    if (findEnd == mesh.Cell2DVertices[c].end())
                    {
                        cerr << "Wrong mesh" << endl;
                        return 3;
                    }
                }
            }
        }

        return true;
    }

    bool ImportCell0Ds(const string& filename,
                     PolygonalMesh& mesh)
    {
        ifstream file;
        file.open(filename);

        if(file.fail())
        {
            cerr << "File open failed." << endl;
            return false;
        }

        list<string> listLines;
        string line;
        while (getline(file, line))
            listLines.push_back(line);

        file.close();

        listLines.pop_front();

        mesh.NumberCell0D = listLines.size();

        if (mesh.NumberCell0D == 0)
        {
            cerr << "There is no cell 0D" << endl;
            return false;
        }

        mesh.Cell0DId.reserve(mesh.NumberCell0D);
        mesh.Cell0DCoordinates.reserve(mesh.NumberCell0D);

        for (const string& line : listLines)
        {
            istringstream converter(line);

            char delimiter;
            unsigned int id;
            unsigned int marker;
            Vector2d coord;

            converter >> id >> delimiter >> marker >> delimiter >> coord(0) >> delimiter >> coord(1);

            mesh.Cell0DId.push_back(id);
            mesh.Cell0DCoordinates.push_back(coord);

            if( marker != 0)
            {
                auto ret = mesh.Cell0DMarkers.insert({marker, {id}});
                if(!ret.second)
                    (ret.first)->second.push_back(id);
            }
        }

        file.close();

        return true;
    }

    bool ImportCell1Ds(const string& filename,
                     PolygonalMesh& mesh)
    {
        ifstream file;
        file.open(filename);

        if(file.fail())
        {
            cerr << "File open failed." << endl;
            return false;
        }

        list<string> listLines;
        string line;
        while (getline(file, line))
            listLines.push_back(line);

        listLines.pop_front();

        mesh.NumberCell1D = listLines.size();

        if (mesh.NumberCell1D == 0)
        {
            cerr << "There is no cell 1D" << endl;
            return false;
        }

        mesh.Cell1DId.reserve(mesh.NumberCell1D);
        mesh.Cell1DVertices.reserve(mesh.NumberCell1D);

        for (const string& line : listLines)
        {
            istringstream converter(line);

            char delimiter;
            unsigned int id;
            unsigned int marker;
            Vector2i vertices;

            converter >> id >> delimiter >> marker >> delimiter >> vertices(0) >> delimiter >> vertices(1);

            mesh.Cell1DId.push_back(id);
            mesh.Cell1DVertices.push_back(vertices);

            if( marker != 0)
            {
                auto ret = mesh.Cell1DMarkers.insert({marker, {id}});
                if(!ret.second)
                    (ret.first)->second.push_back(id);
            }
        }

        file.close();

        return true;
    }

    bool ImportCell2Ds(const string& filename,
                     PolygonalMesh& mesh)
    {
        ifstream file;
        file.open(filename);

        if(file.fail())
        {
            cerr << "File open failed." << endl;
            return false;
        }

        list<string> listLines;
        string line;
        while (getline(file, line))
            listLines.push_back(line);

        listLines.pop_front();

        mesh.NumberCell2D = listLines.size();

        if (mesh.NumberCell2D == 0)
        {
            cerr << "There is no cell 2D" << endl;
            return false;
        }

        mesh.Cell2DId.reserve(mesh.NumberCell2D);
        mesh.Cell2DVertices.reserve(mesh.NumberCell2D);
        mesh.Cell2DEdges.reserve(mesh.NumberCell2D);

        for (const string& line : listLines)
        {
            istringstream converter(line);

            char delimiter;
            unsigned int id;
            unsigned int marker;
            unsigned int numVertices;

            converter >> id >> delimiter >> marker >> delimiter >> numVertices;

            vector<unsigned int> vertices(numVertices);
            for (unsigned int i = 0; i < numVertices; i++)
            {
                converter >> delimiter >> vertices[i];
            }

            unsigned int numEdges;

            converter >> delimiter >> numEdges;

            vector<unsigned int> edges(numEdges);
            for (unsigned int i = 0; i < numEdges; i++)
            {
                converter >> delimiter >> edges[i];
            }

            mesh.Cell2DId.push_back(id);
            mesh.Cell2DMarkers[marker].push_back(id);
            mesh.Cell2DVertices.push_back(vertices);
            mesh.Cell2DEdges.push_back(edges);
        }

        file.close();

        return true;
    }

    bool testMarkers(const PolygonalMesh& mesh)
    {
        bool valid = true;

        valid &= !mesh.Cell0DMarkers.empty();
        valid &= !mesh.Cell1DMarkers.empty();
        valid &= !mesh.Cell2DMarkers.empty();

        for (const auto& [marker, ids] : mesh.Cell1DMarkers)
        {
            if (mesh.Cell0DMarkers.find(marker) == mesh.Cell0DMarkers.end())
            {
                valid = false;
                break;
            }
        }

        return valid;
    }

    bool testEdgesLength(const PolygonalMesh& mesh)
    {
        bool valid = true;

        double epsilon = numeric_limits<double>::epsilon();

        for (const auto& edge : mesh.Cell1DVertices)
        {
            Vector2d v1 = mesh.Cell0DCoordinates[edge[0]];
            Vector2d v2 = mesh.Cell0DCoordinates[edge[1]];

            if ((v2 - v1).norm() <= epsilon)
            {
                valid = false;
                break;
            }
        }

        return valid;
    }

    double calculatePolygonArea(const vector<Vector2d>& vertices)
    {
        double area = 0.0;
        size_t n = vertices.size();
        for (size_t i = 0; i < n; ++i)
        {
            size_t j = (i + 1) % n;
            area += vertices[i].x() * vertices[j].y() - vertices[j].x() * vertices[i].y();
        }
        return fabs(area) / 2.0;
    }

    bool testPolygonArea(const PolygonalMesh& mesh)
    {
        for (unsigned int i = 0; i < mesh.NumberCell2D; i++)
        {
            const vector<unsigned int> &vertices_id = mesh.Cell2DVertices[i];
            vector<Vector2d> vertices;
            for (unsigned int id : vertices_id)
            {
                vertices.push_back(mesh.Cell0DCoordinates[id]);
            }

            double area = calculatePolygonArea(vertices);
            double epsilon = numeric_limits<double>::epsilon();

            if (area <= epsilon)
            {
                cerr << "Error: Polygon area is zero or negative." << endl;
                return false;
            }
        }

        return true;
    }
}
