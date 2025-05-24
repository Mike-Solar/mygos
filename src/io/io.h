//
// Created by katherinesolar on 25-5-22.
//

#ifndef IO_H
#define IO_H
enum{
  DEBUG,INFO,WARN,ERROR
};
int kprintf(int level, const char *fmt, ...);
void kputs(const char *s);
#endif //IO_H
