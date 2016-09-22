/** \file RayTracer.cpp */
#include "RayTracer.h"

RayTracer::RayTracer(const shared_ptr<Scene>& s):
    scene(s){};

void RayTracer::traceWholeImage(const shared_ptr<Camera>& camera, const shared_ptr<Scene>& scene, shared_ptr<Image>& img){
Array<shared_ptr<Surface>> surfs;
scene->onPose(surfs);
TriTree triTree;
triTree.setContents(surfs);

// call traceRays
};

void RayTracer::traceRays(const shared_ptr<Camera>& camera, const shared_ptr<Scene>& scene, shared_ptr<Image>& img){
// Nested for loops over pixel array
    // call traceOneRay for each pixel
    // Adjust pixel Radiance through shading 
    // change image
};

shared_ptr<UniversalSurfel> RayTracer::traceOneRay(Point2int32 point, const shared_ptr<Camera>& camera){
    
    //camera->worldRay(point.x,point.y,);
    
    //Trace a single ray from the center of the specified pixel
    
    // for loop over primitive arrays 
    // return foundTriangle, or foundSphere accordingly
};

shared_ptr<Entity> RayTracer::intersects( const Ray& ray, float& d, const Array<shared_ptr<Entity>>& ents){};

shared_ptr<UniversalSurfel> RayTracer::foundTriangle(TriTree triangle,Ray ray){};

shared_ptr<UniversalSurfel> RayTracer::foundSphere(Sphere sphere, const Ray& ray){};

Radiance3 RayTracer::doShading( shared_ptr<UniversalSurfel> surfel){};