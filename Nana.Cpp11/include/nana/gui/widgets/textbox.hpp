#ifndef NANA_GUI_WIDGET_TEXTBOX_HPP
#define NANA_GUI_WIDGET_TEXTBOX_HPP
#include <nana/gui/widgets/widget.hpp>

namespace nana{ namespace gui{
	namespace widgets
	{
		namespace skeletons
		{
			class text_editor;
		}
	}

	namespace drawerbase
	{
		namespace textbox
		{
			//class drawer
			class drawer
				: public drawer_trigger
			{
			public:
				typedef widgets::skeletons::text_editor text_editor;

				drawer();
				bool border(bool);
				text_editor * editor();
				const text_editor * editor() const;
			private:
				void bind_window(widget_reference);
				void attached(graph_reference);
				void detached();
				void refresh(graph_reference);
				void focus(graph_reference, const eventinfo&);
				void mouse_down(graph_reference, const eventinfo&);
				void mouse_move(graph_reference, const eventinfo&);
				void mouse_up(graph_reference, const eventinfo&);
				void mouse_enter(graph_reference, const eventinfo&);
				void mouse_leave(graph_reference, const eventinfo&);
				void key_down(graph_reference, const eventinfo&);
				void key_char(graph_reference, const eventinfo&);
				void mouse_wheel(graph_reference, const eventinfo&);
				void resize(graph_reference, const eventinfo&);
			private:
				void _m_text_area(unsigned width, unsigned height);
				void _m_draw_border(graph_reference);
			private:
				widget*	widget_;
				struct status_type
				{
					bool border;
					bool has_focus;		//Indicates whether it has the keyboard focus
				}status_;

				widgets::skeletons::text_editor * editor_;
			};
		}//end namespace textbox
	}//end namespace drawerbase

	class textbox
		:public widget_object<category::widget_tag, drawerbase::textbox::drawer>
	{
	public:
		textbox();
		textbox(window, bool visible);
		textbox(window, const rectangle& = rectangle(), bool visible = true);

		void load(const nana::char_t* file);
		void store(const nana::char_t* file) const;
		void store(const nana::char_t* file, nana::unicode encoding) const;

		bool getline(std::size_t n, nana::string&) const;
		textbox& append(const nana::string&, bool at_caret);
		textbox& border(bool);
		bool multi_lines() const;
		textbox& multi_lines(bool);
		bool editable() const;
		textbox& editable(bool);
		textbox& tip_string(const nana::string&);
		textbox& mask(nana::char_t);
		bool selected() const;
		void select(bool);

		void copy() const;
		void paste();
		void del();
	protected:
		//Override _m_caption for caption()
		nana::string _m_caption() const;
		void _m_caption(const nana::string&);
		//Override _m_typeface for changing the caret
		void _m_typeface(const nana::paint::font&);
	};

}//end namespace gui
}//end namespace nana
#endif
