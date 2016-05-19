#include <iostream>
#include <fstream>
#include <algorithm>


#include "include/lexer.h"
#include "include/parser.h"
#include "include/printer.h"
#include "include/generator.h"
#include "include/cxxopts.hpp"

using namespace std;

int main(int argc, char *argv[])
{
    cxxopts::Options options(argv[0], " - example command line options");
    options.add_options()
      ("o,output", "Output directory", cxxopts::value<std::string>())
      ("positional",
        "" , cxxopts::value<std::vector<std::string>>())
    ;

    options.parse_positional(std::vector<std::string>({"output", "positional"}));
    options.parse(argc, argv);

    std::vector<std::string> filenames;
    if (options.count("positional"))
    {
      filenames = options["positional"].as<std::vector<std::string>>();
      for (const auto& s : filenames) {
        std::cout << s << ", ";
      }
      std::cout << std::endl;
    }

    if(!filenames.size())
    {
        cout << "fatal error: no input files\ncompilation terminated." << endl;
        return -1;
    }

    for(auto& f : filenames)
    {
        std::ifstream ifs(filenames[0]);
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
            head.Accept(&printer);
            Generator gen;
            string fname = argv[1];
            string base_fname = fname.substr(0, fname.find_last_of("."));
            gen.generate(&head, base_fname);
            ofstream hout("out/" + base_fname+".h");
            hout << gen.header;
            hout.close();
            ofstream sout("out/" + base_fname+".cpp");
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

    return 0;
}
