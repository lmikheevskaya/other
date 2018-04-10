#pragma once

#include <string>
#include <mutex>
#include <deque>

/// очередь 
class Gueue
{
public:
     Gueue() {};
     ~Gueue() {};

     /// Добавить элемент в очередь
     /// param[in] data Данные для добавления
	void SetData( const std::string& data );
     /// Вернуть элемент из очереди и удалить возвращенный элемент
     /// return Значение элемента
	std::string GetAndRemoveData();
     /// Проверить очередь на пустоту
     /// return true, если очередь пустая
     bool IsEmpty() const;

private:	
	std::mutex mutex_;
     std::deque< std::string > deque_;
};