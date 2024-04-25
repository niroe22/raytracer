#include "math.h"
#include "geometry.h"
#include "color.h"
#include <iostream>
#include <vector>
#include <algorithm>

// Die folgenden Kommentare beschreiben Datenstrukturen und Funktionen
// Die Datenstrukturen und Funktionen die weiter hinten im Text beschrieben sind,
// hängen höchstens von den vorhergehenden Datenstrukturen ab, aber nicht umgekehrt.



// Ein "Bildschirm", der das Setzen eines Pixels kapselt
// Der Bildschirm hat eine Auflösung (Breite x Höhe)
// Kann zur Ausgabe einer PPM-Datei verwendet werden oder
// mit SDL2 implementiert werden.



// Eine "Kamera", die von einem Augenpunkt aus in eine Richtung senkrecht auf ein Rechteck (das Bild) zeigt.
// Für das Rechteck muss die Auflösung oder alternativ die Pixelbreite und -höhe bekannt sein.
// Für ein Pixel mit Bildkoordinate kann ein Sehstrahl erzeugt werden.



// Für die "Farbe" benötigt man nicht unbedingt eine eigene Datenstruktur.
// Sie kann als Vector3df implementiert werden mit Farbanteil von 0 bis 1.
// Vor Setzen eines Pixels auf eine bestimmte Farbe (z.B. 8-Bit-Farbtiefe),
// kann der Farbanteil mit 255 multipliziert  und der Nachkommaanteil verworfen werden.


// Das "Material" der Objektoberfläche mit ambienten, diffusem und reflektiven Farbanteil.



// Ein "Objekt", z.B. eine Kugel oder ein Dreieck, und dem zugehörigen Material der Oberfläche.
// Im Prinzip ein Wrapper-Objekt, das mindestens Material und geometrisches Objekt zusammenfasst.
// Kugel und Dreieck finden Sie in geometry.h/tcc


// verschiedene Materialdefinition, z.B. Mattes Schwarz, Mattes Rot, Reflektierendes Weiss, ...
// im wesentlichen Variablen, die mit Konstruktoraufrufen initialisiert werden.


// Die folgenden Werte zur konkreten Objekten, Lichtquellen und Funktionen, wie Lambertian-Shading
// oder die Suche nach einem Sehstrahl für das dem Augenpunkt am nächsten liegenden Objekte,
// können auch zusammen in eine Datenstruktur für die gesammte zu
// rendernde "Szene" zusammengefasst werden.

// Die Cornelbox aufgebaut aus den Objekten
// Am besten verwendet man hier einen std::vector< ... > von Objekten.

// Punktförmige "Lichtquellen" können einfach als Vector3df implementiert werden mit weisser Farbe,
// bei farbigen Lichtquellen müssen die entsprechenden Daten in Objekt zusammengefaßt werden
// Bei mehreren Lichtquellen können diese in einen std::vector gespeichert werden.
class light {
public:
    Vector3df center;
    Vector3df color = {0.f, 0.f, 0.f};

    light(const Vector3df &center)
    : center(center){}
};
// Sie benötigen eine Implementierung von Lambertian-Shading, z.B. als Funktion
// Benötigte Werte können als Parameter übergeben werden, oder wenn diese Funktion eine Objektmethode eines
// Szene-Objekts ist, dann kann auf die Werte teilweise direkt zugegriffen werden.
// Bei mehreren Lichtquellen muss der resultierende diffuse Farbanteil durch die Anzahl Lichtquellen geteilt werden.


// Für einen Sehstrahl aus allen Objekte, dasjenige finden, das dem Augenpunkt am nächsten liegt.
// Am besten einen Zeiger auf das Objekt zurückgeben. Wenn dieser nullptr ist, dann gibt es kein sichtbares Objekt.

// Die rekursive raytracing-Methode. Am besten ab einer bestimmten Rekursionstiefe (z.B. als Parameter übergeben) abbrechen.
class wObject {
public:
    Sphere3df  sphere;
    Vector3df  color;
    bool reflective;

    wObject(): sphere({0.f, 0.f, 0.f}, 0.f), color({0.f, 0.f, 0.f}), reflective(false) {}
    wObject(const Sphere3df &s, const Vector3df &c, const bool &r )
    : sphere(s), color(c), reflective(r) {}
};

class worldObjects {
public:
    std::vector<wObject> objects;
    std::vector<light> lights;
    worldObjects();
    worldObjects(wObject object) { add(object); }

    void add(wObject object) { objects.push_back(object); }

