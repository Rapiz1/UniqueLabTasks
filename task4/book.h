#ifndef BOOK_H__
#define BOOK_H__

#define MAX_BOOKS 130
typedef struct {
  int id, size, filled;
} Book;
Book* OpenBook(int);
Book* CreateBook(int,int);
void InitBook(Book*,int,int);
void DestroyBook(int);
void PrintBook(Book*);
void SaveBook(Book*);
int MasteredBook(Book*);
#endif