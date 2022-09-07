#include "gitOperator.h"
#include <experimental/filesystem>
#include <iostream>
#include <processenv.h>

namespace GithubGrabber {
gitOperator::gitOperator() {
  gitExe[0] = '\0';
  bInit = false;
#ifdef _WIN32
  sprintf(timeStamp, "%lld", time(NULL));
#endif
}

gitOperator::~gitOperator() {
  std::string tmpFolder = GetTmpPath();
  if (tmpFolder.size()) {
    std::error_code errorCode;
    if (!std::experimental::filesystem::remove_all(tmpFolder, errorCode)) {
      std::cout << "Remove directory failed: " << errorCode.message()
                << std::endl;
    }
  }
}

bool gitOperator::Init() {
  if (bInit)
    return true;

  return FindGitExecutable();
}

bool gitOperator::AddRemote(std::string remote, gitRemote &outRemote,
                            gitRemote &originRemote) {
  if (!bInit || (!outRemote.isEmpty() && originRemote.isEmpty()))
    return false;

  if (originRemote.isEmpty()) {
    return newRemote(remote, outRemote);
  } else {
    char szNewRemoteName[64];
    sprintf(szNewRemoteName, "%lld", time(NULL));
    std::string strCmd = "remote add ";
    strCmd += szNewRemoteName;
    strCmd += " ";
    strCmd += remote;

    if (execGit(strCmd, originRemote, originRemote.szLocation)) {
      strcpy(outRemote.szLocation, originRemote.szLocation);
      strcpy(outRemote.szRemoteUrl, remote.c_str());
      strcpy(outRemote.szRemoteName, szNewRemoteName);
      strcpy(outRemote.szBranchName, originRemote.szBranchName);

      return true;
    } else {
      return false;
    }
  }
  return false;
}

bool gitOperator::changeBranch(gitRemote &remote, std::string branch) {
  if (!bInit || remote.isEmpty())
    return false;

  std::string strCmd = "checkout -b ";
  strCmd += branch;
  strCmd += " remotes/origin/";
  strCmd += branch;

  if (execGit(strCmd, remote, remote.szLocation)) {
    strcpy(remote.szBranchName, branch.c_str());
    return true;
  } else {
    return false;
  }
}

bool gitOperator::PushLatestChanges(gitRemote &remote, std::string branch) {
  if (!bInit)
    return false;

  std::string strCmd = "push ";
  strCmd += remote.szRemoteName;
  strCmd += " ";
  if (branch.size())
    strCmd += branch;
  else
    strCmd += remote.szBranchName;

  return execGit(strCmd, remote, remote.szLocation);
}

bool gitOperator::PullLatestChanges(gitRemote &remote, gitRemote &toRemote) {
  if (!bInit)
    return false;

  std::string strCmd = "pull ";
  if (toRemote.isEmpty()) {
    strCmd += remote.szRemoteName;
    strCmd += " ";
    strCmd += remote.szBranchName;
  } else {
    strCmd += toRemote.szRemoteName;
    strCmd += " ";
    strCmd += toRemote.szBranchName;
  }
  return execGit(strCmd, remote, remote.szLocation);
}

bool gitOperator::FindGitExecutable() {
#ifdef _WIN32
  int cacheSize = 512;
  char *pszPath = new char[cacheSize];
  DWORD env = GetEnvironmentVariableA("PATH", pszPath, cacheSize);
  if (!env) {
    std::cout << "Get envirnnment PATH failed" << std::endl;
    delete[] pszPath;
    return false;
  }
  if (env >= cacheSize) {
    cacheSize = env + 1;
    delete[] pszPath;
    pszPath = new char[cacheSize];
    env = GetEnvironmentVariableA("PATH", pszPath, cacheSize);
  }
  std::string strPATHS = pszPath;
  delete[] pszPath;

  std::cout << "ENV PATH: " << std::endl << strPATHS << std::endl;

  auto pos = 0;
  while (strPATHS.size() > 1) {
    pos = strPATHS.find_first_of(";");
    if (pos == std::string::npos) {
      std::cout << "Find next PATH failed." << std::endl;
      break;
    }
    std::string strCurrPATH = strPATHS.substr(0, pos);

    if (strCurrPATH.empty()) {
      strPATHS.erase(0, pos + 1);
      pos = 0;
      continue;
    }
#ifdef _WIN32
    if (strCurrPATH.at(strCurrPATH.size() - 1) != '\\')
      strCurrPATH += "\\";
    strCurrPATH += "git.exe";
#else
    if (strCurrPATH.at(strCurrPATH.size() - 1) != '/')
      strCurrPATH += "/";
    strCurrPATH += "git";
#endif

    std::cout << "Checking " << strCurrPATH << " exist..." << std::endl;

    strPATHS.erase(0, pos + 1);
    pos = 0;

    std::error_code code;
    if (std::experimental::filesystem::exists(strCurrPATH, code)) {
      std::cout << "Found git: " << strCurrPATH << std::endl;
      gitExe = strCurrPATH;
      bInit = true;

      break;
    } else {
      std::cout << "Not found." << std::endl;
    }
  }

  return bInit;
#endif
  return false;
}

std::string gitOperator::GetTmpPath() {
#ifdef _WIN32
  char szTmpDirectory[MAX_PATH] = {0};
  LPSTR;
  GetModuleFileNameA(NULL, szTmpDirectory, MAX_PATH);
  std::string::size_type pos = std::string(szTmpDirectory).find_last_of("\\");
  szTmpDirectory[pos] = '\0';
  strcat(szTmpDirectory, "\\tmp");

  if (std::experimental::filesystem::exists(szTmpDirectory))
    return szTmpDirectory;

  if (!std::experimental::filesystem::create_directory(szTmpDirectory))
    return std::string();

  return szTmpDirectory;
#endif
  return std::string();
}

bool gitOperator::execGit(std::string command, gitRemote &remote,
                          const char *workingDirectory) {
  if (!bInit)
    return false;

  std::string strCmd = gitExe;
  strCmd += " ";
  strCmd += command;

  if (!workingDirectory) {
    strCmd += " \"";
    strCmd += remote.szLocation;
    strCmd += "\"";
  }

#ifdef _WIN32
  STARTUPINFO si;
  PROCESS_INFORMATION pi;
  ZeroMemory(&si, sizeof(si));
  si.cb = sizeof(si);
  ZeroMemory(&pi, sizeof(pi));
  BOOL ret = CreateProcessA(NULL, const_cast<char *>(strCmd.c_str()), NULL,
                            NULL, FALSE, 0, NULL, workingDirectory, &si, &pi);
  if (ret == TRUE)
    WaitForSingleObject(pi.hProcess, INFINITE);

  DWORD exitCode;
  GetExitCodeProcess(pi.hProcess, &exitCode);

  if (exitCode != 0) {
    std::cout << "failed to run git command: " << std::endl
              << strCmd << std::endl;
    return false;
  }

  return ret == TRUE;
#endif
  return false;
}
bool gitOperator::newRemote(std::string remote, gitRemote &outRemote) {
  std::string tmpPATH = GetTmpPath();
  std::hash<std::string> remoteHash;
  char szHASH[128];
  sprintf(szHASH, "%I64d", (size_t)remoteHash(remote));
  tmpPATH += "\\";
  tmpPATH += szHASH;
  tmpPATH += "_";
  tmpPATH += timeStamp;

  if (std::experimental::filesystem::exists(tmpPATH)) {
    return false;
  }

  // if (!std::experimental::filesystem::create_directory(tmpPATH))
  //    return false;

  strcpy(outRemote.szLocation, tmpPATH.c_str());
  strcpy(outRemote.szRemoteUrl, remote.c_str());
  strcpy(outRemote.szRemoteName, "origin");
  strcpy(outRemote.szBranchName, "master");

  std::string opCmd = "clone ";
  opCmd += remote;
  if (execGit(opCmd, outRemote)) {
    return true;
  } else {
    outRemote.clear();
    return false;
  }
}
} // namespace GithubGrabber