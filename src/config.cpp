#include "config.h"
#include "utils.h"


/**************************************************/
/***************** CONSTRUCTORS *******************/
/**************************************************/


Config::Config()
{
}


Config::~Config()
{
}


/**************************************************/
/***************** PUBLIC METHODS *****************/
/**************************************************/


void Config::init(const std::string &global_file)
{
  this->global_file = global_file;

  if (!file_exists(global_file)) {
    write_default();
  }
}


void Config::write_default()
{
  Json::Value fromScratch;

  fromScratch["global_fileig"]["viewport"]["width"] = 1280;
  fromScratch["global_fileig"]["viewport"]["height"] = 720;
  fromScratch["global_fileig"]["camera"]["fov"] = 45;
  fromScratch["global_fileig"]["ssao"]["sample_count"] = 16;
  fromScratch["global_fileig"]["ssao"]["distance_threshold"] = 5;
  fromScratch["global_fileig"]["ssao"]["filter_radius"] = 10;

  output(fromScratch);

  Json::StyledWriter styledWriter;
  std::cout << styledWriter.write(fromScratch);
}


void Config::output(const Json::Value & value)
{
    //std::cout << value["global_fileig"]["camera"];
}
