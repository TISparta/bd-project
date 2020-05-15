#pragma once

#include <fstream>

class Manager {
public:
  Manager () {}

  Manager (std::string file_path, bool trunc):
    file_path(file_path), trunc(trunc) {
    if (trunc) {
      file.open(file_path, std::ios::in | std::ios::out | std::ios::binary | std::ios::trunc);
    } else {
      file.open(file_path, std::ios::in | std::ios::out | std::ios::binary);
    }
  }

  Manager (const Manager& manager) {
    file_path = manager.file_path;
    trunc = manager.trunc;
    if (trunc) {
      file.open(file_path, std::ios::in | std::ios::out | std::ios::binary | std::ios::trunc);
    } else {
      file.open(file_path, std::ios::in | std::ios::out | std::ios::binary);
    }
  }

  ~Manager () {
    if (file.is_open()) {
      file.close();
    }
  }
  
  bool is_empty () {
    return (get_file_size() == 0);
  }
  
  int get_file_size () {
    file.clear();
    file.seekp(0, std::ios::end);
    return file.tellp();
  }

  template <class Record>
  void write (int pos, const Record& record) {
    file.clear();
    file.seekp(pos, std::ios::beg);
    file.write((char *)& record, sizeof record);
  }

  template <class Record>
  void read (int pos, Record& record) {
    file.clear();
    file.seekg(pos, std::ios::beg);
    file.read((char *)& record, sizeof record);
  }

public:
  std::string file_path;
  bool trunc;  
  std::fstream file;
};

