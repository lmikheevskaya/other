#include <veam\deque.h>

void Gueue::SetData( const std::string& data )
{
     std::lock_guard<std::mutex> lock(mutex_);

     deque_.push_front(data);
}

std::string Gueue::GetAndRemoveData()
{
     std::string data;

     std::lock_guard<std::mutex> lock(mutex_);
     //���� ������ �� ����, �� ��������� ��������� �������
     if (deque_.size())
     {
          //�������� ��������� ������� � ������ message
          data = deque_.back();
          deque_.pop_back();
     }

     return data;
}

bool Gueue::IsEmpty() const
{
     return deque_.empty();
}