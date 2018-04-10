#pragma once

#include <thread>
#include <functional>
#include <fstream>

#include <veam\deque.h>

// класс потока, который читаем данные из очереди и записывает из в файл
class HashThread
{
public:	
     HashThread( Gueue* queue, const std::string& fileNameOut );
	~HashThread();

	static void ThreadFunc( void *lp );
     /// Старт работы потока
	void Start();
     /// Остановка работы потока
	void Stop();

private:	
     /// Читаем данные из очереди
     /// return Значение данных
	std::string GetQueue();
     /// Получить hash строки
     /// param[in] входная строка
     /// return hash строки
	size_t GetHash( const std::string& data ) const;
     /// Записать полученный hash в файл
     /// param[in] hash значение hash строки
	void WriteFile( size_t hash );

private:
     std::string fileOutName_;
	std::thread thread_;
     std::ofstream fileOut_;
	Gueue* queue_;
     bool isRun;
};