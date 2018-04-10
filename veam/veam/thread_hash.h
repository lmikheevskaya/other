#pragma once

#include <thread>
#include <functional>
#include <fstream>

#include <veam\deque.h>

// ����� ������, ������� ������ ������ �� ������� � ���������� �� � ����
class HashThread
{
public:	
     HashThread( Gueue* queue, const std::string& fileNameOut );
	~HashThread();

	static void ThreadFunc( void *lp );
     /// ����� ������ ������
	void Start();
     /// ��������� ������ ������
	void Stop();

private:	
     /// ������ ������ �� �������
     /// return �������� ������
	std::string GetQueue();
     /// �������� hash ������
     /// param[in] ������� ������
     /// return hash ������
	size_t GetHash( const std::string& data ) const;
     /// �������� ���������� hash � ����
     /// param[in] hash �������� hash ������
	void WriteFile( size_t hash );

private:
     std::string fileOutName_;
	std::thread thread_;
     std::ofstream fileOut_;
	Gueue* queue_;
     bool isRun;
};