#include <iostream>
#include "PolygonalMesh.hpp"
#include "Utils.hpp"

using namespace std;
using namespace Eigen;
using namespace PolygonalLibrary;

int main()
{
    PolygonalMesh mesh;

    string filepath = "PolygonalMesh";
    if(!ImportMesh(filepath,
                    mesh))
    {
        return 1;
    }

    bool markersValid = testMarkers(mesh);
    cout << "All markers have been stored correctly: " << (markersValid ? "Yes" : "No") << endl;

    bool edgesValid = testEdgesLength(mesh);
    cout << "The edges of the polygons have non-zero length: " << (edgesValid ? "Yes" : "No") << endl;

    bool areaValid = testPolygonArea(mesh);
    cout << "The area of the polygons is non-zero: " << (areaValid ? "Yes" : "No") << endl;

    return 0;
}
