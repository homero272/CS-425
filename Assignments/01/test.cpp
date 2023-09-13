
#include <sys/mman.h>
#include <cmath>
#include <cstdio>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <span>
#include <vector>

using namespace std;

//---------------------------------------------------------------------------
//
//  Data types and typedefs (C++ "using" clauses)
//

using Index = int;
using Distance = float;

//---------------------------------------------------------------------------

struct Vertex {
    float x;
    float y;
    float z;

    friend std::ostream& operator << (std::ostream& os, const Vertex& p) {
        return os << "(" << p.x << ", " << p.y << ", " << p.z << ")";
    }
};

using Vertices = std::span<Vertex>;

//---------------------------------------------------------------------------

struct Face {
    unsigned int numIndices;
    Index* indices;

    friend std::ostream& operator << (std::ostream& os, const Face& f) {
        os << f.numIndices << ": ";
        for (auto i = 0; i < f.numIndices; ++i) {
            os << f.indices[i] << " ";
        }
        return os;
    }
};

using Faces = std::vector<Face>;

//---------------------------------------------------------------------------

struct Transform {
    struct float4 {
        float x;
        float y;
        float z;
        float w;

        float4() = default;
        float4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}

        float dot(const Vertex& v) const
            { return x*v.x + y*v.y + z*v.z + w; }

        Vertex perspectiveDivide() const
            { return Vertex{ x/w, y/w, z/w }; }
    };

    float4 rows[4];

    Transform(float4 r0, float4 r1, float4 r2, float4 r3) {
        rows[0] = r0;
        rows[1] = r1;
        rows[2] = r2;
        rows[3] = r3;
    }

    Vertex operator * (const Vertex& v) {
        float4 r;

        r.x = rows[0].dot(v);
        r.y = rows[1].dot(v);
        r.z = rows[2].dot(v);
        r.w = rows[3].dot(v);

        return r.perspectiveDivide();
    }
};

//---------------------------------------------------------------------------
//
//  readData - read data from input file.  In this case, we memory-map
//    the file in for better performance, and map various arrays of
//    data (i.e., the vertices, and how they're connected together [their
//    faces]) into a few C++ data structures.
//
//  This routine uses some hard-coded "magic numbers" related to sizes
//    in this file to simplify the program.  While this is usually a
//    frowned upon approach, it save a lot of parsing code.  In this
//    input file, there are:
//
//      14027872 vertices, each containing three floating-point values
//      28055742 faces, each with an unsigned interger count, followed
//        by <count> indices    
//

void readData(const char* filename, Vertices& vertices, Faces& faces) {

    // Open the file, determine its size, and "memory map".  A memory
    //   map makes the contents of the file avilable as an array of
    //   bytes, which we can cast to the right types to use.
    //
    FILE* file = fopen(filename, "rb");
    if (!file) {
        std::cerr << "Unable to open '" << filename << "' ... exiting\n";
        exit(EXIT_FAILURE);
    }

    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);

    void* memory = mmap(nullptr, size, PROT_READ, MAP_PRIVATE, fileno(file), 0);
    
    if ((long) memory < 0 ) {
        std::cerr << "Unable to read '" << filename << "' ... exiting\n";
        exit(EXIT_FAILURE);
    }

    // Use a C++ std::span container to present the raw memory of the memory
    //   map as a useful type.  In this case, the variable "vertices"
    //   will look a lot like a std::vector, but not requiring all of the
    //   allocations
    constexpr size_t numVertices = 14027872;
    vertices = Vertices{reinterpret_cast<Vertex*>(memory), numVertices};

    size_t bytesForVertices = numVertices * sizeof(Vertex);

    // Faces are recorded in the file as a <count>, followed by <count>
    //   integers that index into the list of vertices.  Since there
    //   could be faces with different numbers of vertices, we need to
    //   process each face separately
    constexpr size_t numFaces = 28055742;
    faces.reserve(numFaces);
    auto data = reinterpret_cast<unsigned int*>((char*)memory + bytesForVertices);

    for (auto i = 0; i < numFaces; ++i) {
        unsigned int numIndices = *data++;
        faces.emplace_back(Face{numIndices, reinterpret_cast<Index*>(data)});
        data += numIndices;
    }
}

//----------------------------------------------------------------------------
//
//  transform - transforms a vertex by a 4x4 tranformation matrix using
//    the Transform class's operator *.  
//

Transform  xform {
    { 0.1, 0.0, 0.0, 0.0 },
    { 0.0, 0.1, 0.0, 0.0 },
    { 0.0, 0.0, -0.02, -1.01 },
    { 0.0, 0.0, 0.0, 1.0 },            
};

Vertex transform(const Vertex& v) {
    return xform * v;
}

//----------------------------------------------------------------------------
//
//  distance - computes the distance between two vertices
//

Distance distance(const Vertex& p, const Vertex& q) {
    auto dx = p.x - q.x;
    auto dy = p.y - q.y;
    auto dz = p.z - q.z;

    return std::sqrt(dx*dx + dy*dy + dz*dz);
}

//----------------------------------------------------------------------------
//
//  computePerimeter - computes the distance around all of the vertices
//    in a face.  The distance between each pair of vertices is computed
//    using the distance function, and then summed.
//

Distance computePerimeter(const Face& face, const Vertices& vertices) {
    Distance perimeter = 0.0;

    Index* indices = face.indices;

    auto i = 0;
    while (i < face.numIndices - 1) {
        auto p = transform(vertices[indices[i]]);
        auto q = transform(vertices[indices[++i]]);
        perimeter += distance(p, q);
    }

    auto p = transform(vertices[indices[face.numIndices-1]]);
    auto q = transform(vertices[indices[0]]);
    perimeter += distance(p, q);

    return perimeter;
}

//----------------------------------------------------------------------------
//
//  main - loads the data for the model and then computes the perimeter
//    of each face in the model, reporting the index of the face with the
//    smallest perimeter under the given transformation
//

int main() {
    Vertices vertices;
    Faces    faces;

    readData("lucy.bin", vertices, faces);

    struct Min {
        Distance perimeter = std::numeric_limits<Distance>::infinity();
        Index index = 0;
    } minFace;

    for (auto i = 0; i < faces.size(); ++i) {
        auto perimeter = computePerimeter(faces[i], vertices);

        if (perimeter < minFace.perimeter) {
            minFace.perimeter = perimeter;
            minFace.index = i;
        }
    }
    std::cout << "The smallest triangle is " << minFace.index << "\n";
}
