.. include:: /helpers.rst

.. links
.. _Input Iterator: http://www.sgi.com/tech/stl/InputIterator.html

nana::gui::treebox
==================

.. warning:: This page needs improvements.

.. code-block:: cpp

    nana/gui/widgets/treebox.hpp

.. cpp:namespace:: nana::gui

.. cpp:class:: treebox \
    : public widget_object\<widget_tag, DrawerTrigger>

    A treebox is a widget that displays a hierarchical list of items, such as
    the files and directories on a disk.

    Inherits:
        * :cpp:class:`widget_object\<widget_tag, DrawerTrigger>`
    Concept:
        * :doc:`Widget Window </reference/gui/concepts/widget_window>`

Typedefs
--------

.. cpp:type:: treebox::item_proxy

    A type refers to the item and also used to iterate through the node.

.. cpp:type:: treebox::ext_event_type

    The type of extra event defined by treebox.(See Note 2)

.. cpp:type:: treebox::node_image_type

    The type of state image.

.. cpp:type:: treebox::renderer_interface

    The interface of treebox item renderer.

.. cpp:type:: treebox::compset_placer_interface

    The interface of treebox compset_placer.

Public Functions
----------------

.. cpp:function:: treebox::treebox()

    The default constructor without creating the widget.

.. cpp:function:: treebox::treebox(window wd, bool visible)

    Creates a treebox in a specified window.

.. cpp:function:: treebox::treebox(window wd, const rectangle& r = rectangle(), \
    bool visible = true)

    Creates a treebox with a specified rectangle.

.. cpp:function:: void treebox::auto_draw(bool)

    Enables/disables the widget that draws automatically when it is operated.

.. cpp:function:: treebox& treebox::checkable(bool enable)

    Enables/disables the checkbox for each item of the widgets.

.. cpp:function:: bool treebox::checkable() const

    Determine whether the checkbox is enabled.

.. cpp:function:: item_proxy treebox::erase(item_proxy pos)

    Erases the item pointed to by pos.

.. cpp:function:: void treebox::erase(const nana::string& keypath)

    Erases the item specified by keypath.

.. cpp:function:: ext_event_type& treebox::ext_event() const

    Retrieves the object of extra events.

.. cpp:function:: treebox& treebox::icon(const nana::string& id, \
    const node_image_type&)

    Sets the state image with a specified id name.

.. cpp:function:: node_image_type& treebox::icon(const nana::string& id) const

    Retrieves the object of state image by the specified id name.

.. cpp:function:: void treebox::icon_erase(const nana::string& id)

    Erases the state image by the specified id name.

.. cpp:function:: item_proxy treebox::find(const nana::string& keypath)

    Find a item though a specified keypath.

.. cpp:function:: item_proxy treebox::insert(const nana::string& keypath, \
    const nana::string& title)

    Inserts a new node to treebox, keypath specifies the node hierarchical,
    title is used for displaying. If the keypath is existing, it returns the
    existing node.

.. cpp:function:: item_proxy treebox::insert(item_proxy pos, \
    const nana::string& key, const nana::string& title)

    Inserts a new node to treebox, pos is the parent, key specifies the new
    node, title is used for displaying. If the key is existing, it returns the
    existing node.

.. cpp:function:: nana::string treebox::make_key_path(item_proxy pos, \
    const nana::string& splitter)

    It returns the key path of item pointed to by pos, the splitter splits the
    each owner of node.

.. cpp:function:: treebox& treebox::placer(const Placer&)

    **template<typename Placer>**
    |br|
    Sets a new compset placer.

.. cpp:function:: const nana::pat::cloneable<compset_placer_interface>& \
    treebox::placer() const

    Gets the compset placer.

.. cpp:function:: treebox& treebox::renderer(const ItemRenderer&)

    **template<typename ItemRenderer>**
    |br|
    Sets a new item renderer.

.. cpp:function:: const nana::pat::cloneable<renderer_interface> & \
    treebox::renderer() const

    Gets the item renderer.

.. cpp:function:: item_proxy treebox::selected() const

    Gets the item that is selected.

Item Proxy (on the concept of `Input Iterator`_)
------------------------------------------------

.. cpp:class:: item_proxy \
    : public std::iterator\<std::input_iterator_tag, item_proxy>

    Doc Needed.

.. cpp:function:: treebox::item_proxy()

    The default constructor creates an end iterator.

.. cpp:function:: item_proxy treebox::item_proxy::append(\
    const nana::string& key, const nana::string& name)

    Append a child.

.. cpp:function:: item_proxy treebox::item_proxy::append(\
    const nana::string& key, const nana::string& name, const T& usr_obj)

    **template<typename T>**
    |br|
    Append a child with a specified user object.

.. cpp:function:: bool treebox::item_proxy::empty() const

    Returns true if the proxy does not refer to a node, as an end iterator.

.. cpp:function:: std::size_t treebox::item_proxy::level() const

    Returns the distance between the ROOT node and this node, the return is only
    available when empty() is false.

.. cpp:function:: bool treebox::item_proxy::checked() const

    Returns the check state.

.. cpp:function:: item_proxy& treebox::item_proxy::check(bool)

    Sets the check state, and it returns itself.

.. cpp:function:: bool treebox::item_proxy::expanded() const

    Returns true when the node is expanded.

.. cpp:function:: item_proxy& treebox::item_proxy::expand(bool)

    Expand/Shrink children of the node, and it returns itself.

.. cpp:function:: bool treebox::item_proxy::selected() const

    Returns true when the node is selected.

.. cpp:function:: item_proxy& treebox::item_proxy::select(bool)

    Select the node, and it returns itself.