    template<class FLOAT, size_t N>
    wObject* hit(const Ray<FLOAT, N> &r, Intersection_Context<FLOAT, N> &rec) {
        wObject nearest;
        Intersection_Context<FLOAT, N> temp_rec;
        float closest_so_far = std::numeric_limits<float>::max();

        for (const auto &object: objects) {
            if (object.sphere.intersects(r, temp_rec)) {
                if (closest_so_far > temp_rec.t && temp_rec.t > 0) {
                    closest_so_far = temp_rec.t;
                    nearest = object;
                    rec = temp_rec;
                }
            }
        }

        if(closest_so_far == std::numeric_limits<float>::max())
            return nullptr;

        return new wObject(nearest.sphere, nearest.color, nearest.reflective);
    }
};
template<class FLOAT, size_t N>
Vector<FLOAT, N> ray_color(const Ray<FLOAT, N> &r, worldObjects& world, int depth) {
    Intersection_Context<FLOAT, N> rec;
    wObject* object = world.hit(r, rec);
    if (world.hit(r, rec) != nullptr) {
        if(object->reflective) {
            Vector3df reflective_Vec = r.direction - 2.f * (r.direction * rec.normal) * rec.normal;
            Ray3df reflective_r = {rec.intersection + 0.1f * rec.normal, reflective_Vec};
            return ray_color(reflective_r, world, depth -1);
        }
        Vector3df lambertarian = (world.lights[0].center -  rec.intersection);
        lambertarian.normalize();
        float intensety = rec.normal * lambertarian;
        if ( intensety < 0){
            return 0.f * object->color;
        }
        return intensety * object->color;
    }
    return Vector3df {0.f, 0.f, 0.f};
}

int main(void) {
    // Bildschirm erstellen
    // Kamera erstellen
    // Für jede Pixelkoordinate x,y
    //   Sehstrahl für x,y mit Kamera erzeugen
    //   Farbe mit raytracing-Methode bestimmen
    //   Beim Bildschirm die Farbe für Pixel x,y, setzten

    //Image
    float aspect_ratio = 16.0 / 9.0;
    float image_width = 1000;

    int image_height = static_cast<int>(image_width / aspect_ratio);
    image_height = (image_height < 1) ? 1 : image_height;

    //rot
    worldObjects world(wObject(Sphere3df({3.f, -8.f, -13.f}, 2.f), Vector3df({1.f, 0.f, 0.f}), true));
    //Lila
    world.add(wObject(Sphere3df({-9.f, -8.f, -17.f}, 3.f), Vector3df({0.5f, 0.f, 0.5f}), false));

    //RechteWand
    world.add(wObject(Sphere3df({10021.f, 0.0f, 0.f }, 10000.f), Vector3df({0.f, 1.f, 0.f}), false));
    //Linkewand
    world.add(wObject(Sphere3df({ -10021.f, 0.0f, 0.f }, 10000.f), Vector3df({1.f, 1.f, 0.f}), false));
    //Boden
    world.add(wObject(Sphere3df({0.f, -10012.0f, 0.f}, 10000.f), Vector3df({0.f, 0.f, 1.f}), false));
    //Decke
    world.add(wObject(Sphere3df({0.f, 10012.0f, 0.f}, 10000.f), Vector3df({0.5f, 0.5f, 1.f}), false));
    //Rückwand
    world.add(wObject(Sphere3df({0.0f, 0.0f, -10030.f}, 10000.f), Vector3df({0.3f, 1.f, 1.f}), false));
    //Off wand
    world.add(wObject(Sphere3df({0.0f, 0.0f, 10030.f}, 10000.f), Vector3df({.9f, .2f, 0.f}), false));
    //world.add(wObject(Sphere3df({-10.f, 12.f, -18.f}, 1.f), Vector3df({0.f, 1.f, 1.f}), false));

    light left_light(Vector3df {-10.f, 11.f, -18.f});

    world.lights.push_back(left_light);

    float focal_length = 5.f;
    float viewport_height = 9.f;
    float viewport_width = viewport_height * (image_width/image_height);
    Vector3df camera_center = {0.f,0.f,0.f};

    // Calculate the vectors across the horizontal and down the vertical viewport edges.
    Vector3df viewport_u = Vector3df{viewport_width, 0.f, 0.f};
    Vector3df viewport_v = Vector3df{0.f, -viewport_height, 0.f};

    // Calculate the horizontal and vertical delta vectors from pixel to pixel.
    Vector3df pixel_delta_u = (1.f/image_width) * viewport_u;
    Vector3df pixel_delta_v = (1.f/image_height) * viewport_v;

    Vector3df viewport_upper_left = camera_center - Vector3df{0.f, 0.f, focal_length} - ((1/2.f) * viewport_u ) - ((1/2.f) * viewport_v);
    Vector3df pixel00_loc = viewport_upper_left + (0.5f * (pixel_delta_u + pixel_delta_v));

    //Render
    std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";
    for (int j = 0; j < image_height; ++j) {
        std::clog << "\r Scanline remaining: " << (image_height - j) << ' ' << std::flush;
        for (int i = 0; i < image_width; ++i) {
            auto pixel_center = pixel00_loc + ((float) i * pixel_delta_u) + ((float) j * pixel_delta_v);
            auto ray_dircetion = pixel_center + camera_center;

            Ray3df r = Ray3df({camera_center, ray_dircetion});
            Vector pixel_color = ray_color(r,world, 5);
            write_color(std::cout, pixel_color);
        }
    }
    std::clog << "\rDone.                \n";
    return 0;
}

