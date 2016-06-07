#include <iostream>
#include <fstream>
#include <etk/etk.h>

#include "include/lexer.h"
#include "include/parser.h"
#include "include/printer.h"
#include "include/generator.h"

using namespace std;

int main(int argc, char *argv[])
{
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
        }
        catch(ParseError pe)
        {
            cout << "Parse error Line: " << pe.tok.line << " Col: " << pe.tok.col << " " << pe.msg << endl;
        }
        catch(CompilerError ce)
        {
            cout << ce.msg << endl;
        }
        catch(IndentError e)
        {
            cout << "Indentation error on line " << e.tok.line << endl;
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

    return 0;
}
