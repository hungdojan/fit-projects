/**
 * @file    tree_mesh_builder.h
 *
 * @author  Hung Do <xdohun00@stud.fit.vutbr.cz>
 *
 * @brief   Parallel Marching Cubes implementation using OpenMP tasks + octree early elimination
 *
 * @date    02/12/2023
 **/

#ifndef TREE_MESH_BUILDER_H
#define TREE_MESH_BUILDER_H

#include <vector>
#include "base_mesh_builder.h"

class TreeMeshBuilder : public BaseMeshBuilder
{
public:
    TreeMeshBuilder(unsigned gridEdgeSize);

protected:
    unsigned marchCubes(const ParametricScalarField &field);
    float evaluateFieldAt(const Vec3_t<float> &pos, const ParametricScalarField &field);
    void emitTriangle(const Triangle_t &triangle);
    const Triangle_t *getTrianglesArray() const { return mTriangles.data(); }

    std::vector<Triangle_t> mTriangles;
    float maxDepth;

private:
    unsigned divideAndConquer(Vec3_t<float> cubeOffset, unsigned a, const ParametricScalarField &field);
    bool outOfBound(Vec3_t<float> cubeOffset, unsigned a, const ParametricScalarField &field);
    size_t cubeIndices[8][3] = {
        {0, 0, 0},
        {1, 0, 0},
        {0, 1, 0},
        {1, 1, 0},
        {0, 0, 1},
        {1, 0, 1},
        {0, 1, 1},
        {1, 1, 1}
    };
};

#endif // TREE_MESH_BUILDER_H
