/*
 *	A Tree Box Implementation
 *	Copyright(C) 2003-2013 Jinhao(cnjinhao@hotmail.com)
 *
 *	Distributed under the Boost Software License, Version 1.0. 
 *	(See accompanying file LICENSE_1_0.txt or copy at 
 *	http://www.boost.org/LICENSE_1_0.txt)
 *
 *	@file: nana/gui/widgets/treebox.hpp
 *	@brief:
 *		The treebox organizes the nodes by a key string. 
 *		The treebox would have a vertical scrollbar if the node
 *	is too many to display. And it does not have a horizontal scrollbar,
 *	the widget will adjust the node's displaying position for fitting.
 */

#ifndef NANA_GUI_WIDGETS_TREEBOX_HPP
#define NANA_GUI_WIDGETS_TREEBOX_HPP
#include "widget.hpp"
#include "scroll.hpp"
#include "detail/compset.hpp"
#include <nana/paint/gadget.hpp>
#include "detail/tree_cont.hpp"
#include <nana/gui/timer.hpp>
#include <nana/any.hpp>
#include <nana/pat/cloneable.hpp>

namespace nana
{
namespace gui
{
	namespace drawerbase
	{
		namespace treebox
		{
			enum class component
			{
				expender, crook, bground, icon, text, end
			};

			class tooltip_window;

			template<typename NodeType>
			struct extra_events
			{
				typedef NodeType node_type;

				nana::fn_group<void(nana::gui::window, node_type, bool)> expand;
				nana::fn_group<void(nana::gui::window, node_type, bool)> selected;
			};

			struct node_image_tag
			{
				nana::paint::image normal;
				nana::paint::image highlighted;
				nana::paint::image expanded;
			};

			
			struct node_attribute
			{
				bool expended;
				bool checked;
				bool selected;
				bool mouse_pointed;
				nana::paint::image icon;
				nana::string text;
			};

			typedef ::nana::gui::widgets::detail::compset<component, node_attribute> compset_interface;
			
			class renderer_interface
			{
			public:
				typedef ::nana::paint::graphics& graph_reference;
				typedef compset_interface::item_attribute_t item_attribute_t;
				typedef compset_interface::comp_attribute_t comp_attribute_t;

				virtual ~renderer_interface()
				{}

				virtual void render(graph_reference, nana::color_t bgcolor, nana::color_t fgcolor, const compset_interface *) const = 0;
			};

			class trigger
				:public drawer_trigger
			{
				template<typename Renderer>
				struct basic_implement;
			public:
				struct treebox_node_type
				{
					treebox_node_type();
					treebox_node_type(const nana::any&);
					treebox_node_type(const nana::string& text, const nana::any&);
					treebox_node_type& operator=(const treebox_node_type&);

					nana::string text;
					nana::any value;
					bool expanded;
					nana::string img_idstr;
				};

				struct pseudo_node_type{};

				typedef nana::gui::widgets::detail::tree_cont<treebox_node_type> tree_cont_type;
				typedef tree_cont_type::node_type	node_type;
				typedef extra_events<pseudo_node_type*>	ext_event_type;

				trigger();
				~trigger();

				void renderer(const nana::pat::cloneable_interface<renderer_interface>& );
				const nana::pat::cloneable_interface<renderer_interface> & renderer() const;

				void auto_draw(bool);

				const tree_cont_type & tree() const;
				tree_cont_type & tree();

				nana::any & value(node_type*) const;
				node_type* insert(node_type* node, const nana::string& key, const nana::string& title, const nana::any& v);
				node_type* insert(const nana::string& path, const nana::string& title, const nana::any& v);

				bool check_kinship(node_type* parent, node_type* child) const;
				void remove(node_type*);
				node_type * selected() const;
				void selected(node_type*);
				void set_expand(node_type* node, bool);
				void set_expand(const nana::string& path, bool);

				void image(const nana::string& id, const node_image_tag&);
				node_image_tag& image(const nana::string&) const;
				void image_erase(const nana::string&);
				void node_image(node_type* node, const nana::string& id);

				unsigned node_width(const node_type *node) const;

				bool rename(node_type *node, const nana::char_t* key, const nana::char_t* name);
				ext_event_type& ext_event() const;
			private:
				//Overrides drawer_trigger methods
				void bind_window(widget_reference)	override;
				void attached(graph_reference)		override;
				void detached()	override;
				void refresh(graph_reference)	override;
				void dbl_click(graph_reference, const eventinfo&)	override;
				void mouse_down(graph_reference, const eventinfo&)	override;
				void mouse_up(graph_reference, const eventinfo&)	override;
				void mouse_move(graph_reference, const eventinfo&)	override;
				void mouse_wheel(graph_reference, const eventinfo&)	override;
				void resize(graph_reference, const eventinfo&)		override;
				void key_down(graph_reference, const eventinfo&)	override;
				void key_char(graph_reference, const eventinfo&)	override;
			private:
				void _m_deal_adjust();
			private:
				class item_renderer;
				class item_locator;
			private:
				typedef basic_implement<item_renderer> implement;

				implement * const impl_;
			}; //end class trigger
		}//end namespace treebox
	}//end namespace drawerbase

