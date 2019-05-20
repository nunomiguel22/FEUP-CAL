#ifndef INTERFACE_UTILS_FILEMANAGER_H_
#define INTERFACE_UTILS_FILEMANAGER_H_

#include <iostream>
#include <fstream>
#include <vector>

#define BASE_DIR "./T01"
#define BASE_DIR_CLION "./../source/T01/"

#define EDGES_FILE "T01_edges_"
#define LAT_LON_FILE "T01_lat_lon_"
#define NODES_X_Y_FILE "T01_nodes_X_Y_"
#define TAGS_FILE "T01_tags_"
#define FILE_EXT ".txt"

class FileManager {
 private:
  std::vector<std::string> fileLines;
  std::fstream file;
  void convFileToVector();

 public:
  explicit FileManager(const std::string& file);
  std::string trimSpaces(const std::string& s);
  std::vector<std::string> explode(char c, const std::string& s);
  std::vector<std::string> getVectorFileLines();
  void printFile();
  void closeFileStream();

};

#endif //INTERFACE_UTILS_FILEMANAGER_H_