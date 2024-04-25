#include <fstream>

struct Svg
{
    std::string code;
    std::string filename;
    uint32_t generation = 0;

    Svg(std::string name):
        filename(name)
    {}

    std::string
    bake() const
    {

        std::string pre = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\
        <!-- Created with Inkscape (http://www.inkscape.org/) -->\
<svg\
   width=\"200mm\"\
   height=\"200mm\"\
   viewBox=\"-100 -100 200 200\"\
   version=\"1.1\"\
   id=\"svg1\"\
   xmlns=\"http://www.w3.org/2000/svg\"\
   xmlns:svg=\"http://www.w3.org/2000/svg\">\
  <defs\
     id=\"defs1\" />\
  <g\
     id=\"layer1\"> ";

                                    
        std::string post = "  </g> \
</svg>";
        return pre + code + post;

                                    
                                    
    }

    ~Svg()
    {
        std::ofstream myfile;
        myfile.open (filename);
        myfile << bake();
        myfile.close();
    }
    
    Svg &
    circle(const float x,
           const float y,
           const float r,
           const std::string & fill_color,
           const std::string & stroke_color,
           const float thickness)
    {
        const std::string name = "path" + std::to_string(generation++);
        const std::string circle_code = "<circle style=\"fill:" + fill_color + ";stroke:" + stroke_color + ";stroke-width:"+std::to_string(thickness)+"\" id=\""+name+"\" cx=\""+std::to_string(x)+"\" cy=\""+std::to_string(y)+"\" r=\"" + std::to_string(r) + "\" />\n";
        code += circle_code ;
        
        return *this;
    }

    Svg &
    line(float u,
         float v,
         float w,
         const std::string & stroke_color,
         const float thickness)
    {
        const std::string name = "path" + std::to_string(generation++);

        const float norm = std::sqrt(u*u+v*v);
        u /= norm;
        v /= norm;
        w /= norm;
        const float x1 = u*w - v*300.;
        const float x2 = u*w + v*300.;
        const float y1 = v*w + u*300.;
        const float y2 = v*w - u*300.;
        
        code += "<path   style=\"fill:none;stroke:"+stroke_color+";stroke-width:"+std::to_string(thickness)+";stroke-linecap:butt;stroke-linejoin:miter;stroke-opacity:1\" d=\"M "+std::to_string(x1)+","+std::to_string(y1)+" "+std::to_string(x2)+","+std::to_string(y2)+"\" id=\""+name+"\" />\n";
        return *this;
    }

    Svg &
    cga(const Mvec & blade)
    {
        const int g = blade.grade();
        switch (g)
        {
            case 1:
            {
                const float x = blade | e_1;
                const float y = blade | e_2;
                const float w = -blade | e_i;
                circle(x/w, y/w, 1.5, "#000000", "none", 0.0);
                break;
            }
            case 3:
            {
                auto dual = blade.dual();
                const float w = -dual | e_i;
                if (std::abs(w) < 0.001)
                { // line todo (maybe)
                    const float u =  dual | e_1;
                    const float v =  dual | e_2;
                    line(u,v, -dual | e_o, "#ff0000", 0.5);
                    
                }
                else
                {
                    dual /= w;
                    const float x = dual | e_1;
                    const float y = dual | e_2;
                    const float r2 = dual | dual;
                    const float r = std::sqrt(std::abs(r2));
                    circle(x, y, r, "none", (r2 >= 0)? "#002b36" : "#0000ff", 0.3);
                }
                break;
            }
            default:
            {
            
            }
        }
        return *this;
       
    }
    
};



