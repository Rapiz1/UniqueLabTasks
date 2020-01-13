#include "read_data.h"

#include <stdio.h>

void PrintReadData(ReadData *data) {
  printf("Book\t%d:\n\tSize:\t%d\n\tContent:\t[%d, %d+%d)\n", data->book_id, data->book_size,data->data_offset, data->data_offset, data->data_size);
}