#include <wchar.h>
#include <windows.h>
#include <stdio.h>

#define MAX_CMDLINE 32768

void ErrorExit(const char *fn) {
  char *error;
  DWORD dw = GetLastError();

  FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, dw,
                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&error, 0, NULL);

  fprintf(stderr, "%s: %s\n", fn, error);
  ExitProcess(dw);
}

/**
 * This comes from
 * https://github.com/pypa/setuptools/blob/main/launcher.c
 *
 * Unlike the original, the xpack version does not rely on a separate
 * Python script file - the Python script file is included in the EXE
 * via the Windows Resource Compiler
 */
int child_pid = 0;

void pass_control_to_child(DWORD control_type) {
  /*
   * distribute-issue207
   * passes the control event to child process (Python)
   */
  if (!child_pid) {
    return;
  }
  GenerateConsoleCtrlEvent(child_pid, 0);
}

BOOL control_handler(DWORD control_type) {
  /*
   * distribute-issue207
   * control event handler callback function
   */
  switch (control_type) {
  case CTRL_C_EVENT:
    pass_control_to_child(0);
    break;
  }
  return TRUE;
}

int create_and_wait_for_subprocess(char *command) {
  /*
   * distribute-issue207
   * launches child process (Python)
   */
  DWORD return_value = 0;
  LPSTR commandline = command;
  STARTUPINFOA s_info;
  PROCESS_INFORMATION p_info;
  ZeroMemory(&p_info, sizeof(p_info));
  ZeroMemory(&s_info, sizeof(s_info));
  s_info.cb = sizeof(STARTUPINFO);
  // set-up control handler callback function
  SetConsoleCtrlHandler((PHANDLER_ROUTINE)control_handler, TRUE);
  if (!CreateProcessA(NULL, commandline, NULL, NULL, TRUE, 0, NULL, NULL, &s_info, &p_info)) {
    fprintf(stderr, "failed to create process.\n");
    return 0;
  }
  child_pid = p_info.dwProcessId;
  // wait for Python to exit
  WaitForSingleObject(p_info.hProcess, INFINITE);
  if (!GetExitCodeProcess(p_info.hProcess, &return_value)) {
    fprintf(stderr, "failed to get exit code from process.\n");
    return 0;
  }
  return return_value;
}

int main(int argc, char *argv[]) {
  char *cmd, *code;
  int i;

  cmd = malloc(MAX_CMDLINE);
  snprintf(cmd, MAX_CMDLINE, "python");
  for (i = 1; i < argc; i++)
    snprintf(cmd + strlen(cmd), MAX_CMDLINE - strlen(cmd), " %s", argv[i]);

  if (strlen(cmd) >= MAX_CMDLINE - 1) {
    fprintf(stderr, "Command line too long, limited to %d characters\n", MAX_CMDLINE - 1);
    exit(1);
  }

  return create_and_wait_for_subprocess(cmd);
}
