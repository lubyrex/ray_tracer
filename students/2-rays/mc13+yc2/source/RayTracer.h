/**
  \file RayTracer.h

  Specifies procedures for raytracing in image rendering
 */
#pragma once
#include <G3D/G3DAll.h>

/** \brief Application framework. */
class RayTracer : public RayTracer {
protected:
    shared_ptr<Scene> scene;
 
    /** Driver function to traceOneRay, traces a ray through
    the center of each of the image pixels */
    void traceRays(const shared_ptr<Camera>& camera, const shared_ptr<Scene>& scene, shared_ptr<Image>& img); // Traces one ray per pixel 
    
    /** Herlper function to the rayTracing function to find 
    the intersection distance and surfels for a single ray */
    shared_ptr<UniversalSurfel> traceOneRay(Point2int32 point, const shared_ptr<Camera>& camera, shared_ptr<Image>& img);

    shared_ptr<Entity> intersects( const Ray& ray, float& d, const Array<shared_ptr<Entity>>& ents);
    
    /** The following return the surfels for the primitives intersected */
    shared_ptr<UniversalSurfel> foundSphere(Sphere sphere, const Ray& ray);
    shared_ptr<UniversalSurfel> foundTriangle(TriTree triangle,Ray ray);

    Radiance3 doShading( shared_ptr<UniversalSurfel> surfel );

public:
    RayTracer(const shared_ptr<Scene>& scene);
    /** Main driver function, makes use of all 
    RayTracer functions to render the image*/
    void traceWholeImage(const shared_ptr<Camera>& camera, const shared_ptr<Scene>& scene, shared_ptr<Image>& img);

};