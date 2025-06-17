//
// Created by katherinesolar on 25-5-22.
//
/*
#include "io.h"

#include <string.h>

#include "kmemory.h"
struct fmt_to_str {
	int pos;
	char *str;
	struct fmt_to_str *next;
};
int kprintk(int level, const char *fmt, ...){
	long long a2,a3,a4,a5,a6,a7;
	asm volatile(
      "mv %[out3],a2\n"
      "mv %[out4],a3\n"
      "mv %[out5],a4\n"
      "mv %[out6],a5\n"
      "mv %[out7],a6\n"
      "mv %[out8],a7\n"
      :[out3]"=rm"(a2), [out4]"=rm"(a3),
       [out5]"=rm"(a4), [out6]"=rm"(a5),
       [out7]"=rm"(a6), [out8]"=rm"(a7)
       :
       :"a2", "a3", "a4", "a5", "a6", "a7"
	);
	long long reg[]={a2,a3,a4,a5,a6,a7};
	int count = 0;
	struct fmt_to_str *fmt_to_str=(struct fmt_to_str *)kmalloc(sizeof(struct fmt_to_str));
	fmt_to_str->pos=0;
	fmt_to_str->str=NULL;
	fmt_to_str->next=NULL;
	struct fmt_to_str *cur=fmt_to_str;
	for (int i=0;fmt[i]!=0;i++) {
		if (fmt[i]=='%') {
			if (fmt[i+1]=='s' && fmt[i-1]!='\\') {
				char *str=(char *)(reg[i]);
				cur->next=(struct fmt_to_str *)kmalloc(sizeof(struct fmt_to_str));
				cur->next->pos=0;
				cur->next->str=str;
				cur->next->next=NULL;
				cur=cur->next;
			}
			else if (fmt[i+1]=='d' && fmt[i-1]=='\\') {
				//TODO
			}
			count++;
		}
	}

}
*/