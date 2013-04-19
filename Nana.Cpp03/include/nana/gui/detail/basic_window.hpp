/*
 *	A Basic Window Widget Definition
 *	Copyright(C) 2003-2013 Jinhao(cnjinhao@hotmail.com)
 *
 *	Distributed under the Boost Software License, Version 1.0. 
 *	(See accompanying file LICENSE_1_0.txt or copy at 
 *	http://www.boost.org/LICENSE_1_0.txt)
 *
 *	@file: nana/gui/detail/basic_window.hpp
 */

#ifndef NANA_GUI_DETAIL_BASIC_WINDOW_HPP
#define NANA_GUI_DETAIL_BASIC_WINDOW_HPP
#include "drawer.hpp"
#include "../basis.hpp"
#include <nana/basic_types.hpp>
#include <nana/system/platform.hpp>
#include <nana/gui/effects.hpp>

namespace nana
{
namespace gui
{
	namespace category
	{
		struct flags
		{
			enum
			{
				super,
				widget = 0x1,
				lite_widget_tag = 0x3,
				root = 0x5,
				frame = 0x9
			};
		};

		struct widget_tag{ enum{value = flags::widget}; };
		struct lite_widget_tag : widget_tag{ enum{ value = flags::lite_widget_tag};};
		struct root_tag : widget_tag{ enum { value = flags::root}; };
		struct frame_tag: widget_tag{ enum { value = flags::frame}; };
	}// end namespace category

namespace detail
{
	template<typename BasicWindow, typename NativeAPI>
	class caret_descriptor
	{
	public:
		typedef BasicWindow	core_window_t;
		typedef NativeAPI	interface_type;

		caret_descriptor(core_window_t* wd, unsigned width, unsigned height)
			:wd_(wd), size_(width, height), visible_(false), real_visible_state_(false), out_of_range_(false)
		{}

		~caret_descriptor()
		{
			if(wd_)	interface_type::caret_destroy(wd_->root);
		}

		void set_active(bool active)
		{
			if(wd_)
			{
				if(active)
				{
					interface_type::caret_create(wd_->root, size_.width, size_.height);
					real_visible_state_ = false;
					this->position(point_.x, point_.y);
				}
				else
					interface_type::caret_destroy(wd_->root);

				wd_->root_widget->other.attribute.root->ime_enabled = active;
			}
		}

		core_window_t* window() const
		{
			return wd_;
		}

		void position(int x, int y)
		{
			point_.x = x;
			point_.y = y;
			update();
		}

		void effective_range(nana::rectangle rect)
		{
			//Chech rect
			if(	(rect.width && rect.height) &&
				(rect.x + rect.width > 0) &&
				(rect.y + rect.height > 0))
			{
				if(rect.x < 0)
				{
					rect.width += rect.x;
					rect.x = 0;
				}

				if(rect.y < 0)
				{
					rect.height += rect.y;
					rect.y = 0;
				}

				if(effective_range_ != rect)
				{
					effective_range_ = rect;
					update();
				}
			}
		}

		nana::point position() const
		{
			return point_;
		}

		void visible(bool isshow)
		{
			if(visible_ != isshow)
			{
				visible_ = isshow;
				if(visible_ == false || false == out_of_range_)
					_m_visible(isshow);
			}
		}

		bool visible() const
		{
			return visible_;
		}

		nana::size size() const
		{
			return size_;
		}

		void size(const nana::size& s)
		{
			size_ = s;
			update();

			if(visible_)	this->visible(true);
		}

