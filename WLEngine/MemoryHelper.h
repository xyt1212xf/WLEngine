#pragma once
namespace WL
{
	size_t GetAllMemUsage();  //获取当前使用的内存总量
	size_t GetAllCapacity(); //获取当前占用的内存总量

	void PrintMemInfo();	//在打开内存泄露查找的情况下，打印当前所有内存分配堆栈情况

	//如果打开了find leakage，则开始或者结束find leakage
	//bThoroughly为true表示进行详细内存泄漏查找（对性能影响极大），false表示只进行简单内存泄漏查找。
	void BeginWatchingLeakage(const wchar_t* szFile, const wchar_t* szFilePath, bool bThoroughly);
	void EndWatchingLeakage(bool bShowMessage);
	bool WatchLeakageEnabled();
}