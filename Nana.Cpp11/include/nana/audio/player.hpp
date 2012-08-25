#ifndef NANA_AUDIO_PLAYER_HPP
#define NANA_AUDIO_PLAYER_HPP
#include <nana/traits.hpp>
namespace nana{	namespace audio
{
	class player
		: private nana::noncopyable
	{
		struct implementation;
	public:
		player();
		player(const char* file);
		~player();

		bool open(const char* file);
		void play();
		void close();
	private:
		implementation* impl_;
	};
}//end namespace audio
}//end namespace nana
#endif