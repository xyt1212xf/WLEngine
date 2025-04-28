#pragma once
namespace WL
{
	size_t GetAllMemUsage();  //��ȡ��ǰʹ�õ��ڴ�����
	size_t GetAllCapacity(); //��ȡ��ǰռ�õ��ڴ�����

	void PrintMemInfo();	//�ڴ��ڴ�й¶���ҵ�����£���ӡ��ǰ�����ڴ�����ջ���

	//�������find leakage����ʼ���߽���find leakage
	//bThoroughlyΪtrue��ʾ������ϸ�ڴ�й©���ң�������Ӱ�켫�󣩣�false��ʾֻ���м��ڴ�й©���ҡ�
	void BeginWatchingLeakage(const wchar_t* szFile, const wchar_t* szFilePath, bool bThoroughly);
	void EndWatchingLeakage(bool bShowMessage);
	bool WatchLeakageEnabled();
}