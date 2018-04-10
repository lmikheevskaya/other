
#include <string>
#include <fstream>
#include <stdexcept>
#include <iostream>
#include <stdio.h>
#include <string.h>

#include <veam\deque.h>
#include <veam\thread_hash.h>
#include <veam\utils.h>

namespace
{
const uint64_t BlockSize = 1024 * 1024;
}

struct CmdParams
{
     CmdParams() : blockSize( BlockSize )
     {
     };

     std::string fileIn;
     std::string fileOut;
     uint64_t blockSize;
};

class FileSignatureGenerator
{
public:
     FileSignatureGenerator( const CmdParams& data ) : 
          blockSize_( data.blockSize ),
          fileName_( data.fileIn ), 
          thread_( &deque_, data.fileOut )
     {

     }

     ~FileSignatureGenerator()
     {
          /// TODO при необходимости можно добавить ожидание событи€ завершени€ работы с очередью
     }

     /// —читать данные из файла и записать блоки данных в очередь
     void Read()
     {
          FILE *stream = nullptr;
          SCOPE_EXIT( fclose (stream ) );

          fopen_s( &stream, fileName_.c_str(),"rb" );     

          if( !stream )
          {
               std::string massage = "could not open input file " + '\"' + fileName_ + '\"';
               throw std::exception( massage.c_str() );
          }
         
          _fseeki64( stream, 0, SEEK_END );
          uint64_t  fileSize = _ftelli64( stream );
          _fseeki64( stream, 0, SEEK_SET );

          uint64_t  pos( SEEK_SET );

          std::string buffer( static_cast<size_t>( blockSize_ ), '\0' );
          
          thread_.Start();
       
          while( pos != fileSize )
          {
               uint64_t  block = ( fileSize > pos ) ? blockSize_ : (fileSize - pos);
               fread( &buffer[0], sizeof( char ), static_cast<size_t>( block ), stream );
               deque_.SetData( buffer );
               pos = ftell( stream );
          }

          thread_.Stop();       
     };

private:
     const uint64_t blockSize_;
     const std::string& fileName_;
     Gueue deque_;    
     HashThread thread_;
};


void ShowHelp()
{
     std::cout << "Test \n"
          "[-file_in] input file \n"  
          "[-file_out] output file \n"
          "[-block_size] data block length" << std::endl; 
}

void ShowError( const std::string& msg )
{
     std::cout << "error - " << msg << std::endl;
}

void ParseCmdParams( int argc, char **argv, CmdParams &params )
{
     for ( int i = 1; i < argc; i++ )
     {
          if ( strcmp( argv[i], "-file_in" ) == 0 )
          {
               i++;
               if ( i < argc )
               {
                    params.fileIn.assign( argv[i] );    
               }
               continue;
          }
          if ( strcmp( argv[i], "-file_out" ) == 0 )
          {
               i++;
               if ( i < argc )
               {
                    params.fileOut.assign( argv[i] );    
               }
               continue;
          }
          if ( strcmp( argv[i], "-block_size" ) == 0 )
          {
               i++;
               if ( i < argc )
               {
                    params.blockSize = std::stoll( argv[i] );  
               }
               continue;
          }
     }
}

int main( int argc, char *argv[] )
{
     if ( argc < 2 )
     {
          ShowHelp();
          return 0;
     }
     if ( strcmp( argv[1], "-help" ) == 0 )
     {
          ShowHelp();
          return 0;
     }
     CmdParams params;

     ParseCmdParams( argc, argv, params );

     if( params.fileIn.empty() )
     {
          ShowHelp();
          return 0;
     }

     try
     {
          FileSignatureGenerator generator( params );
          generator.Read();
     }
     catch( const std::exception& e )
     {
          ShowError( e.what() );
          return 0;
     }

	return 0;
}