#include<emscripten.h>
#include<emscripten/bind.h>
#include"lexer.hpp"
#include<sstream>

std::unordered_map<std::string, double> variables;
std::unordered_map<std::string, lex::Func> functions;

double val_sqrt(double d) { return sqrtf(d); }
double val_abs(double d) { return abs(static_cast<int>(d)); }
double val_print(double d) {
    //stream << "value of variable: " << d << "\n";
    return d;
}
double val_cosf(double d) { return cosf(d); }
double val_sinf(double d) { return sinf(d); }
double val_log(double d) { return logf(d); }
double val_tan(double d) { return tanf(d); }
double val_rand(double d) { return std::rand()%static_cast<int>(d); }
double val_exit(double /*d*/) {
    return 0;
}

void add_func(std::initializer_list<lex::Func> lst) {
    for(auto &i : lst) {
        functions[i.name] = i;
    }
}

void initFunctions() {
    std::srand(static_cast<unsigned int>(std::time(0)));
    add_func({
        {"sqrt",val_sqrt}, {"abs",val_abs},
        {"print",val_print}, {"cos",val_cosf},
        {"sin",val_sinf}, {"tan",val_tan},
        {"log",val_log}, {"exit",val_exit},
        {"rand",val_rand}
    });
}
/* end temporary */

class ScanExport {
public:

    ScanExport() {
        initFunctions();
    }

    std::string scanLex(std::string text) {
        std::ostringstream stream;
        if(text.find(";")  == std::string::npos) {
            stream << "Error missing semi-colon\n";
            return stream.str();
        }

        stream << "<table border=\"1\">\n<tr><th>Variable</th><th>Value</th></tr>\n";
        std::istringstream stream_in(text);
        lex::Parser pscan(stream_in, variables, functions);
        if(pscan.eval()) {
            for (auto &s : variables) {
             stream << "<tr><th>" << s.first << "</th><th>" << s.second << "</th></tr>\n";
            }
        }
        stream << "</table>\n";
        return stream.str();
    }

};

using namespace emscripten;

EMSCRIPTEN_BINDINGS(my_Scan) {
    class_<ScanExport>("Scan")
        .constructor()
        .function("scanLex", &ScanExport::scanLex)
    ;
}