	template<typename UserData>
	class treebox
		:public widget_object<category::widget_tag, drawerbase::treebox::trigger>
	{
	public:
		typedef UserData value_type;
		typedef typename drawer_trigger_t::pseudo_node_type* node_type;
		typedef typename drawer_trigger_t::ext_event_type ext_event_type;
		typedef drawerbase::treebox::node_image_tag node_image_type;
		typedef drawerbase::treebox::renderer_interface renderer_interface;

		template<typename Renderer>
		class cloneable_renderer
			: public nana::pat::cloneable<Renderer, renderer_interface>
		{};

		treebox(){}

		treebox(window wd, bool visible)
		{
			create(wd, rectangle(), visible);
		}

		treebox(window wd, const rectangle& r, bool visible)
		{
			create(wd, r, visible);
		}

		treebox & renderer(const ::nana::pat::cloneable_interface<renderer_interface> & rd)
		{
			get_drawer_trigger().renderer(rd);
			return *this;
		}

		const nana::pat::cloneable_interface<renderer_interface> & renderer() const
		{
			return get_drawer_trigger().renderer();
		}


		void auto_draw(bool ad)
		{
			get_drawer_trigger().auto_draw(ad);
		}

		ext_event_type& ext_event() const
		{
			return get_drawer_trigger().ext_event();
		}

		value_type& value(node_type node) const
		{
			return get_drawer_trigger().value(reinterpret_cast<drawer_trigger_t::node_type*>(node));
		}

		treebox& image(const nana::string& id, const nana::paint::image& img)
		{
			node_image_type node_img;
			node_img.normal = img;
			get_drawer_trigger().image(id, node_img);
			return *this;
		}

		treebox& image(const nana::string& id, const node_image_type& node_img)
		{
			get_drawer_trigger().image(id, node_img);
			return *this;
		}

		node_image_type& image(const nana::string& id) const
		{
			return get_drawer_trigger().image(id);
		}

		void image_erase(const nana::string& id)
		{
			get_drawer_trigger().image_erase(id);
		}

		void node_image(node_type node, const nana::string& id)
		{
			get_drawer_trigger().node_image(reinterpret_cast<drawer_trigger_t::node_type*>(node), id);
		}

		node_type insert(const nana::string& path_key, const nana::string& title, value_type value)
		{
			return reinterpret_cast<node_type>(get_drawer_trigger().insert(path_key, title, value));
		}

		node_type insert(node_type node, const nana::string& key, const nana::string& title, value_type value)
		{
			return reinterpret_cast<node_type>(get_drawer_trigger().insert(reinterpret_cast<drawer_trigger_t::node_type*>(node), key, title, value));
		}

		void remove(node_type node)
		{
			get_drawer_trigger().remove(reinterpret_cast<drawer_trigger_t::node_type*>(node));
		}

		void remove(const nana::string& key_path)
		{
			get_drawer_trigger().remove(
				get_drawer_trigger().find(key_path)
				);
		}

		void expand(node_type node, bool exp)
		{
			get_drawer_trigger().set_expand(reinterpret_cast<drawer_trigger_t::node_type*>(node), exp);
		}

		void expand(const nana::string& path_key, bool exp)
		{
			get_drawer_trigger().set_expand(path_key, exp);
		}

		bool expend(node_type node) const
		{
			if(get_drawer_trigger().check(reinterpret_cast<drawer_trigger_t::node_type*>(node)))
				return reinterpret_cast<drawer_trigger_t::node_type*>(node)->value.second.expanded;
			return false;	
		}

		node_type node(const nana::string& keypath)
		{
			return reinterpret_cast<node_type>(get_drawer_trigger().find(keypath));
		}

		nana::string key(node_type node) const
		{
			if(get_drawer_trigger().check(reinterpret_cast<drawer_trigger_t::node_type*>(node)))
				return reinterpret_cast<drawer_trigger_t::node_type*>(node)->value.first;
			
			return nana::string();
		}

		bool key(node_type node, const nana::string& key)
		{
			return (get_drawer_trigger().rename(reinterpret_cast<drawer_trigger_t::node_type*>(node), key.c_str(), 0));
		}

		bool text(node_type node, const nana::string& str)
		{
			return (get_drawer_trigger().rename(reinterpret_cast<drawer_trigger_t::node_type*>(node), 0, str.c_str()));
		}

		nana::string text(node_type node) const
		{
			if(get_drawer_trigger().check(reinterpret_cast<drawer_trigger_t::node_type*>(node)))
				return reinterpret_cast<drawer_trigger_t::node_type*>(node)->value.second.text;

			return nana::string();
		}

		node_type selected() const
		{
			return reinterpret_cast<node_type>(get_drawer_trigger().selected());
		}

		void selected(node_type node)
		{
			get_drawer_trigger().selected(reinterpret_cast<drawer_trigger_t::node_type*>(node));
		}

		std::size_t children_size(node_type node) const
		{
			typedef drawer_trigger_t::node_type * node_t;
			if(get_drawer_trigger().check(reinterpret_cast<node_t*>(node)))
			{
				node_t * child = reinterpret_cast<node_t*>(node)->child;

				std::size_t n = 0;
				for(; child; child = child->next)
					++n;
				return n;
			}
			return 0;
		}

		node_type get_sibling(node_type node) const
		{
			if(get_drawer_trigger().check(reinterpret_cast<drawer_trigger_t::node_type*>(node)))
				return reinterpret_cast<node_type>(
							reinterpret_cast<drawer_trigger_t::node_type*>(node)->next
							);
			return 0;
		}

		node_type get_child(node_type node) const
		{
			if(get_drawer_trigger().check(reinterpret_cast<drawer_trigger_t::node_type*>(node)))
				return reinterpret_cast<node_type>(
							reinterpret_cast<drawer_trigger_t::node_type*>(node)->child
							);
			return 0;
		}

		node_type get_owner(node_type node) const
		{
			return reinterpret_cast<node_type>(
					get_drawer_trigger().get_owner(reinterpret_cast<drawer_trigger_t::node_type*>(node))
					);
		}

		nana::string make_key_path(node_type node, const nana::string& splitter) const
		{
			auto & tree = get_drawer_trigger().tree();
			auto pnode = reinterpret_cast<drawer_trigger_t::node_type*>(node);
			if(tree.check(pnode))
			{
				auto root = tree.get_root();
				nana::string path;
				nana::string temp;
				while(pnode->owner != root)
				{
					temp = splitter;
					temp += pnode->value.first;
					path.insert(0, temp);
					pnode = pnode->owner;
				}

				path.insert(0, pnode->value.first);
				return path;
			}
			return STR("");
		}
	};
}//end namespace gui
}//end namespace nana

#endif
