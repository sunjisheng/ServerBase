// example.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <iostream>
#include "GlobalFunction.h"
#include "TimeClock.h"
void testlist_main();
void testvector_main();
void testhashmap_main();
void testlog_main();
void testjemalloc_main();
void testskiplist_main();
void testmap_main();
int main()
{
	//testskiplist_main();
	//testmap_main();
	testhashmap_main();
}