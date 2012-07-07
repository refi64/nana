#ifndef NANA_GUI_WIDGETS_LOGIN_HPP
#define NANA_GUI_WIDGETS_LOGIN_HPP

#include "widget.hpp"
#include <nana/pat/cloneable.hpp>

namespace nana{ namespace gui{
	class login;

	namespace drawerbase
	{
		namespace login
		{
			struct ext_event_tag
			{
				struct flags_t
				{
					bool remember_user;
					bool remember_password;
				};

				nana::functor_group<void(nana::gui::login&, const nana::string& user)> forget;
				nana::functor_group<bool(nana::gui::login&, const nana::string& user, const nana::string& password, const flags_t&)> verify;
				nana::functor_group<void(nana::gui::login&, const nana::string& user)> remove;
			};

			class trigger
				: public drawer_trigger
			{
				class drawer;
			public:
				struct label_strings
				{
					nana::string user;
					nana::string password;
					nana::string forget;
					nana::string remember_user;
					nana::string remember_password;
					nana::string login;
					nana::string remove;
					nana::string require_user;
					nana::string require_password;
					nana::string other_user;
				};

				trigger();
				~trigger();

				drawer * get_drawer() const;
			private:
				void bind_window(widget_reference);
				void attached(graph_reference);
				void detached();
				void refresh(graph_reference);
				void mouse_move(graph_reference, const eventinfo&);
				void mouse_up(graph_reference, const eventinfo&);
				void mouse_leave(graph_reference, const eventinfo&);
			private:
				drawer * impl_;
			};
		}
	}//end namespace drawerbase

	class login
		: public widget_object<category::widget_tag, drawerbase::login::trigger>
	{
	public:
		typedef drawer_trigger_t::label_strings label_strings_t;
		typedef drawerbase::login::ext_event_tag ext_event_type;

		login();
		login(window);
		login(window, const nana::rectangle& r);
		login(window, int x, int y, unsigned width, unsigned height);

		bool transparent() const;
		void transparent(bool);

		void insert(const nana::string& user, const nana::string& password);
		void insert(const nana::string& user, const nana::string& password, const nana::paint::image&);

		ext_event_type& ext_event() const;

		void selection(bool);
		bool selection() const;
		void set(const label_strings_t&);
		void reset();
	};

}//end namespace gui 
}//end namespace nana

#endif
