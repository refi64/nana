#ifndef NANA_GUI_FILEBOX_HPP
#define NANA_GUI_FILEBOX_HPP
#include <nana/gui/wvl.hpp>

namespace nana{	namespace gui
{
	class filebox
		: nana::noncopyable
	{
		struct implement;
	public:
		typedef std::vector<std::pair<nana::string, nana::string> > filters;
		filebox(window owner, bool is_open_mode);
		~filebox();

		/**	@brief	specify a title for the dialog
		 *	@param	string	a text for title
		 */
		nana::string title(nana::string);

		/**	@brief	specify a suggestion directory
		 *	@param	string	a path of initial directory
		 *	@note	the behavior of init_path is different between Win7 and Win2K/XP/Vista, but its behavior under Linux is conformed with Win7.
		 */
		filebox& init_path(const nana::string&);
		filebox& init_file(const nana::string&);
		filebox& add_filter(const nana::string& description, const nana::string& filetype);
		filebox& add_filter(const filters & type_filters)
		{
			for (filters::const_iterator i = type_filters.begin(); i != type_filters.end(); ++i)
                add_filter(i->first, i->second);
            return *this;
		}

		nana::string path() const;
		nana::string file() const;
		
		/**	@brief	Display the filebox dialog
		 */
		bool show() const;
		
		/** @brief	Display the filebox dialog
		 *	@note	A function object method alternative to show()
		 */
		bool operator()() const
		{
			return show();
		}
	private:
		implement * impl_;
	};
}//end namespace gui
}//end namespace nana
#endif