.. cpp:function:: const nana::string& treebox::item_proxy::icon() const

    Returns the id of icon.

.. cpp:function:: item_proxy& treebox::item_proxy::icon(const nana::string& id)

    Set the id of icon.

.. cpp:function:: const nana::string& treebox::item_proxy::text() const

    Returns the text.

.. cpp:function:: item_proxy& treebox::item_proxy::text(const nana::string&)

    Set a new text.

.. cpp:function:: const nana::string& treebox::item_proxy::key() const

    Returns the key.

.. cpp:function:: item_proxy& treebox::item_proxy::key(const nana::string&)

    Set a new key.

.. cpp:function:: std::size_t treebox::item_proxy::size() const

    Returns the number of child nodes.

.. cpp:function:: item_proxy treebox::item_proxy::child() const

    Returns the first child of the node.

.. cpp:function:: item_proxy treebox::item_proxy::owner() const

    Returns the owner of the node.

.. cpp:function:: item_proxy treebox::item_proxy::sibling() const

    Returns the sibling of the node.

.. cpp:function:: item_proxy treebox::item_proxy::begin() const

    Returns an proxy pointing to the beginning of node.

.. cpp:function:: item_proxy treebox::item_proxy::end() const

    Returns an proxy pointing to the end of node.

.. cpp:function:: bool treebox::item_proxy::operator==(\
    const nana::string& s) const

    Compare the text of node with the s.

.. cpp:function:: bool treebox::item_proxy::operator==(const char* s) const

    Ditto.

.. cpp:function:: bool treebox::item_proxy::operator==(const wchar_t* s) const

    Ditto.

Notes
-----

Note 1
^^^^^^

The key string is case sensitive.

Note 2
^^^^^^

*treebox::ext_event_type*. Nana.GUI event is a general-purpose, such as
mouse_down, mouse_up, mouse_move event. But some widgets need some events that
satisfies its own specifical demand, so the widget class provides an
ext_event_type for this demand. Its definition is:

.. code-block:: cpp

    struct ext_event_type
    {
        typedef Implementation-Specified item_proxy;

        nana::fn_group<void(nana::gui::window, item_proxy, bool exp)> expand;  //Expands or shrinks an item, exp is true if it is expanded.
        nana::fn_group<void(nana::gui::window, item_proxy, bool set)> checked; //The item is checked, set is true if it is checked.
        nana::fn_group<void(nana::gui::window, item_proxy, bool set)> selected; //Selects an item, set is true if it is selected.
    };

If a node is expanded or shrinked, the expand method will be called, and the 3rd
parameter specifies whether the node is expanded.

If a node is selected or unselected, the selected method will be called, and the
3rd parameter specifies whether the node is selected.

Note 3
^^^^^^

The definition of *node_image_type*

.. code-block:: cpp

    struct node_image_type
    {
        nana::paint::image normal;
        nana::paint::image highlighted;
        nana::paint::image expanded;
    };

Examples
--------

1. Loading directories into the treebox
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. image:: /_static/images/treebox.png
    :align: center

.. code-block:: cpp

    #include <nana/gui/wvl.hpp>
    #include <nana/gui/widgets/treebox.hpp>
    #include <nana/filesystem/file_iterator.hpp>


    class folder_tree
        : public nana::gui::form
    {
    public:
        typedef nana::gui::treebox::item_proxy item_proxy;

        folder_tree()
            : nana::gui::form(nana::gui::API::make_center(300, 300),
                              appear::decorate<appear::taskbar>())
        {

            tree_.create(*this, 0, 0, 300, 300);

    #if defined(NANA_WINDOWS)
            item_proxy node = tree_.insert(STR("C:"), STR("Local Drive(C:)"));
            nana::filesystem::file_iterator i(STR("C:\\")), end;
    #elif defined(NANA_LINUX)
            //Use a whitespace for the root key since the root under linux
            //is / character.
            item_proxy node = tree_.insert(STR("/"), STR("Root"));
            nana::filesystem::file_iterator i(STR("/")), end;
    #endif
            for(; i != end; ++i)
            {
                if(false == i->directory) continue;

                tree_.insert(node, i->name, i->name);
                break;
            }
            tree_.ext_event().expand = nana::make_fun(*this, &folder_tree::_m_expand);
        }
    private:
        void _m_expand(nana::gui::window, item_proxy node, bool exp)
        {
            if(!exp) return; //If this is contracted.

            //Windows supports the path separator '/'
            nana::string path = tree_.make_key_path(node, STR("/")) + STR("/");

            //Trim the whitespace at head of the path. So the code can be work
            //well in both Windows and Linux.
            nana::string::size_type path_start_pos = path.find_first_not_of(STR(" "));
            if(path_start_pos != path.npos)
                path.erase(0, path_start_pos);

            //Walk in the path directory for sub directories.
            nana::filesystem::file_iterator i(path), end;
            for(; i != end; ++i)
            {
                if(false == i->directory) continue; //If it is not a directory.

                item_proxy child = tree_.insert(node, i->name, i->name);
                if(child.empty()) continue;

                //Find a directory in child directory, if there is a directory,
                //insert it into the child, just insert one node to indicate the
                //node has a child and an arrow symbol will be displayed in the
                //front of the node.
                nana::filesystem::file_iterator u(path + i->name);
                for(; u != end; ++u)
                {
                    if(false == u->directory) continue; //If it is not a directory.
                    tree_.insert(child, u->name, u->name);
                    break;
                }
            }
        }
    private:
        nana::gui::treebox tree_;
    };

    int main()
    {
        folder_tree fdtree;
        fdtree.show();
        nana::gui::exec();
    }

See Also
--------

None.
