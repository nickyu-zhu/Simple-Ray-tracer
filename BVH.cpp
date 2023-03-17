#include<algorithm>
#include<cassert>
#include"global.hpp"
#include"BVH.hpp"
#include"Object.hpp"

bool cmp_x(Object* f1, Object* f2, vec3* vertex)
{
    return f1->getObjectBbox(vertex).Centroid().x < f2->getObjectBbox(vertex).Centroid().x;
}
bool cmp_y(Object* f1, Object* f2, vec3* vertex)
{
    return f1->getObjectBbox(vertex).Centroid().y < f2->getObjectBbox(vertex).Centroid().y;
}
bool cmp_z(Object* f1, Object* f2, vec3* vertex)
{
    return f1->getObjectBbox(vertex).Centroid().z < f2->getObjectBbox(vertex).Centroid().z;
}

BVHAccel::BVHAccel(std::vector<Object*> p, int maxPrimsInNode, SplitMethod splitMethod, vec3* vertex)
	: maxPrimsInNode(std::min(255, maxPrimsInNode)), splitMethod(SplitMethod::Naive),
	primitives(std::move(p))
{
    time_t start, stop;
    time(&start);
    if (primitives.empty())
        return;

    root = recursiveBuild(primitives, vertex);

    time(&stop);
    double diff = difftime(stop, start);
    int hrs = (int)diff / 3600;
    int mins = ((int)diff / 60) - (hrs * 60);
    int secs = (int)diff - (hrs * 3600) - (mins * 60);

    printf(
        "\rBVH Generation complete: \nTime Taken: %i hrs, %i mins, %i secs\n\n",
        hrs, mins, secs);
}

BVHBuildNode* BVHAccel::recursiveBuild(std::vector<Object*> objects, vec3* vertex)
{
    BVHBuildNode* node = new BVHBuildNode();

    Bbox bounds;
    for (int i = 0; i < objects.size(); i++)
    {
        bounds = Union(bounds, objects[i]->getObjectBbox(vertex));
    }
    if (objects.size() == 1)
    {
        //leafNode created
        node->bounds = objects[0]->getObjectBbox(vertex);
        node->object = objects[0];
        node->left = nullptr;
        node->right = nullptr;
        return node;
    }
    else if (objects.size() == 2)
    {
        node->left = recursiveBuild(std::vector{ objects[0] }, vertex);
        node->right = recursiveBuild(std::vector{ objects[1] }, vertex);

        node->bounds = Union(node->left->bounds, node->right->bounds);
        return node;
    }
    else
    {
        Bbox centroidBounds;
        for (int i = 0; i < objects.size(); i++)
            centroidBounds =
            Union(centroidBounds, objects[i]->getObjectBbox(vertex).Centroid());
        int dim = centroidBounds.maxExtent();
        if (dim == 0)
            std::sort(objects.begin(), objects.end(), cmp_x);
        else if (dim == 1)
            std::sort(objects.begin(), objects.end(), cmp_y);
        else
            std::sort(objects.begin(), objects.end(), cmp_z);

        auto beginning = objects.begin();
        auto middling = objects.begin() + (objects.size() / 2);
        auto ending = objects.end();

        auto leftshapes = std::vector<Object*>(beginning, middling);
        auto rightshapes = std::vector<Object*>(middling, ending);

        assert(objects.size() == (leftshapes.size() + rightshapes.size()));

        node->left = recursiveBuild(leftshapes, vertex);
        node->right = recursiveBuild(rightshapes, vertex);

        node->bounds = Union(node->left->bounds, node->right->bounds);
    }

    return node;
}