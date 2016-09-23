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
    void traceImage(const shared_ptr<Camera>& camera, const shared_ptr<Scene>& scene, shared_ptr<Image>& img); // Traces one ray per pixel 
    
    /** Called by traceImage()
        Calls intersects()
        Returns nullptr if no intersection is found */
    shared_ptr<Surfel> traceOneRay(Point2int32 point,const Rect2D& plane, const shared_ptr<Camera>& camera);

    /** Calls intersectSphere()
        Calls intersectTriangle()
        Returns nullptr if no intersection is found */
    shared_ptr<Surfel> intersects( const Ray& ray, float& d, const TriTree& tris) const;
    
    /** Returns nullptr if no intersecting surface is found */
    shared_ptr<Surfel> intersectSphere(const Sphere& sphere, const Ray& ray, const Color3& color ) const;

    /** \copybrief intersectSphere*/
    shared_ptr<Surfel> intersectTriangle(const Tri& triangle, const Ray& ray) const;

    Radiance3 L_o( shared_ptr<UniversalSurfel> surfel, const Array<shared_ptr<Light>>& lights, const Vector3& w_o) const;

public:
    RayTracer(const shared_ptr<Scene>& scene);
    /** Main driver function, makes use of all 
    RayTracer functions to render the image*/
    void traceWholeImage(const shared_ptr<Camera>& camera, const shared_ptr<Scene>& scene, shared_ptr<Image>& img);

};