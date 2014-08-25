#include <cassert>

#include <iostream>
#include <fstream>
//#include <regex>
#include <string>

#include <boost/regex.hpp>

//using namespace std;
using namespace boost;

regex expression("([0-9]+)(\\-| |$)(.*)");

int process_ftp(const char* response, std::string* msg)
{
   cmatch what;
   if(regex_match(response, what, expression))
   {
      // what[0] contains the whole string
      // what[1] contains the response code
      // what[2] contains the separator character
      // what[3] contains the text message.
      if(msg)
         msg->assign(what[3].first, what[3].second);
      return std::atoi(what[1].first);
   }
   // failure did not match
   if(msg)
      msg->erase();
   return -1;
}

void parse_timing_file(const std::string& file_path) {
  std::ifstream input_file(file_path);
  //assert(input_file.is_open());

  /*
  boost::regex re(".*epim_.*",
  //regex re(".*epim_[0-9]+_[0-9]+_timing.txt:[ ]*Data Path Delay:[ ]*[0-9.]+ns.*",
           boost::regex::extended);
  while (!input_file.eof()) {
    string line;
    getline(input_file, line);
    if (!line.empty() && !boost::regex_match(line.c_str(), what, re)) {
      cout << "Couldn't match for line:" << endl;
      cout << line << endl;
      exit(1);
    }
  }
  */
}

int main(int argc, char* argv[]) {
  assert(argc == 3);
  std::string flag(argv[1]);
  assert(flag == "-t" || flag == "-a");
  if (flag == "-t") {
    parse_timing_file(argv[2]);
  } else {
    exit(0);
  }
}
