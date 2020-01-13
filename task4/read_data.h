#ifndef RD_H__
#define RD_H__

#define MAX_DATA 4096
typedef struct {
    int book_id; //本次读取的书籍的编号
    int book_size; //本次读取的书籍的总大小
    int data_offset; //本次读取获得的数据的开头位置在书中的位置
    int data_size; //本次读取获得的数据大小
    char data[0];
}ReadData;
void PrintReadData(ReadData *data);
#endif