		void update()
		{
			nana::point pos = point_;
			nana::size	size = size_;

			nana::rectangle rect = effective_range_;
			if(0 == effective_range_.width || 0 == effective_range_.height)
			{
				rect.x = rect.y = 0;
				rect.width = wd_->rect.width;
				rect.height = wd_->rect.height;
			}
			else
			{
				pos.x += effective_range_.x;
				pos.y += effective_range_.y;
			}

			if(	(pos.x + static_cast<int>(size.width) <= rect.x) || (pos.x >= rect.x + static_cast<int>(rect.width)) ||
				(pos.y + static_cast<int>(size.height) <= rect.y) || (pos.y >= rect.y + static_cast<int>(rect.height))
				)
			{//Out of Range without overlap
				if(false == out_of_range_)
				{
					out_of_range_ = true;

					if(visible_)
						_m_visible(false);
				}
			}
			else
			{
				if(pos.x < rect.x)
				{
					size.width -= (rect.x - pos.x);
					pos.x = rect.x;
				}
				else if(pos.x + size.width > rect.x + rect.width)
				{
					size.width -= pos.x + size.width - (rect.x + rect.width);
				}

				if(pos.y < rect.y)
				{
					size.width -= (rect.y - pos.y);
					pos.y = rect.y;
				}
				else if(pos.y + size.height > rect.y + rect.height)
					size.height -= pos.y + size.height - (rect.y + rect.height);

				if(out_of_range_)
				{
					if(paint_size_ == size)
						_m_visible(true);

					out_of_range_ = false;
				}

				if(paint_size_ != size)
				{
					interface_type::caret_destroy(wd_->root);
					interface_type::caret_create(wd_->root, size.width, size.height);
					real_visible_state_ = false;
					if(visible_)
						_m_visible(true);

					paint_size_ = size;
				}
				
				interface_type::caret_pos(wd_->root, wd_->root_x + pos.x, wd_->root_y + pos.y);
			}
		}
	private:
		void _m_visible(bool isshow)
		{
			if(real_visible_state_ != isshow)
			{
				real_visible_state_ = isshow;
				interface_type::caret_visible(wd_->root, isshow);
			}
		}
	private:
		core_window_t*	wd_;
		nana::point point_;
		nana::size	size_;
		nana::size	paint_size_;
		bool		visible_;
		bool		real_visible_state_;
		bool		out_of_range_;
		nana::rectangle effective_range_;
	};

	//tab_type
	//@brief: Define some constant about tab category, these flags can be combine with operator |
	struct tab_type
	{
		enum t
		{
			none,		//process by nana
			tabstop,	//move to the next tabstop window
			eating,		//process by current window
		};
	};

	//struct basic_window
	//@brief: a window data structure descriptor 
	template<typename THWND, typename Bedrock>
	struct basic_window
	{
		typedef THWND		native_window_type;
		typedef Bedrock		bedrock_type;
		typedef std::vector<basic_window*> container;

		struct root_context
		{
			bool focus_changed;
		};

		struct update_state
		{
			enum t{none, lazy, refresh};
		};

		struct edge_nimbus_action
		{
			basic_window * window;
			bool rendered;
		};

		typedef std::vector<edge_nimbus_action>	edge_nimbus_container;

		//basic_window
		//@brief: constructor for the root window
		basic_window(basic_window* owner, gui::category::root_tag**)
			: other(category::root_tag::value)
		{
			_m_init_pos_and_size(0, rectangle());
			this->other.category = category::root_tag::value;
			this->_m_initialize(owner);
		}

		template<typename Category>
		basic_window(basic_window* parent, const rectangle& r, Category**)
			: other(Category::value)
		{
			if(parent)
			{
				_m_init_pos_and_size(parent, r);
				this->_m_initialize(parent);
			}
		}

		~basic_window()
		{
			delete together.caret;
			together.caret = 0;
		}

		//native_window
		//@brief: bind a native window and baisc_window
		void bind_native_window(native_window_type wd, unsigned width, unsigned height, unsigned extra_width, unsigned extra_height, nana::paint::graphics& graphics)
		{
			if(category::root_tag::value == this->other.category)
			{
				this->root = wd;
				this->rect.width = width;
				this->rect.height = height;
				this->extra_width = extra_width;
				this->extra_height = extra_height;
				this->root_widget = this;
				this->root_graph = &graphics;
			}
		}

		void frame_window(native_window_type wd)
		{
			if(category::frame_tag::value == this->other.category)
				other.attribute.frame->container = wd;
		}
	private:
		void _m_init_pos_and_size(basic_window* parent, const rectangle& r)
		{
			this->rect.width = r.width;
			this->rect.height = r.height;
			this->rect.x = this->root_x = r.x;
			this->rect.y = this->root_y = r.y;

			if(parent)
			{
				this->root_x += parent->root_x;
				this->root_y += parent->root_y;
			}
		}

