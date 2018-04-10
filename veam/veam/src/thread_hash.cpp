#include <veam\thread_hash.h>

HashThread::HashThread( Gueue* queue, const std::string& fileNameOut ) : 
     fileOutName_( fileNameOut ),
     queue_( queue ),
     isRun( false )
{
}

HashThread::~HashThread()
{
     if( thread_.joinable() )
     {
          thread_.join();
     }
     fileOut_.close();
}

void HashThread::ThreadFunc( void *lp )
{
     HashThread* data = static_cast< HashThread* >( lp );

     while( data->isRun || !data->queue_->IsEmpty() )
     {
          std::string tmp = data->GetQueue();
          if( !tmp.empty() )
          {               
               data->WriteFile( data->GetHash( tmp ) );
          }
     }
}

void HashThread::Start()
{
     fileOut_.open( fileOutName_, std::ios::out | std::ios::binary | std::ios::ate );
     if( !fileOut_.is_open() )
     {
          std::string massage = "could not open output file " + '\"' + fileOutName_ + '\"';
          throw std::exception( massage.c_str() );
     }
     isRun = true;
     thread_ = std::thread( ThreadFunc, this );  
}

void HashThread::Stop()
{
     isRun = false;
}

std::string HashThread::GetQueue()
{
     return queue_->GetAndRemoveData();
}

size_t HashThread::GetHash( const std::string& data ) const
{
     return std::hash<std::string>()(data);
}

void HashThread::WriteFile( size_t hash ) 
{
     fileOut_ << hash;
}
