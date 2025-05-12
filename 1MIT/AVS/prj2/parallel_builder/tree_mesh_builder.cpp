/**
 * @file    tree_mesh_builder.cpp
 *
 * @author  Hung Do <xdohun00@stud.fit.vutbr.cz>
 *
 * @brief   Parallel Marching Cubes implementation using OpenMP tasks + octree early elimination
 *
 * @date    02/12/2023
 **/

#include <iostream>
#include <math.h>
#include <limits>

#include "tree_mesh_builder.h"
#define SMALL_CUBE_EDGE 2
#define SMALL_CUBE_GRID_SIZE (SMALL_CUBE_EDGE * SMALL_CUBE_EDGE * SMALL_CUBE_EDGE)

TreeMeshBuilder::TreeMeshBuilder(unsigned gridEdgeSize)
    : BaseMeshBuilder(gridEdgeSize, "Octree")
{
}

bool TreeMeshBuilder::outOfBound(Vec3_t<float> cubeOffset, unsigned a, const ParametricScalarField &field) {
    // get distance to the closest iso surface point
    const float sphereRadius = mIsoLevel + sqrt(3) / 2.0 * a * mGridResolution;
    // get middle point
    Vec3_t<float> middlePoint(
        (cubeOffset.x + a / 2.0) * mGridResolution,
        (cubeOffset.y + a / 2.0) * mGridResolution,
        (cubeOffset.z + a / 2.0) * mGridResolution
    );

    // iterate through field
    float value = evaluateFieldAt(middlePoint, field);
    // check return the result of the comparison with the radius of the sphere
    return value > sphereRadius;
}

unsigned TreeMeshBuilder::divideAndConquer(Vec3_t<float> cubeOffset, unsigned a, const ParametricScalarField &field)
{
    if (a <= 1)
        return buildCube(cubeOffset, field);

    Vec3_t<float> currCubeOffset(
        cubeOffset.x,
        cubeOffset.y,
        cubeOffset.z
    );

    if (outOfBound(currCubeOffset, a, field))
        return 0;

    const unsigned newEdgeSize = a / 2;
    unsigned triangles[8] = {0, };
    for (size_t i = 0; i < SMALL_CUBE_GRID_SIZE; i++)
    {
        #pragma omp task shared(newEdgeSize, triangles) // if(a < 8)
        {
            Vec3_t<float> smallCubeOffset(
                cubeOffset.x + cubeIndices[i][0] * (newEdgeSize),
                cubeOffset.y + cubeIndices[i][1] * (newEdgeSize),
                cubeOffset.z + cubeIndices[i][2] * (newEdgeSize)
            );
            triangles[i] = divideAndConquer(smallCubeOffset, newEdgeSize, field);
        }
    }
    #pragma omp taskwait
    return triangles[0] + triangles[1] + triangles[2] + triangles[3]
           + triangles[4] + triangles[5] + triangles[6] + triangles[7];
}

unsigned TreeMeshBuilder::marchCubes(const ParametricScalarField &field)
{
    // Suggested approach to tackle this problem is to add new method to
    // this class. This method will call itself to process the children.
    // It is also strongly suggested to first implement Octree as sequential
    // code and only when that works add OpenMP tasks to achieve parallelism.
    Vec3_t<float> cubeOffset(0, 0, 0);
    unsigned totalTriangles = 0;
    #pragma omp parallel shared(totalTriangles)
    {
        #pragma omp master
        {
            totalTriangles = divideAndConquer(0, mGridSize, field);
        }
    }
    return totalTriangles;
}

float TreeMeshBuilder::evaluateFieldAt(const Vec3_t<float> &pos, const ParametricScalarField &field)
{
    // NOTE: This method is called from "buildCube(...)"!

    // 1. Store pointer to and number of 3D points in the field
    //    (to avoid "data()" and "size()" call in the loop).
    const Vec3_t<float> *pPoints = field.getPoints().data();
    const unsigned count = unsigned(field.getPoints().size());

    float value = std::numeric_limits<float>::max();

    // 2. Find minimum square distance from points "pos" to any point in the
    //    field.
    for(unsigned i = 0; i < count; ++i)
    {
        float distanceSquared  = (pos.x - pPoints[i].x) * (pos.x - pPoints[i].x);
        distanceSquared       += (pos.y - pPoints[i].y) * (pos.y - pPoints[i].y);
        distanceSquared       += (pos.z - pPoints[i].z) * (pos.z - pPoints[i].z);

        // Comparing squares instead of real distance to avoid unnecessary
        // "sqrt"s in the loop.
        value = std::min(value, distanceSquared);
    }

    // 3. Finally take square root of the minimal square distance to get the real distance
    return sqrt(value);
}

void TreeMeshBuilder::emitTriangle(const BaseMeshBuilder::Triangle_t &triangle)
{
    #pragma omp critical(PushTriangle)
    mTriangles.push_back(triangle);
}
