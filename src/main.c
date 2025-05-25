//
// Created by katherinesolar on 25-5-23.
//
#include <sbi_console.h>
#include <sbi_init.h>
#include "kmemory.h"
#include "device_tree_parser.h"
int kernel_main(void *dtd) {
	sbi_init(dtd);
	parse_device_tree(dtd);

}