		void _m_initialize(basic_window* parent)
		{
			if(this->other.category == category::root_tag::value)
			{
				if(parent && (nana::system::this_thread_id() != parent->thread_id))
					parent = 0;

				while(parent && (parent->other.category != nana::gui::category::root_tag::value))
					parent = parent->parent;
				
				this->owner = parent;
				this->parent = 0;
				this->index = 0;
			}
			else
			{
				this->parent = parent;
				this->owner = 0;
				this->root_widget = parent->root_widget;
				this->root = parent->root;
				this->root_graph = parent->root_graph;
				this->index = static_cast<unsigned>(parent->children.size());
				parent->children.push_back(this);
			}

			this->predef_cursor = cursor::arrow;
			this->flags.capture = false;
			this->flags.dbl_click = true;
			this->flags.enabled = true;
			this->flags.modal = false;
			this->flags.glass = false;
			this->flags.take_active = true;
			this->flags.dropable = false;
			this->flags.fullscreen = false;
			this->flags.tab = nana::gui::detail::tab_type::none;
			this->flags.action = mouse_action::normal;
			
			this->visible = false;

			this->color.foreground = 0x0;
			this->color.background = nana::gui::color::button_face;
			this->color.active = 0x60C8FD;

			this->effect.edge_nimbus = effects::edge_nimbus::none;

			this->together.caret = 0;
			this->flags.refreshing = false;
			this->flags.destroying = false;

			this->extra_width = 0;
			this->extra_height = 0;

			//The window must keep its thread_id same as its parent if it is a child.
			//Otherwise, its root buffer would be mapped repeatly if it is in its parent thread.
			this->thread_id = nana::system::this_thread_id();
			if(parent && (this->thread_id != parent->thread_id))
				this->thread_id = parent->thread_id;
		}
	public:
		nana::rectangle	rect;
		nana::size	min_track_size;
		nana::size	max_track_size;

#if defined(NANA_LINUX)
		nana::point pos_native;
#endif
		int	root_x;			//coordinate for root window
		int	root_y;			//coordinate for root window
		bool	visible;

		unsigned extra_width;
		unsigned extra_height;

		basic_window	*parent;
		basic_window	*owner;

		nana::string	title;
		nana::gui::detail::drawer	drawer;	//Self Drawer with owen graphics
		basic_window*		root_widget;	//A pointer refers to the root basic window, if the window is a root, the pointer refers to itself.
		paint::graphics*	root_graph;		//Refer to the root buffer graphics
		nana::gui::cursor::t	predef_cursor;

		struct flags_type
		{
			bool enabled	:1;
			bool dbl_click	:1;
			bool capture	:1;	//if mouse button is down, it always receive mouse move even the mouse is out of its rectangle
			bool modal		:1;
			bool glass		:1;
			bool take_active:1;	//If take_active is false, other.active_window still keeps the focus.
			bool refreshing	:1;
			bool destroying	:1;
			bool dropable	:1; //Whether the window has make mouse_drop event.
			bool fullscreen	:1;	//When the window is maximizing whether it fit for fullscreen.
			unsigned Reserved: 22;
			unsigned char tab;		//indicate a window that can receive the keyboard TAB
			mouse_action::t	action;
		}flags;

		struct
		{
			class caret_descriptor<basic_window, typename bedrock_type::interface_type>* caret;
		}together;

		struct
		{
			color_t foreground;
			color_t	background;
			color_t	active;
		}color;

		struct
		{
			effects::edge_nimbus::t	edge_nimbus;
		}effect;
		
		struct other_tag
		{
			struct	attr_frame_tag
			{
				native_window_type container;
				std::vector<native_window_type> attach;
			};

			struct	attr_root_tag
			{
				container	frames;	//initialization is null, it will be created while creating a frame widget. Refer to WindowManager::create_frame
				container	tabstop;
				std::vector<edge_nimbus_action> effects_edge_nimbus;
				basic_window	*focus;
				basic_window	*menubar;
				root_context	context;
				bool			ime_enabled;
			};

			unsigned category;
			basic_window *active_window;	//if flags.take_active is false, the active_window still keeps the focus,
											//if the active_window is null, the parent of this window keeps focus.
			paint::graphics glass_buffer;	//if flags.glass is true. Refer to window_layout::make_glass_background.
			typename update_state::t	upd_state;

			union
			{
				attr_root_tag * root;
				attr_frame_tag * frame;
			}attribute;

			other_tag(unsigned categ)
				: category(categ), active_window(0), upd_state(update_state::none)
			{
				switch(categ)
				{
				case nana::gui::category::root_tag::value:
					attribute.root = new attr_root_tag;
					attribute.root->focus	= 0;
					attribute.root->menubar	= 0;
					attribute.root->context.focus_changed = false;
					attribute.root->ime_enabled = false;
					break;
				case nana::gui::category::frame_tag::value:
					attribute.frame = new attr_frame_tag;
					attribute.frame->container = 0;
					break;
				default:
					attribute.root = 0;
				}
			}

			~other_tag()
			{
				switch(category)
				{
				case nana::gui::category::root_tag::value:
					delete attribute.root;
					break;
				case nana::gui::category::frame_tag::value:
					delete attribute.frame;
					break;
				}
			}
		}other;

		native_window_type	root;		//root Window handle
		unsigned			thread_id;		//the identifier of the thread that created the window.
		unsigned			index;
		container			children;
	};

}//end namespace detail
}//end namespace gui
}//end namespace nana
#endif

