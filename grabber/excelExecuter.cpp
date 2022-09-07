#include "excelExecuter.h"

#define DEFAULT_LABEL_NAME "data"
const int DEFAULT_COLUMN_LENGTH = 10;

excelExecuter *excelExecuter::_ptr = nullptr;

excelExecuter::excelExecuter() {}

excelExecuter::~excelExecuter() {
  if (_ptr)
    delete _ptr;
}

excelExecuter &excelExecuter::GetInstance() {
  if (_ptr) {
    return *_ptr;
  } else {
    _ptr = new excelExecuter;
  }
}

bool excelExecuter::WriteToFile(const char *filePath,
                                const ItemsDataList &dataList,
                                const ConditionList &filterConditions) {
  if (!filePath || !(*filePath))
    return false;

  if (dataList.empty())
    return true;

  std::string filename = filePath;

  xlsxiowriter writer = nullptr;
  if ((writer = xlsxiowrite_open(filename.c_str(), DEFAULT_LABEL_NAME)) ==
      nullptr) {
    return false;
  }
  std::vector<std::string> vPositionNames;
  for (auto i = filterConditions.begin(); i != filterConditions.end(); i++) {
    vPositionNames.push_back(GetFliterName(*i));
  }
  // Writer row names
  TraverseHeader(*dataList.begin(), writer, vPositionNames);
  xlsxiowrite_next_row(writer);

  // Start to write data
  for (auto i = dataList.begin(); i != dataList.end(); i++) {
    TraverseData(*i, writer, vPositionNames);
  }

  xlsxiowrite_close(writer);

  return true;
}

bool excelExecuter::WriteBase(const char *filePath,
                              const ItemsDataList &dataList) {
  if (!filePath || !(*filePath))
    return false;

  if (dataList.empty())
    return true;

  return true;
}

void excelExecuter::TraverseHeader(const OneItemDataList &dataList,
                                   const xlsxiowriter writer,
                                   const std::vector<std::string> &filter) {
  for (auto i = dataList.begin(); i != dataList.end(); i++) {
    switch (i->type) {
    case DataType::DATATYPE_OBJECT:
    case DataType::DATATYPE_ARRAY: {
      TraverseSubHeader(i->value, writer, filter, i->name.c_str());
    } break;
    default: {
      std::string currentName = GetIteamName(*i, nullptr).c_str();
      if (!currentName.empty()) {
        auto found = find(filter.begin(), filter.end(), currentName);
        if (filter.empty() || found != filter.end()) {
          xlsxiowrite_add_column(writer, currentName.c_str(),
                                 DEFAULT_COLUMN_LENGTH);
        }
      }
    } break;
    }
  }
}

void excelExecuter::TraverseSubHeader(const void *dataList,
                                      const xlsxiowriter writer,
                                      const std::vector<std::string> &filter,
                                      const char *originName) {
  if (!dataList) {
    return;
  }

  ItemsDataList *subList = (ItemsDataList *)dataList;
  for (auto i = subList->begin(); i != subList->end(); i++) {
    for (auto j = i->begin(); j != i->end(); j++) {
      switch (j->type) {
      case DataType::DATATYPE_ARRAY:
      case DataType::DATATYPE_OBJECT: {
        TraverseSubHeader(j->value, writer, filter, originName);
      } break;
      default: {
        std::string currentName = GetIteamName(*j, originName).c_str();
        if (!currentName.empty()) {
          auto found = find(filter.begin(), filter.end(), currentName);
          if (filter.empty() || found != filter.end()) {
            xlsxiowrite_add_column(writer, currentName.c_str(),
                                   DEFAULT_COLUMN_LENGTH);
          }
        }
      } break;
      }
    }
  }
}

