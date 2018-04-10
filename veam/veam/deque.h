#pragma once

#include <string>
#include <mutex>
#include <deque>

/// ������� 
class Gueue
{
public:
     Gueue() {};
     ~Gueue() {};

     /// �������� ������� � �������
     /// param[in] data ������ ��� ����������
	void SetData( const std::string& data );
     /// ������� ������� �� ������� � ������� ������������ �������
     /// return �������� ��������
	std::string GetAndRemoveData();
     /// ��������� ������� �� �������
     /// return true, ���� ������� ������
     bool IsEmpty() const;

private:	
	std::mutex mutex_;
     std::deque< std::string > deque_;
};