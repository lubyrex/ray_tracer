/** \file RayTracer.cpp */
#include "RayTracer.h"

RayTracer::RayTracer(const shared_ptr<Scene>& s):
    scene(s){};

void RayTracer::traceImage(const shared_ptr<Camera>& camera,/* const shared_ptr<Scene>& scene,*/const shared_ptr<Image>& img){
// Nested for loops over pixel array
    // call traceOneRay for each pixel
    // Adjust pixel Radiance through shading 
    // change image
  
    Array<shared_ptr<Surface>> surfs; // Pass Arrays down 
    scene->onPose(surfs);
    tris = shared_ptr<TriTree>(new TriTree());
    tris->setContents(surfs);
    const int width = img->width();
    const int height = img->height(); 

    Vector2 dimensions((float)width,(float)height);
    Rect2D plane(dimensions);
    for(int y = 0; y< height; ++y){
        for(int x = 0; x < width; ++x){
            Point2int32 pixel(x, y);
            traceOneRay(pixel,plane,camera, img);
        };
    };

    

};

shared_ptr<Surfel> RayTracer:: traceOneRay(Point2int32 pixel,const Rect2D& plane, const shared_ptr<Camera>& camera, const shared_ptr<Image>& img){
    Ray ray(camera->worldRay(pixel.x,pixel.y,plane));
    Array<shared_ptr<Light>> lights;
   
    img->set(pixel.x,pixel.y, L_o(tris->intersectRay(ray), lights, -ray.direction()));
    
    return nullptr;
    //Trace a single ray from the center of the specified pixel
    
    // for loop over primitive arrays 
    // return foundTriangle, or foundSphere accordingly
};

shared_ptr<Surfel> RayTracer::intersects( const Ray& ray, const TriTree& tris) const{
    return nullptr;
};

shared_ptr<Surfel> RayTracer::intersectTriangle(const Tri& triangle, const Ray& ray) const{
    return nullptr;
};

shared_ptr<Surfel> RayTracer::intersectSphere(const Sphere& sphere, const Ray& ray, const Color3& color ) const{
      return nullptr;
};

Radiance3 RayTracer::L_o(const shared_ptr<Surfel> surfel, const Array<shared_ptr<Light>>& lights, const Vector3& w_o) const{
    if(notNull(surfel)){
        return Radiance3(1,1,1);
    } else {
        return Radiance3(0,0,0);
    };
};

bool RayTracer::isVisible(const shared_ptr<Surfel>& surfel, const TriTree& tris) const {
    return false;
};