#pragma once

#include <string>
#ifdef _WIN32
#include <windows.h>
#endif
namespace GithubGrabber {
typedef struct _gitRemote {
  _gitRemote() { memset(this, 0, sizeof(_gitRemote)); }
  ~_gitRemote() {}
  bool isEmpty() { return szLocation[0] == '\0' && szRemoteUrl[0] == '\0'; }
  void clear() { memset(this, 0, sizeof(_gitRemote)); }

  char szLocation[MAX_PATH];
  char szRemoteUrl[512];
  char szRemoteName[64];
  char szBranchName[64];
} gitRemote;

class gitOperator {
public:
  gitOperator();
  ~gitOperator();

  bool Init();

  bool AddRemote(std::string remote, gitRemote &outRemote,
                 gitRemote &originRemote = gitRemote());

  bool changeBranch(gitRemote &remote, std::string branch);

  bool PushLatestChanges(gitRemote &remote, std::string branch = std::string());

  bool PullLatestChanges(gitRemote &remote, gitRemote &toRemote = gitRemote());

private:
  bool FindGitExecutable();
  std::string GetTmpPath();
  bool execGit(std::string command, gitRemote &remote,
               const char *workingDirectory = NULL);
  bool newRemote(std::string remote, gitRemote &outRemote);

private:
  std::string gitExe;
  bool bInit;
  char timeStamp[64];
};
} // namespace GithubGrabber