void excelExecuter::TraverseData(const OneItemDataList &dataList,
                                 const xlsxiowriter writer,
                                 const std::vector<std::string> &filter) {
  for (auto j = dataList.begin(); j != dataList.end(); j++) {
    switch (j->type) {
    case DataType::DATATYPE_NULL: {
      auto found = find(filter.begin(), filter.end(), j->name);
      if (filter.empty() || found != filter.end()) {
        const int data = *(int *)(j->value);
        xlsxiowrite_add_cell_int(writer, data);
      }
    } break;
    case DataType::DATATYPE_INT: {
      auto found = find(filter.begin(), filter.end(), j->name);
      if (filter.empty() || found != filter.end()) {
        const int data = *(int *)(j->value);
        xlsxiowrite_add_cell_int(writer, data);
      }
    } break;
    case DataType::DATATYPE_STRING: {
      auto found = find(filter.begin(), filter.end(), j->name);
      if (filter.empty() || found != filter.end()) {
        const std::string &data = *(std::string *)(j->value);
        if (data.length())
          xlsxiowrite_add_cell_string(writer, data.c_str());
        else
          xlsxiowrite_add_cell_string(writer, "null");
      }
    } break;
    case DataType::DATATYPE_BOOLEAN: {
      auto found = find(filter.begin(), filter.end(), j->name);
      if (filter.empty() || found != filter.end()) {
        const bool data = *(bool *)(j->value);
        xlsxiowrite_add_cell_int(writer, data ? 1 : 0);
      }
    } break;
    case DataType::DATATYPE_OBJECT:
    case DataType::DATATYPE_ARRAY: {
      TraverseSubData(j->value, writer, filter, j->name.c_str());
    } break;
    default:
      break;
    }
  }
  xlsxiowrite_next_row(writer);
}

void excelExecuter::TraverseSubData(const void *dataList,
                                    const xlsxiowriter writer,
                                    const std::vector<std::string> &filter,
                                    const char *originName) {
  if (!dataList) {
    return;
  }

  ItemsDataList *subList = (ItemsDataList *)dataList;
  for (auto i = subList->begin(); i != subList->end(); i++) {
    for (auto j = i->begin(); j != i->end(); j++) {
      switch (j->type) {
      case DataType::DATATYPE_NULL: {
        std::string currentName = GetIteamName(*j, originName).c_str();
        auto found = find(filter.begin(), filter.end(), currentName);
        if (filter.empty() || found != filter.end()) {
          const int data = *(int *)(j->value);
          xlsxiowrite_add_cell_int(writer, data);
        }
      } break;
      case DataType::DATATYPE_INT: {
        std::string currentName = GetIteamName(*j, originName).c_str();
        auto found = find(filter.begin(), filter.end(), currentName);
        if (filter.empty() || found != filter.end()) {
          const int data = *(int *)(j->value);
          xlsxiowrite_add_cell_int(writer, data);
        }
      } break;
      case DataType::DATATYPE_STRING: {
        std::string currentName = GetIteamName(*j, originName).c_str();
        auto found = find(filter.begin(), filter.end(), currentName);
        if (filter.empty() || found != filter.end()) {
          const std::string &data = *(std::string *)(j->value);
          if (data.length())
            xlsxiowrite_add_cell_string(writer, data.c_str());
          else
            xlsxiowrite_add_cell_string(writer, "null");
        }
      } break;
      case DataType::DATATYPE_BOOLEAN: {
        std::string currentName = GetIteamName(*j, originName).c_str();
        auto found = find(filter.begin(), filter.end(), currentName);
        if (filter.empty() || found != filter.end()) {
          const bool data = *(bool *)(j->value);
          xlsxiowrite_add_cell_int(writer, data ? 1 : 0);
        }
      } break;
      case DataType::DATATYPE_OBJECT:
      case DataType::DATATYPE_ARRAY: {
        std::string currentName = GetIteamName(*j, originName).c_str();
        TraverseSubData(j->value, writer, filter, currentName.c_str());
      } break;
      default:
        break;
      }
    }
  }
}

std::string excelExecuter::GetIteamName(const ItemData &dataList,
                                        const char *prevString) {
  std::string strFullString;
  if (prevString && *prevString) {
    strFullString.append(prevString);
    strFullString.append(".");
  }
  strFullString.append(dataList.name);

  switch (dataList.type) {
  case DataType::DATATYPE_OBJECT:
  case DataType::DATATYPE_ARRAY: {
    return GetIteamName((const ItemData &)dataList.value,
                        strFullString.c_str());
  } break;
  default:
    break;
  }

  return strFullString;
}

std::string excelExecuter::GetFliterName(const Condition &cdt,
                                         const char *prevString) {
  std::string strFullString;
  if (prevString && *prevString) {
    strFullString.append(prevString);
    strFullString.append(".");
  }
  strFullString.append(cdt.main);

  if (cdt.sub)
    return GetFliterName(*(cdt.sub), strFullString.c_str());
  else
    return strFullString;
}