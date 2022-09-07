#include "dataAnalysis.h"

dataAnalysis *dataAnalysis::_ptr = nullptr;
dataAnalysis::dataAnalysis() {}

dataAnalysis::~dataAnalysis() {
  if (_ptr)
    delete _ptr;
}

dataAnalysis &dataAnalysis::GetInstance() {
  if (_ptr) {
    return *_ptr;
  } else {
    _ptr = new dataAnalysis;
  }
}

int dataAnalysis::GetTotalSize(const std::string &inString,
                               const char *keyword) {
  if (inString.empty() || !keyword || !(*keyword))
    return 0;

  Json::Reader reader;
  Json::Value root;

  int iCount = 0;

  if (reader.parse(inString, root)) {
    iCount = root[keyword].asInt64();
  }
  return iCount;
}

bool dataAnalysis::ParseData(const std::string &inString, const char *subLable,
                             ItemsDataList &pOut) {
  if (inString.empty())
    return false;

  Json::Reader reader;
  Json::Value root;
  if (reader.parse(inString, root)) {
    if (subLable) {
      if (root[subLable].isArray()) {
        handleArray(root[subLable], pOut);
        return true;
      } else {
        return false;
      }
    } else {
      if (root.isObject()) {
        handleObject(root, pOut);
        return true;
      } else {
        return false;
      }
    }
  } else {
    return false;
  }
}

void dataAnalysis::handleObject(const Json::Value &root, ItemsDataList &pOut) {
  // Get first iteam's members
  auto names = root.getMemberNames();

  OneItemDataList onelist;
  for (auto i = 0; i < root.size(); i++) {
    void *pData = nullptr;
    DataType datatype = DataType::DATATYPE_UNKNOWN;
    switch (root[names[i]].type()) {
    case Json::ValueType::objectValue: {
      pData = new ItemsDataList;
      ItemsDataList &iData = *(ItemsDataList *)pData;
      handleObject(root[names[i]], iData);

      onelist.push_back({names[i], DataType::DATATYPE_OBJECT, pData});
    } break;
    case Json::ValueType::arrayValue: {
      pData = new ItemsDataList;
      ItemsDataList &iData = *(ItemsDataList *)pData;
      handleArray(root[names[i]], iData);

      onelist.push_back({names[i], DataType::DATATYPE_ARRAY, pData});
    } break;
    default: {
      ItemData data;
      data.name = names[i];
      handleNormal(root[names[i]], data);
      onelist.push_back(data);
    } break;
    }
  }
  pOut.push_back(onelist);
}

void dataAnalysis::handleArray(const Json::Value &root, ItemsDataList &pOut) {
  for (auto i = 0; i < root.size(); i++) {
    const auto &iteam = root[i];
    switch (iteam.type()) {
    case Json::ValueType::arrayValue: {
      handleArray(iteam, pOut);
    } break;
    case Json::ValueType::objectValue: {
      handleObject(iteam, pOut);
    } break;
    default:
      break;
    }
  }
}

void dataAnalysis::handleNormal(const Json::Value &root, ItemData &pOut) {
  DataType datatype = DataType::DATATYPE_UNKNOWN;
  switch (root.type()) {
  case Json::ValueType::nullValue: {
    datatype = DataType::DATATYPE_NULL;
  } break;
  case Json::ValueType::intValue:
  case Json::ValueType::uintValue:
  case Json::ValueType::realValue: {
    datatype = DataType::DATATYPE_INT;
  } break;
  case Json::ValueType::stringValue: {
    datatype = DataType::DATATYPE_STRING;
  } break;
  case Json::ValueType::booleanValue: {
    datatype = DataType::DATATYPE_BOOLEAN;
  } break;
  case Json::ValueType::objectValue: {
    datatype = DataType::DATATYPE_OBJECT;
  } break;
  case Json::ValueType::arrayValue: {
    datatype = DataType::DATATYPE_ARRAY;
  } break;
  default:
    break;
  }

  if (datatype != DataType::DATATYPE_UNKNOWN) {
    void *pData = nullptr;
    switch (datatype) {
    case DataType::DATATYPE_NULL: {
      pData = new int(0);
    } break;
    case DataType::DATATYPE_INT: {
      pData = new int;
      int &iData = *(int *)pData;
      iData = root.asInt64();
    } break;
    case DataType::DATATYPE_STRING: {
      pData = new std::string;
      std::string &iData = *(std::string *)pData;
      if (root.asCString() && *root.asCString()) {
        iData = root.asCString();
      }
    } break;
    case DataType::DATATYPE_BOOLEAN: {
      pData = new bool;
      bool &iData = *(bool *)pData;
      iData = root.asBool();
    } break;
    default:
      break;
    }
    pOut.type = datatype;
    pOut.value = pData;
  }
}