#ifndef SHELL_SHELL_H_
void ShInit();
void ShLoop();
char* ShReadLine();
char** ShSplitLine(char*);
int ShExecute(char**);
int ShLaunch(char**);
#endif