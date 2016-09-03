#include <iostream>
#include <fstream>
//#include <etk/etk.h>
#include <chrono>
#include <iomanip>

#include "include/lexer.h"
#include "include/parser.h"
#include "include/printer.h"
#include "include/generator.h"

using namespace std;

int main(int argc, char *argv[])
{
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

    int pos = 1;
    string outpath = "./";
    vector<string> filenames;
    bool create_headers = false;
    while(pos < argc)
    {
        string arg = argv[pos++];
        if(arg == "-o")
        {
            outpath = argv[pos++];
            outpath += "/";
        }
        else if(arg == "-h")
        {
            create_headers = true;
        }
        else
        {
            filenames.push_back(arg);
        }
    }

    if(!filenames.size())
    {
        cout << "fatal error: no input files\ncompilation terminated." << endl;
        return -1;
    }

    ofstream cofheader(outpath + "coffee_header.h");
    if(!cofheader.is_open())
    {
        cout << "Couldn't open coffee header at : " << outpath + "coffee_header.h" << endl;
        return -1;
    }
    cofheader << "#ifndef COFFEE_CAT_PROJECT_HEADER\n#define COFFEE_CAT_PROJECT_HEADER\n\n";

    for(auto& f : filenames)
    {
        std::ifstream ifs(f);
        string code = string((std::istreambuf_iterator<char>(ifs)),
                             (std::istreambuf_iterator<char>()));
        try
        {
            Lexer lexer(code);
            NBlock head;
            Parser parser(lexer);
            /*
                        auto e = make_shared<NClass>();
                        parser.parse_class(e);
            */
            parser.parse(head);
            /*
                        auto el = make_shared<NBlock>();
                        parser.parse_block_item(head);

                        head.items.push_back(el);
            */
            Printer printer;
            //head.Accept(&printer);
            Generator gen;
            string fname = f;
            string base_fname = fname.substr(0, fname.find_last_of("."));
            cofheader << "#include \"" << base_fname << ".h\"\n";
            cout << base_fname << endl;
            gen.generate(&head, base_fname);
            ofstream hout(outpath + base_fname+".h");
            hout << gen.header;
            hout.close();
            ofstream sout(outpath + base_fname+".cpp");
            sout << gen.source;
            sout.close();
        }
        catch(LexerError le)
        {
            cout << "Lexer error: " << le.msg << endl;
            return -1;
        }
        catch(ParseError pe)
        {
            cout << "Parse error Line: " << pe.tok.line << " Col: " << pe.tok.col << " " << pe.msg << endl;
            return -1;
        }
        catch(CompilerError ce)
        {
            cout << ce.msg << endl;
            return -1;
        }
        catch(IndentError e)
        {
            cout << "Indentation error on line " << e.tok.line << endl;
            return -1;
        }
        catch(std::string msg)
        {
            cout << msg << endl;
        }
        catch(const char* msg)
        {
            cout << msg << endl;
        }

    }

    cofheader << "\n#endif\n";
    cofheader.close();

    std::chrono::steady_clock::time_point end= std::chrono::steady_clock::now();

    std::cout << "Success. " << std::setprecision(3) << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count()/1000000.0 << " s" << std::endl;

    return 0;
}
