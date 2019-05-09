#include "../parameters/ParamParser_getopt.hpp"
#include "../io/fileIO.hpp"
#include "../io/histogramIO.hpp"
#include "../io/imageIO.hpp"
#include <chrono>
#include <fstream>

using namespace utk;

#define D 2

int main(int argc, char** argv)
{
  bool displayhelp=false;
  std::string param_input;
  std::string param_output;
  
  utk::ParamParser_getopt parser;
  parser.addShortOption('i', &param_input, 1, utk::assignString, utk::displayString, (char*)"\t <string> The input pointset", (char*)"Input: ");
  parser.addShortOption('o', &param_output, 1, utk::assignString, utk::displayString, (char*)"\t <string> The output image", (char*)"Ouput: ");
  parser.addShortOption('h', &displayhelp, 0, utk::assignBoolTrue, utk::displayBool, (char*)"\t Displays this help message");
 
  parser.parse(argc, argv);
  
  if(displayhelp)
  {
    std::cout << parser.getHelp() << std::endl;
    return 0;
  }
  if(param_input.empty())
  {
    ERROR("Param -i mandatory");
    std::cout << parser.getHelp() << std::endl;
    return 1;
  }
  if(param_output.empty())
  {
    ERROR("Param -o mandatory");
    std::cout << parser.getHelp() << std::endl;
    return 1;
  }
  
  parser.displayState();
  
  PointsetReader<2, uint, Point<2, uint>> pts_reader;
  pts_reader.open(param_input);
  
  Pointset<2, uint, Point<2, uint> > pts;
  PointsetWriter<2, double, Point<2,double>> writer;
  writer.open(param_output.c_str());
  while(pts_reader.readPointset(pts))
  {
    Pointset<2, double, Point<2, double> > ptsout;
    ptsout.resize(pts.size());

    
    for(uint i=0; i<pts.size(); i++)
      for(uint d=0; d<2; d++)
        ptsout[i].pos()[d] = ((double)pts.at(i).pos()[d]) / (double)pts.size();
    
    //pts.normalizePositions(ptsout);
    
    writer.writePointset(ptsout);
  }
  
  pts_reader.close();
  writer.close();
  
  return 0;
}
