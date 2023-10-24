#include <iostream>
#include "parser.h"
#include "ppm.h"

#define MAX_VALUE 99999

typedef unsigned char RGB[3];

using namespace parser;

struct Ray{
    Vec3f origin;
    Vec3f direction;
};


Vec3f normalize(Vec3f& v){
    double length = sqrt(pow(v.x, 2) + pow(v.y, 2) + pow(v.z, 2));
    Vec3f result;
    result.x = v.x / length;
    result.y = v.y / length;
    result.z = v.z / length;
    return result;
}

Vec3f RaySphereIntersect(std::vector<Sphere> spheres, Ray ray, Scene scene){
    Vec3f color;
    color.x = scene.background_color.x;
    color.y = scene.background_color.y;
    color.z = scene.background_color.z;
    double t_min = MAX_VALUE;
    for (int i = 0; i < spheres.size(); i++){
        Vec3f center = scene.vertex_data[spheres[i].center_vertex_id];
        double radius = spheres[i].radius;
        Vec3f origin = ray.origin;
        Vec3f direction = ray.direction;
        double a = dotProduct(direction, direction);
        double b = 2 * (direction.x * (origin.x - center.x) + direction.y * (origin.y - center.y) + direction.z * (origin.z - center.z));
        double c = dotProduct(origin - center, origin - center) - pow(radius, 2);
        double delta = pow(b, 2) - 4 * a * c;
        if (delta >= 0){
            double t1 = (-b + sqrt(delta)) / (2 * a);
            double t2 = (-b - sqrt(delta)) / (2 * a);
            double t = t1 < t2 ? t1 : t2;
            //std::cout << t << std::endl;
            if (t < t_min && t > 0){
                t_min = t;

                color.x = scene.materials[spheres[i].material_id].diffuse.x ;
                color.y = scene.materials[spheres[i].material_id].diffuse.y ;
                color.z = scene.materials[spheres[i].material_id].diffuse.z ;
            }
        }
    }
    
    if (t_min < MAX_VALUE ){
        color.x = 255;
        color.y = 255;
        color.z = 255;
        //std::cout << t_min << std::endl;
        //std::cout << color.x << " " << color.y << " " << color.z << std::endl;
    } 
    
    
    
    return color;
   
}

int main(int argc, char* argv[]){
    // Sample usage for reading an XML scene file
    parser::Scene scene;

    scene.loadFromXml(argv[1]);

    

    // The code below creates a test pattern and writes
    // it to a PPM file to demonstrate the usage of the
    // ppm_write function.
    //
    // Normally, you would be running your ray tracing
    // code here to produce the desired image.

    const RGB BAR_COLOR[8] =
    {
        { 255, 255, 255 },  // 100% White
        { 255, 255,   0 },  // Yellow
        {   0, 255, 255 },  // Cyan
        {   0, 255,   0 },  // Green
        { 255,   0, 255 },  // Magenta
        { 255,   0,   0 },  // Red
        {   0,   0, 255 },  // Blue
        {   0,   0,   0 },  // Black
    };

    int width = 640, height = 480;
    int columnWidth = width / 8;

    unsigned char* image = new unsigned char [width * height * 3];

    int i = 0;
    /*
    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            int colIdx = x / columnWidth;
            image[i++] = BAR_COLOR[colIdx][0];
            image[i++] = BAR_COLOR[colIdx][1];
            image[i++] = BAR_COLOR[colIdx][2];
        }
    }
    */

   // make changes here (adjust as needed based on the camera's length)

    
    
    int count = 0;
    std::cout<< "sphere size" << scene.spheres.size() << std::endl;
    Camera camera = scene.cameras[0];
    for (int y = 0; y < camera.image_height; y++){

        for (int x = 0; x < camera.image_width; x++){
             
            Vec3f m = camera.position + (camera.gaze)*camera.near_distance;
            Vec3f u = crossProduct(camera.up,  - camera.gaze);
            Vec3f q = m + u * camera.near_plane.x +  camera.up * camera.near_plane.w ;
            //ok std::cout << q.x << " " << q.y << " " << q.z << std::endl;

            double s_u = ((float) x + 0.5) * (camera.near_plane.y - camera.near_plane.x) / (float)camera.image_width;
            double s_v = ((float) y + 0.5) * (camera.near_plane.w - camera.near_plane.z) / (float)camera.image_height;

            

            Vec3f s = q + u * s_u - camera.up * s_v;
            // ok std::cout << s.x << " " << s.y << " " << s.z << std::endl;
            Vec3f d = s - camera.position;
            // ok std::cout << d.x << " " << d.y << " " << d.z << std::endl;
            Ray ray;
            ray.origin = camera.position;
            ray.direction = d;
            normalize(ray.direction);
            //std::cout << camera.position.x << " " << camera.position.y << " " << camera.position.z << "+" << d.x << " " << d.y << " " << d.z << "t"  << std::endl;
            Vec3f color = RaySphereIntersect(scene.spheres, ray, scene);
            if(color.x != scene.background_color.x || color.y != scene.background_color.y || color.z != scene.background_color.z){
                std::cout << color.x << " " << color.y << " " << color.z << std::endl;
                count++;
            }
            image[i++] = color.x;
            image[i++] = color.y;
            image[i++] = color.z;
            //std::cout << ray.direction.x << " " << ray.direction.y << " " << ray.direction.z << std::endl;
            //std::cout << ray.origin.x << " " << ray.origin.y << " " << ray.origin.z << std::endl;
            
        }
   }

    std::cout << scene.spheres.size() << std::endl;
    std::cout << count << std::endl;
   

    write_ppm("test.ppm", image, width, height);

}
