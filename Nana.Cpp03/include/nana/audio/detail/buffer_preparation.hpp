#ifndef NANA_AUDIO_DETAIL_BUFFER_PREPARATION_HPP
#define NANA_AUDIO_DETAIL_BUFFER_PREPARATION_HPP
#include <nana/deploy.hpp>
#include <nana/audio/detail/audio_stream.hpp>
#include <nana/threads/locks.hpp>
#include <nana/threads/thread.hpp>

#include <sstream>
#include <vector>

#if defined(NANA_WINDOWS)
	#include <windows.h>
#endif

namespace nana{	namespace audio
{
	namespace detail
	{
		class buffer_preparation
		{
		public:
#if defined(NANA_WINDOWS)
			typedef WAVEHDR meta;
#elif defined(NANA_LINUX)
			struct meta
			{
				char * buf;
				std::size_t bufsize;
			};
#endif

		public:
			buffer_preparation(audio_stream& as, std::size_t seconds);

			~buffer_preparation();

			meta * read();
			//Revert the meta that returned by read()
			void revert(meta * m);
			bool data_finished() const;
		private:
			void _m_prepare_routine();
		private:
			volatile bool running_;
			volatile bool wait_for_buffer_;
			nana::threads::thread thr_;
			nana::threads::token token_buffer_, token_prepared_;
			nana::threads::condition cond_buffer_, cond_prepared_;

			std::vector<meta*> buffer_, prepared_;
			std::size_t block_size_;
			audio_stream & as_;
		};
	}//end namespace detail
}//end namespace audio
}//end namespace nana
#endif
