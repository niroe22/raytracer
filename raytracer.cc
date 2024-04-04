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

// Sie benötigen eine Implementierung von Lambertian-Shading, z.B. als Funktion
// Benötigte Werte können als Parameter übergeben werden, oder wenn diese Funktion eine Objektmethode eines
// Szene-Objekts ist, dann kann auf die Werte teilweise direkt zugegriffen werden.
// Bei mehreren Lichtquellen muss der resultierende diffuse Farbanteil durch die Anzahl Lichtquellen geteilt werden.

// Für einen Sehstrahl aus allen Objekte, dasjenige finden, das dem Augenpunkt am nächsten liegt.
// Am besten einen Zeiger auf das Objekt zurückgeben. Wenn dieser nullptr ist, dann gibt es kein sichtbares Objekt.

// Die rekursive raytracing-Methode. Am besten ab einer bestimmten Rekursionstiefe (z.B. als Parameter übergeben) abbrechen.

template <class FLOAT, size_t N>
Vector<FLOAT,N> ray_color(const Ray<FLOAT,N>& r){
    return Vector3df{0,0,0};

}

int main(void) {
    // Bildschirm erstellen
    // Kamera erstellen
    // Für jede Pixelkoordinate x,y
    //   Sehstrahl für x,y mit Kamera erzeugen
    //   Farbe mit raytracing-Methode bestimmen
    //   Beim Bildschirm die Farbe für Pixel x,y, setzten

    //Image
    float aspect_ratio = 16.0/9.0;
    int image_width = 256;

    int image_height = static_cast<int>(image_width / aspect_ratio);
    image_height = (image_height < 1) ? 1 : image_height;

    float focal_length = 1.0;
    float viewport_height = 2.0;
    float viewport_width = viewport_height * (static_cast<double>(image_width)/image_height);
    auto camera_center = Vector3df {0,0,0};

    auto viewport_u = Vector3df {viewport_width, 0, 0};
    auto viewport_v = Vector3df {0, -viewport_height, 0};

    auto pixel_delta_u = viewport_u.operator/=(image_width);
    auto pixel_delta_v = viewport_v.operator/=(image_height);

    auto viewport_upper_left = camera_center.operator-=(Vector3df{0, 0, focal_length})
                               - viewport_u.operator/=(2) - viewport_v.operator/=(2);
    auto pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);
    //Render
    std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";
    for (int j = 0; j < image_height; ++j) {
        std::clog<<"\r Scanline remaining: "<<(image_height - j) << ' ' <<std::flush;
        for (int i = 0; i < image_width; ++i) {
            auto pixel_color = Vector3df {float(i)/((float)image_width-1), float(j)/((float)image_height-1), 0};
            write_color(std::cout, pixel_color);
        }
    }
    std::clog <<"\rDone.                \n";
    return 0;
}

